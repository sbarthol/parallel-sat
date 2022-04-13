//#define NDEBUG

#include "multi_bit_solver.h"

#include <numeric>
#include <queue>
#include <unordered_set>

#include "rng.h"

#define LIT(i) ((i) << 1)
#define NEG_LIT(i) (((i) << 1) | 1)
#define COMPL(i) ((i) ^ 1)

using namespace std;

MultiBitSolver::MultiBitSolver(vector<vector<int>> clauses_, int n_)
    : clauses(clauses_), n(n_) {
  m = clauses_.size();
  assert(m > 0);
  phi_master = vector<uintk_t>(2 * n, 0);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < sizeof(uintk_t) * 8; j++) {
      if (RNG::uniform_bool()) {
        phi_master[LIT(i)] |= 1L << j;
      }
    }
    phi_master[NEG_LIT(i)] = ~phi_master[LIT(i)];
  }
  inv_clauses = vector<vector<int>>(2 * n);
  for (int i = 0; i < m; i++) {
    for (int j : clauses[i]) {
      inv_clauses[j].push_back(i);
    }
  }
}

MultiBitSolver::uintk_t MultiBitSolver::satisfies(const vector<uintk_t>& phi) {
  uintk_t conj = -1;
  for (vector<int>& clause : clauses) {
    uintk_t disj = 0;
    for (int u : clause) {
      disj |= phi[u];
    }
    conj &= disj;
    if (!conj) {
      return false;
    }
  }
  return conj;
}

vector<pair<int, MultiBitSolver::uintk_t>> MultiBitSolver::get_rem_lits(
    const vector<int>& clause, const vector<uintk_t>& phi) {
  uintk_t m_lt1 = -1, m_lt2 = -1;
  for (int u : clause) {
    m_lt2 = (m_lt2 & phi[COMPL(u)]) | m_lt1;
    m_lt1 = m_lt1 & phi[COMPL(u)];
  }
  uintk_t m_1 = m_lt1 ^ m_lt2;

  vector<pair<int, uintk_t>> rem_lits;
  for (int u : clause) {
    uintk_t ass = m_1 & (~(phi[u] | phi[COMPL(u)]));
    if (ass) {
      rem_lits.push_back({u, ass});
    }
  }

  return rem_lits;
}

void MultiBitSolver::unit_propagation(vector<uintk_t>& phi) {
  queue<int> q;
  unordered_set<int> in_queue;

  for (int i = 0; i < m; i++) {
    vector<pair<int, uintk_t>> rem_lits = get_rem_lits(clauses[i], phi);
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
      vector<pair<int, uintk_t>> rem_lits = get_rem_lits(clauses[i], phi);
      for (auto p : rem_lits) {
        assert(p.first != u);
        assert(p.first != -1);
        phi[p.first] |= p.second;
        if (!in_queue.count(p.first) && !in_queue.count(COMPL(p.first))) {
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
    assert((phi_master[LIT(i)] ^ phi_master[NEG_LIT(i)]) == -1);
  }

  vector<int> pi(n);
  iota(pi.begin(), pi.end(), 0);

  uintk_t conj = satisfies(phi_master);
  int period;
  for (period = 0; !conj; period++) {
    shuffle(pi.begin(), pi.end(), RNG::m_mt);
    vector<uintk_t> phi_active(2 * n, 0);
    bool change = true;

    for (int i = 0; i < n; i++) {
      if (change) {
        unit_propagation(phi_active);
        change = false;
      }
      uintk_t unassigned =
          ~(phi_active[LIT(pi[i])] | phi_active[NEG_LIT(pi[i])]);
      if (unassigned) {
        phi_active[LIT(pi[i])] |= phi_master[LIT(pi[i])] & unassigned;
        phi_active[NEG_LIT(pi[i])] |= phi_master[NEG_LIT(pi[i])] & unassigned;
        change = true;
        assert((phi_active[LIT(pi[i])] ^ phi_active[NEG_LIT(pi[i])]) == -1);
      }
    }
    for (int i = 0; i < n; i++) {
      assert((phi_active[LIT(i)] ^ phi_active[NEG_LIT(i)]) == -1);
    }
    if (phi_active == phi_master) {
      int k = RNG::uniform_int(n);
      phi_active[LIT(k)] = ~phi_active[LIT(k)];
      phi_active[NEG_LIT(k)] = ~phi_active[NEG_LIT(k)];
    }
    phi_master = phi_active;

    // phi_master must not have conflicts or unassigned positions
    for (int i = 0; i < n; i++) {
      assert((phi_master[LIT(i)] ^ phi_master[NEG_LIT(i)]) == -1);
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
