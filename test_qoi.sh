#!/bin/bash
set -e
cd /workspace/problem_005
g++-13 -O2 -std=c++17 -o conv main.cpp

# Test encode: ppm -> qoi, then decode back to ppm
echo "Testing RGB encode/decode roundtrip..."
./conv -e -3 -o < /workspace/data/005/sample/rgb/testcard.ppm > /tmp/testcard_enc.qoi
./conv -d -3 -o < /tmp/testcard_enc.qoi > /tmp/testcard_dec.ppm
diff /workspace/data/005/sample/rgb/testcard.ppm /tmp/testcard_dec.ppm && echo "RGB roundtrip OK" || echo "RGB roundtrip FAIL"

echo "Testing RGBA encode/decode roundtrip..."
./conv -e -4 -o < /workspace/data/005/sample/rgba/testcard.pam > /tmp/testcard_enc_rgba.qoi
./conv -d -4 -o < /tmp/testcard_enc_rgba.qoi > /tmp/testcard_dec.pam
diff /workspace/data/005/sample/rgba/testcard.pam /tmp/testcard_dec.pam && echo "RGBA roundtrip OK" || echo "RGBA roundtrip FAIL"

# Compare encoded QOI with reference
echo "Comparing QOI outputs with references..."
./conv -e -3 -o < /workspace/data/005/sample/rgb/testcard.ppm > /tmp/ref_testcard.qoi
cmp /workspace/data/005/sample/rgb/testcard.qoi /tmp/ref_testcard.qoi && echo "RGB QOI matches reference" || echo "RGB QOI differs from reference"

./conv -e -4 -o < /workspace/data/005/sample/rgba/testcard.pam > /tmp/ref_testcard_rgba.qoi
cmp /workspace/data/005/sample/rgba/testcard.qoi /tmp/ref_testcard_rgba.qoi && echo "RGBA QOI matches reference" || echo "RGBA QOI differs from reference"
