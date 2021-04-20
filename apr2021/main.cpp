#include <iostream>
#include <chrono>
#include <vector>
#include <assert.h>
#include <unordered_map>

using namespace std;
using Clock = std::chrono::high_resolution_clock;

ostream& operator<<(ostream& os, const vector<int>& vec);
double probabilityOfTeamWinning(const vector<int>& seed, const int team);

int main(int argc, char** argv) {

  vector<int> curr_bracket = {1, 16, 8, 9, 5, 12, 4, 13, 6, 11, 3, 14, 7, 10, 2, 15};
  // vector<int> curr_bracket = {1, 3, 4, 2};
  // vector<int> curr_bracket = {2, 1};
  // vector<int> curr_bracket = {1};
  cout.precision(12);
  int n = curr_bracket.size();
  double best_prob = 0.0;
  vector<int> best_bracket = curr_bracket;
  auto t1 = Clock::now();
  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      swap(curr_bracket[i], curr_bracket[j]);
      double curr_prob = probabilityOfTeamWinning(curr_bracket, 2);

      cout << "bracket: " << curr_bracket;
      cout << "Probability of " << 2 << " winning: " << curr_prob << endl;
      
      if (curr_prob > best_prob) {
        best_prob = curr_prob;
        best_bracket = curr_bracket;
      }
      swap(curr_bracket[i], curr_bracket[j]);
    }
  }
  auto t2 = Clock::now();
  chrono::duration<double> elapsed_time = t2 - t1;
  cout << "Elapsed time: " << elapsed_time.count() << "s" << endl;
  cout << "best: " << best_bracket;
  cout << "Probability of " << 2 << " winning: " << best_prob << endl;
  
  
  return 0;
}


ostream& operator<<(ostream& os, const vector<int>& vec)
{
  for (int i = 0; i < vec.size(); i++) {
    os << vec[i] << " ";
  }
  return os;
}

double probabilityOfTeamWinning(const vector<int>& bracket, const int team) {
  // for tournament bracket we require number of teams to be power of 2
  int n = bracket.size();
  assert((n & n - 1) == 0);

  vector<unordered_map<int, double>> probabilities(n);
  for (int i = 0; i < n; i++) {
    probabilities[i].insert({bracket[i], 1.0});
  }

  while (n > 0) {
    n = n / 2;
    for (int i  = 0; i < n; i++) {
      unordered_map<int, double> teams_left = probabilities[i*2];
      unordered_map<int, double> teams_right = probabilities[i*2 + 1];
      unordered_map<int, double> curr;
      
      for (auto a : teams_left) {
        double prob_a = 0.0;
        for (auto b : teams_right) {
          prob_a += b.second * ((double)b.first / (double)(a.first + b.first));
        }
        curr[a.first] = prob_a * a.second;
      }
      
      for (auto a : teams_right) {
        double prob_a = 0.0;;
        for (auto b : teams_left) {
          prob_a += b.second * ((double)b.first / (double)(a.first + b.first));
        }
        curr[a.first] = prob_a * a.second;
      }
      
      probabilities[i] = curr;
    }
  }  

  return probabilities[0][team];
}
