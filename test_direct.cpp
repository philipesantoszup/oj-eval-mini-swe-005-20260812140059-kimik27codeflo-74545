#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include "qoi.h"

std::vector<uint8_t> encode_direct(uint32_t w, uint32_t h, uint8_t ch, const std::vector<uint8_t>& pixels) {
    std::stringstream in(std::string(pixels.begin(), pixels.end()), std::ios::binary | std::ios::in);
    std::stringstream out(std::ios::binary | std::ios::out);
    auto old_in = std::cin.rdbuf(in.rdbuf());
    auto old_out = std::cout.rdbuf(out.rdbuf());
    QoiEncode(w, h, ch, 0);
    std::cin.rdbuf(old_in);
    std::cout.rdbuf(old_out);
    std::string s = out.str();
    return std::vector<uint8_t>(s.begin(), s.end());
}

std::vector<uint8_t> decode_direct(const std::vector<uint8_t>& qoi, uint32_t& w, uint32_t& h, uint8_t& ch) {
    std::stringstream in(std::string(qoi.begin(), qoi.end()), std::ios::binary | std::ios::in);
    std::stringstream out(std::ios::binary | std::ios::out);
    auto old_in = std::cin.rdbuf(in.rdbuf());
    auto old_out = std::cout.rdbuf(out.rdbuf());
    uint8_t cs;
    bool ok = QoiDecode(w, h, ch, cs);
    std::cin.rdbuf(old_in);
    std::cout.rdbuf(old_out);
    if (!ok) {
        std::cerr << "decode failed\n";
        return {};
    }
    std::string s = out.str();
    return std::vector<uint8_t>(s.begin(), s.end());
}

bool roundtrip(uint32_t w, uint32_t h, uint8_t ch, const std::vector<uint8_t>& pixels, const char* name) {
    auto qoi = encode_direct(w, h, ch, pixels);
    uint32_t dw, dh;
    uint8_t dch;
    auto dec = decode_direct(qoi, dw, dh, dch);
    bool ok = (dec == pixels) && (dw == w) && (dh == h) && (dch == ch);
    std::cerr << name << ": " << (ok ? "OK" : "FAIL") << " qoi_size=" << qoi.size() << "\n";
    if (!ok) {
        std::cerr << "  decoded:";
        for (auto x : dec) std::cerr << " " << (int)x;
        std::cerr << "\n  expected:";
        for (auto x : pixels) std::cerr << " " << (int)x;
        std::cerr << "\n";
    }
    return ok;
}

int main() {
    bool all_ok = true;
    
    // Single pixel
    all_ok &= roundtrip(1, 1, 3, {255, 0, 0}, "single RGB");
    all_ok &= roundtrip(1, 1, 4, {255, 0, 0, 128}, "single RGBA");
    
    // Run of 2
    all_ok &= roundtrip(2, 1, 3, {255, 0, 0, 255, 0, 0}, "run2 RGB");
    all_ok &= roundtrip(2, 1, 4, {255, 0, 0, 128, 255, 0, 0, 128}, "run2 RGBA");
    
    // Run of 62 (boundary)
    {
        std::vector<uint8_t> p;
        for (int i = 0; i < 62*3; ++i) p.push_back(50);
        all_ok &= roundtrip(62, 1, 3, p, "run62 RGB");
    }
    
    // Run of 63 (split)
    {
        std::vector<uint8_t> p;
        for (int i = 0; i < 63*3; ++i) p.push_back(50);
        all_ok &= roundtrip(63, 1, 3, p, "run63 RGB");
    }
    
    // Run of 100
    {
        std::vector<uint8_t> p;
        for (int i = 0; i < 100*3; ++i) p.push_back(50);
        all_ok &= roundtrip(100, 1, 3, p, "run100 RGB");
    }
    
    // Diff
    all_ok &= roundtrip(2, 1, 3, {0, 0, 0, 2, 1, 0}, "diff RGB");
    
    // Luma
    all_ok &= roundtrip(2, 1, 3, {100, 100, 100, 120, 130, 110}, "luma RGB");
    
    // Index
    all_ok &= roundtrip(3, 1, 3, {10, 20, 30, 40, 50, 60, 10, 20, 30}, "index RGB");
    
    // RGB in RGBA stream
    all_ok &= roundtrip(2, 1, 4, {0, 0, 0, 255, 255, 255, 255, 255}, "rgba via rgb RGBA");
    
    // Mixed pattern
    {
        std::vector<uint8_t> p;
        for (int i = 0; i < 10; ++i) {
            p.push_back(i); p.push_back(i*2); p.push_back(i*3);
        }
        all_ok &= roundtrip(10, 1, 3, p, "mixed RGB");
    }
    
    std::cerr << "ALL: " << (all_ok ? "OK" : "FAIL") << "\n";
    return all_ok ? 0 : 1;
}
