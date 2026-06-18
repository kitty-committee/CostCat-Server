#include <costcat/costcat.hpp>
using namespace nathcat::cost;

std::vector<struct debt>
nathcat::cost::simplify_with_collector(int collector,
                                       std::vector<struct balance> balances) {
  std::vector<struct debt> debts;

  for (int i = 0; i < balances.size(); i++) {
    // If this is the collectors balance, no action is needed
    if (balances[i].user == collector)
      continue;

    // If the balance is below zero, create a debt indicating that the user must
    // pay the collector the value of their balance. Otherwise, create a debt
    // indicating that the collector must pay the user the value of the user's
    // balance.
    if (balances[i].amount < 0) {
      debts.push_back({collector, balances[i].user, -balances[i].amount});
    } else {
      debts.push_back({balances[i].user, collector, balances[i].amount});
    }
  }

  return debts;
}
