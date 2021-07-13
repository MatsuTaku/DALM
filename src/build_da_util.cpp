#include "dalm/build_da_util.h"

#include <cassert>
#include <algorithm>

#include "dalm/bit_util.h"
#include "dalm/convolution.h"

#if !defined(DALM_NEW_XCHECK) || defined(DALM_EL_SKIP)
#define FIND_BASE_ACCESS_DA
#endif

using namespace DALM;

int build_da_util::find_base(
        const DAPair* da_array,
        long array_size, const VocabId* children, size_t n_children, int initial_base, const uint64_t* validate,
        uint64_t words_prefix, size_t prefix_length,
        size_t& skip_counts, size_t& loop_counts, size_t& mem_access_counts) {

#ifndef DALM_NEW_XCHECK
    return build_da_util::find_base_brute_force(
            da_array,
            array_size, children, n_children, initial_base, validate,
            words_prefix, prefix_length,
            skip_counts, loop_counts, mem_access_counts);
#elif !defined(DALM_CNV_XCHECK)
    return build_da_util::find_base_bit_parallel(
            da_array,
            array_size, children, n_children, initial_base, validate,
            skip_counts, loop_counts, mem_access_counts);
#elif defined(DALM_CNV_XCHECK)
    return build_da_util::find_base_convolution(
            da_array,
            array_size, children, n_children, initial_base, validate,
            skip_counts, loop_counts, mem_access_counts);
#endif

}

int build_da_util::find_base_brute_force(
        const DAPair* da_array,
        long array_size, const VocabId* children, size_t n_children, int initial_base, const uint64_t* validate,
        uint64_t words_prefix, size_t prefix_length,
        size_t& skip_counts, size_t& loop_counts, size_t& mem_access_counts) {

    auto base = initial_base;
    while (base + children[0] < array_size) {
        skip_counts++;

#ifndef DALM_PLAIN_XCHECK
        assert(da_array[base + children[0]].check.check_val < 0);
        std::size_t index = base / DALM_N_BITS_UINT64_T;
        std::size_t bit = base % DALM_N_BITS_UINT64_T;
        assert(index + 1 < array_size / DALM_N_BITS_UINT64_T + 1);

        uint64_t array_prefix;
        if(bit == 0){
            array_prefix = validate[index];
        }else{
            array_prefix = (validate[index] >> bit);
            array_prefix |= ((validate[index+1] & ((0x1ULL << bit) - 1))<<(64-bit));
        }

        if((array_prefix & words_prefix) == 0){
            std::size_t k = prefix_length + 1;
            while(k < n_children){
                loop_counts++;
                int pos = base + children[k];

                if(pos < array_size && da_array[pos].check.check_val >= 0) {
                    assert(da_array[base + children[0]].check.check_val < 0);
                    base -= da_array[base + children[0]].check.check_val;
                    k = prefix_length + 1;
                    assert(base >= 0);
                    break;
                } else {
                    ++k;
                }
            }
            if(k >= n_children) break;
        }else{
            assert(da_array[base + children[0]].check.check_val < 0);
            base -= da_array[base + children[0]].check.check_val;
            assert(base >= 0);
        }
#else
        size_t k = 0;
        while (k < n_children) {
            loop_counts++;
            int pos = base + children[k];

            if(pos < array_size && da_array[pos].check.check_val >= 0) {
                break;
            } else {
                ++k;
            }
        }
        if (k == n_children)
            break;
        assert(da_array[base + children[0]].check.check_val < 0);
        base -= da_array[base + children[0]].check.check_val;
        assert(base >= 0);
#endif
    }
    return base;
}

namespace {

inline uint64_t validate_at(long array_size, const uint64_t* validate, size_t index) {
    return index*64 < array_size ? validate[index] : 0ull;
}

inline uint64_t validate_word_from(long array_size, const uint64_t* validate, size_t pos) {
    auto index = pos/64;
    auto inset = pos%64;
    if (inset == 0) {
        return validate_at(array_size, validate, index);
    } else {
        return (validate_at(array_size, validate, index) >> inset)
               | (validate_at(array_size, validate, index+1) << (64-inset));
    }
}

}

int build_da_util::find_base_bit_parallel(
    const DAPair* da_array,
    long array_size, const VocabId* children, size_t n_children, int initial_base, const uint64_t* validate,
    size_t& skip_counts, size_t& loop_counts, size_t& mem_access_counts) {

    auto base = initial_base;
    while (base + children[0] < array_size) {
        skip_counts++;

        uint64_t base_mask = 0;
        for (size_t k = 0; k < n_children; k++) {
            loop_counts++;
            base_mask |= validate_word_from(array_size, validate, base + children[k]);
            if (~base_mask == 0ull)
                break;
        }
        if (~base_mask != 0ull) {
            return base + (int) bit_util::ctz(~base_mask);
        } else {
#ifndef DALM_EL_SKIP
            base += 64;
#else
/*
 *  0           64
 * |ooxo...xoox|xxxxxo...
 *  ↑        ↑ →→→→→ ↑
 *  1        2 link  3
 * 1. Current empty head
 * 2. Back empty element of 64s alignment
 * 3. Next empty head
 */
            assert(da_array[base + children[0]].check.check_val < 0);
            int trailing_front = base + (int) children[0]; // 1
            if (trailing_front + 64 < array_size) {
                int trailing_insets = 63 - (int) bit_util::clz(~validate_word_from(array_size, validate, trailing_front)); // 2-1
                assert(da_array[trailing_front + trailing_insets].check.check_val < 0);
                int skip_distance = trailing_insets + (-da_array[trailing_front + trailing_insets].check.check_val); // 3-1
                assert(skip_distance >= 64); // The advantage to above one.
                base += skip_distance;
            } else {
                base += 64;
            }
#endif
        }
    }

    return std::max((int) array_size - (int) children[0], initial_base);
}

int build_da_util::find_base_convolution(
        const DAPair* da_array,
        long array_size, const VocabId* children, size_t n_children, int initial_base, const uint64_t* validate,
        size_t& skip_counts, size_t& loop_counts, size_t& mem_access_counts) {

    const VocabId min_c = children[0], max_c = children[n_children-1];

    const size_t m = max_c - min_c + 1;
    size_t _n = 1;
    while (_n < m*2) _n <<= 1;
    const size_t n = _n;
    const size_t window_size = n-(m-1);
    using mint = build_da_util::convolution::ModuloNTT;
    std::vector<mint> fda(n), fch(n, 0);
    for (int i = 0; i < n_children; i++) {
        fch[m-1-(children[i]-min_c)] = 1;
    }
    build_da_util::convolution::ntt(fch.data(), n);
    for (int i = initial_base + (int) min_c; i < array_size; ) {
        skip_counts++;

        assert(da_array[i].check.check_val < 0);
        int empty_back = i;
        {
            int j = 0;
            for (; j < n and i + j < array_size; j++) {
                bool exists = da_array[i + j].check.check_val >= 0;
                fda[j] = (mint) (int) exists;
                if (j < window_size and !exists)
                    empty_back = i + j;
            }
            for (; j < n; j++) {
                fda[j] = 0;
                if (j < window_size)
                    empty_back = i + j;
            }
        }
        build_da_util::convolution::index_sum_convolution_for_xcheck(fda.data(), fch.data(), n);
        for (int j = 0; j < window_size; j++) {
            if (fda[m-1+j] == 0) {
                return i + j - (int) min_c;
            }
        }
        if (empty_back < array_size) {
            i = empty_back + -da_array[empty_back].check.check_val;
        } else {
            break;
        }
    }

    return std::max((int) array_size - (int) children[0], initial_base);

}
