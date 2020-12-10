#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <queue>

class TwentyFourSeven {

public:
  TwentyFourSeven(std::vector<std::vector<int>> givens, std::vector<std::vector<int>> rows, std::vector<std::vector<int>> cols);
  bool solve();
  void print();
  std::vector<std::vector<int>> get_board();
  
  bool is_valid_board() const;
private:

  void insert_number(const int i, const int j, const int n);
  bool can_place_number(const int i, const int j, const int n);
  bool is_continuous_land() const;
  void restrict_based_on_seens(std::vector<std::vector<bool>>& avail_nums, std::vector<std::vector<int>>& seens);
  void restrict_based_on_placed(std::vector<std::vector<bool>>& avail_nums, const std::vector<int>& sums, const std::vector<int>& counts);
  std::vector<bool> make_two_sum(int sum, std::vector<bool> can_use);

  
  // represents the 7x7 playing board
  std::vector<std::vector<int>> board = std::vector<std::vector<int>> (7, std::vector<int> (7, -1));
  // list of available numbers to place
  std::vector<int> avail_nums = {21,1,2,3,4,5,6,7};
  // 7x2 representing first number to be seen in the rows
  std::vector<std::vector<int>> row_seens;
  // 7x2 representing first number to be seen in the cols
  std::vector<std::vector<int>> col_seens;
  // running total of current sum in the rows, count of numbers, and empty cells
  std::vector<int> row_sums = std::vector<int> (7, 0);
  std::vector<int> row_count = std::vector<int> (7, 0);
  std::vector<int> row_empty = std::vector<int> (7, 7);
  // running total of current sum in the cols, count of numbers, and empty cells
  std::vector<int> col_sums = std::vector<int> (7, 0);
  std::vector<int> col_count = std::vector<int> (7, 0);
  std::vector<int> col_empty = std::vector<int> (7, 7);

  // numbers that can be placed specific to each row and col
  std::vector<std::vector<bool>> row_avail_nums = std::vector<std::vector<bool>> (7, std::vector<bool> (8, true));
  std::vector<std::vector<bool>> col_avail_nums = std::vector<std::vector<bool>> (7, std::vector<bool> (8, true));
  
  
};

TwentyFourSeven::TwentyFourSeven(std::vector<std::vector<int>> givens, std::vector<std::vector<int>> rows, std::vector<std::vector<int>> cols) {
  // populate board with givens
  for (int k = 0; k < givens.size(); k++) {
    int i = givens[k][0];
    int j = givens[k][1];
    int n = givens[k][2];
    insert_number(i, j, n);

    // snippet targeted specifically for n = 1,
    if (n == 1) {
      // restrict dof in current row and col
      row_avail_nums[i][1] = false;
      row_avail_nums[i][2] = false;
      row_avail_nums[i][3] = false;
      row_avail_nums[i][4] = false;
      col_avail_nums[j][1] = false;
      col_avail_nums[j][2] = false;
      col_avail_nums[j][3] = false;
      col_avail_nums[j][4] = false;
      // restrict degrees of freedom related to other rows/cols
    }
  }
  row_seens = rows;
  col_seens = cols;
  // manual pruning, for puzzle 4

  // perform pre-processing, for the purpose of pruning the number of cases to test
  restrict_based_on_placed(row_avail_nums, row_sums, row_count);
  restrict_based_on_placed(col_avail_nums, col_sums, col_count);
  
  restrict_based_on_seens(row_avail_nums, row_seens);
  restrict_based_on_seens(col_avail_nums, col_seens);

  restrict_based_on_placed(row_avail_nums, row_sums, row_count);
  restrict_based_on_placed(col_avail_nums, col_sums, col_count);

  
}

// generalizes the procedure of eleiminating which numbers can be placed in each row/col based on the seen constraints
void TwentyFourSeven::restrict_based_on_seens(std::vector<std::vector<bool>>& avail_nums, std::vector<std::vector<int>>& seens) {  
  for (int i = 0; i < 7; i++) {
    // 1 is very restricted, remove them where they cannot be placed
    if ((seens[i][0] < 5 && seens[i][0] > 1) || (seens[i][1] < 5 && seens[i][1] > 1)) {
      // 1 cannot be in the same row as 2,3 or 4
      avail_nums[i][1] = false;
    }
    if (seens[i][0] == 1 || seens[i][1] == 1) {
      // if 1 is a seen constraint, remove other nums
      avail_nums[i][2] = false;
      avail_nums[i][3] = false;
      avail_nums[i][4] = false;
    }

    // if there is one seen
    if (seens[i][0] > 0 || seens[i][1] > 0) {
      int sum = seens[i][0] + seens[i][1];
      if (sum == 1) {
        // single seen constrain of 1
        avail_nums[i][2] = false;
        avail_nums[i][3] = false;
        avail_nums[i][4] = false;
      } else if (sum == 3) {
        // single seen contraint 3
        avail_nums[i][1] = false;
        avail_nums[i][2] = false;
        
      } else if (sum == 4) {
        avail_nums[i][1] = false;
      }

    }

    // if there are 2 seens, find possible ways to make remainder
    if (seens[i][0] > 0 && seens[i][1] > 0) {
      int remainder = 20 - seens[i][0] - seens[i][1];
      avail_nums[i] = make_two_sum(remainder, avail_nums[i]);
      avail_nums[i][seens[i][0]] = true;
      avail_nums[i][seens[i][1]] = true;
    }

  }
  return;
}

// eliminates numbers can be placed in each row/col based on the interaction of whats placed on the board, and the seens
void TwentyFourSeven::restrict_based_on_placed(std::vector<std::vector<bool>>& avail_nums_loc, const std::vector<int>& sums, const std::vector<int>& counts) {
  for (int n = 1; n < 8; n++) {
    if (avail_nums[n] == 0) {
      for (int i = 0; i < 7; i++) {
        avail_nums_loc[i][n] = false;
      }
    }
  }

  for (int i = 0; i < 7; i++) {
    // if there are 2 numbers in the row/col already, check the possible ways to make remainder
    if (counts[i] == 2) {
      int remainder = 20 - sums[i];
      avail_nums_loc[i] = make_two_sum(remainder, avail_nums_loc[i]);
    }
    if (counts[i] == 1) {
      if (sums[i] == 1) {
         // single given of 1
        avail_nums_loc[i][2] = false;
        avail_nums_loc[i][3] = false;
        avail_nums_loc[i][4] = false;
      } else if (sums[i] == 3) {
        // single given of  3
        avail_nums_loc[i][1] = false;
        avail_nums_loc[i][2] = false;
        
      } else if (sums[i] == 4) {
        avail_nums_loc[i][1] = false;
      }
    }
    
    
  }
  return;
}

std::vector<bool> TwentyFourSeven::make_two_sum(int sum, std::vector<bool> can_use) {
  std::vector<bool> result(8, false);
  result[0] = true;
  for (int i = 1; i < 8; i++) {
    if (can_use[i]) {
      for (int j = i; j < 8; j++) {
        if (can_use[j] && i + j == sum) {
          result[i] = true;
          result[j] = true;
        }
      }
    }
  }
  return result;
}

// solve board using a guess and check method with backtracking
bool TwentyFourSeven::solve() {
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 7; j++) {
      if (board[i][j] == -1) {
        // loop through available numbers
        for (int n = 7; n >= 0; n--) {
          if (row_avail_nums[i][n] && col_avail_nums[j][n] && can_place_number(i, j, n)) {
            insert_number(i, j, n);
            if(solve()) {return true;}
            // "backtrack"
            insert_number(i, j, -1);
          }
        }
        return false;
      }
    }
  }
  // reached end of board, must perform continuos land constraint check
  return is_continuous_land();
}

// checks to see if the numbers placed forms one continuous "land mass"
// this assumes that all 28 numbers have been placed on the board
bool TwentyFourSeven::is_continuous_land() const {
  std::queue<int> to_check;
  // find first ind of a number
  for (int j = 0; j < 7; j++) {
    if (board[0][j] > 0) {
      to_check.push(j);
      break;
    }
  }
  int count = 0;
  std::vector<bool> visited(49, false);
  while (!to_check.empty()) {
    int ind = to_check.front();
    to_check.pop();
    visited[ind] = true;
    int i = ind / 7;
    int j = ind % 7;
    if (i-1 >= 0 && board[i-1][j] > 0 && visited[(i-1)*7 + j] == false) {to_check.push((i-1)*7 + j);}
    if (i+1 < 7 && board[i+1][j] > 0 && visited[(i+1)*7 + j] == false) {to_check.push((i+1)*7 + j);}
    if (j-1 >= 0 && board[i][j-1] > 0 && visited[i*7 + j-1] == false) {to_check.push(i*7 + j-1);}
    if (j+1 < 7 && board[i][j+1] > 0 && visited[i*7 + j+1] == false) {to_check.push(i*7 + j+1);}
    count++;
  }
  return count == 28;
}

// print out the board to std::cout
void TwentyFourSeven::print() {
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 7; j++) {
      std::cout << std::setw(2) << board[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

// insert number n at location i,j
// also updates all vectors that keep track of sums, counts etc.
void TwentyFourSeven::insert_number(const int i, const int j, const int n) {
  int prev = board[i][j];
  board[i][j] = -1;
  if (prev >= 0) {
    avail_nums[prev]++;
    row_sums[i] -= prev;
    row_empty[i]++;
    col_sums[j] -= prev;
    col_empty[j]++;
    if (prev > 0) {
      row_count[i]--;
      col_count[j]--;
    }
  }
  
  if (n >= 0) {
    board[i][j] = n;
    avail_nums[n]--;

    row_sums[i] += n;
    row_empty[i]--;

    col_sums[j] += n;
    col_count[j]++;
    if (n > 0) {
      row_count[i]++;
      col_empty[j]--;
    }
  }
  
}

// confirms that the number n can be placed on board at index i,j
bool TwentyFourSeven::can_place_number(const int i, const int j, const int n) {
  // check n is within range
  if (n < 0 || n > 7) {return false;}
  // check location is empty
  if (board[i][j] != -1) {return false;}
  
  // check that n is still available
  if (avail_nums[n] < 1) {return false;}
  
  // check count and sum for row i
  if (row_sums[i] + n > 20) {return false;}
  if (n != 0 && row_count[i] == 4) {return false;}
  if (n == 0 && row_empty[i] < (4 - row_count[i]) + 1) {return false;}

  // check count and sum for col j
  if (col_sums[j] + n > 20) {return false;}
  if (n != 0 && col_count[j] == 4) {return false;}
  if (n == 0 && col_empty[j] < (4 - col_count[j]) + 1) {return false;}
    
  // check that inserting a number would not make a 2 x 2 filled square
  board[i][j] = n;
  if (n > 0) {
    for (int ii = std::max(i - 1, 0); ii < std::min(i, 6); ++ii) {
      for(int jj = std::max(j - 1, 0); jj < std::min(j, 6); ++jj) {
        bool test = false;
        test = test || (board[ii][jj] <= 0);
        test = test || (board[ii+1][jj] <= 0);
        test = test || (board[ii][jj+1] <= 0);
        test = test || (board[ii+1][jj+1] <= 0);
        if (test == false) {
          board[i][j] = -1;
          return false;
        }
      }
    }
  }
  board[i][j] = -1;
  
  // check that seen constraints are not violated
  board[i][j] = n;
  if (row_seens[i][0] > 0) {
    // check the first 4 numbers looking from left
    for (int k = 0; k < 4; k++) {
      if (board[i][k] != 0) {
         if (!(board[i][k] == row_seens[i][0] || board[i][k] == -1)) {
           board[i][j] = -1;
           return false;
         }
        break;
      }
    }
   
  }
  if (row_seens[i][1] > 0) {
    // check the first 4 numbers looking from right
    for (int k = 6; k >= 3; k--) {
      if (board[i][k] != 0) {
        if (!(board[i][k] == row_seens[i][1] || board[i][k] == -1)) {
          board[i][j] = -1;
          return false;
        }
        break;
      }
    }
  }
  if (col_seens[j][0] > 0) {
    // check the first 4 numbers looking from top
    for (int k = 0; k < 4; k++) {
      if (board[k][j] != 0) {
        if (!(board[k][j] == col_seens[j][0] || board[k][j] == -1)) {
          board[i][j] = -1;
          return false;
        }
        break;
      }
    }
  }
  // check the first 4 numbers looking from bot
  if (col_seens[j][1] > 0) {
    for (int k = 6; k >= 3; k--) {
      if (board[k][j] != 0) {
        if (!(board[k][j] == col_seens[j][1] || board[k][j] == -1)) {
          board[i][j] = -1;
          return false;
        }
        break;
      }
    }
  }  
  board[i][j] = -1;
  return true;
}

bool TwentyFourSeven::is_valid_board() const {
  // confirm at most 4 numbers in each row and column
  // simultaneously confirm that every row and column does not sum past 20
  // check the rows
  for (int i = 0; i < 7; i++) {
    int count = 0;
    int sum = 0;
    for (int j = 0; j < 7; j++) {
      if (board[i][j] > 0) {
        count++;
        sum += board[i][j];
      }
    }
    if (sum > 20) {return false;}
    if (count > 4) {return false;}
  }
  // check the columns
  for (int j = 0; j < 7; j++) {
    int count = 0;
    int sum = 0;
    for (int i = 0; i < 7; i++) {  
      if (board[i][j] > 0) {
        count++;
        sum += board[i][j];
      }
    }
    if (sum > 20) {return false;}
    if (count > 4) {return false;}
  }
  
  // confirm each 2 x 2 square has at least one empty square
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      bool test = false;
      test = test || (board[i][j] <= 0);
      test = test || (board[i+1][j] <= 0);
      test = test || (board[i][j+1] <= 0);
      test = test || (board[i+1][j+1] <= 0);
      if (test == false) {return false;}
    }
  }

  // verify that the seens along each row and col match
  // row wise
  for (int i = 0; i < 7; i++) {
    // check left side
    if (row_seens[i][0] > 0) {
      int j = 0;
      while (j < 7 && board[i][j] == 0) {
        j++;
      }
      if (j < 7 && !(board[i][j] == row_seens[i][0] || board[i][j] == -1)) {return false;}
    }
    // check right side
    if (row_seens[i][1] > 0) {
      int j = 6;
      while (j >= 0 && board[i][j] == 0) {
        j--;
      }
      if (j >= 0 && !(board[i][j] == row_seens[i][1] || board[i][j] == -1)) {return false;}
    }
  }
  // col wise
  for (int j = 0; j < 7; j++) {
    // check top side
    if (col_seens[j][0] > 0) {
      int i = 0;
      while (i < 7 && board[i][j] == 0) {
        i++;
      }
      if (i < 7 && !(board[i][j] == col_seens[j][0] || board[i][j] == -1)) {return false;}
    }
    // check bot side
    if (col_seens[j][1] > 0) {
      int i = 6;
      while (i >= 0 && board[i][j] == 0) {
        i--;
      }
      if (i >= 0 && !(board[i][j] == col_seens[j][1] || board[i][j] == -1)) {return false;}
    }
  }
  return true;
}

std::vector<std::vector<int>> TwentyFourSeven::get_board() {
  return board;
}

  
