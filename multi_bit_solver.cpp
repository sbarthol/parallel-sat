#include "multi_bit_solver.h"

#include <numeric>
#include <queue>
#include <unordered_set>

#include "rng.h"

#define LIT(i) ((i) << 1)
#define NEG_LIT(i) (((i) << 1) | 1)
#define COMPL(i) ((i) ^ 1)

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

int MultiBitSolver::satisfies(const vector<int>& phi) {
  int conj = ~0;
  for (vector<int>& clause : clauses) {
    int disj = 0;
    for (int u : clause) {
      disj |= phi[u];
    }
    conj &= disj;
  }
  return conj;
}

vector<pair<int, int>> MultiBitSolver::get_rem_lits(const vector<int>& clause,
                                                    const vector<int>& phi) {
  vector<pair<int, int>> rem_lits;
  // Todo, turn O(k^2) into O(k) using mask technique
  for (int u : clause) {
    int ass = ~(phi[u] | phi[COMPL(u)]);
    for (int v : clause) {
      if (u != v) {
        ass &= phi[COMPL(v)];
      }
    }
    if (ass) {
      rem_lits.push_back({u, ass});
    }
  }
  return rem_lits;
}

void MultiBitSolver::unit_propagation(vector<int>& phi) {
  queue<int> q;
  unordered_set<int> in_queue;

  for (int i = 0; i < m; i++) {
    vector<pair<int, int>> rem_lits = get_rem_lits(clauses[i], phi);
    for (auto p : rem_lits) {
      assert(p.first != -1);
      phi[p.first] |= p.second;
      if (!in_queue.count(p.first) && !in_queue.count(COMPL(p.first))) {
        q.push(p.first);
        in_queue.insert(p.first);
      }
    }
  }

  while (!q.empty()) {
    int u = q.front();
    q.pop();
    in_queue.erase(u);

    for (int i : inv_clauses[COMPL(u)]) {
      vector<pair<int, int>> rem_lits = get_rem_lits(clauses[i], phi);
      for (auto p : rem_lits) {
        assert(p.first != u);
        assert(p.first != -1);
        phi[p.first] |= p.second;
        if (!in_queue.count(p.first)) {
          // Todo: how to be sure that !v is not in the queue?
          assert(!in_queue.count(COMPL(p.first)));
          q.push(p.first);
          in_queue.insert(p.first);
        }
      }
    }
  }
}

vector<bool> MultiBitSolver::solve(int& periods) {
  // phi_master must not have conflicts or unassigned positions
  assert(phi_master.size() == 2 * n);
  for (int i = 0; i < n; i++) {
    assert(!(~(phi_master[LIT(i)] ^ phi_master[NEG_LIT(i)])));
  }

  vector<int> pi(n);
  iota(pi.begin(), pi.end(), 0);

  int conj = satisfies(phi_master);
  int period;
  for (period = 0; !conj; period++) {
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
    conj = satisfies(phi_master);
  }

  periods = period;
  assert(conj);

  vector<bool> assignment(n);
  for (int i = 0; i < n; i++) {
    assignment[i] = phi_master[LIT(i)] & (conj & ~(conj - 1));
  }
  return assignment;
}

vector<bool> MultiBitSolver::solve() {
  int foo;
  return solve(foo);
}
