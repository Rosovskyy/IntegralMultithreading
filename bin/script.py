import subprocess
import sys
import re


def parse_result(result):
    return [float(i) for i in re.findall(r'\[(.+)\]', result)]


def check_results(number_of_solutions, sol_number, mistake):
    return True if number_of_solutions == sol_number else False


def run_multi_thread_solution(times, multi_thread, num_of_threads, mistake, conf):
    results_are_same_1 = True
    result = 0
    min_multi_thread = [0, 0, 0, sys.maxsize]

    for i in range(0, times):
        res_multi = parse_result(subprocess.Popen([multi_thread, num_of_threads, conf],
                                                  stdout=subprocess.PIPE).communicate()[0].decode('utf-8'))

        if not result:
            result = res_multi[0]
        else:
            results_are_same_1 = check_results(result, res_multi[0], mistake)

        if res_multi[3] < min_multi_thread[3]:
            min_multi_thread = res_multi

    for i in min_multi_thread:
        print("[" + str(i) + "]")
    if results_are_same_1:
        print("Results are the same for all the multiple solutions runs")
    else:
        print("Results are NOT the same for all the multiple solutions runs")

    return result


def run_one_thread_solution(times, one_thread, mistake, conf):
    min_one_thread = [0, 0, 0, sys.maxsize]
    results_are_same_2 = True
    result = 0

    for i in range(0, times):
        res_one = parse_result(subprocess.Popen([one_thread, conf], stdout=subprocess.PIPE).communicate()[0].decode('utf-8'))
        if not result:
            result = res_one[0]
        else:
            results_are_same_2 = check_results(result, res_one[1], mistake)

        if res_one[3] < min_one_thread[3]:
            min_one_thread = res_one

    for i in min_one_thread:
        print("[" + str(i) + "]")
    if results_are_same_2:
        print("Results are the same for all the one solutions runs")
    else:
        print("Results are NOT the same for all the one solutions runs")

    return result


def main():
    one_thread = ''
    multi_thread = ''

    if len(sys.argv) < 6 or len(sys.argv) > 7:
        print('Usage: <program_name> <number_of_threads_for_multiple_threads_program>'
              ' <times_to_run_the_program> <multiple_threads/both> <wanted_error> <conf_file> <exe>?')
        exit(-1)

    if len(sys.argv) == 7:
        if sys.argv[6] == 'exe':
            one_thread = './integrate_one_thread.exe'
            multi_thread = './integrate_multi_thread.exe'
        else:
            print('The fourth argument should be "exe" if you want to run .exe files')
            exit(-2)
    elif len(sys.argv) == 6:
        one_thread = './integrate_one_thread'
        multi_thread = './integrate_multi_thread'

    num_of_threads = sys.argv[1]
    times = int(sys.argv[2])
    threads_to_run = sys.argv[3]
    mistake = float(sys.argv[4])
    conf = sys.argv[5]

    if threads_to_run == 'multiple_threads':
        run_multi_thread_solution(times, multi_thread, num_of_threads, mistake, conf)
    elif threads_to_run == 'both':
        number_of_solutions_1 = run_one_thread_solution(times, one_thread, mistake, conf)
        number_of_solutions_2 = run_multi_thread_solution(times, multi_thread, num_of_threads, mistake, conf)

        if number_of_solutions_1 != number_of_solutions_2:
            print("\nResults are NOT the same for one thread and multiple threads solutions!")
        else:
            print("\nResults are the same for one thread and multiple threads solutions!")


if __name__ == '__main__':
    main()
