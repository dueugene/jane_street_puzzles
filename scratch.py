from scraggle import connectivity, grid, place_word, print_grid
def main():
    print_grid(grid)

    # start = [1,2,3,7,8,9,13,14,15]
    # end = [8,9,14,15]
    # word = 'JOKING'

    start = [8,9,14,15]
    end = [15,16,21,22]
    word = 'GROTESQUELY'
    available_chars = 'BCDFGHJKLMNPQRSTVWYXZ'
    flag,new_grid,available_chars2 = place_word(grid,connectivity,start,end,word,available_chars)

    start = [15,16,21,22]
    end = [22,23,28,29]
    word = 'YCLEPT'
    print(available_chars2)
    place_word(new_grid,connectivity,start,end,word,available_chars2)
    print_grid(grid)
    return

def is_vowel(char):
    return char in 'AEIOU'

if __name__ == '__main__':
    main()
