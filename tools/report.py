#!/usr/bin/env python3
"""report.py: turn results/store.csv into tables and figures.

  python3 tools/report.py --algos oppts,aeft,peft,heft --highlight oppts --name compare-v1

Writes results/reports/<name>/
  results.csv     one row per run (current version of each algorithm only)
  summary.xlsx    validity, averages by nodes / processors, pairwise better-equal-worse, gains, runtime
  summary.md      the key tables as text
  figures/        makespan, SLR, speedup, efficiency charts (PNG + PDF), one panel per family

Averages use only valid runs on instances where every selected algorithm produced a valid schedule,
so every algorithm is compared on exactly the same set of instances.
Needs pandas, matplotlib, openpyxl.
"""
import argparse, os, sys
import pandas as pd

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from stats import significance_tables
from bench import ROOT, STORE, load_algos, pick_algos, csvset, split_of  # noqa: E402

# categorical palette, fixed order (validated set); slots past 8 fall back to grays with marker/dash encoding
PALETTE = ["#2a78d6", "#eb6834", "#1baf7a", "#eda100", "#e87ba4", "#008300", "#6250d6", "#e34948"]
GRAYS = ["#52514e", "#8a8984", "#b4b3ad"]
MARKERS = ["o", "s", "^", "D", "v", "P", "X", "<", ">", "h", "*", "p"]
DASHES = ["-", "--", "-.", ":"]
VALID = {"ok", "unchecked"}


def load(a):
    algos = load_algos()
    names = pick_algos(a.algos, algos)
    if a.highlight and a.highlight in names:
        names = [a.highlight] + [n for n in names if n != a.highlight]
    df = pd.read_csv(STORE)
    cur = {n: algos[n]["hash"] for n in names}
    df = df[df.apply(lambda r: cur.get(r.algo) == r.algo_hash, axis=1)].copy()
    for col, flt, cast in (("dataset", a.datasets, str), ("family", a.families, str),
                           ("nodes", a.nodes, int), ("p", a.procs, int)):
        s = csvset(flt, cast)
        if s:
            df = df[df[col].isin(s)]
    if a.split != "all":
        df = df[df.path.map(split_of) == a.split]
    missing = [n for n in names if n not in set(df.algo)]
    if missing:
        print(f"warning: no results for {missing} (current version); leaving them out of this report")
        names = [n for n in names if n not in missing]
    if not names:
        sys.exit("no results to report")
    if a.highlight and a.highlight not in names:
        print(f"warning: highlight '{a.highlight}' has no results in this selection; no gain chart")
        a.highlight = None
    return df, names


def comparable(df, names):
    """Valid runs restricted to instances where all algorithms are valid."""
    v = df[df.status.isin(VALID)]
    ok = v.groupby("path").algo.nunique()
    keep = ok[ok == len(names)].index
    return v[v.path.isin(keep)]


def pivot(c, by, metric, names):
    t = c.pivot_table(index=["dataset", "family", by], columns="algo", values=metric, aggfunc="mean")
    return t[[n for n in names if n in t.columns]].round(4)


def pairwise(c, names):
    m = c.pivot_table(index="path", columns="algo", values="makespan")
    rows = []
    for x in names:
        for y in names:
            if x == y:
                continue
            d = m[x] - m[y]
            rows.append(dict(algorithm=x, versus=y, better_pct=round(100 * (d < 0).mean(), 2),
                             equal_pct=round(100 * (d == 0).mean(), 2), worse_pct=round(100 * (d > 0).mean(), 2),
                             avg_gain_pct=round(100 * ((m[y] - m[x]) / m[y]).mean(), 3)))
    return pd.DataFrame(rows)


def style(names):
    s = {}
    for i, n in enumerate(names):
        color = PALETTE[i] if i < len(PALETTE) else GRAYS[(i - len(PALETTE)) % len(GRAYS)]
        s[n] = dict(color=color, marker=MARKERS[i % len(MARKERS)], ls=DASHES[(i // len(PALETTE)) % len(DASHES)],
                    lw=2.4 if i == 0 else 1.6, zorder=10 if i == 0 else 3)
    return s


def figures(c, names, outdir, highlight):
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    plt.rcParams.update({"font.size": 9, "axes.spines.top": False, "axes.spines.right": False,
                         "axes.edgecolor": "#8a8984", "axes.labelcolor": "#0b0b0b", "xtick.color": "#52514e",
                         "ytick.color": "#52514e", "axes.grid": True, "grid.color": "#e6e5e0",
                         "grid.linewidth": 0.6, "legend.frameon": False, "figure.dpi": 150})
    st = style(names)
    fams = list(c[["dataset", "family"]].drop_duplicates().itertuples(index=False, name=None))
    specs = [("makespan", "nodes", "Average makespan", "Number of tasks"),
             ("makespan", "p", "Average makespan", "Number of processors"),
             ("slr", "nodes", "Average SLR", "Number of tasks"),
             ("slr", "p", "Average SLR", "Number of processors"),
             ("speedup", "p", "Average speedup", "Number of processors"),
             ("efficiency", "p", "Average efficiency", "Number of processors")]
    os.makedirs(outdir, exist_ok=True)
    made = []
    for metric, by, ylab, xlab in specs:
        k = len(fams); cols = min(3, k); rows = (k + cols - 1) // cols
        fig, axes = plt.subplots(rows, cols, figsize=(3.6 * cols, 2.9 * rows + 0.6), squeeze=False)
        for ax, (ds, fam) in zip(axes.flat, fams):
            sub = c[(c.dataset == ds) & (c.family == fam)]
            t = sub.groupby([by, "algo"])[metric].mean().unstack()
            xs = list(t.index)
            for n in reversed(names):  # draw highlight last (on top)
                if n not in t:
                    continue
                s = st[n]
                ax.plot(range(len(xs)), t[n].values, color=s["color"], marker=s["marker"], ls=s["ls"],
                        lw=s["lw"], ms=4.5, zorder=s["zorder"], label=n.upper())
            ax.set_xticks(range(len(xs)), [str(x) for x in xs])
            ax.set_title(f"{fam} ({ds})", fontsize=9.5, color="#0b0b0b", loc="left")
            ax.set_xlabel(xlab); ax.set_ylabel(ylab)
        for ax in list(axes.flat)[k:]:
            ax.axis("off")
        h, l = axes.flat[0].get_legend_handles_labels()
        order = sorted(range(len(l)), key=lambda i: names.index(l[i].lower()))
        ncol = min(len(names), 6); lrows = -(-len(names) // ncol)
        fig.tight_layout()
        fig_h = fig.get_size_inches()[1]
        fig.subplots_adjust(top=1 - (0.32 * lrows + 0.12) / fig_h)
        fig.legend([h[i] for i in order], [l[i] for i in order], loc="upper center", ncol=ncol,
                   fontsize=8.5, bbox_to_anchor=(0.5, 1.0), handlelength=2.6)
        base = os.path.join(outdir, f"{metric}_vs_{'tasks' if by == 'nodes' else 'processors'}")
        fig.savefig(base + ".png", bbox_inches="tight"); fig.savefig(base + ".pdf", bbox_inches="tight")
        plt.close(fig)
        made.append(os.path.basename(base))
    return made


def gain_figure(pw, highlight, names, outdir):
    """Average % makespan reduction of the highlighted algorithm over each other algorithm."""
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    t = pw[pw.algorithm == highlight].set_index("versus").reindex([n for n in names if n != highlight])
    t = t.sort_values("avg_gain_pct")
    fig, ax = plt.subplots(figsize=(6.4, 0.42 * len(t) + 1.1))
    y = range(len(t))
    ax.barh(y, t.avg_gain_pct, height=0.6, color=PALETTE[0])
    ax.axvline(0, color="#52514e", lw=0.8)
    span = max(1e-9, t.avg_gain_pct.abs().max())
    for yi, (v, b, w) in zip(y, zip(t.avg_gain_pct, t.better_pct, t.worse_pct)):
        ax.text(v + (0.02 if v >= 0 else -0.02) * span, yi, f"{v:+.2f}%  (better {b:.0f}%, worse {w:.0f}%)",
                va="center", ha="left" if v >= 0 else "right", fontsize=8, color="#0b0b0b")
    ax.set_yticks(list(y), [n.upper() for n in t.index])
    ax.set_xlabel(f"Average makespan reduction of {highlight.upper()} (%)")
    ax.set_xlim(-span * 1.9 if (t.avg_gain_pct < 0).any() else 0, span * 1.9)
    ax.grid(axis="y", visible=False)
    ax.set_title(f"{highlight.upper()} versus each algorithm (same instances)", loc="left", fontsize=9.5)
    fig.tight_layout()
    base = os.path.join(outdir, "gain_vs_baselines")
    fig.savefig(base + ".png", bbox_inches="tight"); fig.savefig(base + ".pdf", bbox_inches="tight")
    plt.close(fig)
    return "gain_vs_baselines"


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--algos", default="all")
    ap.add_argument("--highlight", default=None, help="your proposed algorithm: drawn first, gains computed against it")
    for k in ("datasets", "families", "nodes", "procs"):
        ap.add_argument(f"--{k}", default="all")
    ap.add_argument("--split", default="all", choices=["all", "dev", "test"])
    ap.add_argument("--name", default="latest")
    ap.add_argument("--no-figures", action="store_true")
    a = ap.parse_args()

    df, names = load(a)
    out = os.path.join(ROOT, "results", "reports", a.name)
    os.makedirs(out, exist_ok=True)
    df.sort_values(["dataset", "family", "nodes", "p", "instance", "algo"]).to_csv(
        os.path.join(out, "results.csv"), index=False)

    validity = df.pivot_table(index="algo", columns="status", values="path", aggfunc="count", fill_value=0)
    validity = validity.reindex(names)
    first_bad = (df[~df.status.isin(VALID)].groupby("algo").first_violation.first().reindex(names).fillna(""))
    validity["example_problem"] = first_bad
    c = comparable(df, names)
    n_inst, n_all = c.path.nunique(), df.path.nunique()
    pw = pairwise(c, names)
    runtime = df.groupby(["algo", "nodes"]).runtime_s.mean().unstack().reindex(names).round(4)
    cols = ["makespan", "slr", "speedup", "efficiency", "runtime_s"]
    extra = [k for k in ("comm_ratio", "imbalance", "utilization", "rob_ratio", "rob_cv")
             if k in c and c.groupby("algo")[k].count().reindex(names).fillna(0).min() == c.groupby("algo").size().reindex(names).min()]
    overall = c.groupby("algo")[cols].mean().reindex(names).round(4)
    secondary = c.groupby("algo")[extra].mean().reindex(names).round(4) if extra else None

    tables = {
        "Overall": overall,
        "Validity": validity,
        "Makespan_by_tasks": pivot(c, "nodes", "makespan", names),
        "Makespan_by_procs": pivot(c, "p", "makespan", names),
        "SLR_by_tasks": pivot(c, "nodes", "slr", names),
        "SLR_by_procs": pivot(c, "p", "slr", names),
        "Speedup_by_procs": pivot(c, "p", "speedup", names),
        "Efficiency_by_procs": pivot(c, "p", "efficiency", names),
        "Pairwise": pw.set_index(["algorithm", "versus"]),
        "Runtime_s_by_tasks": runtime,
    }
    if secondary is not None:
        tables["Secondary_metrics"] = secondary
    sig = significance_tables(c, names, a.highlight) if len(names) > 1 else {"_markdown": ""}
    sig_md = sig.pop("_markdown")
    tables.update(sig)
    xlsx = os.path.join(out, "summary.xlsx")
    with pd.ExcelWriter(xlsx, engine="openpyxl") as xw:
        info = pd.DataFrame({"item": ["algorithms", "instances selected", "instances compared",
                                      "rule", "highlight"],
                             "value": [", ".join(names), n_all, n_inst,
                                       "averages use only instances where every algorithm produced a valid schedule",
                                       a.highlight or ""]})
        info.to_excel(xw, sheet_name="About", index=False)
        for name, t in tables.items():
            t.to_excel(xw, sheet_name=name)
        for ws in xw.book.worksheets:
            ws.freeze_panes = "B2"
            for col in ws.columns:
                width = max(len(str(cell.value)) if cell.value is not None else 0 for cell in col)
                ws.column_dimensions[col[0].column_letter].width = min(max(10, width + 2), 60)

    md = [f"# Benchmark report: {a.name}", "",
          f"Algorithms: {', '.join(names)}  ",
          f"Instances compared: {n_inst} of {n_all} (only those where every algorithm is valid)", "",
          "## Validity", validity.to_markdown(), "", "## Overall averages", overall.to_markdown(), ""]
    if secondary is not None:
        md += ["## Secondary metrics (means over the compared instances)",
               "comm_ratio = communication paid / total; imbalance = max / mean processor busy time; "
               "utilization = busy / (p x makespan); rob_ratio = mean makespan under +-20% cost noise / makespan; "
               "rob_cv = coefficient of variation of the perturbed makespans.", "", secondary.to_markdown(), ""]
    if a.highlight:
        hp = pw[pw.algorithm == a.highlight].set_index("versus").drop(columns="algorithm")
        md += [f"## {a.highlight} versus each algorithm", hp.to_markdown(), ""]
    md += [sig_md]
    open(os.path.join(out, "summary.md"), "w").write("\n".join(md))

    made = [] if a.no_figures else figures(c, names, os.path.join(out, "figures"), a.highlight)
    if a.highlight and not a.no_figures and len(names) > 1:
        made.append(gain_figure(pw, a.highlight, names, os.path.join(out, "figures")))
    print(f"report written to {os.path.relpath(out, ROOT)}  ({n_inst}/{n_all} instances compared, "
          f"{len(made)} figures)")
    print(validity.to_string())
    if a.highlight:
        print(pw[pw.algorithm == a.highlight].to_string(index=False))
    print(sig_md)


if __name__ == "__main__":
    main()
