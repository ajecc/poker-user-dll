#pragma once
#include "clean_windows.h"
#include "debug.h"
#include <random>
#include <chrono>
#include <filesystem>

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
Generates, at compile time, combinations up to comb(N, K).
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


/*
Generates a random float in the range [lower_bound, upper_bound]

Parameters: IN float lower_bound - the lower bound of the range
			IN float upper_bound - the upper bound of the range

Returns: float - the random number in range [lower_bound, upper_bound]
*/
inline float
generate_random(float lower_bound = 0, float upper_bound = 1)
{
	static std::mt19937_64 rng_generator(
		std::chrono::steady_clock::now().time_since_epoch().count()
	);
	return std::uniform_real_distribution<float>
		(lower_bound, upper_bound)(rng_generator);
}

/*
Checks if an element is contained in a container.
The container needs to have an iterator defined.
Works for STL containers out of the box.

Parameters: IN T1 container - the container
			IN T2 elem - the element to be checked

Returns: bool - true if elem is in container and false otherwise
*/
template<typename T1, typename T2> inline bool 
contains(const T1& container, T2 elem)
{
	return std::find(all(container), elem) != container.end();
}

/*
Concatenates two std::vectors.

Parameters: IN std::vector<T> lhs - the first vector
			IN std::vector<T> rhs - the second vector

Returns: std::vector<T> - the (lhs + rhs) vector
*/
template<typename T> inline std::vector<T>
concat(const std::vector<T>& lhs, const std::vector<T>& rhs)
{
	std::vector<T> result = lhs;
	result.insert(result.end(), rhs.begin(), rhs.end());
	return result;
}


/*
Opens (or creates) a cache. The cache is basically a memory mapped file.
Note that the cache is in a shared memory region - it wont be loaded more
than once if multiple processes want to use this cache.

Parameters: IN std::string cache_name - the name of the file to be loaded in memory
			IN std::string shared_cache_name - the name of the shared memory region
						make sure this is unique (one can generate a GUID for this)

Returns: void* - the pointer to the cache
*/
inline const void* 
open_cache(const std::string& cache_name, const std::string& shared_cache_name)
{
	auto file_size = (DWORD)std::filesystem::file_size(cache_name);
	DLOG(INFO, ("Shared memory file size = " + std::to_string(file_size)).c_str());
	// TODO: find a way to free this handle
	HANDLE cache_map = OpenFileMappingA(
		FILE_MAP_READ,
		FALSE,
		shared_cache_name.c_str()
	);
	if (cache_map == NULL)
	{
		DWORD error = GetLastError();
		if (error == ERROR_FILE_NOT_FOUND)
		{
			HANDLE cached_handle = CreateFileA(
				cache_name.c_str(),
				GENERIC_ALL,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL,
				OPEN_EXISTING,
				0,
				NULL
			);
			if (cached_handle == INVALID_HANDLE_VALUE)
			{
				throw std::runtime_error("open_cache: CreateFileA failed with last err-code=" 
					+ std::to_string(error));
			}
			cache_map = CreateFileMappingA(
				cached_handle,
				NULL,
				PAGE_READWRITE,
				0,
				file_size,
				shared_cache_name.c_str()
			);
			CloseHandle(cached_handle);
			if (cache_map == NULL)
			{
				error = GetLastError();
				throw std::runtime_error("open_cache: CreateFileMappingA failed with last err-code=" 
					+ std::to_string(error));
			}
			LOG_F(INFO, "Successfully created the shared memory region");
		}
		else
		{
			throw std::runtime_error("create_all_hand_board_results: OpenFileMappingA failed with last err-code=" 
				+ std::to_string(error));
		}
	}
	const void* cache = MapViewOfFile(
		cache_map,
		FILE_MAP_READ,
		0,
		0,
		file_size	
	);
	if (cache == NULL)
	{
		throw std::runtime_error("cache: MapViewOfFile failed with last err-code=" 
			+ std::to_string(GetLastError()));
	}
	LOG_F(INFO, "Successfully opened shared memory region: %s", cache_name.c_str());
	return cache;
}
