#include <iostream>
#include <vector>

using namespace std;

// first we create a matrix to hold the values of n choose k
// there are 78 total figurines, so we only need n, k <= 78
// the largest value in here that ive checked is ~2.72e22 (78choose39) which is too large to fit in unsigned long long
// so we use double instead, which will mean we will loose precision, I am assuming here that it will not affect the final solution
// alternatively we could use a library that supports larger ints
#define N 3
#define TOT_FIGS (N * (N + 1) / 2)
vector<vector<double>> nChoosek(TOT_FIGS+1, vector<double>(TOT_FIGS+1, 0.0));

double probOfFig(int n_avail, int fig_id, int k);

int main(int argc, char** argv) {
  // we use the pascals triangle method to populate n choose k matrix
  for (int i = 0; i < TOT_FIGS + 1; i++) {
    nChoosek[i][0] = 1.0;
    for (int j = 1; j < i + 1; j++) {
      nChoosek[i][j] = nChoosek[i-1][j-1] + nChoosek[i-1][j];
    }
  }
  // perform a few sanity checks
  for (int i = 0; i <= 8; i++) {
    // cout << nChoosek[8][i] << endl;
  }
  cout.precision(17);
  // cout << nChoosek[78][39] << endl;

  // idea:
  // it all depends on in which position the partidge gets picked, it can get picked in any of 78 positions,
  // all with equal probability
  // lets assume that it gets picked in the 10th position, then we care about all the figurines picked before that (pos 1 - 9)
  // we can calculate the expected value of n, given that partridge is in 10th position
  // this is done by calculating P(n=0), P(n=1),..., P(n=12), given that partridge = 10. which leads to E[n] given partridge = 10
  // if we calculate this for all 78 positions, we can calculate total expected n, as an average, since all 78 positions have equal probability

  // probabilities[i][j] represents that probablity of n = j given that partidge is picked at pos = i + 1
  vector<vector<double>> probabilities(TOT_FIGS, vector<double>(N + 1, 0.0));
  
  for (int i = 0; i < TOT_FIGS; i++) {
    for (int j = 0; j < N + 1; j++) {
      // calculate probability of n = j
      for (int fig = 2; fig < N + 1; fig++) {
        probabilities[i][j] += probOfFig(i, fig, j);
      }
    }
  }

  
  for (int i = 0; i < TOT_FIGS; i++) {
    for (int j = 0; j < N + 1; j++) {
      cout << probabilities[i][j] << " ";
    }
    cout << endl;
  }
  
  
  return 0;
}

// return the probablity of drawing k of fig_id, given n_avail draws
double probOfFig(int n_avail, int fig_id, int k) {
  if (k > fig_id) {return 0.0;}
  if (k > n_avail) {return 0.0;}
  if (k == 0) {return 0.0;}
  if (TOT_FIGS-1-fig_id < 0) {return 0.0;} 
  double tot_combinations = nChoosek[TOT_FIGS-1][n_avail];
  double tot_fig_picks = nChoosek[fig_id][k];
  double blah = nChoosek[TOT_FIGS-1-fig_id][n_avail-k];
  return tot_fig_picks*blah/tot_combinations;
}
