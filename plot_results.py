#!/usr/bin/env python3
import matplotlib.pyplot as plt
import numpy as np

def read_results(filename="results.txt"):
    with open(filename, "r") as f:
        nodes_seq = int(f.readline().strip())
        nodes_par = int(f.readline().strip())
        times_seq = list(map(float, f.readline().strip().split()))
        times_par = list(map(float, f.readline().strip().split()))
    return nodes_seq, nodes_par, times_seq, times_par

def plot_results(nodes_seq, nodes_par, times_seq, times_par):
    labels = [f"Move {i+1}" for i in range(max(len(times_seq), len(times_par)))]
    x = np.arange(len(labels))
    width = 0.35

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))

    ax1.bar(x - width/2, times_seq, width, label='Sequential', color='lime')
    ax1.bar(x + width/2, times_par, width, label='Parallel', color='cyan')
    ax1.set_xlabel('Move')
    ax1.set_ylabel('Execution Time (s)')
    ax1.set_title('Execution Time per Move')
    ax1.legend()
    ax1.set_xticks(x)
    ax1.set_xticklabels(labels, rotation=45)

    ax2.bar(['Sequential', 'Parallel'], [nodes_seq, nodes_par], color=['lime', 'cyan'])
    ax2.set_ylabel('Nodes Visited')
    ax2.set_title('Total Nodes Visited')

    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    nodes_seq, nodes_par, times_seq, times_par = read_results()
    plot_results(nodes_seq, nodes_par, times_seq, times_par) 