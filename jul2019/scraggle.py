# this file contains stuff to solve the scraggle problem
# https://www.janestreet.com/puzzles/current-puzzle/

# setup graph of grid, in dictionary format
# number the grid 1-36, from top left to bottom right
# maybe a better way to encode?
connectivity = { 1  : [ 2,  7,  8 ],
                 2  : [ 1,  3,  7,  8,  9 ],
                 3  : [ 2,  4,  8,  9, 10 ],
                 4  : [ 3,  5,  9, 10, 11 ],
                 5  : [ 4,  6, 10, 11, 12 ],
                 6  : [ 5, 11, 12 ],
                 7  : [ 1,  2,  8, 13, 14 ],
                 8  : [ 1,  2,  3,  7,  9, 13, 14, 15 ],
                 9  : [ 2,  3,  4,  8, 10, 14, 15, 16 ],
                 10 : [ 3,  4,  5,  9, 11, 15, 16, 17 ],
                 11 : [ 4,  5,  6, 10, 12, 16, 17, 18 ],
                 12 : [ 5,  6, 11, 17, 18 ],
                 13 : [ 7,  8, 14, 19, 20 ],
                 14 : [ 7,  8,  9, 13, 15, 19, 20, 21 ],
                 15 : [ 8,  9, 10, 14, 16, 20, 21, 22 ],
                 16 : [ 9, 10, 11, 15, 17, 21, 22, 23 ],
                 17 : [10, 11, 12, 16, 18, 22, 23, 24 ],
                 18 : [11, 12, 17, 23, 24 ],
                 19 : [13, 14, 20, 25, 26 ],
                 20 : [13, 14, 15, 19, 21, 25, 26, 27 ],
                 21 : [14, 15, 16, 20, 22, 26, 27, 28 ],
                 22 : [15, 16, 17, 21, 23, 27, 28, 29 ],
                 23 : [16, 17, 18, 22, 24, 28, 29, 30 ],
                 24 : [17, 18, 23, 29, 30 ],
                 25 : [19, 20, 26, 31, 32 ],
                 26 : [19, 20, 21, 25, 27, 31, 32, 33 ],
                 27 : [20, 21, 22, 26, 28, 32, 33, 34 ],
                 28 : [21, 22, 23, 27, 29, 33, 34, 35 ],
                 29 : [22, 23, 24, 28, 30, 34, 35, 36 ],
                 30 : [23, 24, 29, 35, 36 ],
                 31 : [25, 26, 32],
                 32 : [25, 26, 27, 31, 33],
                 33 : [26, 27, 28, 32, 34],
                 34 : [27, 28, 29, 33, 35],
                 35 : [28, 29, 30, 34, 36],
                 36 : [29, 30, 35 ]
                 }
# indicates initial playing board
grid = { 1  : [],
         2  : ['O'],
         3  : [],
         4  : ['E'],
         5  : [],
         6  : ['U'],
         7  : ['I'],
         8  : [],
         9  : ['A'],
         10 : [],
         11 : ['A'],
         12 : [],
         13 : [],
         14 : ['E'],
         15 : [],
         16 : ['I'],
         17 : [],
         18 : ['O'],
         19 : ['A'],
         20 : [],
         21 : ['O'],
         22 : [],
         23 : ['E'],
         24 : [],
         25 : [],
         26 : ['E'],
         27 : [],
         28 : ['A'],
         29 : [],
         30 : ['I'],
         31 : ['U'],
         32 : [],
         33 : ['I'],
         34 : [],
         35 : ['O'],
         36 : []
         }

def print_grid(grid):
    for num,char in grid.items():
        if not char:
            print('*',end=',')
        else:
            print(char[0],end=',')
        if (num%6 == 0):
            print(' ')
    return

def place_word(grid,connectivity,start,end,word,available_chars):
    print(word)
    print(available_chars)
    # check if word list is empty
    if len(word) == 0:
        return True, grid, available_chars

    # check if the end is still valid
    # print(end)
    # print(word[-1])
    valid_ending = False
    for index in end:
        if not grid[index]:
            valid_ending = True
        else:
            valid_ending = (word[-1] == grid[index][0])
        if valid_ending:
            break
    if not valid_ending:
        print('not a valid ending')
        return False, grid, available_chars

    # place next word on board
    next_char = word[0];
    # check if next_char is available to place on board
    if next_char in available_chars:
        # loop through each available slot insert character if slot is empty
        print(start)
        for index in start:
            if not grid[index]:
                grid[index] = next_char
                next_start = connectivity[index]
                print(next_start)
                if len(word[1:])==1:
                    next_start = intersection(next_start,end)
                flag,temp1,temp2 = place_word(grid,connectivity,next_start,end,word[1:],available_chars.replace(next_char, ''))
                # check if succseful
                if flag:
                    available_chars = temp2
                    new_grid = temp1
                    return True, new_grid, available_chars;
                else:
                    grid[index] = [];
    # check if next_char is already placed in starting points
    else:
        print('looking for ' + next_char + ' on board')
        # print(start)
        # print_grid(grid)
        for index in start:
             if next_char in grid[index]:
                 next_start = connectivity[index]
                 flag,temp1,temp2 = place_word(grid,connectivity,next_start,end,word[1:],available_chars)
                 # check if succseful
                 if flag:
                     available_chars = temp2
                     new_grid = temp1
                     return True, new_grid, available_chars;

    return False, grid, available_chars;

def intersection(lst1, lst2):
    # taken from  https://www.geeksforgeeks.org/python-intersection-two-lists/
    # Use of hybrid method
    temp = set(lst2)
    lst3 = [value for value in lst1 if value in temp]
    return lst3
