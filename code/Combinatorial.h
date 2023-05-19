
#ifndef CODE_COMBINATORIAL_H
#define CODE_COMBINATORIAL_H

#include <set>
#include <vector>
#include <unordered_map>
#include <tuple>

class Combinatorial {
public:
    static std::set<std::tuple<int, std::vector<int>>> findCombinationsWithLimit(std::vector<int> &nums, int target);

    static void findCombination(std::vector<int> &nums, int target, std::unordered_map<int, int> &countMap,
                                std::vector<int> &currentCombination,
                                std::set<std::tuple<int, std::vector<int>>> &result);

    static void
    find_all_combinations(std::vector<int> &nums, std::set< std::vector<int>> &result, int min_k,
                          int max_k);

    static void find_combinations(std::vector<int> &nums, int k, std::vector<int> &combination,
                                  std::set<std::vector<int>> &result,
                                  std::set<std::vector<int>> &result_ind);
};


#endif //CODE_COMBINATORIAL_H
