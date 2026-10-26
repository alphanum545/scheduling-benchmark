#!/usr/bin/env python3
"""Branch-only extension: preserve the original report, then audit BCR rows."""
import runpy
import subprocess
import sys
from pathlib import Path

here=Path(__file__).resolve().parent
try:
    runpy.run_path(str(here/'report_bcr_original.py'),run_name='__main__')
except SystemExit as error:
    if error.code not in (None,0):
        raise
name='latest'
for i,value in enumerate(sys.argv[1:],1):
    if value=='--name' and i+1<len(sys.argv):
        name=sys.argv[i+1]
    elif value.startswith('--name='):
        name=value.split('=',1)[1]
if name=='bcr-dev-v1':
    subprocess.run([sys.executable,str(here.parent/'experiments/bottleneck-repair/analyze.py'),
                    '--report',name,'--root',str(here.parent)],check=True)
