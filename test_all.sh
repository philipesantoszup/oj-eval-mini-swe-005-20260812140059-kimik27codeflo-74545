#!/bin/bash
set -e
cd /workspace/problem_005
g++-13 -O2 -std=c++17 -o conv main.cpp

echo "=== Testing decode of reference QOI files ==="
for f in /workspace/data/005/sample/rgb/*.qoi; do
  name=$(basename "$f" .qoi)
  ppm="/workspace/data/005/sample/rgb/$name.ppm"
  if [ -f "$ppm" ]; then
    ./conv -d -3 -o < "$f" > /tmp/dec_test.ppm
    python3 -c "
import sys
with open('$ppm','rb') as f: lines1 = f.read().decode().splitlines()
with open('/tmp/dec_test.ppm','rb') as f: lines2 = f.read().decode().splitlines()
data1 = ' '.join(lines1[3:]).split()
data2 = ' '.join(lines2[3:]).split()
ok = data1 == data2
print('RGB $name decode:', 'OK' if ok else 'FAIL')
sys.exit(0 if ok else 1)
"
  fi
done

for f in /workspace/data/005/sample/rgba/*.qoi; do
  name=$(basename "$f" .qoi)
  pam="/workspace/data/005/sample/rgba/$name.pam"
  if [ -f "$pam" ]; then
    ./conv -d -4 -o < "$f" > /tmp/dec_test.pam
    python3 -c "
import sys
with open('$pam','rb') as f: lines1 = f.read().decode().splitlines()
with open('/tmp/dec_test.pam','rb') as f: lines2 = f.read().decode().splitlines()
data1 = ' '.join(lines1[7:]).split()
data2 = ' '.join(lines2[7:]).split()
ok = data1 == data2
print('RGBA $name decode:', 'OK' if ok else 'FAIL')
sys.exit(0 if ok else 1)
"
  fi
done

echo "=== Testing encode->decode roundtrip ==="
for f in /workspace/data/005/sample/rgb/*.ppm; do
  name=$(basename "$f" .ppm)
  ./conv -e -3 -o < "$f" > /tmp/rt.qoi
  ./conv -d -3 -o < /tmp/rt.qoi > /tmp/rt.ppm
  python3 -c "
import sys
with open('$f','rb') as f: lines1 = f.read().decode().splitlines()
with open('/tmp/rt.ppm','rb') as f: lines2 = f.read().decode().splitlines()
data1 = ' '.join(lines1[3:]).split()
data2 = ' '.join(lines2[3:]).split()
ok = data1 == data2
print('RGB $name roundtrip:', 'OK' if ok else 'FAIL')
sys.exit(0 if ok else 1)
"
done

for f in /workspace/data/005/sample/rgba/*.pam; do
  name=$(basename "$f" .pam)
  ./conv -e -4 -o < "$f" > /tmp/rt.qoi
  ./conv -d -4 -o < /tmp/rt.qoi > /tmp/rt.pam
  python3 -c "
import sys
with open('$f','rb') as f: lines1 = f.read().decode().splitlines()
with open('/tmp/rt.pam','rb') as f: lines2 = f.read().decode().splitlines()
data1 = ' '.join(lines1[7:]).split()
data2 = ' '.join(lines2[7:]).split()
ok = data1 == data2
print('RGBA $name roundtrip:', 'OK' if ok else 'FAIL')
sys.exit(0 if ok else 1)
"
done
