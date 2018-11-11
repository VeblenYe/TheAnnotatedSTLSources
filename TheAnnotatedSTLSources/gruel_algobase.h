#pragma once


#include <cstring>
#include "gruel_iterator.h"
#include "gruel_type_traits.h"
#include "gruel_functional.h"


namespace gruel {

	
	/* equal���ж���������Ԫ���Ƿ���ȣ��ڶ������ж������Ԫ�ز��迼�� */
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


	/* fill����[first, last)�е�����Ԫ�ظ�����ֵ */
	template <typename ForwardIterator, typename T>
	void fill(ForwardIterator first, ForwardIterator last, const T &value) {
		for (; first != last; ++first)
			*first = value;
	}


	/* fill_n����[first, first+n]�е�����Ԫ�ظ�����ֵ�����صĵ�����ָ����������һ��Ԫ�ص���һ��λ�� */
	// ע������ĵ���������ΪOutputIterator
	template <typename OutputIterator, typename Size, typename T>
	OutputIterator fill_n(OutputIterator first, Size n, const T &value) {
		for (; n != 0; --n, ++first)
			*first = value;
		return first;
	}

	// ǰ������
	template <typename ForwardIterator1, typename ForwardIterator2, typename T>
	inline void _iter_swap(ForwardIterator1 a, ForwardIterator2 b, T *);

	/* iter_swap����������ForwardIterator */
	template <typename ForwardIterator1, typename ForwardIterator2>
	inline void iter_swap(ForwardIterator1 a, ForwardIterator2 b) {
		return _iter_swap(a, b, value_type(a));
	}

	// ʹ��value_type��ȡ��a��������ָ�������ͣ��Ա����ڽ���
	template <typename ForwardIterator1, typename ForwardIterator2, typename T>
	inline void _iter_swap(ForwardIterator1 a, ForwardIterator2 b, T *) {
		T tmp = *a;
		*a = *b;
		*b = tmp;
	}


	/* lexicographical_compare�����ֵ�����������н��бȽ� */
	/* �����ֵ�һ�鲻��ȵ�Ԫ��ʱ����������� 
	1. *first1��С������true�����򷵻�false;
	2. ����last1��δ����last2���򷵻�true;
	3. ����last2��δ����last1���򷵻�false;
	4. ͬʱ����last1��last2���򷵻�false�� */
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

	// ���ԭ��ָ����ػ��汾��Ȼ������ʵ���ϲ����Ǻ�����ȫ�ػ�
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


	/* max��ȡ�����нϴ��� */
	template <typename T>
	inline bool max(const T &a, const T &b) {
		return a < b ? b : a;
	}

	template <typename T, typename Compare>
	inline bool max(const T &a, const T &b, Compare comp) {
		return comp(a, b) ? b : a;
	}


	/* min��ȡ�����н�С�� */
	template <typename T>
	inline bool min(const T &a, const T &b) {
		return a < b ? a : b;
	}

	template <typename T, typename Compare>
	inline bool min(const T &a, const T &b, Compare comp) {
		return comp(a, b) ? a : b;
	}


	/* mismatch���ҳ���������֮��ĵ�һ����ƥ��� */
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

	
	/* swap������������������� */
	template <typename T>
	inline void swap(T &lhs, T &rhs) {
		T tmp = lhs;
		lhs = rhs;
		rhs = tmp;
	}


	/* copy������������[first, last)�ڵ�Ԫ�ظ��Ƶ�����ռ�[result, result+(last-first))�� */

	template <typename InputIterator, typename OutputIterator, typename Distance>
	inline OutputIterator _copy_d(InputIterator first, InputIterator last, OutputIterator result, Distance *) {
		for (Distance n = last - first; n > 0; --n, ++first, ++result)
			*first = *result;
		return result;
	}

	// ����������Ϊinput_iterator�����õ������еȣ��ٶ���
	template <typename InputIterator, typename OutputIterator>
	inline OutputIterator _copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag) {
		for (; first != last; ++first, ++result)
			*first = *result;
		return result;
	}

	// ����������Ϊrandom_access_iterator�����þ����жϣ��ٶȿ�
	template <typename InputIterator, typename OutputIterator>
	inline OutputIterator _copy(InputIterator first, InputIterator last, OutputIterator result, random_access_iterator_tag) {
		return _copy_d(first, last, result, difference_type(first));
	}

	// ��trivial_assignment_operator��ֱ�Ӷ���memmove
	template <typename T>
	inline T *_copy_t(const T *first, const T *last, T *result, _true_type) {
		std::memmove(result, first, sizeof(T) * (last - first));
		return result + (last - first);
	}

	// û��trivial_assignment_operator�������_copy_d��ԭʼָ��Ҳ��RAI
	template <typename T>
	inline T *_copy_t(const T *first, const T *last, T *result, _false_type) {
		return _copy_d(first, last, result, (std::ptrdiff_t)nullptr);
	}

	// ��ȫ�����棬����������ΪʲôҪ�÷º���
	template <typename InputIterator, typename OutputIterator>
	struct _copy_dispatch {
		OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
			return _copy(first, last, result, iterator_category(first));
		}
	};

	// ƫ�ػ��棬����������Ϊָ��
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

	// �����ṩ�ķ����ӿ�
	template <typename InputIterator, typename OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
		return _copy_dispatch(first, last, result);
	}

	// ���غ��������ԭ��ָ��
	inline char *copy(const char *first, const char *last, char *result) {
		std::memmove(result, first, last - first);
		return result + (last - first);
	}

	inline wchar_t *copy(const wchar_t *first, const wchar_t *last, wchar_t *result) {
		std::memmove(result, first, sizeof(wchar_t) * (last - first));
		return result + (last - first);
	}


	/* copy_backward��������[first, last)�е�ÿһ��Ԫ�أ������еķ�ʽ���Ƶ���result-1Ϊ��㣬������Ϊ���е������� */

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