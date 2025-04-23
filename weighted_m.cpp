#include "weighted_m.hpp"

void hf_ini() {
    std::ifstream fin;
    std::string hff;
    std::cerr << "High-frequency seed record filename:";
    std::cin >> hff;
    fin.open(hff, std::ios::in);

    if (!fin.is_open()) {
        std::cerr << "File Not Found" << std::endl;
        return;
    }

    int n = 0;
    fin >> n;

    for(int i=1 ; i<=n ; i++) {
        uint64_t hfm;
        fin >> hfm;
        high_fre[hfm] = 1;
    }

    return;
}

int hf_che(uint64_t x) {
    return high_fre.count(x);
}
