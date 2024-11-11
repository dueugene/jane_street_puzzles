
print("hello")

# load grid that looks like this
# ABBCCC
# ABBCCC
# AABBCC
# AABBCC
# AAABBC
# AAABBC

def load_grid(filepath):
    grid = []
    with open(filepath, 'r') as file:
        lines = file.readlines()
        for line in lines:
            row = list(line.strip())
            grid.append(row)
    return grid

def in_bounds(position, n, m):
    x, y = position
    return 0 <= x < n and 0 <= y < m

def knights_moves(position, n, m):
    x, y = position
    moves = [(x+1, y+2), (x+1, y-2), (x-1, y+2), (x-1, y-2),
             (x+2, y+1), (x+2, y-1), (x-2, y+1), (x-2, y-1)]
    return [move for move in moves if in_bounds(move, n, m)]


from dataclasses import dataclass
@dataclass
class MoveContents:
    prev_c: str
    path: list
    visited: set
    curr_score: int
    scoring: dict
    row_map: list
    col_map: list

def find_all_knights_moves(grid, curr, end, mc):
    mc.visited.add(curr)
    mc.path.append(curr)
    x, y = curr
    c = grid[x][y]
    pc = mc.prev_c
    prev_score = mc.curr_score
    if c == mc.prev_c:
        curr_score = prev_score + mc.scoring[c]
    else:
        curr_score = prev_score * mc.scoring[c]
    if curr == end and curr_score == 2024:
        for p in mc.path:
            print(mc.col_map[p[1]]+mc.row_map[p[0]], end=",")
        print("")
        mc.path.pop()
        mc.visited.remove(curr)
        # count = count + 1
        return True
    if curr_score > 2024:
        mc.path.pop()
        mc.visited.remove(curr)
        return False

    n = len(grid)
    m = len(grid[0])
    mc.prev_c = c
    mc.curr_score = curr_score
    for next in knights_moves(curr, n, m):
        if next not in mc.visited:
            if find_all_knights_moves(grid, next, end, mc):
                mc.path.pop()
                mc.visited.remove(curr)
                mc.prev_c = pc
                mc.curr_score = prev_score
                return True
    mc.path.pop()
    mc.visited.remove(curr)
    mc.prev_c = pc
    mc.curr_score = prev_score

def pairs_sum_to_n_or_less(n, banned):
    pairs = []
    for i in range(1, n):
        if i is not banned:
            for j in range(i + 1, n):
                if j is not banned:
                    if i + j <= n:
                        pairs.append((i, j))
    return pairs

if __name__ == "__main__":
    file_path = "grid.txt"
    grid = load_grid(file_path)
    for row in grid:
        print(row)
    print(grid[0][1])
    mc = MoveContents("A", [], set(), 0, {}, "654321", "abcdef")
    # observations
    # 1. lowest possible sum of distinct A, B, and C is 6
    # therefore best possible value is some combo of 1, 2, 3
    # 2. to jump from A->....->C we end up with (prev score) * C
    # or (prev score)*C + C +C... which factors into (prev_score + x)*C, where x is some numbner of C's
    # therfore c must be a factor of 2024
    # here are the possible factors less than 50
    possible_c = [46, 44, 23, 22, 11, 8, 4, 2, 1]
    # 3. leaderboard says A+B+C < 50 so we start there
    best_sum = 50
    for c in possible_c:
        remaining  = best_sum-c-1
        for a, b in pairs_sum_to_n_or_less(remaining, c):
            mc.scoring = {'A': a, 'B': b, 'C': c}
            print(a,end=",")
            print(b,end=",")
            print(c,end=",")
            if find_all_knights_moves(grid, (5, 0), (0, 5), mc) and find_all_knights_moves(grid, (0, 0), (5, 5), mc):
                print("found a a solution!!!")
                # after finding a solution, we only need to find better solutions
                best_sum = a + b + c
                break
            else:
                # if no solutions are found, we should swap a and b as that changes the layout of the board
                # but after running this algorithm I found the lowest possible sum of A, B, and C
                # so no need to go further
                print("no solution")
    print("end")