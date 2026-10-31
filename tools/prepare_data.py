#!/usr/bin/env python3
"""prepare_data.py: build data/ from the committed archives and the seeded random generator.

  python3 tools/prepare_data.py

1. Verifies data/archives/*.zip against data/archives/SHA256SUMS.
2. Unpacks the six workflow families (FFT, GE, LA, Genome, LIGO, Montage).
3. Regenerates the nine random families R-ccr<C>-b<B> with tools/generate.py (seed 1).
Writes data/.assets.json (family -> source), which `bench.py index` records.
"""
import hashlib, json, os, re, shutil, subprocess, sys, tempfile, zipfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA = os.path.join(ROOT, "data")
ARCH = os.path.join(DATA, "archives")
NODES = re.compile(r"^\d+Nodes$")
RANDOM = ["--nodes", "100,300,500,1000", "--procs", "4,8,16,32", "--ccr", "0.1,1,10", "--beta", "0.1,0.5,1",
          "--count", "5", "--seed", "1", "--prefix", "R-", "--format", "D2"]


def families_in(folder):
    found = []
    for dirpath, dirnames, _ in os.walk(folder):
        if any(NODES.match(d) for d in dirnames):
            found.append(dirpath); dirnames[:] = []
    return found


def place(src_root, assets, label):
    for f in families_in(src_root):
        name = os.path.basename(f)
        target = os.path.join(DATA, name)
        shutil.rmtree(target, ignore_errors=True)
        shutil.move(f, target)
        for junk in ("run.sh", ".DS_Store"):
            if os.path.exists(os.path.join(target, junk)):
                os.remove(os.path.join(target, junk))
        assets[name] = label
        print(f"  {label} -> data/{name} ({sum(len(x) for _, _, x in os.walk(target))} files)")


def main():
    sums = dict(reversed(l.split()) for l in open(os.path.join(ARCH, "SHA256SUMS")) if l.strip())
    for name, digest in sorted(sums.items()):
        h = hashlib.sha256(open(os.path.join(ARCH, name), "rb").read()).hexdigest()
        if h != digest:
            sys.exit(f"checksum mismatch for {name}: {h} != {digest}")
    print(f"verified {len(sums)} archives")
    assets = {}
    for name in sorted(sums):
        with tempfile.TemporaryDirectory(dir=DATA) as tmp:
            with zipfile.ZipFile(os.path.join(ARCH, name)) as z:
                for m in z.infolist():
                    n = m.filename
                    if "__MACOSX" in n or os.path.basename(n).startswith("._") or n.endswith(".DS_Store"):
                        continue
                    z.extract(m, tmp)
            place(tmp, assets, name)
    with tempfile.TemporaryDirectory(dir=DATA) as tmp:
        subprocess.run([sys.executable, os.path.join(ROOT, "tools", "generate.py"), "--out", tmp, *RANDOM], check=True)
        place(tmp, assets, "gen:random-v1")
    json.dump(assets, open(os.path.join(DATA, ".assets.json"), "w"), indent=1)


if __name__ == "__main__":
    main()
