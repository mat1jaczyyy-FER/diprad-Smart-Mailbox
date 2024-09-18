import os
import re
from datetime import datetime, timedelta
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from matplotlib import ticker

def save(a):
    plot_filename = os.path.splitext(os.path.basename(csv_file))[0] + (f'-{a}' if a else '') + '.svg'
    plt.savefig(plot_filename, format='svg')

def plot_ultrasonic(file):
    with open(file, 'r') as file:
        x = file.readlines()

    d = [re.search("W \(([0-9]*)\) MAIN: Mail detected!", i) for i in x]
    d = [i.groups() for i in d if i]
    d = [int(i[0]) for i in d]

    x = [re.search("I \(([0-9]*)\) HC_SR04: roundtrip =>\s*([0-9\.]*) mm$", i) for i in x]
    x = [i.groups() for i in x if i]
    x = [[int(i[0]), float(i[1])] for i in x]
    
    detections = [i - x[0][0] for i in d]
    readings = [[i[0] - x[0][0], i[1]] for i in x]

    for i in range(1, len(readings)):
        if readings[i][0] == readings[i - 1][0]:
            readings[i - 1][0] -= 3
            readings[i][0] += 3
    
    start_rec = datetime.strptime("02:27:42.100", "%H:%M:%S.%f")

    times = [start_rec + timedelta(milliseconds=i[0]) for i in readings]
    dtimes = [start_rec + timedelta(milliseconds=i) for i in detections]
    mm = [i[1] for i in readings]

    plt.figure(figsize=(12, 4))
    plt.vlines(dtimes, -1000, 3500, colors='yellow', linestyles="dashed", label='Mail detected')
    plt.plot(times, mm, label='Sensor', color='blue')
    plt.plot([times[0], times[-1]], [54, 54], label='Threshold', color='green')
    plt.plot([times[0], times[-1]], [58, 58], color='green')

    plt.xlabel('Time')
    plt.ylabel('Sensor reading [mm]')
    plt.grid(True)
    plt.legend(loc='lower right', bbox_to_anchor=(1, 1), ncol=4)

    start, end, xfmajor, xlmajor, xlminor, ylim, ylmajor = [
        datetime.strptime("02:28:00", "%H:%M:%S"),
        datetime.strptime("02:28:35", "%H:%M:%S"),
        '%H:%M:%S',
        mdates.SecondLocator(interval=5),
        mdates.SecondLocator(interval=1),
        [0, 700, 60], [0, 50, 100, 150, 200, 250, 300, 400, 500]
    ]
    
    plt.xlim([start, end])
    plt.ylim(ylim[:2])
    if len(ylim) == 3:
        plt.yscale('symlog', linthresh=ylim[2])
    else:
        plt.yscale('linear')

    plt.gcf().autofmt_xdate()

    plt.gca().xaxis.set_major_formatter(mdates.DateFormatter(xfmajor))
    plt.gca().xaxis.set_major_locator(xlmajor)
    plt.gca().xaxis.set_minor_locator(xlminor)

    plt.gca().yaxis.set_major_formatter(ticker.ScalarFormatter())
    plt.gca().yaxis.get_major_formatter().set_useOffset(False)
    plt.gca().yaxis.get_major_formatter().set_scientific(False)
    plt.gca().yaxis.set_major_locator(ticker.FixedLocator(ylmajor))
    plt.gca().yaxis.set_minor_locator(ticker.AutoMinorLocator())
    
    plt.tight_layout(pad=0.1)
    save("")
    plt.close()

current_directory = os.getcwd()
csv_files = [file for file in os.listdir(current_directory) if file == 'ultrasonic_test.txt']

for csv_file in csv_files:
    plot_ultrasonic(csv_file)
