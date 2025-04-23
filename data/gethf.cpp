#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <thread>
#include <mutex>

#define occ 32

const int k = 15, w = 300000;
const int mask = (1 << k * 2) - 1;
std::vector<unsigned int> buc;
std::string title, body;
std::unordered_map<char ,int> m={{'A', 0}, {'C', 1}, {'G', 2}, {'T', 3}};

// 读取FASTA文件的主要函数
void read_fasta(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + filename);
    }

    std::string line, last_line;

    int cnt = 0;

    while (std::getline(file, line)) {
        cnt++;

        // 移除行尾的\r（Windows换行符兼容处理）
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (!line.empty() && line.back() == '\n') {
            line.pop_back();
        }

        if (line.empty()) continue;  // 跳过空行

        // 检测头部标记
        if (line[0] == '>') {
            title=line;

        } else {
            std::string upline;
            
            // 过滤非字母字符并转为大写
            for (char c : line) {
                if (isalpha(c)) {
                    upline += toupper(c);
                }else{
                    // upline += '-';
                }
            }

            // if(upline.length() != 70){
            //     std::cerr << upline.length() << ' ' << cnt <<std::endl;
            //     assert(upline.length() == 70);
            // }

            body += upline;
        }
    }
    
    for (int i = 0 ; i <= body.length() - k ; i++){
        int hash = 0;
        int rehash = mask;
        for (int j = i ; j < i + k ; j++){
            if (body[j] == 'A'){
                hash = hash * 4 + 0; 
                // rehash = rehash | (3 << (j - i) * 2);
            }else if (body[j] == 'C'){
                hash = hash * 4 + 1; 
                // rehash = rehash | (2 << (j - i) * 2);
            }else if (body[j] == 'G'){
                hash = hash * 4 + 2; 
                // rehash = rehash | (1 << (j - i) * 2);
            }else if (body[j] == 'T'){
                hash = hash * 4 + 3; 
                // rehash = rehash | (0 << (j - i) * 2);
            }else{
                hash = -1;
                i = j;
                break;
            }
        }
        if (hash != -1){
            buc[std::min(hash , rehash)]++;
        }
    }

    return;
}

int main() {
    std::string outname = "X.fa";
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

        buc.resize((1 << k * 2) + 5);
        
        // 创建并启动线程
        read_fasta(name);

        std::cerr << m['G'] << std::endl;



        int fr = 0, ba = 0;
        for(int i = 0 ; i < k ; i++)
            fr = ((fr << 2) | m[body[i]]) & mask;
        for(int i = w - k ; i < w ; i++)
            ba = ((ba << 2) | m[body[i]]) & mask;
            
        unsigned long long sum = 0;
        int ha = fr;
        for(int i = 0 ; i <= w - k ; i++){
            sum += buc[ha] > occ;
            ha = ((ha << 2) | m[body[i + k]]) & mask;
        }

        int fr1 = fr, ba1 = ba;
        unsigned long long ma = 0, sum1 = sum;
        for(int i = 0 ; i < body.length() - w ; i++){
            ma = std::max (ma, sum1);
            if(i % (body.length() / 20) == body.length() / 40)
                std::cerr << sum1 << std::endl;

            if(body[i + k - 1] != 'N'){
                sum1 -= buc[fr1] > occ;
            }
            fr1 = ((fr1 << 2) | m[body[i + k]]) & mask;

            ba1 = ((ba1 << 2) | m[body[i + w]]) & mask;
            if(body[i + w] != 'N'){
                sum1 += buc[ba1] > occ;
            }
        }

        fr1 = fr, ba1 = ba;
        sum1 = sum;
        for(int i = 0 ; i < body.length() - w ; i++){

            if(ma == sum1){
                std::cout << title << std::endl;
                std::cerr << i << ' ' << 1.0 * i / 70.0 << std::endl;
                std::cerr << sum1 << std::endl;

                for(int j = i ; j < i + w ; j++){
                    std::cout<<body[j];
                }
                break;
            }

            if(body[i + k - 1] != 'N'){
                sum1 -= buc[fr1] > occ;
            }
            fr1 = ((fr1 << 2) | m[body[i + k]]) & mask;

            ba1 = ((ba1 << 2) | m[body[i + w]]) & mask;
            if(body[i + w] != 'N'){
                sum1 += buc[ba1] > occ;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}