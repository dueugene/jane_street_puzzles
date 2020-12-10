#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#include "twentyfourseven.cpp"


using namespace std;
using Clock = std::chrono::high_resolution_clock;

template <typename A>
using mat = vector<vector<A>>;


// todo:
// find better way for checking 2x2 and seen constraints so that method is const
// cleanup validate grid method

// ways to prune the brute force method:
// consider algorithm where we attempt to place 1 first
// consider if checking for island will help
// should consider interaction of placed and seens, to determine what can and cannot be in each row/col

int main(int argc, char** argv) {
  // this will allow different input files to be passed
  string filename;
  if (argc > 1) {
    filename = argv[1];
  } else {
    // default input
    filename = "test.txt";
  }

  // read input and populate matrices
  ifstream input;
  input.open(filename);
  int a, b, c;
  int n_givens;
  mat<int> givens;
  input >> n_givens;
  while (n_givens--) {
    input >> a >> b >> c;
    givens.push_back({a, b, c});
  }
  n_givens = 7;
  mat<int> rows;
  while (n_givens--) {
    input >> a >> b;
    rows.push_back({a, b});
  }
  n_givens = 7;
  mat<int> cols;
  while (n_givens--) {
    input >> a >> b;
    cols.push_back({a, b});
  } 
  input.close();

  cout << filename << endl; 
  TwentyFourSeven test(givens, rows, cols);
  test.print();
  cout << "Board is valid: " << test.is_valid_board() << endl;

  auto t1 = Clock::now();
  bool solved = test.solve();
  auto t2 = Clock::now();
  test.print();
  chrono::duration<double> elapsed_s = t2 - t1;
  cout << "Elapsed time: " << elapsed_s.count() << "s" << endl;
  
  if (solved) {
    cout << "Solve success" << endl;
  } else {
    cout << "Solve failed" <<endl;
  }
  
  return 0;
}
