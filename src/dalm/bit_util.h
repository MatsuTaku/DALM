#ifndef BIT_UTIL_H_
#define BIT_UTIL_H_

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

namespace DALM::bit_util {

// ctz (Count trailing zeros) (***10000 = 4)
inline unsigned long long ctz64(unsigned long long x);
inline unsigned long long ctz(unsigned long long x) {
    return ctz64(x);
}
inline unsigned int ctz32(unsigned int x) {
#ifdef __BMI__
    return _tzcnt_u32(x);
#elif defined(__POPCNT__)
    return _mm_popcnt_u32((x & -x) - 1);
#else
    // Arrange of nlz algorithm in http://www.nminoru.jp/~nminoru/programming/bitcount.html#leading-0bits
    if (x == 0)
        return 32;
    unsigned int c = 0;
    if (x & 0x0000FFFF) {
        x &= 0x0000FFFF;
        c |= 16;
    }
    if (x & 0x00FF00FF) {
        x &= 0x00FF00FF;
        c |= 8;
    }
    if (x & 0x0F0F0F0F) {
        x &= 0x0F0F0F0F;
        c |= 4;
    }
    if (x & 0x33333333) {
        x &= 0x33333333;
        c |= 2;
    }
    if (x & 0x55555555) {
        c |= 1;
    }
    return c ^ 31;
#endif
}
inline unsigned long long ctz64(unsigned long long x) {
#ifdef __BMI__
    return _tzcnt_u64(x);
#elif defined(__POPCNT__)
    return _mm_popcnt_u64((x & -x) - 1);
#else
    // Arrange of nlz algorithm in http://www.nminoru.jp/~nminoru/programming/bitcount.html#leading-0bits
    if (x == 0)
        return 64;
    unsigned long long c = 0;
    if (x & 0x00000000FFFFFFFF) {
        x &= 0x00000000FFFFFFFF;
        c |= 32;
    }
    if (x & 0x0000FFFF0000FFFF) {
        x &= 0x0000FFFF0000FFFF;
        c |= 16;
    }
    if (x & 0x00FF00FF00FF00FF) {
        x &= 0x00FF00FF00FF00FF;
        c |= 8;
    }
    if (x & 0x0F0F0F0F0F0F0F0F) {
        x &= 0x0F0F0F0F0F0F0F0F;
        c |= 4;
    }
    if (x & 0x3333333333333333) {
        x &= 0x3333333333333333;
        c |= 2;
    }
    if (x & 0x5555555555555555) {
        c |= 1;
    }
    return c ^ 63;
#endif
}


// clz (Count leading zeros) (00001*** = 4)
inline unsigned long long clz64(unsigned long long x);
inline unsigned long long clz(unsigned long long x) {
    return clz64(x);
}
inline unsigned int clz32(unsigned int x) {
#ifdef __LZCNT__
    return _lzcnt_u32(x);
#elif defined(__POPCNT__)
    x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return _mm_popcnt_u32(~x);
#else
    // Reference to http://www.nminoru.jp/~nminoru/programming/bitcount.html#leading-0bits
    if (x == 0)
        return 32;
    unsigned int c = 0;
    if (x & 0xFFFF0000) {
        x &= 0xFFFF0000;
        c |= 16;
    }
    if (x & 0xFF00FF00) {
        x &= 0xFF00FF00;
        c |= 8;
    }
    if (x & 0xF0F0F0F0) {
        x &= 0xF0F0F0F0;
        c |= 4;
    }
    if (x & 0xCCCCCCCC) {
        x &= 0xCCCCCCCC;
        c |= 2;
    }
    if (x & 0xAAAAAAAA) {
        c |= 1;
    }
    return c ^ 31;
#endif
}
inline unsigned long long clz64(unsigned long long x) {
#ifdef __LZCNT__
	return _lzcnt_u64(x);
#elif defined(__POPCNT__)
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x |= x >> 32;
	return _mm_popcnt_u64(~x);
#else
	// Reference to http://www.nminoru.jp/~nminoru/programming/bitcount.html#leading-0bits
	if (x == 0)
		return 64;
	unsigned long long c = 0;
	if (x & 0xFFFFFFFF00000000) {
		x &= 0xFFFFFFFF00000000;
		c |= 32;
	}
	if (x & 0xFFFF0000FFFF0000) {
		x &= 0xFFFF0000FFFF0000;
		c |= 16;
	}
	if (x & 0xFF00FF00FF00FF00) {
		x &= 0xFF00FF00FF00FF00;
		c |= 8;
	}
	if (x & 0xF0F0F0F0F0F0F0F0) {
		x &= 0xF0F0F0F0F0F0F0F0;
		c |= 4;
	}
	if (x & 0xCCCCCCCCCCCCCCCC) {
		x &= 0xCCCCCCCCCCCCCCCC;
		c |= 2;
	}
	if (x & 0xAAAAAAAAAAAAAAAA) {
		c |= 1;
	}
	return c ^ 63;
#endif
}

}

#endif //BIT_UTIL_H_
