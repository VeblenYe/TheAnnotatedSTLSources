#pragma once


#include <cstring>
#include "gruel_iterator.h"
#include "gruel_type_traits.h"
#include "gruel_functional.h"


namespace gruel {

	
	/* equal：判断两个区间元素是否相等，第二个序列多出来的元素不予考虑 */
	template <typename InputIterator1, typename InputIterator2>
	inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
		for (; first1 != last1; ++first1, ++first2)
			if (*first1 != *first2)
				return false;
		return true;
	}

	template <typename InputIterator1, typename InputIterator2, typename BinaryPredicate>
	inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
		BinaryPredicate binary_pred) {
		for (; first1 != last1; ++first1, ++first2)
			if (!binary_pred(*first1, *first2))
				return false;
		return true;
	}


	/* fill：将[first, last)中的所有元素改填新值 */
	template <typename ForwardIterator, typename T>
	void fill(ForwardIterator first, ForwardIterator last, const T &value) {
		for (; first != last; ++first)
			*first = value;
	}


	/* fill_n：将[first, first+n]中的所有元素改填新值，返回的迭代器指向被填入的最后一个元素的下一个位置 */
	// 注意这里的迭代器类型为OutputIterator
	template <typename OutputIterator, typename Size, typename T>
	OutputIterator fill_n(OutputIterator first, Size n, const T &value) {
		for (; n != 0; --n, ++first)
			*first = value;
		return first;
	}

	// 前置声明
	template <typename ForwardIterator1, typename ForwardIterator2, typename T>
	inline void _iter_swap(ForwardIterator1 a, ForwardIterator2 b, T *);

	/* iter_swap：交换两个ForwardIterator */
	template <typename ForwardIterator1, typename ForwardIterator2>
	inline void iter_swap(ForwardIterator1 a, ForwardIterator2 b) {
		return _iter_swap(a, b, value_type(a));
	}

	// 使用value_type萃取出a迭代器的指向物类型，以便用于交换
	template <typename ForwardIterator1, typename ForwardIterator2, typename T>
	inline void _iter_swap(ForwardIterator1 a, ForwardIterator2 b, T *) {
		T tmp = *a;
		*a = *b;
		*b = tmp;
	}


	/* lexicographical_compare：以字典序对两个序列进行比较 */
	/* 当发现第一组不相等的元素时，有四种情况 
	1. *first1较小，返回true，否则返回false;
	2. 到达last1而未到达last2，则返回true;
	3. 到达last2而未到达last1，则返回false;
	4. 同时到达last1和last2，则返回false。 */
	template <typename InputIterator1, typename InputIterator2>
	bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, InputIterator2 last2) {
		for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
			if (*first1 < *first2)
				return true;
			if (*first2 < *first1)
				return false;
		}
		return first1 == last1 && first2 != last2;
	}

	template <typename InputIterator1, typename InputIterator2, typename Compare>
	bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, InputIterator2 last2, Compare comp) {
		for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
			if (comp(*first1, *first2))
				return true;
			if (comp(*first2, *first1))
				return false;
		}
		return first1 == last1 && first2 != last2;
	}

	// 针对原生指针的特化版本，然而这里实际上并不是函数的全特化
	inline bool lexicographical_compare(const unsigned char *first1, const unsigned char *last1,
		const unsigned char *first2, const unsigned char *last2) {
		const std::size_t len1 = last1 - first1;
		const std::size_t len2 = last2 - first2;
		const int result = std::memcmp(first1, first2, min(len1, len2));
		return result != 0 ? result < 0 : len1 < len2;
	}

	inline bool lexicographical_compare(const char *first1, const char *last1,
		const char *first2, const char *last2) {
		const std::size_t len1 = last1 - first1;
		const std::size_t len2 = last2 - first2;
		const int result = std::memcmp(first1, first2, min(len1, len2));
		return result != 0 ? result < 0 : len1 < len2;
	}


	/* max：取二者中较大者 */
	template <typename T>
	inline bool max(const T &a, const T &b) {
		return a < b ? b : a;
	}

	template <typename T, typename Compare>
	inline bool max(const T &a, const T &b, Compare comp) {
		return comp(a, b) ? b : a;
	}


	/* min：取二者中较小者 */
	template <typename T>
	inline bool min(const T &a, const T &b) {
		return a < b ? a : b;
	}

	template <typename T, typename Compare>
	inline bool min(const T &a, const T &b, Compare comp) {
		return comp(a, b) ? a : b;
	}


	/* mismatch：找出两个序列之间的第一个不匹配点 */
	template <typename InputIterator1, typename InputIterator2>
	pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2) {
		for (; first1 != last1 && *first1 == *first2; ++first1, ++first2);
		return { first1, first2 };
	}

	template <typename InputIterator1, typename InputIterator2, typename BinaryPredicate>
	pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, BinaryPredicate binary_pred) {
		for (; first1 != last1 && binary_pred(*first1, *first2); ++first1, ++first2);
		return { first1, first2 };
	}

	
	/* swap：交换俩个对象的内容 */
	template <typename T>
	inline void swap(T &lhs, T &rhs) {
		T tmp = lhs;
		lhs = rhs;
		rhs = tmp;
	}


	/* copy：将输入区间[first, last)内的元素复制到输出空间[result, result+(last-first))内 */

	template <typename InputIterator, typename OutputIterator, typename Distance>
	inline OutputIterator _copy_d(InputIterator first, InputIterator last, OutputIterator result, Distance *) {
		for (Distance n = last - first; n > 0; --n, ++first, ++result)
			*first = *result;
		return result;
	}

	// 迭代器类型为input_iterator，运用迭代器判等，速度慢
	template <typename InputIterator, typename OutputIterator>
	inline OutputIterator _copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag) {
		for (; first != last; ++first, ++result)
			*first = *result;
		return result;
	}

	// 迭代器类型为random_access_iterator，运用距离判断，速度快
	template <typename InputIterator, typename OutputIterator>
	inline OutputIterator _copy(InputIterator first, InputIterator last, OutputIterator result, random_access_iterator_tag) {
		return _copy_d(first, last, result, difference_type(first));
	}

	// 有trivial_assignment_operator，直接动用memmove
	template <typename T>
	inline T *_copy_t(const T *first, const T *last, T *result, _true_type) {
		std::memmove(result, first, sizeof(T) * (last - first));
		return result + (last - first);
	}

	// 没有trivial_assignment_operator，则调用_copy_d，原始指针也是RAI
	template <typename T>
	inline T *_copy_t(const T *first, const T *last, T *result, _false_type) {
		return _copy_d(first, last, result, (std::ptrdiff_t)nullptr);
	}

	// 完全泛化版，不晓得这里为什么要用仿函数
	template <typename InputIterator, typename OutputIterator>
	struct _copy_dispatch {
		OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
			return _copy(first, last, result, iterator_category(first));
		}
	};

	// 偏特化版，迭代器类型为指针
	template <typename T>
	struct _copy_dispatch<T *, T *> {
		T *operator()(T *first, T *last, T *result) {
			using t = typename _type_traits<T>::has_trivial_assignment_operator;
			return _copy_t(first, last, result, t());
		}
	};

	template <typename T>
	struct _copy_dispatch<const T *, T *> {
		T *operator()(const T *first, const T *last, T *result) {
			using t = typename _type_traits<T>::has_trivial_assignment_operator;
			return _copy_t(first, last, result, t());
		}
	};

	// 向外提供的泛化接口
	template <typename InputIterator, typename OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
		return _copy_dispatch(first, last, result);
	}

	// 多载函数，针对原生指针
	inline char *copy(const char *first, const char *last, char *result) {
		std::memmove(result, first, last - first);
		return result + (last - first);
	}

	inline wchar_t *copy(const wchar_t *first, const wchar_t *last, wchar_t *result) {
		std::memmove(result, first, sizeof(wchar_t) * (last - first));
		return result + (last - first);
	}


	/* copy_backward：将区间[first, last)中的每一个元素，以逆行的方式复制到以result-1为起点，方向亦为逆行的区间上 */

	template <typename BidirectionalIterator1, typename BidirectionalIterator2, typename Distance>
	inline BidirectionalIterator2 _copy_backward_d(BidirectionalIterator1 first, BidirectionalIterator1 last,
		BidirectionalIterator2 result, Distance *) {
		for (Distance n = last - first; n > 0; --n, --result, --last)
			*result = *last;
		return result;
	}

	template <typename BidirectionalIterator1, typename BidirectionalIterator2>
	inline BidirectionalIterator2 _copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last,
		BidirectionalIterator2 result, bidirectional_iterator_tag) {
		while (first != last) {
			*--result = *--last;
		}
		return result;
	}

	template <typename BidirectionalIterator1, typename BidirectionalIterator2>
	inline BidirectionalIterator2 _copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last,
		BidirectionalIterator2 result, random_access_iterator_tag) {
		return _copy_backward_d(first, last, result, difference_type(first));
	}

	template <typename T>
	inline T *_copy_backward_t(const T *first, const T *last, T *result, _true_type) {
		const std::ptrdiff_t n = last - first;
		std::memmove(result - n, first, sizeof(T) * n);
		return result - n;
	}

	template <typename T>
	inline T *_copy_backward_t(const T *first, const T *last, T *result, _false_type) {
		return _copy_backward_d(first, last, result, (std::ptrdiff_t)nullptr);
	}

	template <typename BidirectionalIterator1, typename BidirectionalIterator2>
	struct _copy_backward_dispatch {
		BidirectionalIterator2 operator()(BidirectionalIterator1 first, BidirectionalIterator1 last,
			BidirectionalIterator2 result) {
			return _copy_backward(first, last, result, iterator_category(first));
		}
	};

	template <typename T>
	struct _copy_backward_dispatch<T *, T *> {
		T *operator()(T *first, T *last, T *result) {
			using t = typename _type_traits<T>::has_trivial_assignment_operator;
			return _copy_backward_t(first, last, t());
		}
	};

	template <typename T>
	struct _copy_backward_dispatch<const T *, T *> {
		T *operator()(const T *first, const T *last, T *result) {
			using t = typename _type_traits<T>::has_trivial_assignment_operator;
			return _copy_backward_t(first, last, t());
		}
	};

	template <typename BidirectionalIterator1, typename BidirectionalIterator2>
	inline BidirectionalIterator2 copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last,
		BidirectionalIterator2 result) {
		return _copy_backward_dispatch(first, last, result);
	}

	inline char *copy_backward(const char *first, const char *last, char *result) {
		const std::ptrdiff_t n = last - first;
		std::memmove(result - n, first, n);
		return result - n;
	}

	inline wchar_t *copy_backward(const wchar_t *first, const wchar_t *last, wchar_t *result) {
		const std::ptrdiff_t n = last - first;
		std::memmove(result - n, first, sizeof(wchar_t) * n);
		return result - n;
	}

}