#include "../src/dalm/convolution.h"

#include <iostream>
#include <vector>

using namespace DALM::build_da_util::convolution;

using mint = ModuloNTT;
constexpr mint _f[16]       = {0,0,1,0,1,1,0,1,0,1,1,0,0,0,1,0};
constexpr mint _g[16]       = {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0};
constexpr int expected[] = {1,6,11,13};

int main() {
    std::cout << "Test convolution_find_base..." << std::endl;
    mint f[16], g[16];
    for (int i = 0; i < 16; i++) {
        f[i] = _f[i];
        g[i] = _g[i];
    }

    ntt(g, 16);
    index_sum_convolution_for_xcheck(f, g, 16);
    std::vector<int> bases;
    for (int i = 0; i < 16-3+1; i++) {
        if (f[3-1+i] == 0)
            bases.push_back(i);
    }
    bool ok = 4 == bases.size();
    for (int i = 0; i < 4; i++)
        ok &= expected[i] == bases[i];
    if (!ok) {
        std::cout << "Test failed" << std::endl;

        std::cout << "f: \t\t\t";
        for (int i = 0; i < 16; i++)
            std::cout << _f[i].val() << ' ';
        std::cout << std::endl;

        std::cout << "g: \t\t\t";
        for (int i = 0; i < 16; i++)
            std::cout << _g[i].val() << ' ';
        std::cout << std::endl;

        std::cout << "expected: \t";
        for (int i = 0; i < 4; i++)
            std::cout << expected[i] << ' ';
        std::cout << std::endl;

        std::cout << "result: \t";
        for (int i = 0; i < bases.size(); i++)
            std::cout << bases[i] << ' ';
        std::cout << std::endl;

        return 1;
    }
    std::cout << "OK" << std::endl;

    return 0;
}
