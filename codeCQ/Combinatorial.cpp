
#include "Combinatorial.h"
#include <functional>
#include <cmath>

using namespace std;

void Combinatorial::find_combinations(vector<int> &nums, int k, vector<int> &combination, set<vector<int>> &result, set
        <vector<int>> &result_ind) {
    vector<int> cur(k);
    vector<int> cur2(k);
    function<void(int, int)> dfs = [&](int start, int depth) {
        if (depth == k) {
            if (result.find(cur) == result.end()) {
                result_ind.insert(cur2);
            }
            result.insert(cur);
            return;
        }
        for (int i = start; i < nums.size(); i++) {
            cur[depth] = nums[i];
            cur2[depth] = i;

            dfs(i + 1, depth + 1);
        }
    };

    dfs(0, 0);
}

void Combinatorial::find_all_combinations(vector<int> &nums, set<vector<int>> &result, int min_k, int max_k) {
    set<vector<int>> result_ind;
    for (int k = min_k; k <= max_k; k++) {
        vector<int> combination;
        find_combinations(nums, k, combination, result, result_ind);
    }
    result = result_ind;
}

std::set<std::tuple<int, std::vector<int>>> Combinatorial::findCombinationsWithLimit(std::vector<int> &services, std::vector<int> const &all_services, int target,int distance) {
    unordered_map<int, int> countMap;
    for (auto service: services) {
        countMap[service] = std::ceil(double(target) / service);
    }

    std::set<std::tuple<int, std::vector<int>>> result;
    vector<int> currentCombination;
    findCombination(services,all_services, target,distance, countMap, currentCombination, result);
    return result;
}

void Combinatorial::findCombination(vector<int> &services, vector<int> const &all_services, int target,int distance,  unordered_map<int, int> &countMap,
                                    vector<int> &currentCombination,
                                    std::set<std::tuple<int, std::vector<int>>> &result) {
    int currentSum = 0;
    for (int num: currentCombination)
        currentSum += num;

    if (currentSum >= target) {
        result.insert(make_tuple(currentSum, currentCombination));
        return;
    }

    for (int i = 0; i < services.size(); i++) {
        int num = services[i];
        if (countMap[num] > 0) {
            if (currentCombination.size() > 0) {
                if (currentCombination[currentCombination.size() - 1] == num) {
                    if(distance > Parameters::INTRA_ORDER_DELIVERY){
                        if(std::count(all_services.begin(),all_services.end(),num)<=1){
                            continue;
                        }
                    }
                }
            }
            countMap[num]--;
            currentCombination.push_back(num);
            findCombination(services, all_services,  target, distance,  countMap,currentCombination,result);
            currentCombination.pop_back();
            countMap[num]++;
        }
    }
}