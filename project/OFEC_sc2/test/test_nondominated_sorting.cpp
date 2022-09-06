#include "../utility/catch.hpp"
#include "../utility/nondominated_sorting/fast_sort.h"
#include "../utility/nondominated_sorting/deductive_sort.h"
#include "../utility/nondominated_sorting/filter_sort.h"
#include <iostream>

using namespace ofec;

TEST_CASE("fast noodominated sorting", "[FNS]") {
	std::vector<std::vector<int>> int_2d_array{ {2,4},{4,8},{6,12},{3,3},{6,6},{9,9},{4,2},{8,4},{12,6} };
	std::vector<std::vector<int>*> data;
	for (auto& i : int_2d_array)
		data.emplace_back(&i);
	std::vector<int> rank;
	std::vector<optimization_mode> opt_mode(2, optimization_mode::Minimization);

	NS::fast_sort<int>(data, rank, opt_mode);
    for (auto i : rank)
        std::cout << i << " ";
    std::cout << std::endl;

	NS::fast_sort_p<int>(data, rank, opt_mode, 8);
	for (auto i : rank)
		std::cout << i << " ";
	std::cout << std::endl;
}

TEST_CASE("Deductive Sort", "[DeductiveSort]") {
	std::vector<std::vector<int>> int_2d_array{ {2,4},{4,8},{6,12},{3,3},{6,6},{9,9},{4,2},{8,4},{12,6} };
	std::vector<std::vector<int>*> data;
	for (auto& i : int_2d_array)
		data.emplace_back(&i);
	std::vector<int> rank;
	std::vector<optimization_mode> opt_mode(2, optimization_mode::Minimization);

	NS::deductive_sort<int>(data, rank, opt_mode);
	for (auto i : rank)
		std::cout << i << " ";
	std::cout << std::endl;
}

TEST_CASE("Filter Sort", "[FilterSort]") {
//    std::vector<std::vector<float>> int_2d_array{ {2,4},{4,8},{6,12},{3,3},{6,6},{9,9},{4,2},{8,4},{12,6} };
    const size_t N = 250, M = 2;

    std::vector<std::vector<float>> int_2d_array(N, std::vector<float>(M));
    for (int i = 0; i < N; ++i) {
        int_2d_array[i][0] = rand();
        int_2d_array[i][1] = 1;
    }
    std::vector<std::vector<float>*> data;
    for (auto& i : int_2d_array)
        data.emplace_back(&i);
    std::vector<int> rank;
    std::vector<optimization_mode> opt_mode({optimization_mode::Minimization, optimization_mode::Maximization});

    const int N_ = data.size(), M_ = data[0]->size();
    std::cout << N_ << " " << M_ << std::endl;

    NS::fast_sort<float>(data, rank, opt_mode);
    for (auto i : rank)
        std::cout << i << " ";
    std::cout << std::endl;

    NS::fast_sort_p<float>(data, rank, opt_mode, 8);
    for (auto i : rank)
        std::cout << i << " ";
    std::cout << std::endl;

    NS::filter_sort<float>(data, rank, opt_mode);
    for (auto i : rank)
        std::cout << i << " ";
    std::cout << std::endl;

    NS::filter_sort_p<float>(data, rank, opt_mode);
    for (auto i : rank)
        std::cout << i << " ";
    std::cout << std::endl;
}