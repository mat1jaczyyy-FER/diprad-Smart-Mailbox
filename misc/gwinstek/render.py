import os
import csv
import matplotlib.pyplot as plt
import numpy as np

to_watts=3.3
smoothing_threshold=6E-3*to_watts

def plot_waveform(csv_file):
    name = os.path.splitext(os.path.basename(csv_file))[0]
    color = '#d62728' if name == 'INFRARED' else '#1f77b4'
    x_values = []
    y_values = []

    with open(csv_file, 'r') as file:
        csv_reader = csv.reader(file)
        for row in csv_reader:
            if row and row[0] == 'Waveform Data':
                break

        for row in csv_reader:
            if row:
                x_values.append(float(row[0]) + 10.0)
                y_values.append(float(row[1]) * to_watts)

    smoothed_x = [x_values[0]]
    smoothed_y = [y_values[0]]

    for i in range(1, len(x_values)):
        if abs(y_values[i] - y_values[i-1]) < smoothing_threshold:
            smoothed_y[-1] = (smoothed_y[-1] + y_values[i]) / 2
        
        smoothed_x.append(x_values[i])
        smoothed_y.append(y_values[i])

    plt.figure(figsize=(12, 4))

    plt.plot(smoothed_x, smoothed_y, linewidth=0.3, color=color)
    plt.xlabel('Time [s]')
    plt.ylabel('Power draw [W]')
    plt.xticks(range(0, int(smoothed_x[-1]) + 2, 1))
    plt.grid(True)

    plot_filename = name + '.svg'
    plt.savefig(plot_filename, format='svg')

    plt.close()

    dx = 0.004
    if name == 'ULTRASONIC':
        mask = 12.5
        mask_precise = (12.58, 14.46)
    elif name == 'INFRARED':
        mask = 11.65
        mask_precise = (11.724, 12.86)
    else:
        return
    
    smoothed_x = np.array(smoothed_x)
    smoothed_y = np.array(smoothed_y)

    filtered_x = smoothed_x[(smoothed_x >= mask) & (smoothed_x <= mask + 2)]
    filtered_y = smoothed_y[(smoothed_x >= mask) & (smoothed_x <= mask + 2)]

    plt.figure(figsize=(9, 3))

    plt.plot(filtered_x, filtered_y, linewidth=1, color=color)
    plt.xlabel('Time [s]')
    plt.ylabel('Power draw [W]')
    plt.xticks(np.arange(0, int(smoothed_x[-1]) + 2, 0.2))
    plt.grid(True)

    plt.xlim(mask, mask + 2)
    plt.ylim(-0.02, 0.22)
    plt.tight_layout()

    plot_filename = name + '_ZOOM.svg'
    plt.savefig(plot_filename, format='svg')

    plt.close()

    y_hill = y_values[int(mask_precise[0] / dx):int(mask_precise[1] / dx)]
    energy = (dx/2) * (y_hill[0] + y_hill[-1] + 2 * sum(y_hill[1:-1])) # trapez
    print(f"{name}: {energy} J")

current_directory = os.getcwd()
csv_files = [file for file in os.listdir(current_directory) if file.endswith('.csv')]

for csv_file in csv_files:
    plot_waveform(csv_file)
