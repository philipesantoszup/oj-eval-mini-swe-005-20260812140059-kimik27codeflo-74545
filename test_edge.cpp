#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include "qoi.h"

bool decode_raw(const std::vector<uint8_t>& qoi, uint32_t& w, uint32_t& h, uint8_t& ch, uint8_t& cs) {
    std::stringstream in(std::string(qoi.begin(), qoi.end()), std::ios::binary | std::ios::in);
    auto old_in = std::cin.rdbuf(in.rdbuf());
    bool ok = QoiDecode(w, h, ch, cs);
    std::cin.rdbuf(old_in);
    return ok;
}

std::vector<uint8_t> qoi_header(uint32_t w, uint32_t h, uint8_t ch, uint8_t cs) {
    return {'q','o','i','f',
            static_cast<uint8_t>(w>>24), static_cast<uint8_t>(w>>16), static_cast<uint8_t>(w>>8), static_cast<uint8_t>(w),
            static_cast<uint8_t>(h>>24), static_cast<uint8_t>(h>>16), static_cast<uint8_t>(h>>8), static_cast<uint8_t>(h),
            ch, cs};
}

std::vector<uint8_t> padding = {0,0,0,0,0,0,0,1};

int main() {
    int fails = 0;
    uint32_t w, h;
    uint8_t ch, cs;

    // Valid minimal RGB
    {
        auto qoi = qoi_header(1,1,3,0);
        qoi.push_back(0xfe); qoi.push_back(10); qoi.push_back(20); qoi.push_back(30);
        qoi.insert(qoi.end(), padding.begin(), padding.end());
        bool ok = decode_raw(qoi, w, h, ch, cs);
        std::cerr << "valid RGB 1x1: " << (ok ? "OK" : "FAIL") << "\n";
        if (!ok) ++fails;
    }

    // Invalid magic
    {
        auto qoi = std::vector<uint8_t>{'q','o','i','g',0,0,0,1,0,0,0,1,3,0,0xfe,10,20,30};
        qoi.insert(qoi.end(), padding.begin(), padding.end());
        bool ok = decode_raw(qoi, w, h, ch, cs);
        std::cerr << "invalid magic: " << (ok ? "FAIL (accepted)" : "OK (rejected)") << "\n";
        if (ok) ++fails;
    }

    // Zero width
    {
        auto qoi = qoi_header(0,1,3,0);
        qoi.insert(qoi.end(), padding.begin(), padding.end());
        bool ok = decode_raw(qoi, w, h, ch, cs);
        std::cerr << "zero width: " << (ok ? "FAIL (accepted)" : "OK (rejected)") << "\n";
        if (ok) ++fails;
    }

    // Invalid channels
    {
        auto qoi = qoi_header(1,1,2,0);
        qoi.push_back(0xfe); qoi.push_back(10); qoi.push_back(20); qoi.push_back(30);
        qoi.insert(qoi.end(), padding.begin(), padding.end());
        bool ok = decode_raw(qoi, w, h, ch, cs);
        std::cerr << "invalid channels: " << (ok ? "FAIL (accepted)" : "OK (rejected)") << "\n";
        if (ok) ++fails;
    }

    // Invalid colorspace
    {
        auto qoi = qoi_header(1,1,3,2);
        qoi.push_back(0xfe); qoi.push_back(10); qoi.push_back(20); qoi.push_back(30);
        qoi.insert(qoi.end(), padding.begin(), padding.end());
        bool ok = decode_raw(qoi, w, h, ch, cs);
        std::cerr << "invalid colorspace: " << (ok ? "FAIL (accepted)" : "OK (rejected)") << "\n";
        if (ok) ++fails;
    }

    // Bad padding
    {
        auto qoi = qoi_header(1,1,3,0);
        qoi.push_back(0xfe); qoi.push_back(10); qoi.push_back(20); qoi.push_back(30);
        qoi.insert(qoi.end(), padding.begin(), padding.end());
        qoi[qoi.size()-1] = 2;
        bool ok = decode_raw(qoi, w, h, ch, cs);
        std::cerr << "bad padding: " << (ok ? "FAIL (accepted)" : "OK (rejected)") << "\n";
        if (ok) ++fails;
    }

    // Truncated stream
    {
        auto qoi = qoi_header(2,1,3,0);
        qoi.push_back(0xfe); qoi.push_back(10); qoi.push_back(20); qoi.push_back(30); // only 1 pixel
        qoi.insert(qoi.end(), padding.begin(), padding.end());
        bool ok = decode_raw(qoi, w, h, ch, cs);
        std::cerr << "truncated stream: " << (ok ? "FAIL (accepted)" : "OK (rejected)") << "\n";
        if (ok) ++fails;
    }

    // Run too long (run value 62 = 63 pixels, but only 2 expected)
    {
        auto qoi = qoi_header(2,1,3,0);
        qoi.push_back(0xfd); // RUN of 62? 0xfd & 0x3f = 61, run length = 62
        qoi.push_back(0xfe); qoi.push_back(10); qoi.push_back(20); qoi.push_back(30);
        qoi.insert(qoi.end(), padding.begin(), padding.end());
        bool ok = decode_raw(qoi, w, h, ch, cs);
        std::cerr << "run too long: " << (ok ? "FAIL (accepted)" : "OK (rejected)") << "\n";
        if (ok) ++fails;
    }

    std::cerr << "Total failures: " << fails << "\n";
    return fails;
}
