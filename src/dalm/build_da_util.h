#ifndef DALM_BUILD_DA_UTIL_H_
#define DALM_BUILD_DA_UTIL_H_

#include <cstddef>
#include <cstdint>
#include "da.h"
#include "vocabulary.h"

namespace DALM::build_da_util {

int find_base(
        const DAPair* da_array,
        long array_size, const VocabId* children, size_t n_children, int initial_base, const uint64_t* validate,
        uint64_t words_prefix, size_t prefix_length,
        size_t& skip_counts, size_t& loop_counts, size_t& mem_access_counts);

int find_base_brute_force (
        const DAPair* da_array,
        long array_size, const VocabId* children, size_t n_children, int initial_base, const uint64_t* validate,
        uint64_t words_prefix, size_t prefix_length,
        size_t& skip_counts, size_t& loop_counts, size_t& mem_access_counts);

int find_base_bit_parallel (
        const DAPair* da_array,
        long array_size, const VocabId* children, size_t n_children, int initial_base, const uint64_t* validate,
        size_t& skip_counts, size_t& loop_counts, size_t& mem_access_counts);

int find_base_convolution (
        const DAPair* da_array,
        long array_size, const VocabId* children, size_t n_children, int initial_base, const uint64_t* validate,
        size_t& skip_counts, size_t& loop_counts, size_t& mem_access_counts);

}

#endif //DALM_BUILD_DA_UTIL_H_
