import os
import csv
import matplotlib.pyplot as plt

to_watts=3.3
smoothing_threshold=6E-3*to_watts

def plot_waveform(csv_file):
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

    plt.plot(smoothed_x, smoothed_y, linewidth=0.3)
    plt.xlabel('Time [t]')
    plt.ylabel('Power draw [W]')
    plt.grid(True)

    # Set transparent background
    plt.gcf().set_facecolor('none')
    plt.gca().xaxis.label.set_color('white')  # Set x-axis label color to white
    plt.gca().yaxis.label.set_color('white')  # Set y-axis label color to white
    plt.gca().tick_params(axis='both', colors='white')  # Set tick colors to white

    plot_filename = os.path.splitext(os.path.basename(csv_file))[0] + '.svg'
    plt.savefig(plot_filename, format='svg')

    plt.close()

current_directory = os.getcwd()
csv_files = [file for file in os.listdir(current_directory) if file.endswith('.csv')]

for csv_file in csv_files:
    plot_waveform(csv_file)
