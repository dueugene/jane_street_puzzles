# to solve the april 2020 jane street puzzle
# initialize matrix representing dots,
# backtracking algorithm, place triangle
# attempting to right first, then to below, then to right and below
# continue placing until solution found, or no solution found
#
import numpy as np
import time


# returns all i < n in which we can triangulate
def triads(n):
    # known results
    result = []
    for i in range(n):
        seed = 1
        for ind in range(len(result)):
            if result[ind] == i:
                break
            seed = result[ind]+1
        start = time.time()
        success = triangulate(seed, i+1)
        end = time.time()
        print("n: %d, can triangulate: %5r, elapsed time: %.4f" % (i+1, success, end - start))
        if success is True:
            result.append(i+1)
    return result


# attempt to triangulate using backtracking method
def triangulate(seed, N):
    # initialize a grid of dots
    global grid
    global n_dots
    global n
    global seedd
    n = N
    n_dots = int(N * (N + 1) / 2) - int((seed-1) * ((seed-1) + 1) / 2)
    seedd = seed
    grid = np.zeros((N + 1, N + 1), dtype=int)
    for i in range(N):
        for j in range(i + 1):
            grid[i][j] = 1
    # print(grid)
    # print(n)
    # print(n_dots)

    # attempt to solve
    if n_dots % 3 != 0:
        return False
    return solve()


def solve():
    global n_dots
    global n
    global grid
    global seedd
    # print(grid)
    # print(n)
    # print(n_dots)
    result = False
    for i in range(seedd-1, n - 1):
        for j in range(i + 1):
            if grid[i][j] == 1:
                if can_place_triangle(i, j, 0):
                    grid[i][j] = 0
                    grid[i + 1][j] = 0
                    grid[i + 1][j + 1] = 0
                    n_dots -= 3
                    if n_dots == 0:
                        return True
                    result = solve()
                    grid[i][j] = 1
                    grid[i + 1][j] = 1
                    grid[i + 1][j + 1] = 1
                    n_dots += 3
                if (result is False) & (can_place_triangle(i, j, 1)):
                    grid[i][j] = 0
                    grid[i][j + 1] = 0
                    grid[i + 1][j + 1] = 0
                    n_dots -= 3
                    if n_dots == 0:
                        return True
                    result = solve()
                    grid[i][j] = 1
                    grid[i][j + 1] = 1
                    grid[i + 1][j + 1] = 1
                    n_dots += 3
                return result
    return False


def can_place_triangle(i, j, orientation):
    # i,j indices of first node
    # orientation: 0 or 1
    if orientation == 0:
        i2 = i + 1
        j2 = j
        i3 = i + 1
        j3 = j + 1
    else:
        i2 = i
        j2 = j + 1
        i3 = i + 1
        j3 = j + 1
    global grid
    if grid[i][j] == 1:
        if grid[i2][j2] == 1:
            if grid[i3][j3] == 1:
                return True
    return False


triads(39)
