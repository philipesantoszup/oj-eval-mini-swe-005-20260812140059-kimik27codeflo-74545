import subprocess
import sys

def run_cmd(cmd, stdin=None):
    result = subprocess.run(cmd, shell=True, input=stdin, capture_output=True)
    return result.stdout, result.stderr, result.returncode

def ppm_to_raw(ppm_bytes):
    lines = ppm_bytes.decode().splitlines()
    assert lines[0] == 'P3'
    w, h = map(int, lines[1].split())
    maxval = int(lines[2])
    data = ' '.join(lines[3:]).split()
    return bytes(int(x) for x in data), w, h

def pam_to_raw(pam_bytes):
    lines = pam_bytes.decode().splitlines()
    assert lines[0] == 'P7'
    data = ' '.join(lines[7:]).split()  # after ENDHDR
    return bytes(int(x) for x in data)

def qoi_to_raw(qoi_bytes):
    # Use our decoder via conv -d -3/-4
    pass

# RGB test
with open('/workspace/data/005/sample/rgb/testcard.ppm', 'rb') as f:
    ppm = f.read()
raw_ppm, w, h = ppm_to_raw(ppm)
print(f"PPM raw size: {len(raw_ppm)}, dims: {w}x{h}")

# Encode to QOI
_, err, rc = run_cmd('cd /workspace/problem_005 && ./conv -e -3 -o', stdin=ppm)
print(f"Encode stderr: {err.decode()[:200]}")
qoi, err, rc = run_cmd('cd /workspace/problem_005 && ./conv -e -3 -o > /tmp/enc2.qoi', stdin=ppm)
with open('/tmp/enc2.qoi', 'rb') as f:
    qoi = f.read()
print(f"QOI size: {len(qoi)}")

# Decode QOI to PPM
ppm2, err, rc = run_cmd('cd /workspace/problem_005 && ./conv -d -3 -o < /tmp/enc2.qoi')
raw_ppm2, w2, h2 = ppm_to_raw(ppm2)
print(f"Decoded PPM raw size: {len(raw_ppm2)}, dims: {w2}x{h2}")

# Find first difference
for i in range(min(len(raw_ppm), len(raw_ppm2))):
    if raw_ppm[i] != raw_ppm2[i]:
        print(f"First diff at byte {i}: orig={raw_ppm[i]} dec={raw_ppm2[i]}")
        print(f"Context orig: {list(raw_ppm[max(0,i-12):i+12])}")
        print(f"Context dec:  {list(raw_ppm2[max(0,i-12):i+12])}")
        break
else:
    print("No diff found in overlapping range")
