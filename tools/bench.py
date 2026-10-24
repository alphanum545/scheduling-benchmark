#!/usr/bin/env python3
"""Public benchmark branch-only entrypoint. All non-study modes delegate unchanged."""
import runpy
import sys
import gzip
import hashlib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
CODE=ROOT/'experiments/dsla-wfinstances-v15/code'
source=CODE/'bridge.py'
if not source.is_file():
    data=gzip.decompress((CODE/'bridge.py.gz').read_bytes())
    if hashlib.sha256(data).hexdigest()!='1a63d0d1f7fe47e709888366e42eb6cdf76b26866fe62032e58ba8615f467883':
        raise RuntimeError('Study bridge source checksum mismatch')
    source.write_bytes(data)
sys.path.insert(0,str(CODE))
import bridge
cmd=sys.argv[1] if len(sys.argv)>1 else ''
args=sys.argv[2:]
handled = (bridge.prep_plan(args) if cmd=='plan' else
           bridge.run_study(args) if cmd=='run' else
           bridge.merge_study(args) if cmd=='merge' else False)
if not handled:
    runpy.run_path(str(Path(__file__).with_name('bench_original.py')),run_name='__main__')
