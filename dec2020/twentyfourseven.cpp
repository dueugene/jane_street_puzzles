#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <queue>

using namespace std;

class TwentyFourSeven {

public:
  TwentyFourSeven();
  TwentyFourSeven(std::vector<std::vector<int>> givens, std::vector<std::vector<int>> rows, std::vector<std::vector<int>> cols);
  bool solve();
  void print();

  bool can_place_number(const int i, const int j, const int n);
  bool is_continuous_land() const;
  bool is_valid_board() const;
private:
  
  std::vector<std::vector<int>> board;
  std::vector<int> nums = {21,1,2,3,4,5,6,7};
  std::vector<std::vector<int>> row_seens;
  std::vector<std::vector<int>> col_seens;
};



TwentyFourSeven::TwentyFourSeven() {
  board = std::vector<std::vector<int>> (7, std::vector<int> (7, -1));

}

TwentyFourSeven::TwentyFourSeven(std::vector<std::vector<int>> givens, std::vector<std::vector<int>> rows, std::vector<std::vector<int>> cols) {
  board = std::vector<std::vector<int>> (7, std::vector<int> (7, -1));
  for (int i = 0; i < givens.size(); i++) {
    board[givens[i][0]][givens[i][1]] = givens[i][2];
    nums[givens[i][2]]--;
  }
  row_seens = rows;
  col_seens = cols;

}

// solve board using a guess and check method with backtracking
bool TwentyFourSeven::solve() {
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 7; j++) {
      if (board[i][j] == -1) {
        // loop through available numbers
        for (int k = 7; k >= 0; k--) {
          if (can_place_number(i, j, k)) {
            board[i][j] = k;
            nums[k]--;
            if(solve()) {return true;}
            board[i][j] = -1;
            nums[k]++;
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
  queue<int> to_check;
  // find first ind of a number
  for (int j = 0; j < 7; j++) {
    if (board[0][j] > 0) {
      to_check.push(j);
      break;
    }
  }
  int count = 0;
  vector<bool> visited(49, false);
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

// confirms that the number n can be placed on board at index i,j
bool TwentyFourSeven::can_place_number(const int i, const int j, const int n) {
  if (n < 0 || n > 7) {return false;}
  if (board[i][j] != -1) {return false;}
  
  // check that n is still available
  if (nums[n] < 1) {return false;}
  
  // count the number of numbers placed and total sum in row i, and col j
  int count = 0;
  int empty = 0;
  int sum = 0;
  for (int k = 0; k < 7; k++) {
    if (board[i][k] == -1) {
      empty++;
    } else if (board[i][k] > 0) {
      count++;
      sum += board[i][k];
    }
  }
  if (sum + n > 20) {return false;}
  if (n != 0 && count == 4) {return false;}
  if (n == 0 && empty < (4 - count) + 1) {return false;}
  
  count = 0;
  empty = 0;
  sum = 0;
  for (int k = 0; k < 7; k++) {
    if (board[k][j] == -1) {
      empty++;
    } else if (board[k][j] > 0) {
      count++;
      sum += board[k][j];
    }
  }
  if (sum + n > 20) {return false;}
  if (n != 0 && count == 4) {return false;}
  if (n == 0 && empty < (4 - count) + 1) {return false;}
    
  // check that inserting a number would not make a 2 x 2 filled square
  board[i][j] = n;
  if (n > 0) {
    for (int ii = max(i - 1, 0); ii < min(i, 6); ++ii) {
      for(int jj = max(j - 1, 0); jj < min(j, 6); ++jj) {
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
    if (sum > 20) {
      cout << "failed on sum" << endl;
      return false;
    }
    if (count > 4) {
      cout << "failed on count" << endl;
      return false;
    }
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
    if (sum > 20) {
      cout << "failed on sum" << endl;
      return false;
    }
    if (count > 4) {
      cout << "failed on count" << endl;
      return false;
    }
  }
  
  // confirm each 2 x 2 square has at least one empty square
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      bool test = false;
      test = test || (board[i][j] <= 0);
      test = test || (board[i+1][j] <= 0);
      test = test || (board[i][j+1] <= 0);
      test = test || (board[i+1][j+1] <= 0);
      if (test == false) {
        cout << "failed on fours" << endl;
        return false;
      }
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
      if (j < 7 && !(board[i][j] == row_seens[i][0] || board[i][j] == -1)) {
        cout << "failed on left" << endl;
        return false;
      }
    }
    // check right side
    if (row_seens[i][1] > 0) {
      int j = 6;
      while (j >= 0 && board[i][j] == 0) {
        j--;
      }
      if (j >= 0 && !(board[i][j] == row_seens[i][1] || board[i][j] == -1)) {
        cout << "failed on right" << endl;
        return false;
      }
    }
  }
  // col wise
  for (int j = 0; j < 7; j++) {
    // check top side
    if (row_seens[j][0] > 0) {
      int i = 0;
      while (i < 7 && board[i][j] == 0) {
        i++;
      }
      if (i < 7 && !(board[i][j] == col_seens[j][0] || board[i][j] == -1)) {
        cout << "failed on top" << endl;
        return false;
      }
    }
    // check bot side
    if (row_seens[j][1] > 0) {
      int i = 6;
      while (i >= 0 && board[i][j] == 0) {
        i--;
      }
      if (i >= 0 && !(board[i][j] == col_seens[j][1] || board[i][j] == -1)) {
        cout << "failed on bot" << endl;
        return false;
      }
    }
  }
  return true;
}

  
