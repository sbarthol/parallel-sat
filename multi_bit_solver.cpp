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

int MultiBitSolver::count_dups(const std::vector<uintk_t>& phi) {
  int count = 0;
  for (int i = 1; i < sizeof(uintk_t) * 8; i++) {
    bool is_dupe = false;
    for (int j = 0; j < i && !is_dupe; j++) {
      bool same = true;
      for (int k = 0; k < n && same; k++) {
        bool both_ones =
            (phi[LIT(k)] & (1L << i)) > 0 && (phi[LIT(k)] & (1L << j)) > 0;
        bool both_zeros =
            (phi[LIT(k)] & (1L << i)) == 0 && (phi[LIT(k)] & (1L << j)) == 0;
        same = same && (both_ones || both_zeros);
      }
      if (same) {
        is_dupe = true;
      }
    }
    if (is_dupe) {
      count++;
    }
  }
  return count;
}

MultiBitSolver::uintk_t MultiBitSolver::compute_duplicate_mask(
    const std::vector<uintk_t>& phi) {
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

  // Todo: remove all these assert loops (here and other places) once it is
  // battle tested
  for (int i = 1; i < p; i++) {
    if (m_dups & (1L << (p - i - 1))) {
      bool is_dupe = false;
      for (int j = 0; j < i && !is_dupe; j++) {
        bool is_dupe_pos = true;
        for (int k = 0; k < n && is_dupe_pos; k++) {
          bool both_ones = (phi[LIT(k)] & (1L << (p - i - 1))) > 0 &&
                           (phi[LIT(k)] & (1L << (p - j - 1))) > 0;
          bool both_zeros = (phi[LIT(k)] & (1L << (p - i - 1))) == 0 &&
                            (phi[LIT(k)] & (1L << (p - j - 1))) == 0;
          is_dupe_pos &= both_ones || both_zeros;
        }
        if (is_dupe_pos) {
          is_dupe = true;
        }
      }
      assert(is_dupe);
    }
  }

  return m_dups;
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

    // phi_master must not have conflicts or unassigned positions
    for (int i = 0; i < n; i++) {
      assert((phi_master[LIT(i)] ^ phi_master[NEG_LIT(i)]) == (uintk_t)(-1));
    }

    printf("before dups = %d\n", count_dups(phi_master));
    uintk_t dup_mask = compute_duplicate_mask(phi_master);
    for (int i = 0; i < n; i++) {
      phi_master[LIT(i)] ^= dup_mask & get_random();
      phi_master[NEG_LIT(i)] = ~phi_master[LIT(i)];
    }
    printf("after dups = %d\n", count_dups(phi_master));

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
