# HSS training guide

This guide explains how to build, run, and train a **Hierarchical State Space
(HSS)** model with SNEPPX-Alg, end to end, on CPU. It walks through the
autodiff tape, the layer-normalization step (the component fixed in
`v0.9.7.890e`), and a minimal training loop.

## 1. Model and parameters

An HSS model is created from a config and a seed:

```c
SNEPPXHSSConfig cfg;
cfg.state_dim   = 4;
cfg.input_dim   = 4;
cfg.output_dim  = 4;
cfg.num_layers  = 1;
cfg.seq_len     = 1;
cfg.dt_min      = 0.01f;
cfg.dt_max      = 0.1f;
cfg.use_hierarchical = 0;

SNEPPXHSSModel* model = SNEPPX_hss_model_create(&cfg, 42);
```

Per-layer parameters (9 tensors each) are enumerated by
`SNEPPX_hss_get_params`:

| idx | name | meaning |
|-----|------|---------|
| 0 | `A` | state transition |
| 1 | `B` | input→state |
| 2 | `C` | state→output |
| 3 | `D` | feedthrough (randn-init in `SNEPPX_hss_layer_create`) |
| 4 | `dt` | per-step size |
| 5 | `xp` | input projection weight |
| 6 | `xpb` | input projection bias (randn-init) |
| 7 | `ng` | **layer-norm gain** (`norm_gamma`) |
| 8 | `nb` | **layer-norm shift** (`norm_beta`) |

> Note: `ng` / `nb` (indices 7/8) are the gain/shift of the layer-norm applied
> to the input projection. In `v0.9.7.890e` their gradients are computed
> correctly — previously the backward pass read the tensor *pointer* as floats
> and produced garbage, which is why training diverged.

## 2. Building the training graph

Each training step:

1. Wraps the model parameters in `SNEPPXVariable*` (with `requires_grad = 1`).
2. Builds the forward graph with `SNEPPX_hss_build_train_graph`.
3. Attaches an `SNEPPX_mse_loss`.
4. Runs `SNEPPX_tape_backward`.
5. Steps the optimizer.
6. Destroys the tape (after nulling the weight `data` pointers so the tape does
   not free the model's parameter tensors).

```c
SNEPPXTape* tape = SNEPPX_tape_create();

SNEPPXVariable** wv = /* SNEPPX_variable_create(pt[i], 1) for each param */;
SNEPPXVariable* inv = SNEPPX_variable_create(input, 0);

SNEPPXVariable* outv = NULL;
SNEPPX_hss_build_train_graph(model, tape, inv, wv, nw, &outv);

SNEPPXVariable* tgv  = SNEPPX_variable_create(target, 0);
SNEPPXVariable* loss = SNEPPX_mse_loss(tape, outv, tgv);

SNEPPX_tape_backward(tape, loss);

SNEPPXTensor** gt = /* wv[i]->grad */;
SNEPPX_optimizer_step(opt, pt, gt, nw);
```

## 3. The layer-norm step (and the v0.9.7.890e fix)

Inside `SNEPPX_hss_build_train_graph` (file
`algorithms/hss/core/forward_train.c`), each timestep computes:

```
x_col  → xp_out = matmul(xp, xt_col)
       → xp_b   = add(xp_out, xpb)
       → xp_b_row = transpose(xp_b)
       → xp_norm  = layer_norm(xp_b_row, ng, nb, 1e-5)   ← gain/shift here
       → xp_norm_col = transpose(xp_norm)
       → x_col2 = add(zv, xp_norm_col)
       → h_new  = add(matmul(A_bar, h), matmul(B_bar, x_col2))
       ...
```

`SNEPPX_layer_norm` stores `ctx->gamma = gamma` (the **variable**). Its
backward (`backward_layer_norm` in `kernel/autodiff/ops.c`) must read the float
buffer as `c->gamma->data->data`, **not** `c->gamma->data`. The original bug
cast the tensor struct to `float*`, so `gd` was garbage and the input gradient
`gx = gamma * (x - mean) / std` exploded. The fix is exactly that one extra
`->data` dereference, plus `#include <math.h>` so `sqrt` (used for `inv_std`)
returns the correct value.

## 4. Optimizer

```c
SNEPPXOptimizerConfig opt_cfg = SNEPPX_optimizer_config_default();
opt_cfg.learning_rate = 0.03f;
opt_cfg.type = SNEPPX_OPTIMIZER_ADAM;
SNEPPXOptimizer* opt = SNEPPX_optimizer_create(&opt_cfg);
```

`SNEPPX_optimizer_step(opt, params, grads, n)` applies the update in place.

## 5. Verifying training works

Run the bundled integration test:

```powershell
cmake -B build -DCMAKE_BUILD_TYPE=Release -DSNEPPX_BUILD_TESTS=ON
cmake --build build --config Release --target test_train_integration
& build\tests\Release\test_train_integration.exe
```

Expected (v0.9.7.890e):

```
init=0.234858 final=0.015788 ratio=0.0672
PASS
```

Loss should drop by >90% over 10 Adam steps. If you see it *increase* or go
`NaN`, the regression is present — rebuild from the `v0.9.7.890e` tag.

## 6. Tips

- **Learning rate:** 0.03 with Adam over 10 steps is enough to show the fix.
  Tune LR/steps for your own data.
- **Determinism:** fixed seed + Adam ⇒ reproducible runs.
- **Mixed precision / CUDA:** the HSS forward is CPU today; CUDA GEMM is
  available via `SNEPPX_BUILD_CUDA=ON` but the HSS train graph itself runs on
  the CPU autodiff tape.
- **Don't free params via the tape:** null `wv[i]->data` before
  `SNEPPX_tape_destroy` so the model's parameter tensors survive across steps.
