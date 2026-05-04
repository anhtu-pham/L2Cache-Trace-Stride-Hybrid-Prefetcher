import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
# from matplotlib.ticker import PercentFormatter

ref_df = pd.read_csv("result_ref.csv")
hybrid_df = pd.read_csv("result_hybrid.csv")

baseline = ref_df[ref_df["L2C Prefetcher"] == "no"]
baseline_IPC = baseline.iloc[0]["IPC"]

ref_df["Speedup"] = ref_df["IPC"] / baseline_IPC
hybrid_df["Speedup"] = hybrid_df["IPC"] / baseline_IPC


def plot_hybrid_performance(criteria):
    curr_values = None
    curr_x = None
    first = True
    first_values = None

    plt.figure(figsize = (10, 6))

    for max_counter, group in hybrid_df.groupby("Max Counter"):
        curr_values = group[criteria].to_list()
        curr_x = group["Max Number of Offsets"].to_list()
        position = 6 - max_counter

        lines = plt.plot(curr_x, curr_values, marker = "o", label = f"Trace-Stride Hybrid Prefetcher, Counter's MAX = {max_counter}")

        if first:
            first_values = curr_values
            first = False
        for idx in range(len(curr_x)):
            plt.text(curr_x[idx], first_values[idx] + 0.004 * position, f"{round(curr_values[idx], 5)}x", fontsize = 11.5, color = lines[0].get_color(), ha = "center")

    baseline_value = 1 if criteria == "Speedup" else baseline_IPC
    hline = plt.axhline(y = baseline_value, linestyle = "--", label = f"No Prefetcher (Baseline)", color = "purple")
    plt.text(curr_x[-1], baseline_value + 0.003, "1.0x", fontsize = 11.5, color = hline.get_color(), ha = "left")

    plt.xlabel("Max Number of Prefetch Offsets")
    plt.ylabel(criteria)
    plt.ylim(top = max(curr_values) + 0.025)
    plt.title(f"Performance of Trace-Stride Hybrid Prefetcher (targeting L2 Cache)")
    plt.legend()
    # plt.grid(visible = True, axis = "y")
    plt.tight_layout()

# plot_hybrid_performance("IPC")
plot_hybrid_performance("Speedup")


labels = []
values = []
colors = []

unique_ref_pref = ref_df["L2C Prefetcher"].unique()
unique_pref = np.append(unique_ref_pref, "hybrid", axis = None)
color_map = plt.get_cmap("gnuplot")
color_dict = {
    pref: color_map(index) for index, pref in zip(np.linspace(0.1, 0.9, len(unique_pref)), unique_pref)
}

def plot_prefetcher_comparison(criteria, hybrid_repr_max_counter):
    for _, row in ref_df.iterrows():
        prefetcher = row["L2C Prefetcher"]
        if prefetcher == "ip_stride":
            labels.append(f"IP_Stride\nDegree = {row["Stride Prefetch Degree"]}")
        elif prefetcher == "no":
            labels.append("No\n(Baseline)")
        else:
            labels.append(prefetcher.upper())
        colors.append(color_dict[prefetcher])
        values.append(row[criteria])

    hybrid_repr_df = hybrid_df[hybrid_df["Max Counter"] == hybrid_repr_max_counter]
    labels.extend(["Trace-Stride\nMedian", "Trace-Stride\nMax", "Trace-Stride\nMin"])
    colors.extend([color_dict["hybrid"], color_dict["hybrid"], color_dict["hybrid"]])
    values.extend([hybrid_repr_df[criteria].median(), hybrid_repr_df[criteria].max(), hybrid_repr_df[criteria].min()])

    plt.figure(figsize = (10, 6))

    plt.scatter(labels, values, s = 500, c = colors, marker = "^")
    for x, value in zip(range(len(labels)), values):
        plt.text(x, value + 0.015, f"{round(value, 5)}x", fontsize = 11.5, ha = "center")

    plt.ylabel(criteria)
    plt.ylim(min(values) - 0.05, max(values) + 0.1)
    plt.title("Comparison of L2C Prefetchers' Performance")
    plt.grid(visible = True, axis = "y")
    plt.tight_layout()

plot_prefetcher_comparison("Speedup", 3)

plt.show()