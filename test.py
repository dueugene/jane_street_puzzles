import matplotlib.pyplot as plt
import numpy as np
import itertools

from matplotlib.table import Table


def main():
    data0 = np.zeros((9, 9), dtype=int)
    data0[8][0] = 9
    data0[7][0] = 8
    data0[6][0] = 8
    data0[5][0] = 8
    data0[4][0] = 8

 

    data_left = np.array([0, 0, 25, 0, 15, 0, 26, 0, 0])
    data_right = np.array([25, 0, 0, 0, 10, 0, 0, 0, 27])
    data_top = np.array([41, 8, 0, 0, 14, 0, 15, 0, 0])
    data_bottom = np.array([0, 9, 0, 17, 0, 15, 0, 35, 0])
    data0_outer = np.array([data_left, data_right, data_top, data_bottom])

    plot_grid(data0, data0_outer)
    # allowed_by_colsum(data0, data0_outer)
    # allowed_by_rowsum(data0, data0_outer)
    plt.show()


def allowed_by_colsum(data0, data0_outer):
    data_top = data0_outer[2]
    data_bottom = data0_outer[3]
    col_sum = data_top + sum(data0) + data_bottom
    print(20 - col_sum)


def allowed_by_rowsum(data0, data0_outer):
    data_left = data0_outer[0]
    data_right = data0_outer[1]
    row_sum = data_left + sum(np.transpose(data0)) + data_right
    print(20 - row_sum)

def GetFactorialAdd(Target):
    numbers = [1,2,2,3,3,3,4,4,4,4,5,5,5,5,5,6,6,6,6,6,7,7,7,7,8,9]

    result = [seq for i in range(len(numbers), 0, -1) for seq in itertools.combinations(numbers, i) if sum(seq) == Target]

    FixedResults = []

    for item in result:
        cVals = np.array([item.count(1),item.count(2),item.count(3),item.count(4),item.count(5),item.count(6),item.count(7)])
        if np.shape(np.where(cVals>=2))[1]<1:
            FixedResults.append(item)
        else:
            RepeatVal = np.where(cVals>=2)[0][-1]
            if np.shape(np.where(cVals[:RepeatVal]>=1))[1]<1:
                FixedResults.append(item)


    FixedResultsSet = set(FixedResults)

    Fixed = FixedResultsSet.copy()

    return Fixed


def plot_grid(data0, data0_outer, fmt='{:d}', bkg_colors=['lightgray', 'white']):
    fig, ax = plt.subplots()
    ax.set_axis_off()
    tb = Table(ax, bbox=[0, 0, 1, 1])

    n_rows, n_cols = data0.shape
    width, height = 1.0 / n_cols, 1.0 / n_rows

    # Add cells
    for (i, j), val in np.ndenumerate(data0):
        # Index either the first or second item of bkg_colors based on
        # a checker board pattern
        idx = [j % 2, (j + 1) % 2][i % 2]
        color = bkg_colors[idx]

        tb.add_cell(i, j, width, height, text=fmt.format(val) if val != 0 else ' ', loc='center', facecolor=color)

    # Row Labels...
    for i, label in enumerate(data0_outer[0]):
        tb.add_cell(i, -1, width, height, text=label if label != 0 else ' ', loc='right', edgecolor='none', facecolor='none')
    for i, label in enumerate(data0_outer[1]):
        tb.add_cell(i, 10, width, height, text=label if label != 0 else ' ', loc='left', edgecolor='none', facecolor='none')

    # Column Labels...
    for j, label in enumerate(data0_outer[2]):
        tb.add_cell(-1, j, width, height/2, text=label if label != 0 else ' ', loc='center', edgecolor='none', facecolor='none')
    for j, label in enumerate(data0_outer[3]):
        tb.add_cell(10, j, width, height/2, text=label if label != 0 else ' ', loc='center', edgecolor='none', facecolor='none')
    ax.add_table(tb)
    return fig


if __name__ == '__main__':
    main()
