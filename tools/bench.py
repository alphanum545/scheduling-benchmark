#!/usr/bin/env python3
"""Public benchmark branch-only entrypoint. All non-study modes delegate unchanged."""
import runpy
import sys
import gzip
import hashlib
import json
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
if cmd=='plan':
    handled=bridge.prep_plan(args)
    if handled:
        # Existing benchmark workflow sets matrix.include: fromJSON(needs.plan.outputs.matrix)
        # therefore the output must be a JSON array, not an object with include.
        output=Path(args[args.index('--github-output')+1])
        lines=output.read_text().splitlines()
        result=[]
        seen=False
        for line in lines:
            if line.startswith('matrix='):
                matrix=json.loads(line[7:])['include']
                if len(matrix)!=20 or {r['shard'] for r in matrix}!=set(range(20)):
                    raise RuntimeError('Invalid fixed twenty-way matrix')
                line='matrix='+json.dumps(matrix,separators=(',',':'))
                seen=True
            result.append(line)
        if not seen:raise RuntimeError('Planning output matrix missing')
        output.write_text('\n'.join(result)+'\n')
elif cmd=='run':
    handled=bridge.run_study(args)
elif cmd=='merge':
    if not list((ROOT/'.work').glob('shard-*.csv')):
        raise RuntimeError('No WfInstances shard artifacts; refusing empty merge')
    handled=bridge.merge_study(args)
else:
    handled=False
if not handled:
    runpy.run_path(str(Path(__file__).with_name('bench_original.py')),run_name='__main__')
