#pragma once


#include "gruel_heap.h"
#include "gruel_iterator.h"
#include "gruel_algobase.h"
#include <random>


namespace gruel {

	/* set相关算法，注意这里的set不能为hash_set和hash_multiset */

	// set_union：取两个set的并集
	template <typename InputIterator1, typename InputIterator2, typename OutputIterator>
	OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, InputIterator2 last2, OutputIterator result) {

		// 以下操作类似归并
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

		// 将没有到达尾部的区间元素拷贝至result处，实现方式很有意思
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


	// set_intersection：取两个set交集
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


	// set_difference：取两个set差集，即s1-s2
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


	// set_symmetric_difference：取两个set的对称差集，即(s1-s2)U(s2-s1)
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


	/* adjacent_find：找出第一组满足条件的相邻元素，默认为重复元素 */
	// 不明白这里为什么要用ForwardIterator，虽然没有影响
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


	/* count：计算区间内与指定元素相等的元素个数 */
	template <typename InputIterator, typename T>
	typename iterator_traits<InputIterator>::difference_type 
		count(InputIterator first, InputIterator last, const T &value) {
		typename iterator_traits<InputIterator>::difference_type n = 0;
		for (; first != last; ++first)
			if (*first == value)
				++n;
		return n;
	}


	/* count_if：计算区间内满足条件的元素个数 */
	template <typename InputIterator, typename Predicate>
	typename iterator_traits<InputIterator>::difference_type 
		count_if(InputIterator first, InputIterator last, Predicate pred) {
		typename iterator_traits<InputIterator>::difference_type n = 0;
		for (; first != last; ++first)
			if (pred(*first))
				++n;
		return n;
	}


	/* find：查找区间中与给定元素相等的第一个元素 */
	template <typename InputIterator, typename T>
	InputIterator find(InputIterator first, InputIterator last, const T &value) {
		while (first != last && *first != value)
			++first;
		return first;
	}


	/* find_if：查找区间中满足给定条件的第一个元素 */
	template <typename InputIterator, typename Predicate>
	InputIterator find_if(InputIterator first, InputIterator last, Predicate pred) {
		while (first != last && !pred(*first))
			++first;
		return first;
	}


	/* search：在第一个序列中找出第二个序列第一次出现的位置 */
	template <typename ForwardIterator1, typename ForwardIterator2, typename Distance1, typename Distance2>
	ForwardIterator1 _search(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2, ForwardIterator2 last2, Distance1 *, Distance2 *) {
		auto d1 = distance(first1, last1);
		auto d2 = distance(first2, last2);

		// 如果第二个序列比第一个序列长，不可能成为其子序列
		if (d1 < d2)
			return last1;

		auto current1 = first1;
		auto current2 = first2;

		// 遍历第二个序列
		while (current2 != last2) {
			// 元素相同，则调整为比对下一个元素
			if (*current1 == *current2) {
				++current1;
				++current2;
			}
			else {
				// 元素不相同，如果第二序列和剩余第一序列一样长，则不可能成功
				if (d1 == d2)
					return last1;
				else {
					// 剔除不相同元素，减少第一序列剩余长度，再找一次
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


	/* search_n：在序列[first, last)中查找连续count个符合条件之元素形成的区间 */
	template <typename ForwardIterator, typename Integer, typename T>
	ForwardIterator search_n(ForwardIterator first, ForwardIterator last, Integer count, const T &value) {
		if (count <= 0)
			return first;
		
		// 找到第一次出现的位置
		first = find(first, last, value);
		while (first != last) {
			Integer n = count - 1;
			auto i = first;
			++i;
			// 判断出现的次数是否满足条件
			while (i != last && n != 0 && *i == value) {
				++i;
				--n;
			}
			if (n == 0)
				// 满足则退出返回
				return first;
			else
				// 否则重新开始查找
				first = find(i, last, value);
		}
		return last;
	}

	template <typename ForwardIterator, typename Integer, typename T, typename BinaryPredicate>
	ForwardIterator search_n(ForwardIterator first, ForwardIterator last, 
		Integer count, const T &value, BinaryPredicate binary_pred) {
		if (count <= 0)
			return first;

		// 注意这里不能用find
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


	/* find_end：在第一个序列中找出第二个序列最后出现的位置 */
	// 解法思路：一是利用search；二是利用反向迭代器
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
				// 因为查找的是最后一次出现，所以找到就调动标兵，查找下一个
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
				// 因为查找的是最后一次出现，所以找到就调动标兵，查找下一个
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

		// 反向则是查找第一次出现的位置
		reviter1 rresult = search(reviter1(last1), rlast1,
			reviter2(last2), rlast2, binary_pred);

		if (rresult == rlast1)
			return last1;
		else {
			// 调整回原迭代器
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


	/* find_first_of：以序列二中的某些元素作为目标，在序列一中查找第一次出现的位置 */
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


	/* for_each：对区间[first, last)中每一个元素身上施行仿函数f */
	template <typename InputIterator, typename Function>
	Function for_each(InputIterator first, InputIterator last, Function f) {
		for (; first != last; ++first)
			f(*first);
		return f;
	}


	/* generate：将仿函数gen的运算结果填写在[first, last)内的所有元素上 */
	template <typename ForwardIterator, typename Generator>
	void generate(ForwardIterator first, ForwardIterator last, Generator gen) {
		for (; first != last; ++first)
			*first = gen();
	}


	/* generate_n：将仿函数gen的运算结果填写在以迭代器first开始的n个元素上 */
	template <typename OutputIterator, typename Size, typename Generator>
	OutputIterator generate_n(OutputIterator first, Size n, Generator gen) {
		for (; n > 0; --n, ++first)
			*first = gen();
		return first;
	}


	/* includes：判断序列s2是否包含在序列s1中，s1、s2必须有序 */
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


	/* max_element：返回迭代器指向数列中最大的元素 */
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


	/* min_element：返回指向序列中最小元素的迭代器 */
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


	/* merge：归并两个序列，两个序列必须有序 */
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


	/* partition：将区间内的元素按所给条件重新排列，并不会保证元素的原始相对位置 */
	template <typename BidirectionalIterator, typename Predicate>
	BidirectionalIterator partition(BidirectionalIterator first, BidirectionalIterator last, Predicate pred) {
		while (true) {
			// 实现方法很有意思，先找到前端位置要交换的元素，再找到后端位置要交换的元素，然后交换
			// 注意前后元素符合移动操作的条件不同
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


	/* remove_copy：移除区间中与给定元素相等的元素，并将结果复制到以result为首的新容器上 */
	template <typename InputIterator, typename OutputIterator, typename T>
	OutputIterator remove_copy(InputIterator first, InputIterator last, OutputIterator result, const T &value) {
		for(; first != last; ++first)
			if (*first == value) {
				*result = *first;
				++result;
			}
		return result;
	}


	/* remove：移除区间中与给定元素相等的元素，但不删除 */
	template <typename ForwardIterator, typename T>
	ForwardIterator remove(ForwardIterator first, ForwardIterator last, const T &value) {
		first = find(first, last, value);
		auto next = first;
		return first == last ? first : remove_copy(++next, last, first, value);
	}


	/* remove_copy_if：移除区间中pred判定为true的元素，并将结果复制到以result为首的新容器上 */
	template <typename InputIterator, typename OutputIterator, typename Predicate>
	OutputIterator remove_copy_if(InputIterator first, InputIterator last, OutputIterator result, Predicate pred) {
		for(; first != last; ++first)
			if (!pred(*first)) {
				*result = *first;
				++result;
			}
		return result;
	}


	/* remove_if：移除区间里pred判定为true的元素，并不删除 */
	template <typename ForwardIterator, typename Predicate>
	ForwardIterator remove(ForwardIterator first, ForwardIterator last, Predicate pred) {
		first = find_if(first, last, pred);
		auto next = first;
		return first == last ? first : remove_copy_if(++next, last, first, pred);
	}


	/* replace：将区间里所有old_value都以new_value代替 */
	template <typename ForwardIterator, typename T>
	void replace(ForwardIterator first, ForwardIterator last, const T &old_value, const T &new_value) {
		for (; first != last; ++first)
			if (*first == old_value) 
				*first = new_value;
	}


	/* replace_copy：与replace功能相似，但会将结果输出到result处 */
	template <typename ForwardIterator, typename OutputIterator, typename T>
	OutputIterator replace_copy(ForwardIterator first, ForwardIterator last, OutputIterator result,
		const T &old_value, const T &new_value) {
		for (; first != last; ++first) {
			*result = (*first == old_value) ? new_value : *first;
			++result;
		}
		return result;
	}


	/* replace_if：将区间里被pred评估为true的元素都以new_value代替 */
	template <typename ForwardIterator, typename Predicate, typename T>
	void replace(ForwardIterator first, ForwardIterator last, Predicate pred, const T &new_value) {
		for (; first != last; ++first)
			if (pred(*first))
				*first = new_value;
	}


	/* replace_copy_if：与replace_if功能相似，但会将结果输出到result处 */
	template <typename ForwardIterator, typename OutputIterator, typename Predicate, typename T>
	void replace(ForwardIterator first, ForwardIterator last, OutputIterator result, Predicate pred, 
		const T &new_value) {
		for (; first != last; ++first) {
			*result = pred(*first) ? new_value : *first;
			++result;
		}
		return result;
	}


	/* reverse：将区间[first, last)内的元素颠倒重排 */
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
		while (first < last)	// 只有Random Access Iterator可以这样比较
			iter_swap(first++, --last);
	}

	// 分派函数（dispatch function）
	template <typename BidirectionalIterator>
	inline void reverse(BidirectionalIterator first, BidirectionalIterator last) {
		return _reverse(first, last, iterator_category(first));
	}


	/* reverse_copy：与reverse功能类似，但将结果输出到以result为首的容器中 */
	template <typename BidirectionalIterator, typename OutputIterator>
	OutputIterator reverse_copy(BidirectionalIterator first, BidirectionalIterator last, OutputIterator result) {
		while (first != last) {
			--last;
			*result = *last;
			++result;
		}
		return result;
	}


	/* rotate：将[first, middle)内的的元素和[middle, last)内的元素互换 */
	// 实现的方式也是分层架构
	// gcd：辗转相除法求最大公因数，用于Random Access Iterator
	template <typename EuclideanRingElement>	// 这个类型参数的意义是欧几里得环元素
	EuclideanRingElement _gcd(EuclideanRingElement m, EuclideanRingElement n) {
		while (n != 0) {
			auto t = m % n;
			m = n;
			n = t;
		}
		return m;
	}

	// _rotate_cycle：周期旋转
	// 思路为从initial开始，找出应放在该位置的元素A，再找出应放在元素A原来位置的元素B，不断重复，直到回到initial
	template <typename RandomAccessIterator, typename Distance, typename T>
	void _rotate_cycle(RandomAccessIterator first, RandomAccessIterator last,
		RandomAccessIterator initial, Distance shift, T *) {
		T value = *initial;
		auto ptr1 = initial;	// 当前位置
		auto ptr2 = ptr1 + shift;	// 需要移动的元素位置
		while (ptr2 != initial) {
			*ptr1 = *ptr2;	// 移动元素
			ptr1 = ptr2;	// 改变当前位置
			// 判断需要从前段查找还是后段查找
			if (last - ptr2 > shift)	
				ptr2 += shift;
			else
				ptr2 = first + (shift - (last - ptr2));
		}
		*ptr1 = value;	// 放回初始元素
	}

	template <typename RandomAccessIterator, typename Distance>
	void _rotate(RandomAccessIterator first, RandomAccessIterator last,
		RandomAccessIterator middle, Distance *, random_access_iterator_tag) {
		// 求出最大公因数，该数即为循环次数，即会有n个周期旋转
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
			iter_swap(first, middle);	// 交换并同时自增
			++first;
			++i;
			if (first == middle)
				if (middle == last)
					// 若前后区段都结束，则旋转结束
					return;
				else
					// 否则设置新的前后区段
					middle = i;
			else if (i == last)
				i = middle;
		}
	}

	// 分派函数
	template <typename ForwardIterator>
	void rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last) {
		return _rotate(first, last, middle, distance(first), iterator_category(first));
	}


	/* rotate_copy：旋转并将结果复制到以result为首的容器中 */
	template <typename ForwardIterator, typename OutputIterator>
	OutputIterator rotate_copy(ForwardIterator first, ForwardIterator middle, ForwardIterator last,
		OutputIterator result) {
		return copy(first, middle, copy(middle, last, result));
	}


	/* swap_range：将[first1, last1)区间的元素同[first2, first2+(last1-first1))区间的元素作交换 */
	template <typename ForwardIterator1, typename ForwardIterator2>
	ForwardIterator2 swap_range(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2) {
		for (; first1 != last1; ++first1, ++first2)
			iter_swap(first1, first2);
		return first2;
	}


	/* transform：版本一，将仿函数op作用于[first, last)区间内的元素，并将结果输出到以result为首的容器中 */
	template <typename InputIterator, typename OutputIterator, typename UnaryOperation>
	OutputIterator transform(InputIterator first, InputIterator last,
		OutputIterator result, UnaryOperation op) {
		for (; first != last; ++first, ++result)
			*result = op(*first);
		return result;
	}

	// 版本二：以仿函数binary_op作用于分别来自[first1, last1)和[first2, last2)的一对元素（各一）*/
	template <typename InputIterator1, typename InputIterator2 ,typename OutputIterator, 
		typename BinaryOperation>
		OutputIterator transform(InputIterator1 first1, InputIterator1 last1,
			InputIterator2 first2, InputIterator2 last2, OutputIterator result, BinaryOperation binary_op) {
		for (; first1 != last1; ++first1, ++first2, ++result)
			*result = binary_op(*first1, *first2);
		return result;
	}


	/* unique_copy：移除区间里重复的元素，并将结果输出到以result为首的区间，注意只能移除相邻的重复元素，要移除所有重复元素要先将序列排序 */
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

	// 设计这个版本是因为output iterator只可写，不可读
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


	/* unique：移除区间里重复的元素，注意只能移除相邻的重复元素，要移除所有重复元素要先将序列排序 */
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


	/* lower_bound：返回区间[first, last)中第一个不小于value的位置的迭代器 */
	// 利用binary search
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


	/* upper_bound：返回区间[first, last)中第一个大于value的位置的迭代器 */
	// 利用binary search
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


	/* binary_search：二分查找区间中的指定元素 */
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


	/* next_permutation：取得序列[first, last)的下一个排列组合 */
	// 这里解释不太来，虽然方法过程都清楚，但不知道为什么这样做
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


	/* prev_permutation：取得序列的上一个排列组合 */
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


	/* random_shuffle：将[first, last)内的元素随机重排 */
	// 需要用到随机数生成器，写法估计有误
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


	/* partial_sort：将[first, last)中的元素区域[first, middle)排序 */
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


	/* partial_sort_copy：将(last-first)个最小元素排序后所得结果置于[result_first, result_last)中 */
	// 实际大小为result_last-result_first
	// 具体思路是先将[first, last)前[result_last-result_first)个元素复制后生成堆，然后再进行比较
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


	/* sort：接受一对随机访问迭代器，对区间[first, last)内的元素进行排序 */
	/* 为了提高效率，STL用了很多方法：当数据量大时采用Quick Sort，分段递归排序，
	 一旦分段后的数据量小于某个门槛，为避免Quick Sort的递归调用带来的过大额外
	 负荷，就改用Insertion Sort，如果递归层次过深，还会改用Heap Sort */

	const int _gruel_threshold = 16; // 阈值，当递归层次大于该值时改用堆排序

	// 此处有一个实现技巧是不用判断循环是否超过边界，在大数据量时，影响还是很可观的
	template <typename RandomAccessIterator, typename T>
	void _unguarded_linear_insert(RandomAccessIterator last, T value) {
		auto next = last;
		--next;
		// 注意区间里一定会有一个值比value小
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
		// 注意区间里一定会有一个值比value小
		while (comp(value, *next)) {
			*last = *next;
			last = next;
			--next;
		}
		*last = value;
	}

	template <typename RandomAccessIterator, typename T>
	inline void _linear_insert(RandomAccessIterator first, RandomAccessIterator last, T *) {
		T value = *last;	// 记录尾元素
		if (value < *first) {	// 如果尾元素比头元素还小，注意头元素为最小元素
			copy_backward(first, last, last + 1);	// 之间将区间右移
			*first = value;	// 设置新头元素值
		}
		else // 尾元素不小于头元素
			_unguarded_linear_insert(last, value);
	}

	template <typename RandomAccessIterator, typename T, typename Compare>
	inline void _linear_insert(RandomAccessIterator first, RandomAccessIterator last, T *, Compare comp) {
		T value = *last;	// 记录尾元素
		if (comp(value, *first)) {	
			copy_backward(first, last, last + 1);	// 之间将区间右移
			*first = value;	// 设置新头元素值
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

	// 分割的一个好的实现方法，从头找到要移动的元素，再从尾找到要移动的元素，然后互换，注意边界条件
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

	// 三数取中
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

	// 计算分割的层数，也可以理解为递归深度
	template <typename Size>
	inline Size _lg(Size n) {
		Size k;
		for (k = 0; n > 1; n >>= 1);
		return k;
	}

	// 内省式排序，行为大部分情况下与三数中值的快速排序相同
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
			// 先使用introsort对整体进行一定的排序
			_introsort_loop(first, last, value_type(first), _lg(last - first) * 2);
			// 再使用insertionsort对几近成功的序列进行排序
			_final_insertion_sort(first, last);
		}
	}

	template <typename RandomAccessIterator, typename Compare>
	inline void sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
		if (first != last) {
			// 先使用introsort对整体进行一定的排序
			_introsort_loop(first, last, value_type(first), _lg(last - first) * 2, comp);
			// 再使用insertionsort对几近成功的序列进行排序
			_final_insertion_sort(first, last, comp);
		}
	}


	/* equal_range：返回一个区间[i, j)，该区间中所有元素都与给定元素value相等，注意该区间必须有序空间 */
	template <typename RandomAccessIterator, typename T, typename Distance>
	pair<RandomAccessIterator, RandomAccessIterator> _equal_range(RandomAccessIterator first, RandomAccessIterator last,
		const T &value, Distance *, random_access_iterator_tag) {
		Distance len = last - first;
		Distance half;
		RandomAccessIterator middle, left, right;

		// 先利用二分法缩小查找范围，再用lower_bound和upper_bound确定区间
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

		// 先利用二分法缩小查找范围，再用lower_bound和upper_bound确定区间
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

		// 先利用二分法缩小查找范围，再用lower_bound和upper_bound确定区间
		// 注意这里的前后段使用len来界定的
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

		// 先利用二分法缩小查找范围，再用lower_bound和upper_bound确定区间
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


	/* nth_element：重排[first, last)，使迭代器nth所指元素与整个区间排序后同一位置的元素同值，且保证[nth, last)内的元素不大于[first, cut)内的元素 */
	// 通俗的将就是选出前(nth-first)个大的元素放于[first, cut)中
	// 具体思路与算法导论中相同，就是利用partition
	template <typename RandomAccessIterator, typename T>
	void _nth_element(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last, T *) {
		while (last - first < 3) {
			auto cut = _unguarded_partition(first, last, T(_median(*first, *(first + (last - first) / 2), *(last - 1))));

			if (cut <= nth)	// 如果右段起点 <= 指定位置（nth落于右段）
				first = cut;	// 对右段实行分割
			else			// 否则nth落于左段
				last = cut;	// 对左段实行分割
		}
		_insertion_sort(first, last);
	}

	template <typename RandomAccessIterator, typename Compare, typename T>
	void _nth_element(RandomAccessIterator first, RandomAccessIterator nth, RandomAccessIterator last, Compare comp, T *) {
		while (last - first < 3) {
			auto cut = _unguarded_partition(first, last, T(_median(*first, *(first + (last - first) / 2), *(last - 1), comp)), comp);

			if (cut <= nth)	// 如果右段起点 <= 指定位置（nth落于右段）
				first = cut;	// 对右段实行分割
			else			// 否则nth落于左段
				last = cut;	// 对左段实行分割
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