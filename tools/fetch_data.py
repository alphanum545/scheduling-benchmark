#!/usr/bin/env python3
"""fetch_data.py: download dataset archives from the GitHub release and unpack them into data/.

  python3 tools/fetch_data.py --all                 # every archive on the release (prepare step)
  python3 tools/fetch_data.py --families LIGO GE    # only the archives holding these families

Archives may be .zip, .tar.gz, .tgz or .tar.xz, made any way (Finder "Compress", zip, tar).
Each may hold one family folder or several; macOS junk (__MACOSX, ._ files, .DS_Store, run.sh)
is ignored. A family folder is any folder whose children are <N>Nodes folders. Data lands in
data/<Family>/<N>Nodes/<P>PROCESSORS/... and data/.assets.json records family -> archive.
Uses the GitHub CLI (gh); in Actions it is authenticated with the workflow token.

Generated families (tools/generated.json, asset names starting with "gen:") are not downloaded:
they are rebuilt with tools/generate.py from a fixed seed, so every machine gets identical files.
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


GENERATED = os.path.join(ROOT, "tools", "generated.json")


def generated_specs():
    if not os.path.exists(GENERATED):
        return {}
    return {k: v for k, v in json.load(open(GENERATED)).items() if k.startswith("gen:")}


def fetch(asset, mapping, only=None):
    tmp = tempfile.mkdtemp(prefix="fetch-", dir=os.path.join(ROOT, ".work"))
    try:
        ex = os.path.join(tmp, "x")
        if asset.startswith("gen:"):
            spec = generated_specs().get(asset)
            if spec is None:
                sys.exit(f"{asset}: not defined in tools/generated.json")
            cmd = [sys.executable, os.path.join(ROOT, "tools", "generate.py"), "--out", ex, *spec]
            if only:
                cmd += ["--only", ",".join(sorted(only))]
            subprocess.run(cmd, check=True)
        else:
            gh("release", "download", TAG, "--pattern", asset, "--dir", tmp, "--clobber")
            arch = os.path.join(tmp, asset)
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
    wanted = {}
    if a.all:
        assets = release_assets() + sorted(generated_specs())
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
        for f in a.families:
            wanted.setdefault(known[f], set()).add(f)
    if not assets:
        sys.exit(f"no archives found on release '{TAG}'")
    print(f"fetching {len(assets)} archive(s) from release '{TAG}' / generator")
    for asset in assets:
        # a generated asset holds many families; build only the ones this run needs
        fetch(asset, mapping, wanted.get(asset) if asset.startswith("gen:") else None)
    json.dump(mapping, open(mpath, "w"), indent=1)


if __name__ == "__main__":
    main()
