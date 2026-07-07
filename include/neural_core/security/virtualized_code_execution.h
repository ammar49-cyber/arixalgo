#pragma once
#ifndef ARIX_OBF_VM_H
#define ARIX_OBF_VM_H

#include "control_flow_obfuscation.h"
#include <cstdint>
#include <vector>
#include <array>

namespace arix {

enum class ArixObfBytecode : uint8_t {
    NOP    = 0x00,
    ADD    = 0x01,
    SUB    = 0x02,
    MUL    = 0x03,
    DIV    = 0x04,
    LOAD   = 0x05,
    STORE  = 0x06,
    PUSH   = 0x07,
    POP    = 0x08,
    JMP    = 0x09,
    CALL   = 0x0A,
    RET    = 0x0B,
    CMP    = 0x0C,
    JZ     = 0x0D,
    JNZ    = 0x0E,
    XOR_OP = 0x0F,
    FADD   = 0x10,
    FSUB   = 0x11,
    FMUL   = 0x12,
    FDIV   = 0x13,
    MREAD  = 0x14,
    MWRITE = 0x15,
    ENTER  = 0x16,
    LEAVE  = 0x17,
    SWAP   = 0x18,
    AND_OP = 0x19,
    OR_OP  = 0x1A,
    HALT   = 0xFF
};

struct ArixObfVMState {
    std::array<uint64_t, 256> regs;
    std::array<double, 8> fregs;
    std::vector<uint64_t> stack;
    std::vector<uint8_t> mem;
    size_t ip;
    bool running;
    uint64_t flags;

    ArixObfVMState() : ip(0), running(true), flags(0) {
        regs.fill(0);
        fregs.fill(0.0);
        mem.resize(65536, 0);
    }
};

using ArixObfHandlerFunc = void(*)(ArixObfVMState& state, uint8_t op1, uint8_t op2);

struct ArixObfHandler {
    ArixObfHandlerFunc func;
    bool initialized;
};

class ArixObfVM {
public:
    ArixObfVM();
    ~ArixObfVM();

    void add_handler(ArixObfBytecode opcode, ArixObfHandler handler);
    void compile_to_bytecode(ArixObfCFG& cfg);
    bool vm_execute(const uint8_t* bytecode, size_t len);
    bool load_bytecode(const std::vector<uint8_t>& bc);

    ArixObfVMState& state() { return vm_state; }
    const std::vector<uint8_t>& bytecode() const { return bytecode_; }

    void encrypt_handler_table();
    void decrypt_handler_table();
    bool validate_bytecode(const uint8_t* bytecode, size_t len);
    void vm_exit_cleanup();

    bool encrypt_bytecode();
    bool decrypt_bytecode();
    void reset_state();
    size_t instruction_count(const uint8_t* bytecode, size_t len);
    bool snapshot_state(ArixObfVMState& out) const;
    bool restore_state(const ArixObfVMState& in);

    void set_opcode_xor_key(uint8_t key) { for (auto& k : opcode_xor_key_) k = key; }
    void set_opcode_xor_key_byte(size_t idx, uint8_t key) { if (idx < opcode_xor_key_.size()) opcode_xor_key_[idx] = key; }

private:
    ArixObfVMState vm_state;
    std::vector<uint8_t> bytecode_;
    std::array<ArixObfHandler, 48> handlers;
    std::array<uint8_t, 8> handler_xor_key;
    std::array<uint8_t, 32> opcode_xor_key_;
    std::array<uint8_t, 48> handler_indirection;
    bool table_encrypted;
    bool per_opcode_key_encrypted_;
    int entry_offset_;

    void dispatch(uint8_t opcode, uint8_t op1, uint8_t op2);
    uint8_t resolve_opcode(uint8_t raw_opcode, size_t ip);

    static void handler_nop(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_add(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_sub(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_mul(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_div(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_load(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_store(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_push(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_pop(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_jmp(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_call(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_ret(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_cmp(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_jz(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_jnz(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_xor_op(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_fadd(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_fsub(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_fmul(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_fdiv(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_mread(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_mwrite(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_enter(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_leave(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_swap(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_and(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_or(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_inc(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_dec(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_not(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_rotl(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_rotr(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_load64(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_jle(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_jg(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_jge(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_mov_reg(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_xchg(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_test(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_pushf(ArixObfVMState& state, uint8_t op1, uint8_t op2);
    static void handler_popf(ArixObfVMState& state, uint8_t op1, uint8_t op2);
};

} // namespace arix

#endif // ARIX_OBF_VM_H
