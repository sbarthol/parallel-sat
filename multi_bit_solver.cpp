//#define NDEBUG

#include "multi_bit_solver.h"

#include <algorithm>
#include <cassert>
#include <omp.h>

#include <numeric>
#include <queue>
#include <unordered_set>

#include "rng.h"

#define LIT(i) ((i) << 1)
#define NEG_LIT(i) (((i) << 1) | 1)
#define COMPL(i) ((i) ^ 1)

#define REMOVE_DUPS_FREQ 5
#define N_OMP_THREADS 1

using namespace std;

MultiBitSolver::MultiBitSolver(vector<vector<int>> clauses_, int n_)
    : clauses(clauses_), n(n_) {
  m = clauses_.size();
  assert(m > 0);
  phi_master = vector<uintk_t>(2 * n, 0);
  for (int i = 0; i < n; i++) {
    phi_master[LIT(i)] = get_random();
    phi_master[NEG_LIT(i)] = ~phi_master[LIT(i)];
  }
  inv_clauses = vector<vector<int>>(2 * n);
  for (int i = 0; i < m; i++) {
    for (int j : clauses[i]) {
      inv_clauses[j].push_back(i);
    }
  }
}

MultiBitSolver::uintk_t MultiBitSolver::get_random() {
  if (sizeof(uintk_t) <= 4) {
    return (uintk_t)RNG::m_mt();
  } else {
    return (uintk_t)(long(RNG::m_mt()) << 32) | RNG::m_mt();
  }
}

MultiBitSolver::uintk_t
MultiBitSolver::compute_duplicate_mask(const std::vector<uintk_t> &phi) {
  uintk_t m_dups = 0;
  int p = sizeof(uintk_t) * 8;
  for (int j = p - 1; j > 0; j--) {
    uintk_t m_col = (1L << j) - 1;
    for (int i = 0; i < n; i++) {
      if (phi[LIT(i)] & (1L << j)) {
        m_col &= phi[LIT(i)];
      } else {
        m_col &= phi[NEG_LIT(i)];
      }
      if (!m_col) {
        break;
      }
    }
    m_dups |= m_col;
  }

  return m_dups;
}

MultiBitSolver::uintk_t MultiBitSolver::satisfies(const vector<uintk_t> &phi) {
  uintk_t conj = -1;
  for (vector<int> &clause : clauses) {
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

vector<pair<int, MultiBitSolver::uintk_t>>
MultiBitSolver::get_rem_lits(const vector<int> &clause,
                             const vector<uintk_t> &phi) {
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

void MultiBitSolver::unit_propagation(vector<uintk_t> &phi) {
  vector<vector<pair<int, uintk_t>>> tmp(N_OMP_THREADS);
  vector<int> all_rem_lits;
  unordered_set<int> all_rem_lits_set;

#pragma omp parallel for schedule(static), shared(tmp)
  for (int i = 0; i < m; i++) {
    vector<pair<int, uintk_t>> rem_lits = get_rem_lits(clauses[i], phi);
    int thread_num = omp_get_thread_num();
    for (auto p : rem_lits) {
      tmp[thread_num].push_back(p);
    }
  }

  for (auto &rem_lits : tmp) {
    for (auto p : rem_lits) {
      if (!all_rem_lits_set.count(COMPL(p.first))) {
        // Avoid bit conflicts
        phi[p.first] |= p.second;
        all_rem_lits_set.insert(p.first);
      }
    }
  }

  all_rem_lits = vector<int>(all_rem_lits_set.begin(), all_rem_lits_set.end());

  while (all_rem_lits.size()) {
    tmp = vector<vector<pair<int, uintk_t>>>(N_OMP_THREADS);

#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < all_rem_lits.size(); i++) {
      int u = all_rem_lits[i];
      int thread_num = omp_get_thread_num();
      for (int j : inv_clauses[COMPL(u)]) {
        vector<pair<int, uintk_t>> rem_lits = get_rem_lits(clauses[j], phi);
        for (auto p : rem_lits) {
          tmp[thread_num].push_back(p);
        }
      }
    }

    all_rem_lits_set.clear();

    for (auto &rem_lits : tmp) {
      for (auto p : rem_lits) {
        if (!all_rem_lits_set.count(COMPL(p.first))) {
          // Avoid bit conflicts
          phi[p.first] |= p.second;
          all_rem_lits_set.insert(p.first);
        }
      }
    }
    all_rem_lits =
        vector<int>(all_rem_lits_set.begin(), all_rem_lits_set.end());
  }
}

void MultiBitSolver::remove_dups() {
  while (true) {
    vector<uintk_t> phi = in_q.wait_and_pop();
    uintk_t dup_mask = compute_duplicate_mask(phi);
    if (dup_mask) {
      out_q.push(dup_mask);
    }
  }
}

vector<bool> MultiBitSolver::solve(int &periods) {
  dup_task = thread(&MultiBitSolver::remove_dups, this);
  dup_task.detach();

  omp_set_num_threads(N_OMP_THREADS);

  // phi_master must not have conflicts or unassigned positions
  assert(phi_master.size() == 2 * n);
  for (int i = 0; i < n; i++) {
    assert((phi_master[LIT(i)] ^ phi_master[NEG_LIT(i)]) == (uintk_t)(-1));
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
        assert((phi_active[LIT(pi[i])] ^ phi_active[NEG_LIT(pi[i])]) ==
               (uintk_t)(-1));
      } else {
        assert((phi_active[LIT(pi[i])] ^ phi_active[NEG_LIT(pi[i])]) ==
               (uintk_t)(-1));
      }
    }
    for (int i = 0; i < n; i++) {
      assert((phi_active[LIT(i)] ^ phi_active[NEG_LIT(i)]) == (uintk_t)(-1));
    }
    if (phi_active == phi_master) {
      int k = RNG::uniform_int(n);
      phi_active[LIT(k)] = ~phi_active[LIT(k)];
      phi_active[NEG_LIT(k)] = ~phi_active[NEG_LIT(k)];
    }
    phi_master = phi_active;

    if (!(period % REMOVE_DUPS_FREQ)) {
      in_q.push(phi_master);
    }

    if (!out_q.empty()) {
      uintk_t dup_mask;
      while (out_q.try_pop(dup_mask)) {
      }
      for (int i = 0; i < n; i++) {
        phi_master[LIT(i)] ^= dup_mask & get_random();
        phi_master[NEG_LIT(i)] = ~phi_master[LIT(i)];
      }
    }

    // phi_master must not have conflicts or unassigned positions
    for (int i = 0; i < n; i++) {
      assert((phi_master[LIT(i)] ^ phi_master[NEG_LIT(i)]) == (uintk_t)(-1));
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
