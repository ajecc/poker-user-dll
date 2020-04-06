#pragma once

#define all(X) (X).begin(), (X).end()

#define EPS 0.001

#define FP_ARE_EQUAL(LHS, RHS) (abs((LHS) - (RHS)) < EPS)
#define FP_ARE_DIFFERENT(LHS, RHS) (abs((LHS) - (RHS)) >= EPS)


template<typename T, typename F>
void insertion_sort(T* to_sort, const size_t& to_sort_size, F cmp_func)
{
	for (size_t i = 1; i < to_sort_size; i++)
	{
		T key = to_sort[i];
		int j = i - 1;
		while (j >= 0 && !cmp_func(to_sort[j], key))
		{
			to_sort[j + 1] = to_sort[j];
			j--;
		}
		to_sort[j + 1] = key;
	}
}


template<typename T, typename F>
void bubble_sort(T* to_sort, const size_t& to_sort_size, F cmp_func)
{
	for (int i = 0; i < (int)to_sort_size - 1; i++)
	{
		bool swapped = false;
		for (int j = 0; j < (int)to_sort_size - i - 1; j++)
		{
			if (!cmp_func(to_sort[j], to_sort[j + 1]))
			{
				std::swap(to_sort[j], to_sort[j + 1]);
				swapped = true;
			}
		}
		if (!swapped)
		{
			break;
		}
	}
}


template<typename T>
void insertion_sort(T* to_sort, const size_t& to_sort_size)
{
	for (size_t i = 1; i < to_sort_size; i++)
	{
		T key = to_sort[i];
		int j = i - 1;
		while (j >= 0 && to_sort[j] > key)
		{
			to_sort[j + 1] = to_sort[j];
			j--;
		}
		to_sort[j + 1] = key;
	}
}
