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

  bool solve_bruteforce();
  void insert_number(const int i, const int j, const int n);
  bool can_place_number(const int i, const int j, const int n);
  bool is_continuous_land() const;
  bool restrict_available_nums();
  std::vector<bool> make_two_sum(int sum, std::vector<bool> can_use);
  std::vector<std::pair<int,int>> make_two_sum(int sum, int can_use);
  // returns the current board, but columwise
  std::vector<std::vector<int>> columnwise_board();
      
  std::vector<int> deduce_required_nums(const std::vector<int>& curr, const std::vector<int>& seens) const;
  
  // represents the 7x7 playing board
  std::vector<std::vector<int>> board = std::vector<std::vector<int>> (7, std::vector<int> (7, -1));
  // list of available numbers to place
  std::vector<int> avail_nums = {21,1,2,3,4,5,6,7};
  int to_place_count = 28;
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
  
  std::vector<unsigned int> row_can_use = std::vector<unsigned int> (7, 255);
  std::vector<unsigned int> col_can_use = std::vector<unsigned int> (7, 255);
  
  
};

std::vector<std::vector<int>> TwentyFourSeven::columnwise_board() {
  std::vector<std::vector<int>> ans = board;
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 7; j++) {
      ans[i][j] = board[j][i];
    }
  }
  return ans;
}

TwentyFourSeven::TwentyFourSeven(std::vector<std::vector<int>> givens, std::vector<std::vector<int>> rows, std::vector<std::vector<int>> cols) {
  // populate board with givens
  for (int k = 0; k < givens.size(); k++) {
    int i = givens[k][0];
    int j = givens[k][1];
    int n = givens[k][2];
    insert_number(i, j, n);
  }
  row_seens = rows;
  col_seens = cols;

  // perform pre-processing, for the purpose of pruning the number of cases to test
  restrict_available_nums();
  
}

bool TwentyFourSeven::restrict_available_nums() {
  // create a local "snapshot" copy of avail_nums
  std::vector<int> avail_nums_loc = avail_nums;
  std::vector<unsigned int> row_can_use_loc = row_can_use;
  std::vector<unsigned int> col_can_use_loc = col_can_use;
  
  // perform row wise restrictions
  for (int i = 0; i < 7; i++) {
    std::vector<int> known(8, 0);    // counts known numbers, already placed
    std::vector<int> required(8, 0); // counts required numbers, dedcued by logic, whos location is not yet deducable
    std::vector<int> curr_row = board[i];
    for (int n : curr_row) {
      if (n > 0) {
        known[n]++;
      }
    }

    // check seen constraints coupled with curr_row
    required = deduce_required_nums(curr_row, row_seens[i]);
    for (int n = 1; n < 8; n++) {
      avail_nums_loc[n] -= required[n];
      if (avail_nums_loc[n] < 0) {return false;}
      if (avail_nums_loc[n] == 0) {
        // row_can_use_loc[i] &= ~(1 << n);
      }
    }
    
    int count = 0;
    int sum = 0;
    for (int n = 1; n < 8; n++) {
      // combine required with known
      known[n] += required[n];

      // caluclate count and sum
      count += known[n];
      sum += n*known[n];
    }
    int remainder = 20 - sum;
    std::cout << "Row: " << i << ", " << count << ", " << sum << std::endl;
    
    switch (count) {
    case 4:
      // most restrictive
      for (int n = 1; n < 8; n++) {
        if (known[n] == 0) {
          row_can_use_loc[i] &= ~(1 << n);
        }
      }
      break;
    case 3:
      for (int n = 1; n < 8; n++) {
        if (n != remainder && known[n] == 0) {
          row_can_use_loc[i] &= ~(1 << n);
        }
      }
      break;
    case 2:
      {
      // make a two sum
      std::vector<std::pair<int, int>> pairs = make_two_sum(remainder, row_can_use_loc[i]);
      row_can_use_loc[i] = 1;
      for (int n = 1; n < 8; n++) {
        if (known[n] > 0) {
          row_can_use_loc[i] |= 1 << n;
        }
      }
      for (auto p : pairs) {
        if (avail_nums_loc[p.first] && avail_nums_loc[p.second]) {
          row_can_use_loc[i] |= 1 << p.first;
          row_can_use_loc[i] |= 1 << p.second;
        }
      }
      break;
      }
    case 1:
      if (sum == 1) {
        // single given of 1
        row_can_use_loc[i] &= ~(1 << 2);
        row_can_use_loc[i] &= ~(1 << 3);
        row_can_use_loc[i] &= ~(1 << 4);
      } else if (sum == 2) {
        row_can_use_loc[i] &= ~(1 << 1);
        row_can_use_loc[i] &= ~(1 << 3);
      } else if (sum == 3) {
        // single given of 3
        row_can_use_loc[i] &= ~(1 << 1);
        row_can_use_loc[i] &= ~(1 << 2);
      } else if (sum == 4) {
        // single given of 4
        row_can_use_loc[i] &= ~(1 << 1);
      }
      break;
    case 0:
      break;
    default:
      std::cout << "error in preprocessing" << std::endl;
    }
    
  } 

  // reset avail nums
  avail_nums_loc = avail_nums;
  auto columnwise = this->columnwise_board();
  
  // perform col wise restrictions
  for (int i = 0; i < 7; i++) {
    std::vector<int> known(8, 0);    // counts known numbers, already placed
    std::vector<int> required(8, 0); // counts required numbers, dedcued by logic, whos location is not yet deducable
    std::vector<int> curr_col = columnwise[i];
    for (int n : curr_col) {
      if (n > 0) {
        known[n]++;
      }
    }

    // check seen constraints coupled with curr_col
    required = deduce_required_nums(curr_col, col_seens[i]);
    for (int n = 1; n < 8; n++) {
      avail_nums_loc[n] -= required[n];
      if (avail_nums_loc[n] < 0) {return false;}
      if (avail_nums_loc[n] == 0) {
        // col_can_use_loc[i] &= ~(1 << n);
      }
    }
    
    int count = 0;
    int sum = 0;
    for (int n = 1; n < 8; n++) {
      // combine required with known
      known[n] += required[n];

      // caluclate count and sum
      count += known[n];
      sum += n*known[n];
    }
    int remainder = 20 - sum;
    std::cout << "Col: " << i << ", " << count << ", " << sum << std::endl;
    
    switch (count) {
    case 4:
      // most restrictive
      for (int n = 1; n < 8; n++) {
        if (known[n] == 0) {
          col_can_use_loc[i] &= ~(1 << n);
        }
      }
      break;
    case 3:
      for (int n = 1; n < 8; n++) {
        if (n != remainder && known[n] == 0) {
          col_can_use_loc[i] &= ~(1 << n);
        }
      }
      break;
    case 2:
      {
      // make a two sum
      std::vector<std::pair<int, int>> pairs = make_two_sum(remainder, col_can_use_loc[i]);
      col_can_use_loc[i] = 1;
      for (int n = 1; n < 8; n++) {
        if (known[n] > 0) {
          col_can_use_loc[i] |= 1 << n;
        }
      }
      for (auto p : pairs) {
        if (avail_nums_loc[p.first] && avail_nums_loc[p.second]) {
          col_can_use_loc[i] |= 1 << p.first;
          col_can_use_loc[i] |= 1 << p.second;
        }
      }
      break;
      }
    case 1:
      if (sum == 1) {
        // single given of 1
        col_can_use_loc[i] &= ~(1 << 2);
        col_can_use_loc[i] &= ~(1 << 3);
        col_can_use_loc[i] &= ~(1 << 4);
      } else if (sum == 2) {
        col_can_use_loc[i] &= ~(1 << 1);
        col_can_use_loc[i] &= ~(1 << 3);
      } else if (sum == 3) {
        // single given of 3
        col_can_use_loc[i] &= ~(1 << 1);
        col_can_use_loc[i] &= ~(1 << 2);
      } else if (sum == 4) {
        // single given of 4
        col_can_use_loc[i] &= ~(1 << 1);
      }
      break;
    case 0:
      break;
    default:
      std::cout << "error in preprocessing" << std::endl;
    }
    
  }
  row_can_use = row_can_use_loc;
  col_can_use = col_can_use_loc;
  
  return true;
}

// generelizes the deduction of required numbers in a given row/col. it considers the interaction of givens, and seen requirements in a given row/col
std::vector<int> TwentyFourSeven::deduce_required_nums(const std::vector<int>& curr, const std::vector<int>& seens) const {
  std::vector<int> required(8, 0);
  if (seens[0] > 0) {
      int k = 0;
      // check first 4 numbers from the 'left'
      while (k < 4 && curr[k] == -1) {
        k++;
      }
      if (k >= 4 || curr[k] != seens[0]) {
        // there is a seen lets call p, in which the first 4 elements does not contain
        // therefore we deduce at least one p is in this row
        required[seens[0]]++;
      }
    }
    if (seens[1] > 0) {
      int k = 6;
      // check first 4 numbers from the 'right'
      while (k >= 3 && curr[k] == -1) {
        k++;
      }
      if (k < 3 || curr[k] != seens[1]) {
        // same logic applies running from the reverse direction
        required[seens[1]]++;
      }
    }
    return required;
}

bool TwentyFourSeven::solve() {
  
  
  // last resort, use the bruteforce solver
  return solve_bruteforce();
}

// solve board using a guess and check method with backtracking
bool TwentyFourSeven::solve_bruteforce() {
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 7; j++) {
      if (board[i][j] == -1) {
        // loop through available numbers
        for (int n = 7; n >= 0; n--) {
          if (((row_can_use[i] >> n) & 1) && ((col_can_use[j] >> n) & 1) && can_place_number(i, j, n)) {
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
      to_place_count++;
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
      to_place_count--;
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

std::vector<std::pair<int, int>> TwentyFourSeven::make_two_sum(int sum, int can_use) {
  std::vector<std::pair<int, int>> ans;
  for (int i = 1; i < 8; i++) {
    if ((can_use >> i) & 1) {
      for (int j = i; j < 8; j++) {
        if (((can_use >> j) & 1) && (i + j == sum)) {
          ans.push_back({i, j});
        }
      }
    }
  }
  return ans;
}
  
