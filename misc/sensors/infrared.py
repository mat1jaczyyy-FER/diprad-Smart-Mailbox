import os
import re
from datetime import datetime, timedelta
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

def save(a):
    plot_filename = os.path.splitext(os.path.basename(csv_file))[0] + (f'-{a}' if a else '') + '.svg'
    plt.savefig(plot_filename, format='svg')

def plot_infrared(file):
    with open(file, 'r') as file:
        x = file.readlines()

    x = [re.search("I \(([0-9]*)\) Infrared: off => ([0-9]*) mV, on => ([0-9]*) mV", i) for i in x]
    x = [i.groups() for i in x if i]
    x = [(int(i[0]), int(i[1]), int(i[2])) for i in x]

    readings = []
    building = []

    def push_building():
        nonlocal building
        building = building[2:]
        if building:
            readings.append([
                building[0][0] - x[0][0],
                sum([j[1] for j in building]) / len(building),
                sum([j[2] for j in building]) / len(building)
            ]),
        building = []

    for i in x:
        if not building:
            building = [i]
        elif i[0] - building[-1][0] < 1000:
            building.append(i)
        else:
            push_building()
            
    push_building()

    readings = [i for i in readings if i[0] < 24 * 60 * 60 * 1000]

    starttime = datetime.strptime("06:00:00", "%H:%M:%S")

    times = [starttime + timedelta(milliseconds=i[0]) for i in readings]
    off = [i[1] for i in readings]
    on = [i[2] for i in readings]

    plt.figure(figsize=(12, 4))
    plt.plot(times, off, label='IR LED Off', color='blue')
    plt.plot(times, on, label='IR LED On', color='red')

    plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
    plt.gca().xaxis.set_major_locator(mdates.HourLocator(interval=3))
    plt.gca().xaxis.set_minor_locator(mdates.MinuteLocator(interval=30))

    plt.xlabel('Time')
    plt.ylabel('ADC reading [mV]')
    plt.grid(True)
    plt.legend()
    
    plt.xlim([starttime, starttime + timedelta(hours=24)])
    plt.gcf().autofmt_xdate()
    save("")

    plt.xlim([starttime, starttime + timedelta(hours=15)])
    plt.ylim([0, 220])
    plt.gcf().autofmt_xdate()
    save("sunny")

    unique_off = sorted(set(off))
    average_on = []
    max_on = []

    for off_val in unique_off:
        relevant_on_values = [on_val for o, on_val in zip(off, on) if abs(o - off_val) <= 7]
        
        if relevant_on_values:
            average_on.append(sum(relevant_on_values) / len(relevant_on_values))
        else:
            average_on.append(None)

        single_on_values = [on_val for o, on_val in zip(off, on) if o == off_val]
        if single_on_values:
            top = max(single_on_values)
            prev = max_on[-1] if max_on else 0
            max_on.append(top if top > prev else prev)
        else:
            max_on.append(None)
    
    x_thresh = list(range(int(max(unique_off)) + 1))
    y_thresh = [max(58, min(i * 1.2 + 22, i + 37)) for i in x_thresh]

    plt.figure(figsize=(6, 6))
    plt.scatter(off, on, color='blue', s=10)
    plt.plot(unique_off, max_on, color='red', label='Max ADC reading when IR LED On')
    plt.plot(x_thresh, y_thresh, color='green', label='Proposed mail detection threshold')

    plt.xlabel('ADC reading when IR LED Off [mV]')
    plt.ylabel('ADC reading when IR LED On [mV]')
    plt.grid(True)
    plt.legend()

    plt.xlim([0, 250])
    plt.ylim([0, 250])
    save("scatter")

    diff_on = [b - a for a, b in zip(unique_off, max_on)]
    diff_thresh = [b - a for a, b in zip(x_thresh, y_thresh)]

    plt.figure(figsize=(6, 6))
    plt.plot(unique_off, diff_on, color='red')
    plt.plot(x_thresh, diff_thresh, color='green', label='Proposed mail detection threshold')

    plt.xlabel('ADC reading when IR LED Off [mV]')
    plt.ylabel('Largest difference in ADC reading when IR LED On [mV]')
    plt.grid(True)
    plt.legend()

    plt.xlim([0, 250])
    plt.ylim([0, 65])
    save("diff")

    plt.close()

current_directory = os.getcwd()
csv_files = [file for file in os.listdir(current_directory) if file == 'infrared.txt']

for csv_file in csv_files:
    plot_infrared(csv_file)
