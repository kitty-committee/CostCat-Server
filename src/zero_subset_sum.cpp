#include <costcat/costcat.hpp>
#include <cstdint>
using namespace nathcat::cost;

std::vector<int> nathcat::cost::subset(std::uint64_t V, std::vector<int> set) {
  assert(set.size() < 64);

  std::vector<int> v;

  for (int i = 0; i < set.size(); i++) {
    if (((V >> i) & 1))
      v.push_back(set[i]);
  }

  return v;
}

std::vector<std::vector<int>>
nathcat::cost::zero_subset_sum(std::vector<int> set) {
  assert(set.size() < 64);
  std::uint64_t V = 1;
  std::uint64_t maxV = ((1 << set.size()) * 2) - 1;

  std::vector<std::vector<int>> zero_sum_subsets;

  while (V <= maxV) {
    std::vector<int> ss = nathcat::cost::subset(V++, set);

    int sum = 0;
    for (int i = 0; i < ss.size(); i++) {
      sum += ss[i];
    }

    if (sum == 0)
      zero_sum_subsets.push_back(ss);
  }

  return zero_sum_subsets;
}
