#!/usr/bin/env python3
"""fetch_data.py: download dataset archives from the GitHub release and unpack them into data/.

  python3 tools/fetch_data.py --all                 # every archive on the release (prepare step)
  python3 tools/fetch_data.py --families LIGO GE    # only the archives holding these families

Archives may be .zip, .tar.gz, .tgz or .tar.xz, made any way (Finder "Compress", zip, tar).
Each may hold one family folder or several; macOS junk (__MACOSX, ._ files, .DS_Store, run.sh)
is ignored. A family folder is any folder whose children are <N>Nodes folders. Data lands in
data/<Family>/<N>Nodes/<P>PROCESSORS/... and data/.assets.json records family -> archive.
Uses the GitHub CLI (gh); in Actions it is authenticated with the workflow token.
"""
import argparse, csv, json, os, re, shutil, subprocess, sys, tarfile, tempfile, zipfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA = os.path.join(ROOT, "data")
TAG = os.environ.get("BENCH_DATA_TAG", "data")
NODES = re.compile(r"^\d+Nodes$")
ARCH = (".zip", ".tar.gz", ".tgz", ".tar.xz", ".txz", ".tar")


def gh(*args):
    r = subprocess.run(["gh", *args], capture_output=True, text=True)
    if r.returncode:
        sys.exit(f"gh {' '.join(args)} failed:\n{r.stderr}")
    return r.stdout


def release_assets():
    out = json.loads(gh("release", "view", TAG, "--json", "assets"))
    return [a["name"] for a in out["assets"] if a["name"].lower().endswith(ARCH)]


def extract(archive, dest):
    if archive.lower().endswith(".zip"):
        with zipfile.ZipFile(archive) as z:
            for m in z.infolist():
                n = m.filename
                if "__MACOSX" in n or os.path.basename(n).startswith("._") or n.endswith(".DS_Store"):
                    continue
                z.extract(m, dest)
    else:
        with tarfile.open(archive) as t:
            members = [m for m in t.getmembers()
                       if "__MACOSX" not in m.name and not os.path.basename(m.name).startswith("._")]
            t.extractall(dest, members=members, filter="data")


def families_in(folder):
    """Folders whose immediate children include <N>Nodes directories."""
    found = []
    for dirpath, dirnames, _ in os.walk(folder):
        if any(NODES.match(d) for d in dirnames):
            found.append(dirpath)
            dirnames[:] = []  # do not descend further
    return found


def fetch(asset, mapping):
    tmp = tempfile.mkdtemp(prefix="fetch-", dir=os.path.join(ROOT, ".work"))
    try:
        gh("release", "download", TAG, "--pattern", asset, "--dir", tmp, "--clobber")
        arch = os.path.join(tmp, asset)
        ex = os.path.join(tmp, "x")
        extract(arch, ex)
        os.remove(arch)
        fams = families_in(ex)
        if not fams:
            print(f"  ! {asset}: no <N>Nodes folders found inside; skipped")
        for f in fams:
            # an archive of the Nodes folders themselves (no family folder) takes the archive name
            name = os.path.basename(f) if f != ex else re.sub(r"(\.tar)?\.[a-z]+$", "", asset)
            target = os.path.join(DATA, name)
            if os.path.exists(target):
                shutil.rmtree(target)
            shutil.move(f, target)
            for junk in ("run.sh", ".DS_Store"):
                if os.path.exists(os.path.join(target, junk)):
                    os.remove(os.path.join(target, junk))
            mapping[name] = asset
            n = sum(len(fs) for _, _, fs in os.walk(target))
            print(f"  {asset} -> data/{name} ({n} files)")
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--all", action="store_true")
    ap.add_argument("--families", nargs="*", default=[])
    a = ap.parse_args()
    os.makedirs(DATA, exist_ok=True); os.makedirs(os.path.join(ROOT, ".work"), exist_ok=True)
    mpath = os.path.join(DATA, ".assets.json")
    mapping = json.load(open(mpath)) if os.path.exists(mpath) else {}
    if a.all:
        assets = release_assets()
    else:
        index = os.path.join(ROOT, "data_index.csv")
        known = {}
        if os.path.exists(index):
            for r in csv.DictReader(open(index)):
                known[r["family"]] = r.get("asset") or ""
        missing = [f for f in a.families if not known.get(f)]
        if missing:
            sys.exit(f"no archive recorded for {missing}; run the Prepare data workflow first")
        assets = sorted({known[f] for f in a.families})
    if not assets:
        sys.exit(f"no archives found on release '{TAG}'")
    print(f"downloading {len(assets)} archive(s) from release '{TAG}'")
    for asset in assets:
        fetch(asset, mapping)
    json.dump(mapping, open(mpath, "w"), indent=1)


if __name__ == "__main__":
    main()
