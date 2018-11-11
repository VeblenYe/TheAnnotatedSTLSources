#pragma once


#include "gruel_heap.h"
#include "gruel_iterator.h"
#include "gruel_algobase.h"
#include <random>


namespace gruel {

	/* set����㷨��ע�������set����Ϊhash_set��hash_multiset */

	// set_union��ȡ����set�Ĳ���
	template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
	OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, InputIterator2 last2, OutputIterator result) {

		// ���²������ƹ鲢
		while (first1 != last1 && first2 != last2) {
			if (*first1 < *first2) {
				*result = *first1;
				++first1;
			}
			else if (*first2 < *first1) {
				*result = *first2;
				++first2;
			}
			else {
				*result = *first1;
				++first1;
				++first2;
			}
			++result;
		}

		// ��û�е���β��������Ԫ�ؿ�����result����ʵ�ַ�ʽ������˼
		return copy(first2, last2, copy(first1, last1, result)) :
	}

	template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename Compare>
	OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, InputIterator2 last2, OutputIterator result,
		Compare comp) {

		while (first1 != last1 && first2 != last2) {
			if (comp(*first1, *first2)) {
				*result = *first1;
				++first1;
			}
			else if (comp(*first2, *first1)) {
				*result = *first2;
				++first2;
			}
			else {
				*result = *first1;
				++first1;
				++first2;
			}
			++result;
		}

		return copy(first2, last2, copy(first1, last1, result));
	}


	// set_intersection��ȡ����set����
	template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
	OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
		while (first1 != last1 && first2 != last2) {
			if (*first1 < *first2)
				++first1;
			else if (*first2 < *first1)
				++first2;
			else {
				*result = *first1;
				++first1;
				++first2;
				++result;
			}
		}
		return result;
	}

	template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename Compare>
	OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare comp) {
		while (first1 != last1 && first2 != last2) {
			if (comp(*first1, *first2))
				++first1;
			else if (comp(*first2, *first1))
				++first2;
			else {
				*result = *first1;
				++first1;
				++first2;
				++result;
			}
		}
		return result;
	}


	// set_difference��ȡ����set�����s1-s2
	template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
	OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
		while (first1 != last1 && first2 != last2) {
			if (*first1 < *first2) {
				*result = *first1;
				++first1;
				++result;
			}
			else if (*first2 < *first1)
				++first2;
			else {
				++first1;
				++first2;
			}
		}

		return copy(first1, last2, result);
	}

	template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename Compare>
	OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare comp) {
		while (first1 != last1 && first2 != last2) {
			if (comp(*first1, *first2)) {
				*result = *first1;
				++first1;
				++result;
			}
			else if (comp(*first2, *first1))
				++first2;
			else {
				++first1;
				++first2;
			}
		}

		return copy(first1, last2, result);
	}


	// set_symmetric_difference��ȡ����set�ĶԳƲ����(s1-s2)U(s2-s1)
	template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
	OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, InputIterator2 last2, OutputIterator result) {
		while (*first1 != *last1 && *first1 != *last2) {
			if (*first1 < *first2) {
				*result = *first1;
				++first1;
				++result;
			}
			else if (*first2 < *first1) {
				*result = *first2;
				++first2;
				++result;
			}
			else {
				++first1;
				++first2;
			}
		}
		return copy(first2, last2, copy(first1, last1, result));
	}

	template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename Compare>
	OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare comp) {
		while (*first1 != *last1 && *first1 != *last2) {
			if (comp(*first1, *first2)) {
				*result = *first1;
				++first1;
				++result;
			}
			else if (comp(*first2, *first1)) {
				*result = *first2;
				++first2;
				++result;
			}
			else {
				++first1;
				++first2;
			}
		}
		return copy(first2, last2, copy(first1, last1, result));
	}


	/* adjacent_find���ҳ���һ����������������Ԫ�أ�Ĭ��Ϊ�ظ�Ԫ�� */
	// ����������ΪʲôҪ��ForwardIterator����Ȼû��Ӱ��
	template <typename ForwardIterator>
	ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last) {
		if (first == last) return;
		auto next = first;
		while (++next != last) {
			if (*first == *next)
				return first;
			first = next;
		}
		return last;
	}

	template <typename ForwardIterator, typename BinaryPredicate>
	ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last, BinaryPredicate binary_pred) {
		if (first == last) return;
		auto next = first;
		while (++next != last) {
			if (binary_pred(*first, *next))
				return first;
			first = next;
		}
		return last;
	}


	/* count��������������ָ��Ԫ����ȵ�Ԫ�ظ��� */
	template <typename InputIterator, typename T>
	typename iterator_traits<InputIterator>::difference_type 
		count(InputIterator first, InputIterator last, const T &value) {
		typename iterator_traits<InputIterator>::difference_type n = 0;
		for (; first != last; ++first)
			if (*first == value)
				++n;
		return n;
	}


	/* count_if����������������������Ԫ�ظ��� */
	template <typename InputIterator, typename Predicate>
	typename iterator_traits<InputIterator>::difference_type 
		count_if(InputIterator first, InputIterator last, Predicate pred) {
		typename iterator_traits<InputIterator>::difference_type n = 0;
		for (; first != last; ++first)
			if (pred(*first))
				++n;
		return n;
	}


	/* find�����������������Ԫ����ȵĵ�һ��Ԫ�� */
	template <typename InputIterator, typename T>
	InputIterator find(InputIterator first, InputIterator last, const T &value) {
		while (first != last && *first != value)
			++first;
		return first;
	}


	/* find_if������������������������ĵ�һ��Ԫ�� */
	template <typename InputIterator, typename Predicate>
	InputIterator find_if(InputIterator first, InputIterator last, Predicate pred) {
		while (first != last && !pred(*first))
			++first;
		return first;
	}


	/* search���ڵ�һ���������ҳ��ڶ������е�һ�γ��ֵ�λ�� */
	template <typename ForwardIterator1, typename ForwardIterator2, typename Distance1, typename Distance2>
	ForwardIterator1 _search(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2, ForwardIterator2 last2, Distance1 *, Distance2 *) {
		auto d1 = distance(first1, last1);
		auto d2 = distance(first2, last2);

		// ����ڶ������бȵ�һ�����г��������ܳ�Ϊ��������
		if (d1 < d2)
			return last1;

		auto current1 = first1;
		auto current2 = first2;

		// �����ڶ�������
		while (current2 != last2) {
			// Ԫ����ͬ�������Ϊ�ȶ���һ��Ԫ��
			if (*current1 == *current2) {
				++current1;
				++current2;
			}
			else {
				// Ԫ�ز���ͬ������ڶ����к�ʣ���һ����һ�������򲻿��ܳɹ�
				if (d1 == d2)
					return last1;
				else {
					// �޳�����ͬԪ�أ����ٵ�һ����ʣ�೤�ȣ�����һ��
					current1 = ++first1;
					current2 = first2;
					--d1;
				}
			}
		}
		return first1;
	}

	template <typename ForwardIterator1, typename ForwardIterator2>
	ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2, ForwardIterator2 last2) {
		return _search(first1, last1, first2, last2, difference_type(first1), difference_type(first2));
	}

	template <typename ForwardIterator1, typename ForwardIterator2, 
		typename Distance1, typename Distance2, typename BinaryPredicate>
	ForwardIterator1 _search(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2, ForwardIterator2 last2, Distance1 *, Distance2 *, BinaryPredicate pred) {
		auto d1 = distance(first1, last1);
		auto d2 = distance(first2, last2);

		if (d1 < d2)
			return last1;

		auto current1 = first1;
		auto current2 = first2;

		while (current2 != last2) {
			if (pred(*current1, *current2)) {
				++current1;
				++current2;
			}
			else {
				if (d1 == d2)
					return last1;
				else {
					current1 = ++first1;
					current2 = first2;
					--d1;
				}
			}
		}
		return first1;
	}

	template <typename ForwardIterator1, typename ForwardIterator2, typename BinaryPredicate>
	ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate pred) {
		return _search(first1, last1, first2, last2, pred, difference_type(first1), difference_type(first2));
	}


	/* search_n��������[first, last)�в�������count����������֮Ԫ���γɵ����� */
	template <typename ForwardIterator, typename Integer, typename T>
	ForwardIterator search_n(ForwardIterator first, ForwardIterator last, Integer count, const T &value) {
		if (count <= 0)
			return first;
		
		// �ҵ���һ�γ��ֵ�λ��
		first = find(first, last, value);
		while (first != last) {
			Integer n = count - 1;
			auto i = first;
			++i;
			// �жϳ��ֵĴ����Ƿ���������
			while (i != last && n != 0 && *i == value) {
				++i;
				--n;
			}
			if (n == 0)
				// �������˳�����
				return first;
			else
				// �������¿�ʼ����
				first = find(i, last, value);
		}
		return last;
	}

	template <typename ForwardIterator, typename Integer, typename T, typename BinaryPredicate>
	ForwardIterator search_n(ForwardIterator first, ForwardIterator last, 
		Integer count, const T &value, BinaryPredicate binary_pred) {
		if (count <= 0)
			return first;

		// ע�����ﲻ����find
		while (first != last) {
			if (binary_pred(*first, value))
				break;
			++first;
		}

		while (first != last) {
			Integer n = count - 1;
			auto i = first;
			++i;
			while (i != last && n != 0 && binary_pred(*i, value)) {
				++i;
				--n;
			}
			if (n == 0)
				return first;
			else {
				while (i != last) {
					if (binary_pred(*i, value))
						break;
					++i;
				}
				first = i;
			}
		}
		return last;
	}


	/* find_end���ڵ�һ���������ҳ��ڶ������������ֵ�λ�� */
	// �ⷨ˼·��һ������search���������÷��������
	template <typename ForwardIterator1, typename ForwardIterator2>
	ForwardIterator1 _find_end(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2, ForwardIterator2 last2, forward_iterator_tag, forward_iterator_tag) {
		if (first2 == last2)
			return last1;
		ForwardIterator1 result = last1;
		while (true) {
			auto new_result = search(first1, last1, first2, last2);
			if (new_result == last1)
				return result;
			else {
				// ��Ϊ���ҵ������һ�γ��֣������ҵ��͵��������������һ��
				result = new_result;
				first1 = new_result;
				++first1;
			}
		}
	}

	template <typename ForwardIterator1, typename ForwardIterator2, typename BinaryPredicate>
	ForwardIterator1 _find_end(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2, ForwardIterator2 last2, forward_iterator_tag, forward_iterator_tag, 
		BinaryPredicate binary_pred) {
		if (first2 == last2)
			return last1;
		ForwardIterator1 result = last1;
		while (true) {
			auto new_result = search(first1, last1, first2, last2, binary_pred);
			if (new_result == last1)
				return result;
			else {
				// ��Ϊ���ҵ������һ�γ��֣������ҵ��͵��������������һ��
				result = new_result;
				first1 = new_result;
				++first1;
			}
		}
	}

	template <typename BidirectionalIterator1, typename BidirectionalIterator2>
	BidirectionalIterator1 _find_end(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
		BidirectionalIterator2 first2, BidirectionalIterator2 last2, bidirectional_iterator_tag, bidirectional_iterator_tag) {
		
		using reviter1 = reverse_iterator<BidirectionalIterator1>;
		using reviter2 = reverse_iterator<BidirectionalIterator2>;

		reviter1 rlast1(first1);
		reviter2 rlast2(first2);

		reviter1 rresult = search(reviter1(last1), rlast1,
			reviter2(last2), rlast2);

		if (rresult == rlast1)
			return last1;
		else {
			auto result = rresult.base();
			advance(result, -distance(first2, last2));
			return result;
		}
	}

	template <typename BidirectionalIterator1, typename BidirectionalIterator2, typename BinaryPredicate>
	BidirectionalIterator1 _find_end(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
		BidirectionalIterator2 first2, BidirectionalIterator2 last2, bidirectional_iterator_tag, bidirectional_iterator_tag, 
		BinaryPredicate binary_pred) {

		using reviter1 = reverse_iterator<BidirectionalIterator1>;
		using reviter2 = reverse_iterator<BidirectionalIterator2>;

		reviter1 rlast1(first1);
		reviter2 rlast2(first2);

		// �������ǲ��ҵ�һ�γ��ֵ�λ��
		reviter1 rresult = search(reviter1(last1), rlast1,
			reviter2(last2), rlast2, binary_pred);

		if (rresult == rlast1)
			return last1;
		else {
			// ������ԭ������
			auto result = rresult.base();
			advance(result, -distance(first2, last2));
			return result;
		}
	}
	
	template <typename ForwardIterator1, typename ForwardIterator2>
	ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2, ForwardIterator2 last2) {
		return _find_end(first1, last1, first2, last2, iterator_category(first1), iterator_category(first2));
	}

	template <typename ForwardIterator1, typename ForwardIterator2, typename BinaryPredicate>
	ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate binary_pred) {
		return _find_end(first1, last1, first2, last2, iterator_category(first1), iterator_category(first2), binary_pred);
	}


	/* find_first_of�������ж��е�ĳЩԪ����ΪĿ�꣬������һ�в��ҵ�һ�γ��ֵ�λ�� */
	template <typename InputIterator, typename ForwardIterator>
	InputIterator find_first_of(InputIterator first1, InputIterator last1,
		ForwardIterator first2, ForwardIterator last2) {
		for (; first1 != last1; ++first1)
			for (auto iter = first2; iter != last2; ++iter)
				if (*iter == *first1)
					return first1;
		return last1;
	}

	template <typename InputIterator, typename ForwardIterator, typename BinaryPredicate>
	InputIterator find_first_of(InputIterator first1, InputIterator last1,
		ForwardIterator first2, ForwardIterator last2, BinaryPredicate binary_pred) {
		for (; first1 != last1; ++first1)
			for (auto iter = first2; iter != last2; ++iter)
				if (binary_pred(*iter, *first1))
					return first1;
		return last1;
	}


	/* for_each��������[first, last)��ÿһ��Ԫ������ʩ�зº���f */
	template <typename InputIterator, typename Function>
	Function for_each(InputIterator first, InputIterator last, Function f) {
		for (; first != last; ++first)
			f(*first);
		return f;
	}


	/* generate�����º���gen����������д��[first, last)�ڵ�����Ԫ���� */
	template <typename ForwardIterator, typename Generator>
	void generate(ForwardIterator first, ForwardIterator last, Generator gen) {
		for (; first != last; ++first)
			*first = gen();
	}


	/* generate_n�����º���gen����������д���Ե�����first��ʼ��n��Ԫ���� */
	template <typename OutputIterator, typename Size, typename Generator>
	OutputIterator generate_n(OutputIterator first, Size n, Generator gen) {
		for (; n > 0; --n, ++first)
			*first = gen();
		return first;
	}


	/* includes���ж�����s2�Ƿ����������s1�У�s1��s2�������� */
	template <typename InputIterator1, typename InputIterator2>
	bool includes(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, InputIterator2 last2) {
		while (first1 != last1 && first2 != last2) {
			if (*first1 < *first2)
				++first1;
			else if (*first2 < *first1)
				++first2;
			else {
				++first1;
				++first2;
			}
		}
		return first2 == last2;
	}

	template <typename InputIterator1, typename InputIterator2, typename Compare>
	bool includes(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, InputIterator2 last2, Compare comp) {
		while (first1 != last1 && first2 != last2) {
			if (comp(*first1, *first2))
				++first1;
			else if (comp(*first2, *first1))
				++first2;
			else {
				++first1;
				++first2;
			}
		}
		return first2 == last2;
	}


	/* max_element�����ص�����ָ������������Ԫ�� */
	template <typename ForwardIterator>
	ForwardIterator max_element(ForwardIterator first, ForwardIterator last) {
		if (first == last)
			return first;
		auto result = first;
		while (++first != last)
			if (*result < *first)
				result = first;
		return result;
	}

	template <typename ForwardIterator, typename Compare>
	ForwardIterator max_element(ForwardIterator first, ForwardIterator last, Compare comp) {
		if (first == last)
			return first;
		auto result = first;
		while (++first != last)
			if (comp(*result, *first))
				result = first;
		return result;
	}


	/* min_element������ָ����������СԪ�صĵ����� */
	template <typename ForwardIterator>
	ForwardIterator min_element(ForwardIterator first, ForwardIterator last) {
		if (first == last)
			return first;
		auto result = first;
		while (++first != last)
			if (*first < *result)
				result = first;
		return result;
	}

	template <typename ForwardIterator, typename Compare>
	ForwardIterator min_element(ForwardIterator first, ForwardIterator last, Compare comp) {
		if (first == last)
			return first;
		auto result = first;
		while (++first != last)
			if (comp(*first, *result))
				result = first;
		return result;
	}


	/* merge���鲢�������У��������б������� */
	template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
	OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, InputIterator2 last2, OutputIterator result) {

		while (first1 != last1 && first2 != last2) {
			if (*first2 < *first1) {
				*result = *first2;
				++first2;
			}
			else {
				*result = *first1;
				++first1;
			}
			++result;
		}

		return copy(first2, last2, copy(first1, last1, result));
	}

	template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename Compare>
	OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, InputIterator2 last2, OutputIterator result, Compare comp) {

		while (first1 != last1 && first2 != last2) {
			if (comp(*first2, *first1)) {
				*result = *first2;
				++first2;
			}
			else {
				*result = *first1;
				++first1;
			}
			++result;
		}

		return copy(first2, last2, copy(first1, last1, result));
	}


	/* partition���������ڵ�Ԫ�ذ����������������У������ᱣ֤Ԫ�ص�ԭʼ���λ�� */
	template <typename BidirectionalIterator, typename Predicate>
	BidirectionalIterator partition(BidirectionalIterator first, BidirectionalIterator last, Predicate pred) {
		while (true) {
			// ʵ�ַ���������˼�����ҵ�ǰ��λ��Ҫ������Ԫ�أ����ҵ����λ��Ҫ������Ԫ�أ�Ȼ�󽻻�
			// ע��ǰ��Ԫ�ط����ƶ�������������ͬ
			while (true)
				if (first == last)
					return first;
				else if (pred(*first))
					++first;
				else
					break;

			--last;
			while (true)
				if (first == last)
					return first;
				else if (!pred(*last))
					--last;
				else
					break;
			iter_swap(first, last);
			++first;
		}
	}


	/* remove_copy���Ƴ������������Ԫ����ȵ�Ԫ�أ�����������Ƶ���resultΪ�׵��������� */
	template <typename InputIterator, typename OutputIterator, typename T>
	OutputIterator remove_copy(InputIterator first, InputIterator last, OutputIterator result, const T &value) {
		for(; first != last; ++first)
			if (*first == value) {
				*result = *first;
				++result;
			}
		return result;
	}


	/* remove���Ƴ������������Ԫ����ȵ�Ԫ�أ�����ɾ�� */
	template <typename ForwardIterator, typename T>
	ForwardIterator remove(ForwardIterator first, ForwardIterator last, const T &value) {
		first = find(first, last, value);
		auto next = first;
		return first == last ? first : remove_copy(++next, last, first, value);
	}


	/* remove_copy_if���Ƴ�������pred�ж�Ϊtrue��Ԫ�أ�����������Ƶ���resultΪ�׵��������� */
	template <typename InputIterator, typename OutputIterator, typename Predicate>
	OutputIterator remove_copy_if(InputIterator first, InputIterator last, OutputIterator result, Predicate pred) {
		for(; first != last; ++first)
			if (!pred(*first)) {
				*result = *first;
				++result;
			}
		return result;
	}


	/* remove_if���Ƴ�������pred�ж�Ϊtrue��Ԫ�أ�����ɾ�� */
	template <typename ForwardIterator, typename Predicate>
	ForwardIterator remove(ForwardIterator first, ForwardIterator last, Predicate pred) {
		first = find_if(first, last, pred);
		auto next = first;
		return first == last ? first : remove_copy_if(++next, last, first, pred);
	}


	/* replace��������������old_value����new_value���� */
	template <typename ForwardIterator, typename T>
	void replace(ForwardIterator first, ForwardIterator last, const T &old_value, const T &new_value) {
		for (; first != last; ++first)
			if (*first == old_value) 
				*first = new_value;
	}


	/* replace_copy����replace�������ƣ����Ὣ��������result�� */
	template <typename ForwardIterator, typename OutputIterator, typename T>
	OutputIterator replace_copy(ForwardIterator first, ForwardIterator last, OutputIterator result,
		const T &old_value, const T &new_value) {
		for (; first != last; ++first) {
			*result = (*first == old_value) ? new_value : *first;
			++result;
		}
		return result;
	}


	/* replace_if���������ﱻpred����Ϊtrue��Ԫ�ض���new_value���� */
	template <typename ForwardIterator, typename Predicate, typename T>
	void replace(ForwardIterator first, ForwardIterator last, Predicate pred, const T &new_value) {
		for (; first != last; ++first)
			if (pred(*first))
				*first = new_value;
	}


	/* replace_copy_if����replace_if�������ƣ����Ὣ��������result�� */
	template <typename ForwardIterator, typename OutputIterator, typename Predicate, typename T>
	void replace(ForwardIterator first, ForwardIterator last, OutputIterator result, Predicate pred, 
		const T &new_value) {
		for (; first != last; ++first) {
			*result = pred(*first) ? new_value : *first;
			++result;
		}
		return result;
	}


	/* reverse��������[first, last)�ڵ�Ԫ�صߵ����� */
	template <typename BidirectionalIterator>
	void _reverse(BidirectionalIterator first, BidirectionalIterator last, bidirectional_iterator_tag) {
		while (true) 
			if (first == last || first == --last)
				return;
			else
				iter_swap(first++, last);
	}

	template <typename RandomAccessIterator>
	void _reverse(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
		while (first < last)	// ֻ��Random Access Iterator���������Ƚ�
			iter_swap(first++, --last);
	}

	// ���ɺ�����dispatch function��
	template <typename BidirectionalIterator>
	inline void reverse(BidirectionalIterator first, BidirectionalIterator last) {
		return _reverse(first, last, iterator_category(first));
	}


	/* reverse_copy����reverse�������ƣ���������������resultΪ�׵������� */
	template <typename BidirectionalIterator, typename OutputIterator>
	OutputIterator reverse_copy(BidirectionalIterator first, BidirectionalIterator last, OutputIterator result) {
		while (first != last) {
			--last;
			*result = *last;
			++result;
		}
		return result;
	}


	/* rotate����[first, middle)�ڵĵ�Ԫ�غ�[middle, last)�ڵ�Ԫ�ػ��� */
	// ʵ�ֵķ�ʽҲ�Ƿֲ�ܹ�
	// gcd��շת��������������������Random Access Iterator
	template <typename EuclideanRingElement>	// ������Ͳ�����������ŷ����û�Ԫ��
	EuclideanRingElement _gcd(EuclideanRingElement m, EuclideanRingElement n) {
		while (n != 0) {
			auto t = m % n;
			m = n;
			n = t;
		}
		return m;
	}

	// _rotate_cycle��������ת
	// ˼·Ϊ��initial��ʼ���ҳ�Ӧ���ڸ�λ�õ�Ԫ��A�����ҳ�Ӧ����Ԫ��Aԭ��λ�õ�Ԫ��B�������ظ���ֱ���ص�initial
	template <typename RandomAccessIterator, typename Distance, typename T>
	void _rotate_cycle(RandomAccessIterator first, RandomAccessIterator last,
		RandomAccessIterator initial, Distance shift, T *) {
		T value = *initial;
		auto ptr1 = initial;	// ��ǰλ��
		auto ptr2 = ptr1 + shift;	// ��Ҫ�ƶ���Ԫ��λ��
		while (ptr2 != initial) {
			*ptr1 = *ptr2;	// �ƶ�Ԫ��
			ptr1 = ptr2;	// �ı䵱ǰλ��
			// �ж���Ҫ��ǰ�β��һ��Ǻ�β���
			if (last - ptr2 > shift)	
				ptr2 += shift;
			else
				ptr2 = first + (shift - (last - ptr2));
		}
		*ptr1 = value;	// �Żس�ʼԪ��
	}

	template <typename RandomAccessIterator, typename Distance>
	void _rotate(RandomAccessIterator first, RandomAccessIterator last,
		RandomAccessIterator middle, Distance *, random_access_iterator_tag) {
		// ������������������Ϊѭ��������������n��������ת
		Distance n = _gcd(last - middle, middle - first);
		while (n--)
			_rotate_cycle(first, last, first + n, middle - first, value_type(first));
	}

	template <typename BidirectionalIterator, typename Distance>
	void _rotate(BidirectionalIterator first, BidirectionalIterator last, 
		BidirectionalIterator middle, Distance *, bidirectional_iterator_tag) {
		reverse(first, middle);
		reverse(middle, last);
		reverse(first, last);
	}

	template <typename ForwardIterator, typename Distance>
	void _rotate(ForwardIterator first, ForwardIterator last,
		ForwardIterator middle, Distance *, forward_iterator_tag) {
		for (ForwardIterator i = middle; ;) {
			iter_swap(first, middle);	// ������ͬʱ����
			++first;
			++i;
			if (first == middle)
				if (middle == last)
					// ��ǰ�����ζ�����������ת����
					return;
				else
					// ���������µ�ǰ������
					middle = i;
			else if (i == last)
				i = middle;
		}
	}

	// ���ɺ���
	template <typename ForwardIterator>
	void rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last) {
		return _rotate(first, last, middle, distance(first), iterator_category(first));
	}


	/* rotate_copy����ת����������Ƶ���resultΪ�׵������� */
	template <typename ForwardIterator, typename OutputIterator>
	OutputIterator rotate_copy(ForwardIterator first, ForwardIterator middle, ForwardIterator last,
		OutputIterator result) {
		return copy(first, middle, copy(middle, last, result));
	}


	/* swap_range����[first1, last1)�����Ԫ��ͬ[first2, first2+(last1-first1))�����Ԫ�������� */
	template <typename ForwardIterator1, typename ForwardIterator2>
	ForwardIterator2 swap_range(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2) {
		for (; first1 != last1; ++first1, ++first2)
			iter_swap(first1, first2);
		return first2;
	}


	/* transform���汾һ�����º���op������[first, last)�����ڵ�Ԫ�أ���������������resultΪ�׵������� */
	template <typename InputIterator, typename OutputIterator, typename UnaryOperation>
	OutputIterator transform(InputIterator first, InputIterator last,
		OutputIterator result, UnaryOperation op) {
		for (; first != last; ++first, ++result)
			*result = op(*first);
		return result;
	}

	// �汾�����Էº���binary_op�����ڷֱ�����[first1, last1)��[first2, last2)��һ��Ԫ�أ���һ��*/
	template <typename InputIterator1, typename InputIterator2 ,typename OutputIterator, 
		typename BinaryOperation>
		OutputIterator transform(InputIterator1 first1, InputIterator1 last1,
			InputIterator2 first2, InputIterator2 last2, OutputIterator result, BinaryOperation binary_op) {
		for (; first1 != last1; ++first1, ++first2, ++result)
			*result = binary_op(*first1, *first2);
		return result;
	}


	/* unique_copy���Ƴ��������ظ���Ԫ�أ���������������resultΪ�׵����䣬ע��ֻ���Ƴ����ڵ��ظ�Ԫ�أ�Ҫ�Ƴ������ظ�Ԫ��Ҫ�Ƚ��������� */
	template <typename InputIterator, typename ForwardIterator>
	ForwardIterator _unique_copy(InputIterator first, InputIterator last, ForwardIterator result, forward_iterator_tag) {
		*result = *first;
		while (++first != last)
			if (*result != *first)
				*++result = *first;
		return ++result;
	}

	template <typename InputIterator, typename ForwardIterator, typename BinaryPredicate>
	ForwardIterator _unique_copy(InputIterator first, InputIterator last, ForwardIterator result, 
		BinaryPredicate binary_pred, forward_iterator_tag) {
		*result = *first;
		while (++first != last)
			if (binary_pred(*result, *first))
				*++result = *first;
		return ++result;
	}

	// �������汾����Ϊoutput iteratorֻ��д�����ɶ�
	template <typename InputIterator, typename OutputIterator, typename T>
	OutputIterator _unique_copy(InputIterator first, InputIterator last, OutputIterator result, T *) {
		T value = *first;
		*result = value;
		while (++first != last) {
			if (value != *first) {
				value = *first;
				*++result = value;
			}
		}
		return ++result;
	}

	template <typename InputIterator, typename OutputIterator, typename T, typename BinaryPredicate>
	OutputIterator _unique_copy(InputIterator first, InputIterator last, OutputIterator result, 
		BinaryPredicate binary_pred, T *) {
		T value = *first;
		*result = value;
		while (++first != last) {
			if (binary_pred(value, *first)) {
				value = *first;
				*++result = value;
			}
		}
		return ++result;
	}

	template <typename InputIterator, typename OutputIterator>
	OutputIterator _unique_copy(InputIterator first, InputIterator last, OutputIterator result, output_iterator_tag) {
		return _unique_copy(first, last, result, value_type(first));
	}

	template <typename InputIterator, typename OutputIterator, typename BinaryPredicate>
	OutputIterator _unique_copy(InputIterator first, InputIterator last, OutputIterator result, 
		BinaryPredicate binary_pred, output_iterator_tag) {
		return _unique_copy(first, last, result, binary_pred, value_type(first));
	}

	template <typename InputIterator, typename OutputIterator>
	OutputIterator unique_copy(InputIterator first, InputIterator last, OutputIterator result) {
		return _unique_copy(first, last, result, iterator_category(result));
	}

	template <typename InputIterator, typename OutputIterator, typename BinaryPredicate>
	OutputIterator unique_copy(InputIterator first, InputIterator last, OutputIterator result, BinaryPredicate binary_pred) {
		return _unique_copy(first, last, result, binary_pred, iterator_category(result));
	}


	/* unique���Ƴ��������ظ���Ԫ�أ�ע��ֻ���Ƴ����ڵ��ظ�Ԫ�أ�Ҫ�Ƴ������ظ�Ԫ��Ҫ�Ƚ��������� */
	template <typename ForwardIterator>
	void unique(ForwardIterator first, ForwardIterator last) {
		first = adjacent_find(first, last);
		return unique_copy(first, last, first);
	}

	template <typename ForwardIterator, typename BinaryPredicate>
	void unique(ForwardIterator first, ForwardIterator last, BinaryPredicate binary_pred) {
		first = adjacent_find(first, last, binary_pred);
		return unique_copy(first, last, first, binary_pred);
	}


	/* lower_bound����������[first, last)�е�һ����С��value��λ�õĵ����� */
	// ����binary search
	template <typename ForwardIterator, typename T, typename Distance>
	ForwardIterator _lower_bound(ForwardIterator first, ForwardIterator last, const T &value, 
		Distance *, forward_iterator_tag) {
		Distance len = distance(first, last);
		Distance half;
		ForwardIterator middle;

		while (half > 0) {
			half = len >> 1;
			middle = first;
			advance(middle, half);
			if (*middle < value) {
				first = middle;
				++first;
				len = len - half - 1;
			}
			else
				len = half;
		}
		return first;
	}

	template <typename ForwardIterator, typename T, typename Compare, typename Distance>
	ForwardIterator _lower_bound(ForwardIterator first, ForwardIterator last, const T &value,
		Compare comp, Distance *, forward_iterator_tag) {
		Distance len = distance(first, last);
		Distance half;
		ForwardIterator middle;

		while (half > 0) {
			half = len >> 1;
			middle = first;
			advance(middle, half);
			if (comp(*middle, value)) {
				first = middle;
				++first;
				len = len - half - 1;
			}
			else
				len = half;
		}
		return first;
	}

	template <typename RandomAccessIterator, typename T, typename Distance>
	RandomAccessIterator _lower_bound(RandomAccessIterator first, RandomAccessIterator last,
		const T &value, Distance *, random_access_iterator_tag) {
		Distance len = last - first;
		Distance half;
		RandomAccessIterator middle;

		while (half > 0) {
			half = len >> 1;
			middle = first + half;
			if (*middle < value) {
				first = middle + 1;
				len = len - half - 1;
			}
			else
				len = half;
		}
		return first;
	}

	template <typename RandomAccessIterator, typename T, typename Compare, typename Distance>
	RandomAccessIterator _lower_bound(RandomAccessIterator first, RandomAccessIterator last,
		const T &value, Compare comp, Distance *, random_access_iterator_tag) {
		Distance len = last - first;
		Distance half;
		RandomAccessIterator middle;

		while (half > 0) {
			half = len >> 1;
			middle = first + half;
			if (comp(*middle, value)) {
				first = middle + 1;
				len = len - half - 1;
			}
			else
				len = half;
		}
		return first;
	}

	template <typename ForwardIterator, typename T>
	ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T &value) {
		return _lower_bound(first, last, value, difference_type(first), iterator_category(first));
	}

	template <typename ForwardIterator, typename T, typename Compare>
	ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T &value, Compare comp) {
		return _lower_bound(first, last, value, comp, difference_type(first), iterator_category(first));
	}


	/* upper_bound����������[first, last)�е�һ������value��λ�õĵ����� */
	// ����binary search
	template <typename ForwardIterator, typename T, typename Distance>
	ForwardIterator _upper_bound(ForwardIterator first, ForwardIterator last, const T &value,
		Distance *, forward_iterator_tag) {
		Distance len = distance(first, last);
		Distance half;
		ForwardIterator middle;

		while (half > 0) {
			half = len >> 1;
			middle = first;
			advance(middle, half);
			if (value < *middle) {
				len = half;
			}
			else {
				first = middle;
				++first;
				len = len - half - 1;
			}
		}
		return first;
	}

	template <typename ForwardIterator, typename T, typename Compare, typename Distance>
	ForwardIterator _upper_bound(ForwardIterator first, ForwardIterator last, const T &value,
		Compare comp, Distance *, forward_iterator_tag) {
		Distance len = distance(first, last);
		Distance half;
		ForwardIterator middle;

		while (half > 0) {
			half = len >> 1;
			middle = first;
			advance(middle, half);
			if (comp(value, *middle)) {
				len = half;
			}
			else {
				first = middle;
				++first;
				len = len - half - 1;
			}
		}
		return first;
	}

	template <typename RandomAccessIterator, typename T, typename Distance>
	RandomAccessIterator _upper_bound(RandomAccessIterator first, RandomAccessIterator last,
		const T &value, Distance *, random_access_iterator_tag) {
		Distance len = last - first;
		Distance half;
		RandomAccessIterator middle;

		while (half > 0) {
			half = len >> 1;
			middle = first + half;
			if (value < *middle) {
				len = half;
			}
			else {
				first = middle + 1;
				len = len - half - 1;
			}
		}
		return first;
	}

	template <typename RandomAccessIterator, typename T, typename Compare, typename Distance>
	RandomAccessIterator _upper_bound(RandomAccessIterator first, RandomAccessIterator last,
		const T &value, Compare comp, Distance *, random_access_iterator_tag) {
		Distance len = last - first;
		Distance half;
		RandomAccessIterator middle;

		while (half > 0) {
			half = len >> 1;
			middle = first + half;
			if (comp(value, *middle)) {
				len = half;
			}
			else {
				first = middle + 1;
				len = len - half - 1;
			}
		}
		return first;
	}

	template <typename ForwardIterator, typename T>
	ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T &value) {
		return _upper_bound(first, last, value, difference_type(first), iterator_category(first));
	}

	template <typename ForwardIterator, typename T, typename Compare>
	ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T &value, Compare comp) {
		return _upper_bound(first, last, value, comp, difference_type(first), iterator_category(first));
	}


	/* binary_search�����ֲ��������е�ָ��Ԫ�� */
	template <typename ForwardIterator, typename T>
	bool binary_search(ForwardIterator first, ForwardIterator last, const T &value) {
		auto i = lower_bound(first, last, value);
		return i != last && !(value < *i);
	}

	template <typename ForwardIterator, typename T, typename Compare>
	bool binary_search(ForwardIterator first, ForwardIterator last, const T &value, Compare comp) {
		auto i = lower_bound(first, last, value, comp);
		return i != last && !comp(value, *i);
	}


	/* next_permutation��ȡ������[first, last)����һ��������� */
	// ������Ͳ�̫������Ȼ�������̶����������֪��Ϊʲô������
	template <typename BidirectionalIterator>
	bool next_permutation(BidirectionalIterator first, BidirectionalIterator last) {
		if (first == last) return false;
		auto i = first;
		++i;
		if (i == last) return false;
		i = last;
		--i;

		while (true) {
			auto ii = i;
			--i;
			if (*i < *ii) {
				auto j = last;
				while (!(*i < *--j));
				iter_swap(i, j);
				reverse(ii, last);
				return true;
			}
			if (i == first) {
				reverse(first, last);
				return false;
			}
		}
	}

	template <typename BidirectionalIterator, typename Compare>
	bool next_permutation(BidirectionalIterator first, BidirectionalIterator last, Compare comp) {
		if (first == last) return false;
		auto i = first;
		++i;
		if (i == last) return false;
		i = last;
		--i;

		while (true) {
			auto ii = i;

			--i;
			if (comp(*i, *ii)) {
				auto j = last;
				while (!comp(*i, *--j));
				iter_swap(i, j);
				reverse(ii, last);
				return true;
			}
			if (i == first) {
				reverse(first, last);
				return false;
			}
		}
	}


	/* prev_permutation��ȡ�����е���һ��������� */
	template <typename BidirectionalIterator>
	bool prev_permutation(BidirectionalIterator first, BidirectionalIterator last) {
		if (first == last) return false;
		auto i = first;
		++i;
		if (i == last) return false;
		i = last;
		--i;

		while (true) {
			auto ii = i;
			--i;
			if (*ii < *i) {
				auto j = last;
				while (!(*--j < *i));
				iter_swap(i, j);
				reverse(ii, last);
				return true;
			}
			if (i == first) {
				reverse(first, last);
				return false;
			}
		}
	}

	template <typename BidirectionalIterator, typename Compare>
	bool prev_permutation(BidirectionalIterator first, BidirectionalIterator last, Compare comp) {
		if (first == last) return false;
		auto i = first;
		++i;
		if (i == last) return false;
		i = last;
		--i;

		while (true) {
			auto ii = i;

			--i;
			if (comp(*ii, *i)) {
				auto j = last;
				while (!comp(*--j, *i));
				iter_swap(i, j);
				reverse(ii, last);
				return true;
			}
			if (i == first) {
				reverse(first, last);
				return false;
			}
		}
	}


	/* random_shuffle����[first, last)�ڵ�Ԫ��������� */
	// ��Ҫ�õ��������������д����������
	template <typename RandomAccessIterator, typename Distance>
	void _random_shuffle(RandomAccessIterator first, RandomAccessIterator last, Distance *) {
		if (first == last) return;
		std::random_device r;
		std::default_random_engine e(r());
		for (auto i = first + 1; i != last; ++i) {
			std::uniform_int_distribution<Distance> u(0, (i - first_ + 1);
			iter_swap(i, first + u(e));
		}
	}

	template <typename RandomAccessIterator>
	inline void random_shuffle(RandomAccessIterator first, RandomAccessIterator last) {
		return _random_shuffle(first, last, difference_type(first));
	}

	template <typename RandomAccessIterator, typename RandomNumberGenerator>
	void random_shuffle(RandomAccessIterator first, RandomAccessIterator last, RandomNumberGenerator &rand) {
		if (first == last) return;
		for (auto i = first + 1; i != last; ++i) {
			iter_swap(i, first + rand((i - first) + 1));
		}
	}


	/* partial_sort����[first, last)�е�Ԫ������[first, middle)���� */
	template <typename RandomAccessIterator, typename T>
	void _partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, T *) {
		make_heap(first, middle);
		for (auto i = middle; i < last; ++i)
			if (*i < *first)
				_pop_heap(first, last, i, T(*i), difference_type(first));
		sort_heap(first, middle);
	}

	template <typename RandomAccessIterator, typename T, typename Compare>
	void _partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, T *, Compare comp) {
		make_heap(first, middle);
		for (auto i = middle; i < last; ++i)
			if (comp(*i, *first))
				_pop_heap(first, last, i, T(*i), difference_type(first));
		sort_heap(first, middle);
	}

	template <typename RandomAccessIterator>
	inline void partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last) {
		return _partial_sort(first, middle, last, value_type(first));
	}

	template <typename RandomAccessIterator, typename Compare>
	inline void partial_sort(RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, Compare comp) {
		return _partial_sort(first, middle, last, value_type(first), comp);
	}


	/* partial_sort_copy����(last-first)����СԪ����������ý������[result_first, result_last)�� */
	// ʵ�ʴ�СΪresult_last-result_first
	// ����˼·���Ƚ�[first, last)ǰ[result_last-result_first)��Ԫ�ظ��ƺ����ɶѣ�Ȼ���ٽ��бȽ�
	template <typename InputIterator, typename RandomAccessIterator, typename Distance, typename T>
	RandomAccessIterator _partial_sort_copy(InputIterator first, InputIterator last,
		RandomAccessIterator result_first, RandomAccessIterator result_last, Distance *, T *) {
		if (result_first == result_last) return;
		auto result_real_last = result_first;
		while (first != last && result_real_last != result_last) {
			*result_real_last = *first;
			++result_real_last;
			++first;
		}
		make_heap(result_first, result_real_last);
		while(first != last)
			if (*first < *result_first) {
				_adjust_heap(result_first, Distance(0), Distance(result_real_last - result_first), T(*first));
				++first;
			}
		sort_heap(result_first, result_real_last);
		return result_real_last;
	}

	template <typename InputIterator, typename RandomAccessIterator, typename Distance, typename T, typename Compare>
	RandomAccessIterator _partial_sort_copy(InputIterator first, InputIterator last,
		RandomAccessIterator result_first, RandomAccessIterator result_last, Compare comp, Distance *, T *) {
		if (result_first == result_last) return;
		auto result_real_last = result_first;
		while (first != last && result_real_last != result_last) {
			*result_real_last = *first;
			++result_real_last;
			++first;
		}
		make_heap(result_first, result_real_last);
		while (first != last)
			if (comp(*first, *last)) {
				_adjust_heap(result_first, Distance(0), Distance(result_real_last - result_first), T(*first));
				++first;
			}
		sort_heap(result_first, result_real_last);
		return result_real_last;
	}

	template <typename InputIterator, typename RandomAccessIterator>
	inline RandomAccessIterator partial_sort_copy(InputIterator first, InputIterator last,
		RandomAccessIterator result_first, RandomAccessIterator result_last) {
		return _partial_sort_copy(first, last, result_first, result_last, difference_type(result_first), value_type(first));
	}

	template <typename InputIterator, typename RandomAccessIterator, typename Compare>
	inline RandomAccessIterator partial_sort_copy(InputIterator first, InputIterator last,
		RandomAccessIterator result_first, RandomAccessIterator result_last, Compare comp) {
		return _partial_sort_copy(first, last, result_first, result_last, comp, difference_type(result_first), value_type(first));
	}


	/* sort������һ��������ʵ�������������[first, last)�ڵ�Ԫ�ؽ������� */
	/* Ϊ�����Ч�ʣ�STL���˺ܶ෽��������������ʱ����Quick Sort���ֶεݹ�����
	 һ���ֶκ��������С��ĳ���ż���Ϊ����Quick Sort�ĵݹ���ô����Ĺ������
	 ���ɣ��͸���Insertion Sort������ݹ��ι���������Heap Sort */

	const int _gruel_threshold = 16; // ��ֵ�����ݹ��δ��ڸ�ֵʱ���ö�����

	// �˴���һ��ʵ�ּ����ǲ����ж�ѭ���Ƿ񳬹��߽磬�ڴ�������ʱ��Ӱ�컹�Ǻܿɹ۵�
	template <typename RandomAccessIterator, typename T>
	void _unguarded_linear_insert(RandomAccessIterator last, T value) {
		auto next = last;
		--next;
		// ע��������һ������һ��ֵ��valueС
		while (value < *next) {
			*last = *next;
			last = next;
			--next;
		}
		*last = value;
	}

	template <typename RandomAccessIterator, typename T, typename Compare>
	void _unguarded_linear_insert(RandomAccessIterator last, T value, Compare comp) {
		auto next = last;
		--next;
		// ע��������һ������һ��ֵ��valueС
		while (comp(value, *next)) {
			*last = *next;
			last = next;
			--next;
		}
		*last = value;
	}

	template <typename RandomAccessIterator, typename T>
	inline void _linear_insert(RandomAccessIterator first, RandomAccessIterator last, T *) {
		T value = *last;	// ��¼βԪ��
		if (value < *first) {	// ���βԪ�ر�ͷԪ�ػ�С��ע��ͷԪ��Ϊ��СԪ��
			copy_backward(first, last, last + 1);	// ֮�佫��������
			*first = value;	// ������ͷԪ��ֵ
		}
		else // βԪ�ز�С��ͷԪ��
			_unguarded_linear_insert(last, value);
	}

	template <typename RandomAccessIterator, typename T, typename Compare>
	inline void _linear_insert(RandomAccessIterator first, RandomAccessIterator last, T *, Compare comp) {
		T value = *last;	// ��¼βԪ��
		if (comp(value, *first)) {	
			copy_backward(first, last, last + 1);	// ֮�佫��������
			*first = value;	// ������ͷԪ��ֵ
		}
		else 
			_unguarded_linear_insert(last, value, comp);
	}

	// Insertion Sort
	template <typename RandomAccessIterator>
	void _insertion_sort(RandomAccessIterator first, RandomAccessIterator last) {
		if (first == last) return;
		for (auto i = first + 1; i != last; ++i)
			_linear_insert(first, i, value_type(first));
	}

	template <typename RandomAccessIterator, typename Compare>
	void _insertion_sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
		if (first == last) return;
		for (auto i = first + 1; i != last; ++i)
			_linear_insert(first, i, value_type(first), comp);
	}

	template <typename RandomAccessIterator, typename T>
	void _unguarded_insertion_sort_aux(RandomAccessIterator first, RandomAccessIterator last, T *) {
		for (auto i = first; i != last; ++i)
			_unguarded_linear_insert(i, T(*i));
	}

	template <typename RandomAccessIterator, typename T, typename Compare>
	void _unguarded_insertion_sort_aux(RandomAccessIterator first, RandomAccessIterator last, T *, Compare comp) {
		for (auto i = first; i != last; ++i)
			_unguarded_linear_insert(i, T(*i), comp);
	}

	template <typename RandomAccessIterator>
	inline void _unguarded_insertion_sort(RandomAccessIterator first, RandomAccessIterator last) {
		_unguarded_insertion_sort_aux(first, last, value_type(first));
	}

	template <typename RandomAccessIterator, typename Compare>
	inline void _unguarded_insertion_sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
		_unguarded_insertion_sort_aux(first, last, value_type(first), comp);
	}

	template <typename RandomAccessIterator>
	void _final_insertion_sort(RandomAccessIterator first, RandomAccessIterator last) {
		if (last - first > _gruel_threshold) {
			_insertion_sort(first, first + _gruel_threshold);
			_unguarded_insertion_sort(first + _gruel_threshold, last);
		}
		else
			_insertion_sort(first, last);
	}

	template <typename RandomAccessIterator, typename Compare>
	void _final_insertion_sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
		if (last - first > _gruel_threshold) {
			_insertion_sort(first, first + _gruel_threshold, comp);
			_unguarded_insertion_sort(first + _gruel_threshold, last, comp);
		}
		else
			_insertion_sort(first, last, comp);
	}

	// �ָ��һ���õ�ʵ�ַ�������ͷ�ҵ�Ҫ�ƶ���Ԫ�أ��ٴ�β�ҵ�Ҫ�ƶ���Ԫ�أ�Ȼ�󻥻���ע��߽�����
	template <typename RandomAccessIterator, typename T>
	void _unguarded_partition(RandomAccessIterator first, RandomAccessIterator last, const T &pivot) {
		while (true) {
			if (*first < pivot) ++first;
			--last;
			if (pivot < *last) --last;
			if (!(first < last)) return first;
			iter_swap(first, last);
			++first;
		}
	}

	template <typename RandomAccessIterator, typename T, typename Compare>
	void _unguarded_partition(RandomAccessIterator first, RandomAccessIterator last, const T &pivot, Compare comp) {
		while (true) {
			if (comp(*first, pivot)) ++first;
			--last;
			if (comp(pivot, *last)) --last;
			if (!(first < last)) return first;
			iter_swap(first, last);
			++first;
		}
	}

	// ����ȡ��
	template <typename T>
	inline const T &_median(const T &a, const T &b, const T &c) {
		if (a < b)
			if (b < c)	// a < b < c
				return b;
			else if (a < c)	// a < c <= b
				return c;
			else	// c <= a < b
				return a;
		else if (a < c)	// b <= a < c
			return a;
		else if (b < c)	//  b < c <= a
			return c;
		else	// c <= b <= a
			return b;
	}

	template <typename T, typename Compare>
	inline const T &_median(const T &a, const T &b, const T &c, Compare comp) {
		if (comp(a, b))
			if (comp(b, c)) 
				return b;
			else if (comp(a, c))	
				return c;
			else
				return a;
		else if (comp(a, c))
			return a;
		else if (comp(b, c))
			return c;
		else
			return b;
	}

	// ����ָ�Ĳ�����Ҳ�������Ϊ�ݹ����
	template <typename Size>
	inline Size _lg(Size n) {
		Size k;
		for (k = 0; n > 1; n >>= 1);
		return k;
	}

	// ��ʡʽ������Ϊ�󲿷��������������ֵ�Ŀ���������ͬ
	template <typename RandomAccessIterator, typename T, typename Size>
	void _introsort_loop(RandomAccessIterator first, RandomAccessIterator last, T *, Size depth_limit) {
		while (last - first > _gruel_threshold) {
			if (depth_limit == 0) {
				partial_sort(first, last, last);
				return;
			}
			--depth_limit;
			
			auto cut = _unguarded_partition(first, last, T(_median(*first, *(first + (last - first) / 2), *(last - 1)));
			_introsort_loop(cut, last, value_type(first), depth_limit);
			last = cut;
		}
	}

	template <typename RandomAccessIterator, typename T, typename Size, typename Compare>
	void _introsort_loop(RandomAccessIterator first, RandomAccessIterator last, T *, Size depth_limit, Compare comp) {
		while (last - first > _gruel_threshold) {
			if (depth_limit == 0) {
				partial_sort(first, last, last, comp);
				return;
			}
			--depth_limit;

			auto cut = _unguarded_partition(first, last, T(_median(*first, *(first + (last - first) / 2), *(last - 1), comp), comp);
			_introsort_loop(cut, last, value_type(first), depth_limit, comp);
			last = cut;
		}
	}

	template <typename RandomAccessIterator>
	inline void sort(RandomAccessIterator first, RandomAccessIterator last) {
		if (first != last) {
			// ��ʹ��introsort���������һ��������
			_introsort_loop(first, last, value_type(first), _lg(last - first) * 2);
			// ��ʹ��insertionsort�Լ����ɹ������н�������
			_final_insertion_sort(first, last);
		}
	}

	template <typename RandomAccessIterator, typename Compare>
	inline void sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
		if (first != last) {
			// ��ʹ��introsort���������һ��������
			_introsort_loop(first, last, value_type(first), _lg(last - first) * 2, comp);
			// ��ʹ��insertionsort�Լ����ɹ������н�������
			_final_insertion_sort(first, last, comp);
		}
	}


	/* equal_range������һ������[i, j)��������������Ԫ�ض������Ԫ��value��ȣ�ע��������������ռ� */
	template <typename RandomAccessIterator, typename T, typename Distance>
	pair<RandomAccessIterator, RandomAccessIterator> _equal_range(RandomAccessIterator first, RandomAccessIterator last,
		const T &value, Distance *, random_access_iterator_tag) {
		Distance len = last - first;
		Distance half;
		RandomAccessIterator middle, left, right;

		// �����ö��ַ���С���ҷ�Χ������lower_bound��upper_boundȷ������
		while (len > 0) {
			half = len >> 1;
			middle = first + half;
			if (*middle < value) {
				first = middle + 1;
				len = len - half - 1;
			}
			else if (value < *middle)
				len = half;
			else {
				left = lower_bound(first, middle, value);
				right = upper_bound(++middle, first + len, value);
				return { left, right };
			}
		}
		return { first, first };
	}

	template <typename RandomAccessIterator, typename T, typename Compare, typename Distance>
	pair<RandomAccessIterator, RandomAccessIterator> _equal_range(RandomAccessIterator first, RandomAccessIterator last,
		const T &value, Compare comp, Distance *, random_access_iterator_tag) {
		Distance len = last - first;
		Distance half;
		RandomAccessIterator middle, left, right;

		// �����ö��ַ���С���ҷ�Χ������lower_bound��upper_boundȷ������
		while (len > 0) {
			half = len >> 1;
			middle = first + half;
			if (comp(*middle, value)) {
				first = middle + 1;
				len = len - half - 1;
			}
			else if (comp(value, *middle))
				len = half;
			else {
				left = lower_bound(first, middle, value);
				right = upper_bound(++middle, first + len, value);
				return { left, right };
			}
		}
		return { first, first };
	}

	template <typename ForwardIterator, typename T, typename Distance>
	pair<ForwardIterator, ForwardIterator> _equal_range(ForwardIterator first, ForwardIterator last,
		const T &value, Distance *, forward_iterator_tag) {
		Distance len = distance(first, last);
		Distance half;
		ForwardIterator middle, left, right;

		// �����ö��ַ���С���ҷ�Χ������lower_bound��upper_boundȷ������
		// ע�������ǰ���ʹ��len���綨��
		while (len > 0) {
			half = len >> 1;
			middle = first;
			advance(middle, half);
			if (*middle < value) {
				first = middle;
				++first;
				len = len - half - 1;
			}
			else if (value < *middle)
				len = half;
			else {
				left = lower_bound(first, middle, value);
				right = upper_bound(++middle, first + len, value);
				return { left, right };
			}
		}
		return { first, first };
	}

	template <typename ForwardIterator, typename T, typename Compare, typename Distance>
	pair<ForwardIterator, ForwardIterator> _equal_range(ForwardIterator first, ForwardIterator last,
		const T &value, Compare comp, Distance *, forward_iterator_tag) {
		Distance len = distance(first, last);
		Distance half;
		ForwardIterator middle, left, right;

		// �����ö��ַ���С���ҷ�Χ������lower_bound��upper_boundȷ������
		while (len > 0) {
			half = len >> 1;
			middle = first;
			advance(middle, half);
			if (comp(*middle, value)) {
				first = middle;
				++first;
				len = len - half - 1;
			}
			else if (comp(value, *middle))
				len = half;
			else {
				left = lower_bound(first, middle, value);
				right = upper_bound(++middle, first + len, value);
				return { left, right };
			}
		}
		return { first, first };
	}

	template <typename ForwardIterator, typename T>
	pair<ForwardIterator, ForwardIterator> equal_range(ForwardIterator first, ForwardIterator last, const T &value) {
		return _equal_range(first, last, value, difference_type(first), iterator_category(first));
	}

	template <typename ForwardIterator, typename T, typename Compare>
	pair<ForwardIterator, ForwardIterator> equal_range(ForwardIterator first, ForwardIterator last, 
		const T &value, Compare comp) {
		return _equal_range(first, last, value, comp, difference_type(first), iterator_category(first));
	}


	/* nth_element������[first, last)��ʹ������nth��ָԪ�����������������ͬһλ�õ�Ԫ��ֵͬ���ұ�֤[nth, last)�ڵ�Ԫ�ز�����[first, cut)�ڵ�Ԫ�� */
	// ͨ�׵Ľ�����ѡ��ǰ(nth-first)�����Ԫ�ط���[first, cut)��
	// ����˼·���㷨��������ͬ����������partition
	template <typename RandomAccessIterator, typename T>
	void _nth_element(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last, T *) {
		while (last - first < 3) {
			auto cut = _unguarded_partition(first, last, T(_median(*first, *(first + (last - first) / 2), *(last - 1))));

			if (cut <= nth)	// ����Ҷ���� <= ָ��λ�ã�nth�����ҶΣ�
				first = cut;	// ���Ҷ�ʵ�зָ�
			else			// ����nth�������
				last = cut;	// �����ʵ�зָ�
		}
		_insertion_sort(first, last);
	}

	template <typename RandomAccessIterator, typename Compare, typename T>
	void _nth_element(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last, Compare comp, T *) {
		while (last - first < 3) {
			auto cut = _unguarded_partition(first, last, T(_median(*first, *(first + (last - first) / 2), *(last - 1), comp)), comp);

			if (cut <= nth)	// ����Ҷ���� <= ָ��λ�ã�nth�����ҶΣ�
				first = cut;	// ���Ҷ�ʵ�зָ�
			else			// ����nth�������
				last = cut;	// �����ʵ�зָ�
		}
		_insertion_sort(first, last);
	}

	template <typename RandomAccessIterator>
	inline void nth_element(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last) {
		_nth_element(first, nth, last, value_type(first));
	}

	template <typename RandomAccessIterator, typename Compare>
	inline void nth_element(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last, Compare comp) {
		_nth_element(first, nth, last, comp, value_type(first));
	}
}