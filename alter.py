
# plays a number between 1-10, to win the game
# assumes that it was allowed to go first
def make_move(sum,last_number):
    if sum == 0:
        return 3
    else:
        return (((99 - sum - 1) % 12) % 10) + 1

def play_against_alter():
    print('play against alter! ...')
    print('play a number between 1-10')
    print('first to make sum 100 or greater loses')
    print('sum of last 2 played numbers cannot equal 11')
    print('alter goes first... \n')

    sum = 0
    your_move = 0
    while (sum < 100):
        alter_move = make_move(sum,your_move)
        # check validity of move
        is_valid_move = check_valid_move(alter_move,your_move)
        if (is_valid_move == False):
            raise ValueError('alter made an invalid move')

        sum = sum + alter_move
        print('alter plays: %2d, current sum: %3d' % (alter_move,sum))

        if (sum >= 100):
            print('you win!')
            return True

        # ask for move
        print('your turn:')
        your_move = int(input())
        # check validity of move
        is_valid_move = check_valid_move(your_move,alter_move)
        while (is_valid_move == False):
            print('invalid move! try again:')
            your_move = int(input())
            # check validity of move
            is_valid_move = check_valid_move(your_move,alter_move)

        sum = sum + your_move
        print('your move: %2d, current sum: %3d' % (your_move,sum))

    print('you lose!')
    return False

def check_valid_move(curr_move,prev_move):
    if (curr_move < 1):
        return False
    if (curr_move > 10):
        return False
    if (curr_move + prev_move == 11):
        return False
    return True
