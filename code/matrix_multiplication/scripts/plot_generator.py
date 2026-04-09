"""
INF-221 Tarea 1 - 2026-1
Generador de gráficos para algoritmos de multiplicación de matrices
Lee: data/measurements/results.csv
Guarda PNGs en: data/plots/

Autor: Al Goritmo Pérez | Rol: 202673100-1
"""

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np
import os

# ─── Configuración ────────────────────────────────────────────────────────────

MEAS_FILE = "data/measurements/results.csv"
PLOTS_DIR = "data/plots"
os.makedirs(PLOTS_DIR, exist_ok=True)

ALGO_COLORS = {
    "naive":    "#FF6F00",
    "strassen": "#6A1B9A",
}
ALGO_LABELS = {
    "naive":    "Naive O(n³)",
    "strassen": "Strassen O(n^2.807)",
}

# ─── Carga de datos ───────────────────────────────────────────────────────────

df = pd.read_csv(MEAS_FILE)

grouped = (df.groupby(["algorithm", "n", "tipo", "dominio"])
             .agg(time_ms=("time_ms", "mean"),
                  memory_kb=("memory_kb", "mean"))
             .reset_index())

algorithms = grouped["algorithm"].unique()
tipos      = grouped["tipo"].unique()
dominios   = grouped["dominio"].unique()
Ns         = sorted(grouped["n"].unique())

# ─── Helpers ─────────────────────────────────────────────────────────────────

def save(fig, name):
    path = os.path.join(PLOTS_DIR, name)
    fig.savefig(path, dpi=150, bbox_inches="tight")
    plt.close(fig)
    print(f"  Guardado: {path}")


def plot_time_vs_n(subset_df, title, filename):
    fig, ax = plt.subplots(figsize=(8, 5))
    for algo in algorithms:
        data = subset_df[subset_df["algorithm"] == algo].sort_values("n")
        if data.empty:
            continue
        ax.plot(data["n"], data["time_ms"],
                marker="o", label=ALGO_LABELS[algo],
                color=ALGO_COLORS[algo], linewidth=2, markersize=6)

    # Curvas de referencia teórica escaladas al primer punto de naive
    ref_naive = subset_df[subset_df["algorithm"] == "naive"].sort_values("n")
    if not ref_naive.empty and len(ref_naive) > 1:
        n_arr = np.array(ref_naive["n"].values, dtype=float)
        t_arr = np.array(ref_naive["time_ms"].values, dtype=float)
        scale_n3      = t_arr[0] / (n_arr[0] ** 3)
        scale_strassen = t_arr[0] / (n_arr[0] ** 2.807)
        ax.plot(n_arr, scale_n3 * n_arr**3,
                "--", color="gray", alpha=0.5, label="O(n³) teórico")
        ax.plot(n_arr, scale_strassen * n_arr**2.807,
                ":", color="purple", alpha=0.5, label="O(n^2.807) teórico")

    ax.set_xlabel("n (dimensión de la matriz n×n)")
    ax.set_ylabel("Tiempo promedio (ms)")
    ax.set_title(title)
    ax.set_xscale("log", base=2)
    ax.set_yscale("log")
    ax.xaxis.set_major_formatter(ticker.FuncFormatter(
        lambda v, _: f"$2^{{{int(np.log2(v))}}}$" if v > 0 else ""))
    ax.legend(fontsize=9)
    ax.grid(True, which="both", linestyle="--", alpha=0.4)
    fig.tight_layout()
    save(fig, filename)


def plot_memory_vs_n(subset_df, title, filename):
    fig, ax = plt.subplots(figsize=(8, 5))
    for algo in algorithms:
        data = subset_df[subset_df["algorithm"] == algo].sort_values("n")
        if data.empty:
            continue
        ax.plot(data["n"], data["memory_kb"],
                marker="s", label=ALGO_LABELS[algo],
                color=ALGO_COLORS[algo], linewidth=2, markersize=6)
    ax.set_xlabel("n (dimensión de la matriz n×n)")
    ax.set_ylabel("Memoria adicional promedio (KB)")
    ax.set_title(title)
    ax.set_xscale("log", base=2)
    ax.xaxis.set_major_formatter(ticker.FuncFormatter(
        lambda v, _: f"$2^{{{int(np.log2(v))}}}$" if v > 0 else ""))
    ax.legend()
    ax.grid(True, which="both", linestyle="--", alpha=0.4)
    fig.tight_layout()
    save(fig, filename)


# ─── Gráfico 1: Tiempo vs N por tipo (promedio sobre dominios) ───────────────
print("Generando gráficos de tiempo vs n por tipo...")
for tipo in tipos:
    sub = grouped[grouped["tipo"] == tipo]
    sub = (sub.groupby(["algorithm", "n"])
              .agg(time_ms=("time_ms", "mean"))
              .reset_index())
    plot_time_vs_n(
        sub,
        title=f"Tiempo vs n — tipo: {tipo} (promedio D0+D10)",
        filename=f"tiempo_vs_n_{tipo}.png"
    )

# ─── Gráfico 2: Tiempo vs N por dominio (promedio sobre tipos) ───────────────
print("Generando gráficos de tiempo vs n por dominio...")
for dominio in dominios:
    sub = grouped[grouped["dominio"] == dominio]
    sub = (sub.groupby(["algorithm", "n"])
              .agg(time_ms=("time_ms", "mean"))
              .reset_index())
    plot_time_vs_n(
        sub,
        title=f"Tiempo vs n — dominio: {dominio} (promedio tipos)",
        filename=f"tiempo_vs_n_{dominio}.png"
    )

# ─── Gráfico 3: Subplots tipo × dominio ──────────────────────────────────────
print("Generando gráfico combinado (subplots tipo × dominio)...")
fig, axes = plt.subplots(len(dominios), len(tipos),
                          figsize=(5 * len(tipos), 4 * len(dominios)),
                          sharey=False)
if len(dominios) == 1:
    axes = [axes]

for row_i, dominio in enumerate(dominios):
    for col_j, tipo in enumerate(tipos):
        ax = axes[row_i][col_j]
        sub = grouped[(grouped["tipo"] == tipo) & (grouped["dominio"] == dominio)]
        sub = sub.sort_values("n")
        for algo in algorithms:
            data = sub[sub["algorithm"] == algo]
            if data.empty:
                continue
            ax.plot(data["n"], data["time_ms"],
                    marker="o", label=ALGO_LABELS[algo],
                    color=ALGO_COLORS[algo], linewidth=2, markersize=4)
        ax.set_title(f"{tipo} / {dominio}", fontsize=9)
        ax.set_xscale("log", base=2)
        ax.set_yscale("log")
        ax.set_xlabel("n")
        ax.set_ylabel("ms")
        ax.grid(True, which="both", linestyle="--", alpha=0.3)
        ax.legend(fontsize=7)

fig.suptitle("Tiempo vs n — Multiplicación de matrices (log-log)", fontsize=13, fontweight="bold")
fig.tight_layout()
save(fig, "tiempo_vs_n_subplots.png")

# ─── Gráfico 4: Comparación barras para n=256 y n=1024 ───────────────────────
print("Generando gráficos de comparación por n fijo...")
for n_val in [256, 1024]:
    sub = grouped[grouped["n"] == n_val].copy()
    if sub.empty:
        continue
    sub["config"] = sub["tipo"] + "\n" + sub["dominio"]
    configs = sub["config"].unique()
    x = np.arange(len(configs))
    width = 0.35

    fig, ax = plt.subplots(figsize=(max(8, len(configs) * 1.3), 5))
    for i, algo in enumerate(algorithms):
        heights = []
        for cfg in configs:
            row = sub[(sub["algorithm"] == algo) & (sub["config"] == cfg)]
            heights.append(row["time_ms"].values[0] if not row.empty else 0)
        ax.bar(x + i * width, heights, width, label=ALGO_LABELS[algo],
               color=ALGO_COLORS[algo], alpha=0.85)

    ax.set_xticks(x + width / 2)
    ax.set_xticklabels(configs, fontsize=8)
    ax.set_ylabel("Tiempo promedio (ms)")
    ax.set_title(f"Comparación de algoritmos — n = {n_val}")
    ax.legend()
    ax.grid(axis="y", linestyle="--", alpha=0.4)
    fig.tight_layout()
    save(fig, f"comparacion_algoritmos_n{n_val}.png")

# ─── Gráfico 5: Speedup Naive/Strassen ───────────────────────────────────────
print("Generando gráfico de speedup...")
sub_naive    = grouped[grouped["algorithm"] == "naive"]
sub_strassen = grouped[grouped["algorithm"] == "strassen"]
merged = pd.merge(sub_naive, sub_strassen,
                  on=["n", "tipo", "dominio"], suffixes=("_naive", "_strassen"))
merged["speedup"] = merged["time_ms_naive"] / merged["time_ms_strassen"]

fig, ax = plt.subplots(figsize=(8, 5))
for tipo in tipos:
    sub = (merged[merged["tipo"] == tipo]
           .groupby("n")["speedup"].mean()
           .reset_index()
           .sort_values("n"))
    ax.plot(sub["n"], sub["speedup"], marker="o", label=tipo, linewidth=2)

ax.axhline(1, color="black", linestyle="--", alpha=0.5, label="speedup = 1")
ax.set_ylim(merged["speedup"].min() * 0.9, merged["speedup"].max() * 1.1)
ax.set_xlabel("n (dimensión de la matriz)")
ax.set_ylabel("Speedup (Naive / Strassen)")
ax.set_title("Speedup de Strassen respecto a Naive (>1 = Strassen más rápido)")
ax.set_xscale("log", base=2)
ax.xaxis.set_major_formatter(ticker.FuncFormatter(
    lambda v, _: f"$2^{{{int(np.log2(v))}}}$" if v > 0 else ""))
ax.legend()
ax.grid(True, which="both", linestyle="--", alpha=0.4)
fig.tight_layout()
save(fig, "speedup_strassen_vs_naive.png")

# ─── Gráfico 6: Memoria vs N ──────────────────────────────────────────────────
print("Generando gráficos de memoria vs n...")
for tipo in tipos:
    sub = grouped[grouped["tipo"] == tipo]
    sub = (sub.groupby(["algorithm", "n"])
              .agg(memory_kb=("memory_kb", "mean"))
              .reset_index())
    plot_memory_vs_n(
        sub,
        title=f"Memoria adicional vs n — tipo: {tipo}",
        filename=f"memoria_vs_n_{tipo}.png"
    )

print("\nTodos los gráficos generados en", PLOTS_DIR)