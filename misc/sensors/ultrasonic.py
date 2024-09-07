import os
import csv
import matplotlib.pyplot as plt
import numpy as np

def plot_sensors(csv_file):
    with open(csv_file, 'r') as file:
        csv_reader = csv.reader(file)
        header = next(csv_reader)
        data = [row for row in csv_reader]
    
    measurements = [row[0] for row in data]
    cleaned = [float(value) * 10 for value in measurements if value != "fail"]

    bins = np.arange(0, 160 + 2, 2)
    fig, ax = plt.subplots(figsize=(12, 4))
    counts, edges, patches = ax.hist(cleaned, bins=bins, color='skyblue', edgecolor='black')
    ax.set_xlim(0, 160)

    highlight_range = (54, 58)
    for i in range(len(edges) - 1):
        bin_start = round(edges[i], 2)
        bin_end = round(edges[i + 1], 2)

        if bin_start < highlight_range[1] and bin_end > highlight_range[0]:
            patches[i].set_facecolor('red')

    ax.set_xlabel(header[0] + ' [mm]')
    ax.set_ylabel('Frequency')
    ax.set_xticks(np.arange(0, 160, 8))

    plot_filename = os.path.splitext(os.path.basename(csv_file))[0] + '.svg'
    plt.savefig(plot_filename, format='svg')

    plt.close()

current_directory = os.getcwd()
csv_files = [file for file in os.listdir(current_directory) if file == 'ultrasonic.csv']

for csv_file in csv_files:
    plot_sensors(csv_file)
