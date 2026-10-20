#!/usr/bin/env python3
"""Rebuild the paper figures from the committed inputs.

  python3 paper/make_figures.py

worked_example/gantt_example.{png,pdf}   Gantt charts of HEFT, AEFT, PMMS and SLAS on example.d3
worked_example/gantt_example2.{png,pdf}  Gantt charts of HEFT, LEFT, SLAS and DSLA on example2.d3
                                          (the algorithms are compiled and run here)
worked_example/gantt_topcuoglu_variant.{png,pdf}, dag_topcuoglu_variant.{png,pdf}
                                         the classic 10-task graph with new costs: Gantt charts and the DAG
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


def dag(inst_name, out_name):
    """The DAG with its communication costs on the edges and the computation-cost table beside it."""
    d = os.path.join(HERE, "worked_example")
    tok = open(os.path.join(d, inst_name)).read().split()
    n, p = int(tok[0]), int(tok[1])
    W = [[int(x) for x in tok[2 + i * p: 2 + (i + 1) * p]] for i in range(n)]
    C = [[int(x) for x in tok[2 + n * p + i * n: 2 + n * p + (i + 1) * n]] for i in range(n)]
    edges = [(i, j, C[i][j]) for i in range(n) for j in range(n) if C[i][j] != -1]
    level = [0] * n
    for i, j, _ in sorted(edges, key=lambda e: e[0]):
        level[j] = max(level[j], level[i] + 1)
    rows = {}
    for t in range(n):
        rows.setdefault(level[t], []).append(t)
    pos = {}
    for lv, ts in rows.items():
        for k, t in enumerate(ts):
            pos[t] = ((k - (len(ts) - 1) / 2) * 1.25, -lv * 1.25)
    fig, (ax, at) = plt.subplots(1, 2, figsize=(7.2, 3.6), gridspec_kw={"width_ratios": [1.6, 1]})
    for i, j, c in edges:
        (x0, y0), (x1, y1) = pos[i], pos[j]
        ax.annotate("", xy=(x1, y1 + 0.27), xytext=(x0, y0 - 0.27),
                    arrowprops=dict(arrowstyle="-|>", color="#8a8984", lw=0.9, shrinkA=0, shrinkB=0))
        dx = abs(x1 - x0)
        f = 0.42 if level[i] == 0 else (0.5 if dx > 3 else 0.28 if dx >= 2 else 0.62 if level[i] == 1 else 0.5)
        ax.text(x0 + f * (x1 - x0), y0 + f * (y1 - y0), str(c), fontsize=7.5,
                ha="center", va="center", color="#0b0b0b",
                bbox=dict(boxstyle="round,pad=0.12", fc="white", ec="none"))
    for t, (x, y) in pos.items():
        ax.add_patch(plt.Circle((x, y), 0.27, fc="#2a78d6", ec="white", lw=1.5, zorder=3))
        ax.text(x, y, f"T{t + 1}", ha="center", va="center", color="white", fontsize=8.5, zorder=4)
    ax.set_aspect("equal"); ax.axis("off")
    ax.set_xlim(min(x for x, _ in pos.values()) - 0.6, max(x for x, _ in pos.values()) + 0.6)
    ax.set_ylim(min(y for _, y in pos.values()) - 0.5, 0.5)
    at.axis("off")
    cells = [[f"T{t + 1}"] + [str(w) for w in W[t]] for t in range(n)]
    tb = at.table(cellText=cells, colLabels=["Task"] + [f"P{k + 1}" for k in range(p)], loc="center",
                  cellLoc="center")
    tb.auto_set_font_size(False); tb.set_fontsize(8.5); tb.scale(1, 1.25)
    for (r, c), cell in tb.get_celld().items():
        cell.set_edgecolor("#e6e5e0")
        if r == 0:
            cell.set_facecolor("#f0efec")
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
          [("heft", [], "HEFT"), ("aeft", [], "AEFT"), ("pmms", [], "PMMS"),
           ("dsla", ["slas"], "SLAS (DSLA's rank-order schedule)")], focus=2)
    gantt("example2.d3", "gantt_example2",
          [("heft", [], "HEFT"), ("left", [], "LEFT"), ("dsla", ["slas"], "SLAS (DSLA's rank-order schedule)"),
           ("dsla", [], "DSLA (keeps its earliest-finish-order schedule)")], focus=3)
    gantt("topcuoglu_variant.d3", "gantt_topcuoglu_variant",
          [("heft", [], "HEFT"), ("left", [], "LEFT (best baseline here)"),
           ("dsla", ["slas"], "SLAS (DSLA's rank-order schedule)"),
           ("dsla", [], "DSLA (keeps its earliest-finish-order schedule)")], focus=1)
    dag("topcuoglu_variant.d3", "dag_topcuoglu_variant")
    scaling()
    print("figures written under paper/")
