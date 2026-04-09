"""
INF-221 Tarea 1 - 2026-1
Generador de gráficos para algoritmos de ordenamiento
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

MEAS_FILE  = "data/measurements/results.csv"
PLOTS_DIR  = "data/plots"
os.makedirs(PLOTS_DIR, exist_ok=True)

ALGO_COLORS = {
    "mergesort": "#2196F3",
    "quicksort": "#F44336",
    "stdsort":   "#4CAF50",
}
ALGO_LABELS = {
    "mergesort": "MergeSort",
    "quicksort": "QuickSort (pivote aleatorio)",
    "stdsort":   "std::sort",
}

# ─── Carga de datos ───────────────────────────────────────────────────────────

df = pd.read_csv(MEAS_FILE)

# Promediar las 3 muestras (a, b, c) para cada combinación
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


def plot_time_vs_n(subset_df, title, filename, logy=True):
    """Tiempo vs N para cada algoritmo, dado un subconjunto del df."""
    fig, ax = plt.subplots(figsize=(8, 5))
    for algo in algorithms:
        data = subset_df[subset_df["algorithm"] == algo].sort_values("n")
        if data.empty:
            continue
        ax.plot(data["n"], data["time_ms"],
                marker="o", label=ALGO_LABELS[algo],
                color=ALGO_COLORS[algo], linewidth=2, markersize=5)
    ax.set_xlabel("n (tamaño del arreglo)")
    ax.set_ylabel("Tiempo promedio (ms)")
    ax.set_title(title)
    ax.set_xscale("log")
    if logy:
        ax.set_yscale("log")
    ax.xaxis.set_major_formatter(ticker.LogFormatter(labelOnlyBase=False))
    ax.legend()
    ax.grid(True, which="both", linestyle="--", alpha=0.4)
    fig.tight_layout()
    save(fig, filename)


def plot_memory_vs_n(subset_df, title, filename):
    """Memoria (VmRSS delta) vs N para cada algoritmo."""
    fig, ax = plt.subplots(figsize=(8, 5))
    for algo in algorithms:
        data = subset_df[subset_df["algorithm"] == algo].sort_values("n")
        if data.empty:
            continue
        ax.plot(data["n"], data["memory_kb"],
                marker="s", label=ALGO_LABELS[algo],
                color=ALGO_COLORS[algo], linewidth=2, markersize=5)
    ax.set_xlabel("n (tamaño del arreglo)")
    ax.set_ylabel("Memoria adicional promedio (KB)")
    ax.set_title(title)
    ax.set_xscale("log")
    ax.legend()
    ax.grid(True, which="both", linestyle="--", alpha=0.4)
    fig.tight_layout()
    save(fig, filename)


# ─── Gráfico 1: Tiempo vs N por tipo de arreglo (promedio sobre dominios) ────
print("Generando gráficos de tiempo vs n por tipo...")
for tipo in tipos:
    sub = grouped[grouped["tipo"] == tipo]
    # promediar sobre dominios
    sub = (sub.groupby(["algorithm", "n"])
              .agg(time_ms=("time_ms", "mean"))
              .reset_index())
    plot_time_vs_n(
        sub,
        title=f"Tiempo vs n — tipo: {tipo} (promedio D1+D7)",
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

# ─── Gráfico 3: Tiempo vs N — todos los tipos en subplots ────────────────────
print("Generando gráfico combinado tiempo vs n (subplots por tipo)...")
fig, axes = plt.subplots(1, len(tipos), figsize=(6 * len(tipos), 5), sharey=False)
for ax, tipo in zip(axes, tipos):
    sub = grouped[grouped["tipo"] == tipo]
    sub = (sub.groupby(["algorithm", "n"])
              .agg(time_ms=("time_ms", "mean"))
              .reset_index())
    for algo in algorithms:
        data = sub[sub["algorithm"] == algo].sort_values("n")
        if data.empty:
            continue
        ax.plot(data["n"], data["time_ms"],
                marker="o", label=ALGO_LABELS[algo],
                color=ALGO_COLORS[algo], linewidth=2, markersize=4)
    ax.set_title(tipo)
    ax.set_xlabel("n")
    ax.set_ylabel("Tiempo (ms)")
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.grid(True, which="both", linestyle="--", alpha=0.4)
    ax.legend(fontsize=8)
fig.suptitle("Tiempo de ordenamiento vs n (escala log-log)", fontsize=13, fontweight="bold")
fig.tight_layout()
save(fig, "tiempo_vs_n_subplots_tipo.png")

# ─── Gráfico 4: Memoria vs N ──────────────────────────────────────────────────
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

# ─── Gráfico 5: Heatmap de tiempo por tipo × algoritmo (n=10^5) ──────────────
print("Generando heatmap...")
sub = grouped[grouped["n"] == 100000].copy()
if not sub.empty:
    pivot = sub.pivot_table(index="tipo", columns="algorithm",
                            values="time_ms", aggfunc="mean")
    pivot.columns = [ALGO_LABELS[c] for c in pivot.columns]

    fig, ax = plt.subplots(figsize=(7, 4))
    im = ax.imshow(pivot.values, aspect="auto", cmap="YlOrRd")
    ax.set_xticks(range(len(pivot.columns)))
    ax.set_xticklabels(pivot.columns, rotation=20, ha="right")
    ax.set_yticks(range(len(pivot.index)))
    ax.set_yticklabels(pivot.index)
    plt.colorbar(im, ax=ax, label="Tiempo (ms)")
    for i in range(len(pivot.index)):
        for j in range(len(pivot.columns)):
            ax.text(j, i, f"{pivot.values[i,j]:.2f}",
                    ha="center", va="center", fontsize=9, color="black")
    ax.set_title("Tiempo promedio (ms) — n = 100,000")
    fig.tight_layout()
    save(fig, "heatmap_tiempo_n100000.png")

print("\nTodos los gráficos generados en", PLOTS_DIR)