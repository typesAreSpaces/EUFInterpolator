import numpy as np
import matplotlib.pyplot as plt
plt.style.use('seaborn-whitegrid')

def parse_meta_data(file_name):
    _file = open(file_name, "r")
    for line in _file.readlines():
        num_constants, num_func_names, max_arity, max_a_lists, _, _, num_vars_to_elim = line.split()[1].split("_")
        return (num_constants, num_func_names, max_arity, max_a_lists, num_vars_to_elim)

def parse_info(file_name, _num_elim_vars_data, _data, num_tests):
    _file = open(file_name, "r")
    num_test_count = 0

    for line in _file.readlines():
        if(num_test_count >= num_tests):
            break
        _line = line.split()
        size_line = len(_line)
        if(size_line == 1):
            _num_elim_vars_data[num_test_count] = int(num_vars_to_elim)
            _data[num_test_count] = float(_line[0]) * 1000
            num_test_count += 1
    _file.close()
    return (_num_elim_vars_data, _data)

if __name__ == "__main__":
    num_tests = 100

    steps = np.arange(0, num_tests)
    num_constants, num_func_names, max_arity, max_a_lists, num_vars_to_elim = parse_meta_data("results/iz3_benchmark.txt")
    data_iz3 = parse_info("results/iz3_benchmark.txt", 
            np.zeros(num_tests, dtype=float), np.zeros(num_tests, dtype=float), num_tests)
    data_mathsat = parse_info("results/mathsat_benchmark.txt", 
            np.zeros(num_tests, dtype=float), np.zeros(num_tests, dtype=float), num_tests)
    data_eufi = parse_info("results/eufi_benchmark.txt", 
            np.zeros(num_tests, dtype=float), np.zeros(num_tests, dtype=float), num_tests)

    fig, ax = plt.subplots()
    num_test_points = np.arange(0, num_tests)

    ax.scatter(num_test_points, data_eufi[1], marker='x', color='red', label='EUF Uniform Interpolator')
    ax.scatter(num_test_points, data_mathsat[1], marker='o', color='blue', label='Mathsat')
    ax.scatter(num_test_points, data_iz3[1], marker='+', color='green', label='iZ3')

    ax.set_ylim(0, 12.5)
    ax.set_xlabel('# of random cases used')
    ax.set_ylabel('Time in milliseconds')
    ax.set_title('Performance comparison of interpolant generation algorithms for EUF \n\
            Number of Constants: ' + num_constants + ' Number of (Dis)Equalities: ' + max_a_lists)

    # legend = ax.legend(loc='upper left', shadow=True, fontsize='x-large')
    print(num_constants)
    legend = ax.legend(bbox_to_anchor=(1.05, 1), loc='upper left', borderaxespad=0.)
    plt.savefig("results/plots/eufi_performance_graph_" 
            + num_constants + "_" 
            + num_func_names + "_" 
            + max_arity + "_" 
            + max_a_lists + "_" 
            + str(num_tests) + ".png", dpi=500, bbox_inches='tight')
