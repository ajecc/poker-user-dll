#pragma once
#include <random>
#include <chrono>

#define all(X) (X).begin(), (X).end()

#define EPS 0.001

#define FP_ARE_EQUAL(LHS, RHS) (abs((LHS) - (RHS)) < EPS)
#define FP_ARE_DIFFERENT(LHS, RHS) (abs((LHS) - (RHS)) >= EPS)


/*
Sort the given array with insertion sort.
This is useful for small arrays (< 10 elements).

Parameters: OUT T* to_sort -- the array to sort.
			IN size_t to_sort_size -- the size of this array
			IN F cmp_func -- the compare function used to sort the array

Returns: none
*/
template<typename T, typename F> inline void
insertion_sort(T* to_sort, const size_t& to_sort_size, F cmp_func)
{
	for (size_t i = 1; i < to_sort_size; i++)
	{
		T key = to_sort[i];
		int j = (int)i - 1;
		while (j >= 0 && !cmp_func(to_sort[j], key))
		{
			to_sort[j + 1] = to_sort[j];
			j--;
		}
		to_sort[j + 1] = key;
	}
}


/*
Generates at compile time combinations up to comb(N, K).
Simply instantiate this structure, and the comb field returns
the combinations.
*/
template<int N, int K>
struct comb_t 
{
	constexpr comb_t() : comb()
	{
		comb[1][0] = comb[1][1] = 1;
		for (int i = 2; i < N + 1; i++)
		{
			comb[i][0] = 1;
			if (i < K + 1)
			{
				comb[i][i] = 1;
			}
			for (int j = 1; j < i && j < K + 1; j++)
			{
				comb[i][j] = comb[i - 1][j - 1] + comb[i - 1][j];
			}
		}
	}
	unsigned int comb[N + 1][K + 1];
};


inline float
generate_random(float lower_bound = 0, float upper_bound = 1)
{
	static std::mt19937_64 rng_generator(
		std::chrono::steady_clock::now().time_since_epoch().count()
	);
	return std::uniform_real_distribution<float>
		(lower_bound, upper_bound)(rng_generator);
}


template<typename T1, typename T2> inline bool 
contains(T1 container, T2 elem)
{
	return std::find(all(container), elem) != container.end();
}
