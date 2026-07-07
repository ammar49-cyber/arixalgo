#pragma once
#ifndef ARIX_OBF_INST_H
#define ARIX_OBF_INST_H

#include "control_flow_obfuscation.h"
#include <random>
#include <vector>
#include <string>
#include <unordered_map>

namespace arix {

class ArixObfSubst {
public:
    ArixObfSubst();
    void set_seed(uint64_t seed);
    void substitute_add(ArixObfBlock& block);
    void substitute_logic(ArixObfBlock& block);
    void substitute_compare(ArixObfBlock& block);
    void substitute_all(ArixObfBlock& block);
    void substitute_all_blocks(ArixObfCFG& cfg);
    void insert_junk(ArixObfBlock& block);
    void insert_junk_extended(ArixObfBlock& block);
    void rename_registers_block(ArixObfBlock& block, int& next_temp);
    void rename_registers_cfg(ArixObfCFG& cfg);

private:
    std::mt19937_64 rng;
    bool choose_substitution();
    int rand_int(int min, int max);

    ArixObfInstruction substitute_add_inst(const ArixObfInstruction& inst);
    std::vector<ArixObfInstruction> substitute_sub_inst(const ArixObfInstruction& inst);
    std::vector<ArixObfInstruction> substitute_mul_inst(const ArixObfInstruction& inst);
    std::vector<ArixObfInstruction> substitute_div_inst(const ArixObfInstruction& inst);
    std::vector<ArixObfInstruction> substitute_and_inst(const ArixObfInstruction& inst);
    std::vector<ArixObfInstruction> substitute_or_inst(const ArixObfInstruction& inst);
    std::vector<ArixObfInstruction> substitute_xor_inst(const ArixObfInstruction& inst);
    std::vector<ArixObfInstruction> substitute_not_inst(const ArixObfInstruction& inst);
    std::vector<ArixObfInstruction> substitute_neg_inst(const ArixObfInstruction& inst);
    std::vector<ArixObfInstruction> substitute_shl_inst(const ArixObfInstruction& inst);

    std::vector<ArixObfInstruction> substitute_add_lea_scaled(const ArixObfInstruction& inst);
    std::vector<ArixObfInstruction> substitute_sub_neg_adc(const ArixObfInstruction& inst);
    std::vector<ArixObfInstruction> substitute_mul_karatsuba(const ArixObfInstruction& inst);
    std::vector<ArixObfInstruction> substitute_and_nand_variant(const ArixObfInstruction& inst);
    std::vector<ArixObfInstruction> substitute_or_nand_variant(const ArixObfInstruction& inst);
    std::vector<ArixObfInstruction> substitute_xor_nand_variant(const ArixObfInstruction& inst);
};

} // namespace arix

#endif // ARIX_OBF_INST_H
