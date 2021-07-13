#ifndef DALM_CONVOLUTION_H_
#define DALM_CONVOLUTION_H_

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <bitset>
#include <cassert>

#include "bit_util.h"

namespace DALM::build_da_util::convolution {

using uint = uint32_t;

template<uint MOD>
class Modulo {
private:
    uint v_;
public:
    constexpr Modulo() : v_(0) {}
    template<typename T>
    constexpr Modulo(T v) : v_(v >= 0 ? v % (T) MOD : v % (T) MOD + (T) MOD) {}

    constexpr uint val() const {
        return v_;
    }
    constexpr bool operator==(Modulo x) const {
        return v_ == x.v_;
    }
    constexpr bool operator!=(Modulo x) const {
        return v_ != x.v_;
    }

    Modulo operator+() const {
        return *this;
    }
    Modulo operator-() const {
        return {MOD - v_};
    }
    constexpr Modulo operator+(Modulo x) const {
        return {v_ + x.v_};
    }
    constexpr Modulo operator-(Modulo x) const {
        return *this + -x;
    }
    constexpr Modulo operator*(Modulo x) const {
        return {(unsigned long long) v_ * x.v_};
    }
    friend constexpr Modulo pow(Modulo x, uint p) {
        Modulo t = 1;
        Modulo u = x;
        while (p > 0) {
            if (p & 1) {
                t *= u;
            }
            u *= u;
            p >>= 1;
        }
        return t;
    }
    constexpr Modulo inv() const {
        return pow(*this, MOD-2);
    }
    constexpr Modulo operator/(Modulo x) const {
        return *this * x.inv();
    }

    constexpr Modulo& operator+=(Modulo x) {
        return *this = *this + x;
    }
    constexpr Modulo& operator-=(Modulo x) {
        return *this = *this - x;
    }
    constexpr Modulo& operator*=(Modulo x) {
        return *this = *this * x;
    }
    constexpr Modulo& operator/=(Modulo x) {
        return *this = *this / x;
    }
};

template<typename T>
void bit_reverse(T f[], size_t n) {
    for (size_t i = 0, j = 1; j < n-1; j++) {
        for (size_t k = n >> 1; k > (i ^= k); k >>= 1) {}
        if (i < j) std::swap(f[i], f[j]);
    }
}

constexpr uint kModNTT = 469762049;
constexpr int kDivLim = 26;
using ModuloNTT = Modulo<kModNTT>;
constexpr ModuloNTT kPrimitiveRoot = 3;

// Number Theoretic Transform
template<bool INV>
void _ntt(ModuloNTT f[], size_t n) {
    if (n == 1)
        return;
    if (n > 1<<kDivLim) {
        throw std::logic_error("Length of input array of NTT is too long.");
    }
    static bool initialized = false;
    static ModuloNTT es[kDivLim+1], ies[kDivLim+1];
    if (!initialized) {
        initialized = true;
        es[kDivLim] = pow(kPrimitiveRoot, (kModNTT-1)>>kDivLim);
        for (int i = kDivLim-1; i >= 0; i--) {
            es[i] = es[i+1] * es[i+1];
        }
        ies[kDivLim] = es[kDivLim].inv();
        for (int i = kDivLim-1; i >= 0; i--) {
            ies[i] = ies[i+1] * ies[i+1];
        }
    }

    bit_reverse(f, n);
    for (int s = 1; 1 << s <= n; s++) {
        const size_t m = 1 << s;
        const auto wm = !INV ? es[s] : ies[s];
        for (size_t k = 0; k < n; k += m) {
            ModuloNTT w = 1;
            for (size_t j = 0; j < m/2; j++) {
                auto a = f[k + j];
                auto b = f[k + j + m/2] * w;
                f[k + j]       = a + b;
                f[k + j + m/2] = a - b;
                w *= wm;
            }
        }
    }

    if constexpr (INV) {
        auto invn = ModuloNTT(n).inv();
        for (size_t i = 0; i < n; i++)
            f[i] *= invn;
    }
}

bool initialized = false;
ModuloNTT sum_e[kDivLim-1], sum_ie[kDivLim-1];

void _initialize_ntt() {
    if (initialized)
        return;
    initialized = true;
    ModuloNTT es[kDivLim+1], ies[kDivLim+1];
    {
        ModuloNTT e = pow(kPrimitiveRoot, (kModNTT-1)>>kDivLim);
        auto ie = e.inv();
        for (int i = kDivLim; i >= 2; i--) {
            es[i-2] = e;
            ies[i-2] = ie;
            e *= e;
            ie *= ie;
        }
    }
    ModuloNTT w = 1, iw = 1;
    for (int i = 0; i <= kDivLim - 2; i++) {
        sum_e[i] = es[i] * w;
        w *= ies[i];
        sum_ie[i] = ies[i] * iw;
        iw *= es[i];
    }
}

void ntt(ModuloNTT f[], size_t n) {
//    _ntt<0>(f, n);
    if (n == 1)
        return;
    if (n > 1<<kDivLim) {
        throw std::logic_error("Length of input array for NTT is too long.");
    }

    _initialize_ntt();

    int h = bit_util::ctz32(n);
    for (int ph = 1; ph <= h; ph++) {
        int w = 1 << (ph - 1), p = 1 << (h - ph);
        ModuloNTT now = 1;
        for (int s = 0; s < w; s++) {
            int offset = s << (h - ph + 1);
            for (int i = 0; i < p; i++) {
                auto l = f[i + offset];
                auto r = f[i + offset + p] * now;
                f[i + offset]     = l + r;
                f[i + offset + p] = l - r;
            }
            now *= sum_e[bit_util::ctz32(~(unsigned int)(s))];
        }
    }
}

void intt(ModuloNTT f[], size_t n) {
//    _ntt<1>(f, n);
    if (n == 1)
        return;
    if (n > 1<<kDivLim) {
        throw std::logic_error("Length of input array for NTT is too long.");
    }

    _initialize_ntt();

    int h = bit_util::ctz32(n);
    for (int ph = h; ph >= 1; ph--) {
        int w = 1 << (ph - 1), p = 1 << (h - ph);
        ModuloNTT inow = 1;
        for (int s = 0; s < w; s++) {
            int offset = s << (h - ph + 1);
            for (int i = 0; i < p; i++) {
                auto l = f[i + offset];
                auto r = f[i + offset + p];
                f[i + offset]     = l + r;
                f[i + offset + p] = (l - r) * inow.val();
            }
            inow *= sum_ie[bit_util::ctz32(~(unsigned int)(s))];
        }
    }
}

void index_sum_convolution_for_xcheck(ModuloNTT f[], ModuloNTT Tg[], size_t n) {
    ntt(f, n);
    for (size_t i = 0; i < n; i++)
        f[i] *= Tg[i];
    intt(f, n);
    ModuloNTT n_inv = ModuloNTT(n).inv();
    for (size_t i = 0; i < n; i++)
        f[i] *= n_inv;
}

}

#endif //DALM_CONVOLUTION_H_
