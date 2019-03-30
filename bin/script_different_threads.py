"""
This file is only for programmer! Don't use it if you are not sure how to do it! :)

"""

import subprocess
import sys
import re


def parse_result(result):
    return [float(z) for z in re.findall(r'\[(.+)\]', result)]


"""
Command line arguments: <program_name> <max_threads_num> <number_of_runs>

"""

max_num_of_threads = int(sys.argv[1])
num_of_runs = int(sys.argv[2])
list_of_results = []
mistake = 1

for i in range(1, max_num_of_threads + 1):
        p = parse_result(subprocess.Popen(["python3", "./script.py", str(i), str(num_of_runs), 'multiple_threads',
                                           str(mistake), '../conf.txt'],
                             stdout=subprocess.PIPE).communicate()[0].decode('utf-8'))
        print("For", i, "threads with", num_of_runs, "runs results are (minimum was taken): \t", p[0], "  time: ",
              p[3]/1000000)
        list_of_results.append([p[0], p[3], i])


with open('results_for_plot.txt', 'w') as f:
    for item in list_of_results:
        for i in item:
            f.write("%s " % i)
        f.write("\n")
