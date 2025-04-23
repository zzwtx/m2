#include <map>
#include <unordered_map>
#include <fstream>
#include <iostream>

static std::unordered_map<uint64_t,int> high_fre;

void hf_ini();

int hf_che(uint64_t x);