#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>


// todo:
// find better way for checking 2x2 and seen constraints so that method is const
// cleanup validate grid method
// add better commenting
// add unit tests

// ways to prune the brute force method:
// consider algorithm where we attempt to place 1 first (priority solve)
// consider ranking indices from most constrained to least constrained in the solve section (also kind of like priority solve)

class TwentyFourSeven {

public:
  TwentyFourSeven(std::vector<std::vector<int>> givens, std::vector<std::vector<int>> rows, std::vector<std::vector<int>> cols);
  bool solve();
  void print();
  std::vector<std::vector<int>> get_board();
  
  bool is_valid_board() const;
private:

  bool solve_bruteforce();
  bool solve_priority(const std::vector<std::vector<int>>& nums, int ind);
  void insert_number(const int i, const int j, const int n);
  bool can_place_number(const int i, const int j, const int n);
  bool is_continuous_land() const;
  bool restrict_available_nums(std::vector<unsigned int>& row_can_use_loc, std::vector<unsigned int>& col_can_use_loc) const;
  bool restrict_available_nums_helper(const std::vector<std::vector<int>>& board_loc, const std::vector<std::vector<int>>& seens, std::vector<unsigned int>& can_use) const;
  std::vector<std::pair<int,int>> make_two_sum(int sum, int can_use) const;
  // returns the current board, but columwise
  std::vector<std::vector<int>> columnwise_board() const;
  
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

std::vector<std::vector<int>> TwentyFourSeven::columnwise_board() const {
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
  
}

bool TwentyFourSeven::restrict_available_nums(std::vector<unsigned int>& row_can_use_loc, std::vector<unsigned int>& col_can_use_loc) const {
  // create a columnwise board
  auto column_board = this->columnwise_board();
  // we call the restrict_available_nums_helper function for the rows, and then the columns
  return (restrict_available_nums_helper(board, row_seens, row_can_use_loc) && restrict_available_nums_helper(column_board, col_seens, col_can_use_loc));
}

// generalizes the restrict_available_nums() function, such that rowise, or columnwise can be passed to this function
bool TwentyFourSeven::restrict_available_nums_helper(const std::vector<std::vector<int>>& board_loc, const std::vector<std::vector<int>>& seens, std::vector<unsigned int>& can_use) const {
  // create a local "snapshot" copy of avail_nums
  std::vector<int> avail_nums_loc = avail_nums;
  std::vector<unsigned int> required_indicator(7, 0); // marks if number n, is required in each "row", but we mean row or col
  std::vector<std::vector<int>> known_count (7, std::vector<int> (8, 0)); // counts known numbers, already placed
  std::vector<std::vector<int>> required_count = known_count; // counts required numbers, dedcued by logic, whos location is not yet deducable
  std::vector<std::vector<int>> total_count = known_count; // used to sum known and required
  

  // perform each "row" wise restriction
  for (int i = 0; i < 7; i++) {
    std::vector<int> curr = board_loc[i];
    for (int n : curr) {
      if (n > 0) {
        known_count[i][n]++;
      }
    }
    
    // check seen constraints coupled with curr
    required_count[i] = deduce_required_nums(curr, seens[i]);
    for (int n = 1; n < 8; n++) {
      if (required_count[i][n] > 0) {
        avail_nums_loc[n] -= required_count[i][n];
        required_indicator[i] |= 1 << n; // indicate that this "row" must have number n
      }
    }
  }
  
  // if any avail_nums are used up, based on required_count, we mark the other "rows"
  for (int n = 1; n < 8; n++) {
    // this indicates we're on a board that has no solution
    if (avail_nums_loc[n] < 0) {return false;}
    if (avail_nums_loc[n] == 0) {
      for (int i = 0; i < 7; i++) {
        if (!(required_indicator[i] >> n & 1)) {
          can_use[i] &= ~(1 << n);
        }
      }
    }
  }
  
  for (int i = 0; i < 7; i++) {
    int count = 0;
    int sum = 0;
    for (int n = 1; n < 8; n++) {
      // combine required with known
      total_count[i][n] = known_count[i][n] + required_count[i][n];
      // caluclate count and sum
      count += total_count[i][n];
      sum += n*total_count[i][n];
    }
    int remainder = 20 - sum;
    // std::cout << "Row: " << i << ", " << count << ", " << sum << std::endl;
    
    switch (count) {
    case 4:
      if (sum != 20) {return false;} // failure based on sum not equalling 20
      // we will remake can_use[i] and write the values we can use
      can_use[i] = 1;
      for (int n = 1; n < 8; n++) {
        if (required_count[i][n] > 0) {
          can_use[i] |= 1 << n;
        }
      }
      break;
    case 3:
      if (remainder > 7) {return false;} // failure based on remainder is greater than 7
      // remake can_use[i]
      can_use[i] = 1;
      for (int n = 1; n < 8; n++) {
        if (required_count[i][n] > 0) {
          can_use[i] |= 1 << n;
        }
      }
      can_use[i] |= 1 << remainder;
      // the remainder will take away 1 from available
      avail_nums_loc[remainder]--;
      required_indicator[i] |= 1 << remainder;
      break;
    case 2:
      {
      // make a two sum
      std::vector<std::pair<int, int>> pairs = make_two_sum(remainder, can_use[i]);
      // remake can_use[i]
      can_use[i] = 1;
      for (int n = 1; n < 8; n++) {
        if (required_count[i][n] > 0) {
          can_use[i] |= 1 << n;
        }
      }
      // filter out pairs that cannot be made
      std::vector<std::pair<int, int>> pairs2;
      for (auto p : pairs) {
        if (avail_nums_loc[p.first] > 0 && avail_nums_loc[p.second] > 0) {
          pairs2.push_back(p);
        }
      } 
      if (pairs2.empty()) {return false;} // failure based on no available pairs 
      for (auto p : pairs2) {
        can_use[i] |= 1 << p.first;
        can_use[i] |= 1 << p.second;
      }
      if (pairs2.size() == 1) {
        auto p = pairs2[0];
        // this is the only pair available, remove from available
        avail_nums_loc[p.first]--;
        avail_nums_loc[p.second]--;
        required_indicator[i] |= 1 << p.first;
        required_indicator[i] |= 1 << p.second;
      }
      
      break;
      }
    case 1:
      if (sum == 1) {
        // single given of 1
        can_use[i] &= ~(1 << 2);
        can_use[i] &= ~(1 << 3);
        can_use[i] &= ~(1 << 4);
      } else if (sum == 2) {
        can_use[i] &= ~(1 << 1);
        can_use[i] &= ~(1 << 3);
      } else if (sum == 3) {
        // single given of 3
        can_use[i] &= ~(1 << 1);
        can_use[i] &= ~(1 << 2);
      } else if (sum == 4) {
        // single given of 4
        can_use[i] &= ~(1 << 1);
      }
      break;
    case 0:
      break;
    default:
      // the board is in a state which has no solution
      return false;
    }
    
  }
  // re-perform avail nums check
  // if any avail_nums are used up, based on required_count, we mark the other "rows"
  for (int n = 1; n < 8; n++) {
    if (avail_nums_loc[n] < 0) {return false;}
    if (avail_nums_loc[n] == 0) {
      for (int i = 0; i < 7; i++) {
        if (!(required_indicator[i] >> n & 1)) {
          can_use[i] &= ~(1 << n);
        }
      }
    }
  }
  
  return true;
}

// generelizes the deduction of required numbers in a given row/col. it considers the interaction of givens, and seen requirements in a given row/col
std::vector<int> TwentyFourSeven::deduce_required_nums(const std::vector<int>& curr, const std::vector<int>& seens) const {
  std::vector<int> required(8, 0);
  if (seens[0] > 0) {
      int k = 0;
      // check first 4 numbers from the 'left'
      while (k < 4 && curr[k] <= 0) {
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
      while (k >= 3 && curr[k] <= 0) {
        k--;
      }
      if (k < 3 || curr[k] != seens[1]) {
        // same logic applies running from the reverse direction
        required[seens[1]]++;
      }
    }
    return required;
}

bool TwentyFourSeven::solve() {
    // perform pre-processing, for the purpose of pruning the number of cases to test
  std::vector<unsigned int> row_can_use_loc = row_can_use;
  std::vector<unsigned int> col_can_use_loc = col_can_use;
  if (restrict_available_nums(row_can_use_loc, col_can_use_loc)) {
    row_can_use = row_can_use_loc;
    col_can_use = col_can_use_loc;
  } else {
    // failed on available numbers
    std::cout << "failed on avail numbers" << std::endl;
    return false;
  }

  // solve based on a hybrid strategy
  // if we have "enough" givens, we will just call the bruteforce solver
  // current cutoff is set to 12 givens, that are non-zero
  // if (to_place_count <= 16) {
  // eugene: currently we only call brute force solver
  if (true) {
    std::cout << "running brute force" << std::endl; 
    return solve_bruteforce();
  }


  // preparing for priority solving
  // eugene: this section is currently on hold, since it seems the brute force solver
  // with "level 2" checks works quite well
  // I've kept this section because it might be worth looking to in the future, and perhaps even applied to other projects
  std::vector<int> avail_nums_loc = avail_nums;
  
  // create a list of numbers, and place-able indices
  std::vector<std::vector<int>> priority_nums;
  auto column_board = this->columnwise_board();

  // list out all givens, that are not satisfied
  for (int i = 0; i < 7; i++) {
    
    if (row_seens[i][0] > 0 || row_seens[i][1] > 0) {
      std::vector<int> required = deduce_required_nums(board[i], row_seens[i]);
      // here we must treat the case when row_seens[i][0] == row_seens[i][1] separately
      if (row_seens[i][0] == row_seens[i][1]) {
        // bad programming lol

      } else if (required[row_seens[i][0]] > 0) {   
        std::vector<int> to_put;
        to_put.push_back(row_seens[i][0]);
        int j = 0;
        while (j < 4 && board[i][j] == - 1) {
          to_put.push_back(i*7 + j);
          j++;
        }
        priority_nums.push_back(to_put);
      } else if (required[row_seens[i][1]] > 0) {
        std::vector<int> to_put;
        to_put.push_back(row_seens[i][1]);
        int j = 6;
        while (j >= 3 && board[i][j] == - 1) {
          to_put.push_back(i*7 + j);
          j--;
        }
        priority_nums.push_back(to_put);
      }
    }
    /*
    // repeat for columnwise
    if (col_seens[i][0] > 0 || col_seens[i][1] > 0) {
      std::vector<int> required = deduce_required_nums(column_board[i], col_seens[i]);
      // here we must treat the case when col_seens[i][0] == col_seens[i][1] separately
      if (col_seens[i][0] == col_seens[i][1]) {
        // bad programming lol

      } else if (required[col_seens[i][0]] > 0) {   
        std::vector<int> to_put;
        to_put.push_back(col_seens[i][0]);
        int j = 0;
        while (j < 4 && board[j][i] == -1) {
          to_put.push_back(j*7 + i);
          j++;
        }
        priority_nums.push_back(to_put);
      } else if (required[col_seens[i][1]] > 0) {
        std::vector<int> to_put;
        to_put.push_back(col_seens[i][1]);
        int j = 6;
        while (j >= 3 && board[j][i] == -1) {
          to_put.push_back(j*7 + i);
          j--;
        }
        priority_nums.push_back(to_put);
      }
      }*/
  }  
  // list out all numbers with only 2 left to place, 1, 2 etc.
  /*for (int n = 1; n < 8; n++) {
    if (avail_nums_loc[n] <= 2 && avail_nums_loc[n] > 0) {
      std::vector<int> to_put;
      to_put.push_back(n);
      for (int i = 0; i < 7; i++) {
        if ((row_can_use[i] >> n) & 1) {
          for (int j = 0; j < 7; j++) {
            if ((col_can_use[j] >> n) & 1) {
              to_put.push_back(i*7 + j);
            }
          }
        }
      }
      priority_nums.push_back(to_put);
      if (avail_nums_loc[n] == 2) {
        priority_nums.push_back(to_put);
      }
    }
    }*/
  
  // determine all points they can go.
  
  // sort list, and perhaps delete end?
  auto comp = [] (std::vector<int>& a, std::vector<int>& b) {
    return a.size() < b.size();
  };
  std::sort(priority_nums.begin(), priority_nums.end(), comp);
  // discard any with more than 4 possible locations
  while (!priority_nums.empty() && priority_nums[priority_nums.size()-1].size() > 4) {
    priority_nums.pop_back();
  }
  std::cout << "Priority solve: " <<  priority_nums.size() << std::endl;
  for (int i = 0; i < priority_nums.size(); i++) {
    for (int j = 0; j < priority_nums[i].size(); j++) {
      std::cout << priority_nums[i][j] << " ";
    }
    std::cout << std::endl;
  }
  
  return solve_priority(priority_nums, 0);
}

// priority solver, use bruteforce method to place all nums in the priority vector first,
// then call the bruteforce solver to finish the rest of the puzzle
bool TwentyFourSeven::solve_priority(const std::vector<std::vector<int>>& nums, int ind) {

  if (ind == nums.size()) {
    // finish off the rest of the solve with bruteforce 
    return solve_bruteforce();
  }
  std::vector<int> num = nums[ind];
  int n = num[0];
  for (int k = 1; k < num.size(); k++) {
    int i = num[k] / 7;
    int j = num[k] % 7;
    if (((row_can_use[i] >> n) & 1) && ((col_can_use[j] >> n) & 1) && can_place_number(i, j, n)) {
      insert_number(i, j, n);
      // store a copy of current state of row_can_use, and col_can_use
      std::vector<unsigned int> row_copy = row_can_use;
      std::vector<unsigned int> col_copy = col_can_use;
      // introcude a "level 2", where we dont just check the board is valid,
      // but rather what the current state of the board implies about whats available for the rest of the rows/cols
      if (restrict_available_nums(row_can_use, col_can_use)) { // this call will update row_can_use, and col_can_use based on current board
        if(solve_priority(nums, ind + 1)) {return true;}
      }
      // "backtrack"
      // use the old row_can_use, col_can_use, since they were determined to be wrong
      row_can_use = row_copy;
      col_can_use = col_copy;
      insert_number(i, j, -1);
    }
  }
  
  // using all the priority nums, we have failed to find a solution
  return false;
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
            // store a copy of current state of row_can_use, and col_can_use
            std::vector<unsigned int> row_copy = row_can_use;
            std::vector<unsigned int> col_copy = col_can_use;
            // introcude a "level 2", where we dont just check the board is valid,
            // but rather what the current state of the board implies about whats available for the rest of the rows/cols
            if (restrict_available_nums(row_can_use, col_can_use)) { // this call will update row_can_use, and col_can_use based on current board
              if(solve_bruteforce()) {return true;}
            }
            // "backtrack"
            // use the old row_can_use, col_can_use, since they were determined to be wrong
            row_can_use = row_copy;
            col_can_use = col_copy;
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
    col_empty[j]--;
    if (n > 0) {
      row_count[i]++;
      col_count[j]++;
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
  int prev = board[i][j];
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
          board[i][j] = prev;
          return false;
        }
      }
    }
  }
  board[i][j] = prev;
  
  // check that seen constraints are not violated
  prev = board[i][j];
  board[i][j] = n;
  if (row_seens[i][0] > 0) {
    // check the first 4 numbers looking from left
    for (int k = 0; k < 4; k++) {
      if (board[i][k] != 0) {
         if (!(board[i][k] == row_seens[i][0] || board[i][k] == -1)) {
           board[i][j] = prev;
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
          board[i][j] = prev;
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
          board[i][j] = prev;
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
          board[i][j] = prev;
          return false;
        }
        break;
      }
    }
  }  
  board[i][j] = prev;
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

std::vector<std::pair<int, int>> TwentyFourSeven::make_two_sum(int sum, int can_use) const{
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
  
