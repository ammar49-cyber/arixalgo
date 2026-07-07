#pragma once
#ifndef ARIX_OBF_H
#define ARIX_OBF_H

#include "control_flow_obfuscation.h"
#include "string_obfuscation_technique.h"
#include "instruction_obfuscation_engine.h"
#include "opaque_predicate_generator.h"
#include "virtualized_code_execution.h"
#include "anti_debugging_countermeasure.h"
#include <chrono>
#include <vector>
#include <cstdint>

namespace arix {

enum class ArixObfuscationLevel {
    ARIX_OBF_NONE,
    ARIX_OBF_LIGHT,
    ARIX_OBF_MEDIUM,
    ARIX_OBF_HEAVY,
    ARIX_OBF_MAXIMUM
};

struct ArixObfuscationReport {
    ArixObfuscationLevel level;
    size_t transformations_applied;
    size_t blocks_flattened;
    size_t strings_encrypted;
    size_t instructions_substituted;
    size_t opaque_predicates_inserted;
    size_t bytecode_size;
    bool virtualization_applied;
    bool anti_debug_applied;
    double execution_time_ms;
    double phase_light_ms;
    double phase_medium_ms;
    double phase_heavy_ms;
    double phase_maximum_ms;
};

class ArixObfuscator {
public:
    ArixObfuscator();
    ~ArixObfuscator();

    void configure(ArixObfuscationLevel level);
    ArixObfuscationLevel get_level() const;

    ArixObfuscationReport obfuscate(ArixObfCFG& cfg);
    bool verify(ArixObfCFG& cfg, const std::vector<uint64_t>& test_inputs);
    bool obfuscate_and_verify(ArixObfCFG& cfg, const std::vector<uint64_t>& test_inputs);
    ArixObfuscationReport obfuscate_with_seed(ArixObfCFG& cfg, uint64_t seed);
    bool run_self_test();
    void reset();
    void clear_string_pool();
    size_t get_transform_count() const;
    void print_report(const ArixObfuscationReport& report);
    bool verify_extended(ArixObfCFG& cfg, const std::vector<uint64_t>& test_inputs);
    void apply_obfuscation_preset(ArixObfCFG& cfg, int preset_id);

    ArixObfStringPool& string_pool() { return string_pool_; }
    ArixObfCFGFlattener& flattener() { return flattener_; }
    ArixObfSubst& substituter() { return substituter_; }
    ArixObfVM& vm() { return vm_; }
    ArixAntiDebug& anti_debug() { return anti_debug_; }

private:
    ArixObfuscationLevel level_;
    ArixObfStringPool string_pool_;
    ArixObfCFGFlattener flattener_;
    ArixObfSubst substituter_;
    ArixObfVM vm_;
    ArixAntiDebug anti_debug_;

    size_t transform_count_;

    void apply_light(ArixObfCFG& cfg);
    void apply_medium(ArixObfCFG& cfg);
    void apply_heavy(ArixObfCFG& cfg);
    void apply_maximum(ArixObfCFG& cfg);
    void shuffle_blocks(ArixObfCFG& cfg);
    void encrypt_string_pool_rotating();
    void seed_rng(uint64_t seed);
    void apply_opaque_predicates_multi(ArixObfCFG& cfg, int layers);
    void split_blocks(ArixObfCFG& cfg);
    void apply_anti_debug_all();
};

} // namespace arix

#endif // ARIX_OBF_H
