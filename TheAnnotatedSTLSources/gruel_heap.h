#pragma once


#include "gruel_iterator.h"


namespace gruel {


	// 这一组函数不允许指定比较规则
	template <typename RandomAccessIterator>
	inline void push_heap(RandomAccessIterator first,
		RandomAccessIterator last) {
		_push_heap_aux(first, last, difference_type(first), 
			value_type(first));
	}


	template <typename RandomAccessIterator, typename Distance, typename T>
	inline void _push_heap_aux(RandomAccessIterator first,
		RandomAccessIterator last, Distance *, T *) {
		_push_heap(first, Distance(last - first - 1), Distance(0), 
			T(*(last - 1)));
	}


	template <typename RandomAccessIterator, typename Distance, typename T>
	void _push_heap(RandomAccessIterator first, Distance holeIndex,
		Distance topIndex, T value) {
		auto parent = (holeIndex - 1) / 2;
		while (holeIndex > topIndex && *(first + parent) < value) {
			*(first + holeIndex) = *(first + parent);
			holeIndex = parent;
			parent = (holeIndex - 1) / 2;
		}
		*(first + holeIndex) = value;
	}


	// 这一组函数允许指定比较规则
	template <typename RandomAccessIterator, typename Compare>
	inline void push_heap(RandomAccessIterator first,
		RandomAccessIterator last, Compare comp) {
		_push_heap_aux(first, last, difference_type(first),
			value_type(first), comp);
	}


	template <typename RandomAccessIterator, typename Distance, 
		typename T, typename Compare>
		inline void _push_heap_aux(RandomAccessIterator first,
			RandomAccessIterator last, Distance *, T *, Compare comp) {
		_push_heap(first, Distance(last - first - 1), Distance(0), T
		(*(last - 1)), comp);
	}


	template <typename RandomAccessIterator, typename Distance, 
		typename T, typename Compare>
		void _push_heap(RandomAccessIterator first, Distance holeIndex,
			Distance topIndex, T value, Compare comp) {
		auto parent = (holeIndex - 1) / 2;
		while (holeIndex > topIndex && comp(*(first + parent), value)) {
			*(first + holeIndex) = *(first + parent);
			holeIndex = parent;
			parent = (holeIndex - 1) / 2;
		}
		*(first + holeIndex) = value;
	}


	template <typename RandomAccessIterator>
	inline void pop_heap(RandomAccessIterator first,
		RandomAccessIterator last) {
		_pop_heap_aux(first, last, value_type(first));
	}


	template <typename RandomAccessIterator, typename T>
	inline void _pop_heap_aux(RandomAccessIterator first,
		RandomAccessIterator last, T *) {
		_pop_heap(first, last - 1, last - 1, T(*(last - 1)),
			difference_type(first));
	}


	template <typename RandomAccessIterator, typename Distance, typename T>
	inline void _pop_heap(RandomAccessIterator first,
		RandomAccessIterator last, RandomAccessIterator result, 
		T value, Distance *) {
		*result = *first;
		_adjust_heap(first, Distance(0), Distance(last - first), value);
	}


	template <typename RandomAccessIterator , typename Distance, typename T>
	void _adjust_heap(RandomAccessIterator first, Distance holeIndex,
		Distance len, T value) {
		auto topIndex = holeIndex;
		auto rightChild = 2 * holeIndex + 2;
		while (rightChild < len) {
			if (*(first + rightChild) < *(first + rightChild - 1))
				--rightChild;
			*(first + holeIndex) = *(first + rightChild);
			holeIndex = rightChild;
			rightChild = 2 * rightChild + 2;
		}
		if (rightChild == len) {
			*(first + holeIndex) = *(first + rightChild - 1);
			holeIndex = rightChild - 1;
		}
	}


	template <typename RandomAccessIterator>
	void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
		while ((last - first) > 1)
			pop_heap(first, last);
	}


	template <typename RandomAccessIterator>
	inline void make_heap(RandomAccessIterator first,
		RandomAccessIterator last) {
		_make_heap(first, last, value_type(first), difference_type(first));
	}


	template <typename RandomAccessIterator, typename Distance, typename T>
	void _make_heap(RandomAccessIterator first,
		RandomAccessIterator last, T *, Distance *) {
		if (last - first < 2) return;
		auto len = last - first;
		auto holeIndex = (len - 2) / 2;
		while (true) {
			_adjust_heap(first, holeIndex, len, T(*(first + holeIndex)));
			if (holeIndex == 0) return;
			--holeIndex;
		}
	}
}
