#include "multi_bit_solver.h"

#include <numeric>
#include <queue>
#include <unordered_set>

#include "rng.h"

#define LIT(i) ((i) << 1)
#define NEG_LIT(i) (((i) << 1) | 1)

using namespace std;

MultiBitSolver::MultiBitSolver(vector<vector<int>> clauses_, int n_,
                               vector<int> phi_master_)
    : clauses(clauses_), n(n_) {
  m = clauses_.size();
  assert(m > 0);
  assert(phi_master_.size() == n);
  phi_master = vector<int>(2 * n);
  for (int i = 0; i < n; i++) {
    phi_master[LIT(i)] = phi_master_[i];
    phi_master[NEG_LIT(i)] = ~phi_master[i];
  }
  inv_clauses = vector<vector<int>>(2 * n);
  for (int i = 0; i < m; i++) {
    for (int j : clauses[i]) {
      inv_clauses[j].push_back(i);
    }
  }
}

MultiBitSolver::MultiBitSolver(vector<vector<int>> clauses_, int n_)
    : clauses(clauses_), n(n_) {
  m = clauses_.size();
  assert(m > 0);
  phi_master = vector<int>(2 * n, 0);
  for (int i = 0; i < n; i++) {
    phi_master[NEG_LIT(i)] = ~0;
  }
  inv_clauses = vector<vector<int>>(2 * n);
  for (int i = 0; i < m; i++) {
    for (int j : clauses[i]) {
      inv_clauses[j].push_back(i);
    }
  }
}

bool MultiBitSolver::satisfies(const vector<int>& phi) {
  for (vector<int>& clause : clauses) {
    int clause_value = 0;
    for (int j = 0; j < clause.size() && !clause_value; j++) {
      clause_value = phi[clause[j]];
    }
    if (!clause_value && !clause.empty()) {
      return false;
    }
  }
  return true;
}

vector<int> MultiBitSolver::get_rem_lits(const vector<int>& clause,
                                         const vector<int>& phi_active) {
  assert(false);
}

void MultiBitSolver::unit_propagation(vector<int>& phi_active) {
  assert(false);
}

vector<bool> MultiBitSolver::solve(int& periods) {
  // phi_master must not have conflicts or unassigned positions
  assert(phi_master.size() == 2 * n);
  for (int i = 0; i < n; i++) {
    assert(!(~(phi_master[LIT(i)] ^ phi_master[NEG_LIT(i)])));
  }

  vector<int> pi(n);
  iota(pi.begin(), pi.end(), 0);

  int period;
  for (period = 0; !satisfies(phi_master); period++) {
    shuffle(pi.begin(), pi.end(), RNG::m_mt);
    vector<int> phi_active(2 * n, 0);
    bool change = true;

    for (int i = 0; i < n; i++) {
      if (change) {
        unit_propagation(phi_active);
        change = false;
      }
      int unassigned = ~(phi_active[LIT(i)] | phi_active[NEG_LIT(i)]);
      if (unassigned) {
        phi_active[LIT(pi[i])] |= phi_master[LIT(pi[i])] & unassigned;
        phi_active[NEG_LIT(pi[i])] |= phi_master[NEG_LIT(pi[i])] & unassigned;
        change = true;
        assert(!(~(phi_active[LIT(i)] | phi_active[NEG_LIT(i)])));
      }
    }
    if (phi_active == phi_master) {
      int random_idx = RNG::uniform(n);
      phi_active[random_idx] = ~phi_active[random_idx];
    }
    phi_master = phi_active;

    // phi_master must not have conflicts or unassigned positions
    for (int i = 0; i < n; i++) {
      assert(!(~(phi_master[LIT(i)] ^ phi_master[NEG_LIT(i)])));
    }
  }

  periods = period;

  // Todo, convert back to single-bit
  assert(false);
  return vector<bool>();
}

vector<bool> MultiBitSolver::solve() {
  int foo;
  return solve(foo);
}
