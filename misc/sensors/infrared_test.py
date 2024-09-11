import os
import re
from datetime import datetime, timedelta
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from matplotlib import ticker

def save(a):
    plot_filename = os.path.splitext(os.path.basename(csv_file))[0] + (f'-{a}' if a else '') + '.svg'
    plt.savefig(plot_filename, format='svg')

def plot_infrared(file):
    with open(file, 'r') as file:
        x = file.readlines()

    d = [re.search("W \(([0-9]*)\) MAIN: Mail detected!", i) for i in x]
    d = [i.groups() for i in d if i]
    d = [int(i[0]) for i in d]

    x = [re.search("I \(([0-9]*)\) Infrared: off => ([0-9]*) mV, thresh => ([0-9\.]*) mV, on => ([0-9]*) mV", i) for i in x]
    x = [i.groups() for i in x if i]
    x = [[int(i[0]), int(i[1]), float(i[2]), int(i[3])] for i in x]
    
    detections = [i - x[0][0] for i in d]
    readings = [[i[0] - x[0][0], i[1], i[2], i[3]] for i in x]

    for i in range(1, len(readings)):
        if readings[i][0] == readings[i - 1][0]:
            readings[i - 1][0] -= 3
            readings[i][0] += 3
    
    start_rec = datetime.strptime("07:27:00", "%H:%M:%S")
    factor = 7.47

    times = [start_rec + timedelta(milliseconds=i[0]*factor) for i in readings]
    dtimes = [start_rec + timedelta(milliseconds=i*factor) for i in detections]
    off = [i[1] for i in readings]
    thresh = [i[2] for i in readings]
    on = [i[3] for i in readings]
    diff = [on_val - thresh_val for on_val, thresh_val in zip(on, thresh)]

    renderQueue = [
        [
            "dawn",
            datetime.strptime("07:29:45", "%H:%M:%S"),
            datetime.strptime("07:30:42", "%H:%M:%S"),
            '%H:%M:%S',
            mdates.SecondLocator(interval=5),
            mdates.SecondLocator(interval=1),
            [0, 530, 300], [0, 50, 100, 150, 200, 250, 300, 400, 500],
            [-35, 500, 150], [-30, 0, 30, 60, 90, 120, 150, 250, 350, 450]
        ],
        #[
        #    "day",
        #    datetime.strptime("10:06:29", "%H:%M:%S"),
        #    datetime.strptime("10:07:21", "%H:%M:%S"),
        #    '%H:%M:%S',
        #    mdates.SecondLocator(interval=5),
        #    mdates.SecondLocator(interval=1),
        #    [0, 2500, 200], [0, 50, 100, 150, 200, 300, 450, 600, 800, 1100, 1500, 2000, 2500],
        #    [-50, 170, 100], [-50, -25, 0, 25, 50, 75, 100, 150]
        #],
        [
            "noon",
            datetime.strptime("13:06:23", "%H:%M:%S"),
            datetime.strptime("13:07:09", "%H:%M:%S"),
            '%H:%M:%S',
            mdates.SecondLocator(interval=5),
            mdates.SecondLocator(interval=1),
            [0, 2800, 250], [0, 50, 100, 150, 200, 250, 350, 500, 700, 1000, 1400, 1800, 2300, 2800],
            [-50, 320, 40], [-50, -25, 0, 25, 50, 100, 200, 300]
        ],
        [
            "dark",
            datetime.strptime("13:25:43", "%H:%M:%S"),
            datetime.strptime("13:26:40", "%H:%M:%S"),
            '%H:%M:%S',
            mdates.SecondLocator(interval=5),
            mdates.SecondLocator(interval=1),
            [0, 1200, 250], [0, 50, 100, 150, 200, 250, 350, 500, 700, 950, 1200],
            [-50, 1150, 150], [-50, 0, 50, 100, 150, 250, 450, 700, 1000]
        ]
    ]

    for i in renderQueue:
        start = min(times, key=lambda x: abs(x - i[1]))
        end = min(times, key=lambda x: abs(x - i[2]))

        start = times.index(start) - 1
        end = times.index(end) + 1

        start = times[start]
        end = times[end]

        start = start - start_rec
        end = end - start_rec

        start = start.total_seconds() * 1000 / factor + x[0][0]
        end = end.total_seconds() * 1000 / factor + x[0][0]

        print(i[0], start, end)
        input()


    def renderAll(suffix = ""):
        nonlocal renderQueue

        plt.xlabel('Time')
        plt.ylabel('ADC reading [mV]')
        plt.grid(True)
        plt.legend(loc='lower right', bbox_to_anchor=(1, 1), ncol=4)

        for name, start, end, xfmajor, xlmajor, xlminor, ylim, ylmajor, ylimdiff, ylmajordiff in renderQueue:
            if suffix == "diff":
                ylim = ylimdiff
                ylmajor = ylmajordiff
            
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
            save(name + suffix)

    plt.figure(figsize=(12, 4))
    plt.vlines(dtimes, -1000, 3500, colors='yellow', linestyles="dashed", label='Mail detected')
    plt.plot(times, off, label='IR LED Off', color='blue')
    plt.plot(times, on, label='IR LED On', color='red')
    plt.plot(times, thresh, label='Threshold', color='green')

    renderAll()
    plt.close()

    plt.figure(figsize=(12, 4))
    plt.plot([times[0], times[-1]], [0, 0], color='black')
    plt.vlines(dtimes, -1000, 3500, colors='yellow', linestyles="dashed", label='Mail detected')
    plt.plot(times, diff, label='Difference between IR LED On and Threshold', color='magenta')

    renderAll("diff")
    plt.close()

current_directory = os.getcwd()
csv_files = [file for file in os.listdir(current_directory) if file == 'infrared_test.txt']

for csv_file in csv_files:
    plot_infrared(csv_file)
