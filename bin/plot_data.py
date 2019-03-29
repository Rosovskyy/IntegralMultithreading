import re
import numpy as np
import matplotlib.pyplot as plt


def get_data(file_name):
    text_file = open(file_name, "r")

    result = text_file.readlines()
    data = []
    for i in result:
        data.append([int(i) for i in re.findall(r'(\d+)', i)])
    text_file.close()

    return data


def plot(data):
    print(data)
    x = [i[4] for i in data]
    y = [i[2]/1000000 for i in data]

    fig = plt.figure()
    ax1 = fig.add_subplot(111)

    ax1.set_title("The correlation between number of threads and time")
    ax1.set_xlabel('Number of threads')
    ax1.set_ylabel('Time in seconds')

    ax1.plot(x, y, c='r')
    plt.show()


def main():
    plot(get_data('results_for_plot.txt'))


if __name__ == '__main__':
    main()
