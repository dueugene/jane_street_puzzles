from scraggle import connectivity, grid, place_word, print_grid
def main():
    print_grid(grid)
    # first starting point
    start = [1,2,3,7,8,9,13,14,15]
    end = [8,9,14,15]

    # second starting point
    # start = [8,9,14,15]
    # end = [15,16,21,22]

    # third starting point
    # start = [15,16,21,22]
    # end = [22,23,28,29]

    # fourth starting point
    # start = [22,23,28,29]
    # end = [22,23,24,28,29,30,34,35,36]
    word = 'SOVKHOZY'
    available_chars = 'BCDFGHJKLMNPQRSTVWYXZ'
    flag,new_grid,available_chars2 = place_word(grid,connectivity,start,end,word,available_chars)


    # word = 'YCLEPT'
    # print(available_chars2)
    # place_word(new_grid,connectivity,start,end,word,available_chars2)
    print_grid(grid)
    return

def is_vowel(char):
    return char in 'AEIOU'

if __name__ == '__main__':
    main()
