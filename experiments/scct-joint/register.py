#!/usr/bin/env python3
"""Register controlled SCCT candidates locally inside public Benchmark workflow.
Deliberately does not modify committed algorithms.toml or original source.
Run once per workflow job, after checkout and before bench/report commands.
"""
from pathlib import Path
p = Path("algorithms.toml")
s = p.read_text()
configs = {
    "scct_joint_base": [],
    "scct_joint": ["joint"],
    "scct_diamond": ["diamond"],
    "scct_joint_diamond": ["joint", "diamond"],
}
extra = ""
for name, args in configs.items():
    if f"[{name}]" not in s:
        tokens = ", ".join(f'"{x}"' for x in args)
        extra += f'\n[{name}]\nsource = "algorithms/makespan/scct_joint.cpp"\nargs = [{tokens}]\nenabled = false\n'
if extra:
    p.write_text(s.rstrip() + "\n" + extra)
print("SCCT experimental variants registered")
