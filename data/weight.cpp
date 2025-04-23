#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <thread>
#include <mutex>

const int k = 15;
const int numThreads = 1;
std::mutex mtx;
std::vector<unsigned int> buc[numThreads];

// 读取FASTA文件的主要函数
void read_fasta(const std::string& filename, std::streampos start, std::streampos end, int threadId) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + filename);
    }

    // 移动文件指针到指定的起始位置
    file.seekg(start);

    std::string line, last_line;

    while (file.tellg() < end && std::getline(file, line)) {

        // 移除行尾的\r（Windows换行符兼容处理）
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.empty()) continue;  // 跳过空行

        // 检测头部标记
        if (line[0] == '>') {
            // 新的一段，没有上一行
            last_line="";

        } else {
            std::string upline;
            
            // 过滤非字母字符并转为大写
            for (char c : line) {
                if (isalpha(c)) {
                    upline += toupper(c);
                }
            }

            std::string s;

            if (upline != ""){
                s = last_line + upline;
                for (int i = 0 ; i <= s.length() - k ; i++){
                    int hash=0;
                    int rehash=0;
                    for (int j = i ; j < i + k ; j++){
                        if (s[j] == 'A'){
                            hash = hash * 4 + 0; 
                            rehash = rehash | (3 << (j - i) * 2);
                        }else if (s[j] == 'C'){
                            hash = hash * 4 + 1; 
                            rehash = rehash | (2 << (j - i) * 2);
                        }else if (s[j] == 'G'){
                            hash = hash * 4 + 2; 
                            rehash = rehash | (1 << (j - i) * 2);
                        }else if (s[j] == 'T'){
                            hash = hash * 4 + 3; 
                            rehash = rehash | (0 << (j - i) * 2);
                        }else{
                            hash = -1;
                            i = j;
                            break;
                        }
                    }
                    if (hash != -1){
                        buc[threadId][std::min(hash , rehash)]++;
                    }
                }
            }

            last_line = "";
            for (int i = std::max (upline.length() - k + 1 , 0ull) ; i < upline.length() ; i++){
                last_line += upline[i]; 
            }
        }
    }

    return;
}

int main() {
    std::string outname = "hf_dmcx.txt";
    // std::cerr << "输出文件名：";
    // std::cin >> outname;

    freopen (outname.c_str(),"w",stdout);

    std::string name = "Drosophila_melanogaster_chromosome_X.fa";
    // std::cerr << "参考序列文件名：";
    // std::cin >> name;

    try {
        // 获取文件大小
        std::ifstream file(name, std::ios::ate);
        std::streampos fileSize = file.tellg();
        file.close();

        // 计算每个线程处理的文件部分
        std::vector<std::thread> threads;
        std::streampos partSize = fileSize / numThreads;
        std::streampos start = 0, end;

        for (int i = 0; i < numThreads; ++i) {
            if(i == numThreads - 1) end = fileSize;
            else{
                end = start + partSize;
                std::ifstream file(name);
                if (!file.is_open())
                    throw std::runtime_error("无法打开文件: " + name);
                file.seekg(end);
                std::string tmp;
                std::getline(file, tmp);
                end = file.tellg();
            }

            buc[i].resize((1 << k * 2) + 5);
    
            // 创建并启动线程
            threads.emplace_back(read_fasta, name, start, end, i);

            start = end;
        }

        // 等待所有线程完成
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }

        int num = 0;
        unsigned int ma = 0;
        std::vector<int> hf;
        for(int i = 0 ; i < (1 << k * 2) ; i++){
            unsigned int sum = 0;
            for(int j = 0 ; j < numThreads ; j++){
                sum += buc[j][i];
            }
            if (sum > 32){
                num++;
                hf.push_back(i);
                ma = std::max (ma, sum);
            }
        }

        std::cerr<< ma << std::endl;
        std::cout << num << std::endl;
        for(int i : hf){
            std::cout << i << ' ';
        }
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}