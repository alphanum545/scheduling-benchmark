#!/usr/bin/env python3
"""Rebuild the paper figures from the committed inputs.

  python3 paper/make_figures.py

worked_example/gantt_example.{png,pdf}   Gantt charts of HEFT, AEFT, OPPTS and SLAS on example.d3
worked_example/gantt_example2.{png,pdf}  Gantt charts of HEFT, LEFT-D, SLAS and DSLA on example2.d3
                                          (the algorithms are compiled and run here)
scaling/bla_heft_scaling.{png,pdf}       SLAS (= BLA-HEFT) scheduling time against v and p
                                         (from scaling/bla_heft_internal_time.csv)
Needs g++, matplotlib, pandas.
"""
import os, subprocess, tempfile
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from matplotlib.ticker import FixedFormatter, FixedLocator, NullFormatter

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
plt.rcParams.update({"font.size": 9, "axes.spines.top": False, "axes.spines.right": False,
                     "axes.edgecolor": "#8a8984", "xtick.color": "#52514e", "ytick.color": "#52514e",
                     "axes.grid": True, "grid.color": "#e6e5e0", "grid.linewidth": 0.6})


def schedule(src, args, inst, tmp):
    exe = os.path.join(tmp, src)
    if not os.path.exists(exe):
        subprocess.run(["g++", "-O2", "-std=c++17", "-w", "-o", exe,
                        os.path.join(ROOT, "algorithms", "makespan", f"{src}.cpp")], check=True)
    out = subprocess.run([exe, *args], stdin=open(inst), capture_output=True, text=True, check=True).stdout
    rows = [l.split() for l in out.split("\n")[2:] if l.strip()]
    return [tuple(int(x) for x in r[:4]) for r in rows]  # task, start, finish, processor


def gantt(inst_name, out_name, algs, focus):
    """algs: (source, args, label); focus: task drawn in the accent colour."""
    d = os.path.join(HERE, "worked_example")
    inst = os.path.join(d, inst_name)
    with tempfile.TemporaryDirectory() as tmp:
        scheds = [schedule(src, args, inst, tmp) for src, args, _ in algs]
    p = max(r[3] for s in scheds for r in s)
    fig, axes = plt.subplots(len(algs), 1, figsize=(6.6, 1.25 * len(algs) + 0.5), sharex=True)
    for ax, s, (_, _, name) in zip(axes, scheds, algs):
        ms = max(f for _, _, f, _ in s)
        for t, st, ft, pr in s:
            if ft == st:
                continue
            col = "#2a78d6" if t == focus else "#b4b3ad"
            ax.broken_barh([(st, ft - st)], (pr - 0.36, 0.72), facecolor=col, edgecolor="white", linewidth=2)
            ax.text((st + ft) / 2, pr, f"T{t}", ha="center", va="center", fontsize=8,
                    color="white" if t == focus else "#0b0b0b")
        ax.axvline(ms, color="#0b0b0b", lw=1, ls="--")
        ax.text(ms + 0.6, p + 0.35, f"makespan {ms}", fontsize=8, va="center")
        ax.set_yticks(range(1, p + 1), [f"P{k}" for k in range(1, p + 1)])
        ax.set_ylim(0.4, p + 0.75); ax.grid(axis="y", visible=False); ax.set_axisbelow(True)
        ax.set_title(name, loc="left", fontsize=9.5)
    axes[-1].set_xlabel("Time")
    axes[-1].set_xlim(0, max(max(f for _, _, f, _ in s) for s in scheds) + 10)
    fig.tight_layout()
    for ext in ("png", "pdf"):
        fig.savefig(os.path.join(d, f"{out_name}.{ext}"), bbox_inches="tight", dpi=200)
    plt.close(fig)


def scaling():
    d = os.path.join(HERE, "scaling")
    t = pd.read_csv(os.path.join(d, "bla_heft_internal_time.csv"))
    a = t[t.set == 1].sort_values("v")
    b = pd.concat([t[t.set == 2], a[a.v == 1000]]).sort_values("p")
    fig, ax = plt.subplots(1, 2, figsize=(7, 2.8))
    for axi, (x, y, lab) in zip(ax, [(a.v.values, a.sec.values * 1000, "Number of tasks v (p = 16)"),
                                     (b.p.values, b.sec.values * 1000, "Number of processors p (v = 1000)")]):
        axi.loglog(x, y, "o-", color="#2a78d6", lw=2, ms=5, label="SLAS")
        axi.loglog(x, y[0] * (x / x[0]) ** 2, "--", color="#8a8984", lw=1, label="quadratic growth (reference)")
        axi.xaxis.set_major_locator(FixedLocator(x))
        axi.xaxis.set_major_formatter(FixedFormatter([str(int(v)) for v in x]))
        axi.xaxis.set_minor_formatter(NullFormatter())
        axi.set_xlabel(lab); axi.set_ylabel("Scheduling time (ms)"); axi.legend(frameon=False, fontsize=8)
    fig.tight_layout()
    for ext in ("png", "pdf"):
        fig.savefig(os.path.join(d, f"bla_heft_scaling.{ext}"), bbox_inches="tight", dpi=200)
    plt.close(fig)


if __name__ == "__main__":
    gantt("example.d3", "gantt_example",
          [("heft", [], "HEFT"), ("aeft", [], "AEFT"), ("oppts", [], "OPPTS"),
           ("dsla", ["slas"], "SLAS (DSLA's rank-order schedule)")], focus=2)
    gantt("example2.d3", "gantt_example2",
          [("heft", [], "HEFT"), ("left", ["dup"], "LEFT-D"), ("dsla", ["slas"], "SLAS (DSLA's rank-order schedule)"),
           ("dsla", [], "DSLA (keeps its earliest-finish-order schedule)")], focus=3)
    scaling()
    print("figures written under paper/")
