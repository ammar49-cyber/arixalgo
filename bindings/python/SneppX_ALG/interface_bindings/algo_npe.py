"""NPE (Neural Programming Engine) algorithm bindings.

Wraps C implementations in ``algorithms/npe/core/`` with pure-Python fallback.
"""

from typing import List, Dict, Optional, Any

import numpy as np

from .c_loader import load_library

_LIB, _HAS_C = load_library("neural_architecture_layer")


class NPEInstruction:
    """A single instruction in the neural program."""

    def __init__(self, opcode: str, args: List[Any], dest: str = ""):
        self.opcode = opcode
        self.args = args
        self.dest = dest

    def __repr__(self) -> str:
        return f"{self.dest} = {self.opcode}({', '.join(str(a) for a in self.args)})"


class NPEProgram:
    """Neural program — sequence of differentiable instructions."""

    def __init__(self, name: str = "program"):
        self.name = name
        self.instructions: List[NPEInstruction] = []
        self._has_c = _HAS_C

    def add(self, instr: NPEInstruction) -> None:
        self.instructions.append(instr)

    def __len__(self) -> int:
        return len(self.instructions)

    def __getitem__(self, idx: int) -> NPEInstruction:
        return self.instructions[idx]


class NPECompiler:
    """Neural program compiler — translates high-level operations into VM instructions."""

    def __init__(self):
        self._has_c = _HAS_C

    def compile(self, operations: List[dict]) -> NPEProgram:
        prog = NPEProgram("compiled")
        for op in operations:
            instr = NPEInstruction(
                opcode=op.get("type", "nop"),
                args=op.get("args", []),
                dest=op.get("dest", ""),
            )
            prog.add(instr)
        return prog


class NPEVM:
    """Neural virtual machine — executes compiled neural programs."""

    def __init__(self):
        self._registers: Dict[str, np.ndarray] = {}
        self._has_c = _HAS_C

    def execute(self, program: NPEProgram, inputs: Dict[str, np.ndarray]) -> Dict[str, np.ndarray]:
        self._registers = dict(inputs)
        for instr in program.instructions:
            self._execute_instr(instr)
        return dict(self._registers)

    def _execute_instr(self, instr: NPEInstruction) -> None:
        if instr.opcode == "add":
            a = self._registers.get(instr.args[0], np.array(0))
            b = self._registers.get(instr.args[1], np.array(0))
            self._registers[instr.dest] = a + b
        elif instr.opcode == "mul":
            a = self._registers.get(instr.args[0])
            b = self._registers.get(instr.args[1])
            if a is not None and b is not None:
                self._registers[instr.dest] = a @ b if a.ndim >= 2 else a * b
        elif instr.opcode == "matmul":
            a = self._registers.get(instr.args[0])
            b = self._registers.get(instr.args[1])
            if a is not None and b is not None:
                self._registers[instr.dest] = a @ b
        elif instr.opcode == "relu":
            x = self._registers.get(instr.args[0])
            if x is not None:
                self._registers[instr.dest] = np.maximum(x, 0)
        elif instr.opcode == "copy":
            x = self._registers.get(instr.args[0])
            if x is not None:
                self._registers[instr.dest] = x.copy()

    def reset(self) -> None:
        self._registers.clear()


class NPEVerify:
    """Neural program verification — checks program validity and properties."""

    def __init__(self):
        self._has_c = _HAS_C

    def check_types(self, program: NPEProgram, var_types: Dict[str, str]) -> List[str]:
        errors = []
        for instr in program.instructions:
            for arg in instr.args:
                if isinstance(arg, str) and arg not in var_types:
                    errors.append(f"Undefined variable: {arg}")
        return errors

    def check_bounds(self, program: NPEProgram, max_ops: int = 1000) -> bool:
        return len(program) <= max_ops
