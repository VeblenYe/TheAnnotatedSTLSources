#pragma once


#include "gruel_functional.h"


namespace gruel {


	/* accumulate：将每个元素值通过指定操作动作在init上 */
	template <typename InputIterator, typename T>
	T accumulate(InputIterator first, InputIterator last, T init) {
		for (; first != last; ++first)
			init = init + *first;	// 这里不写+=可能是考虑自定义的类型没有重载+=
		return init;
	}

	template <typename InputIterator, typename T, typename BinaryOperation>
	T accumulate(InputIterator first, InputIterator last, T init, BinaryOperation binary_op) {
		for (; first != last; ++first)
			init = binary_op(init, *first);
		return init;
	}


	/* adjacent_difference：计算并输出相邻元素的差值 */
	template <typename InputIterator, typename OutputIterator>
	OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result) {
		if (first == last)
			return result;
		*result = *first;
		auto value = *first;
		while (++first != last) {
			auto tmp = *first;
			*++result = tmp - value;
			value = tmp;
		}
		return ++result;
	}

	template <typename InputIterator, typename OutputIterator, typename BinaryOperation>
	OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result,
		BinaryOperation binary_op) {
		if (first == last)
			return result;
		*result = *first;
		auto value = *first;
		while (++first != last) {
			auto tmp = *first;
			*++result = binary_op(tmp, value);	// 优先级为先++后*
			value = tmp;
		}
		return ++result;
	}


	/* inner_product：计算两个区间元素的一般内积 */
	template <typename InputIterator1, typename InputIterator2,  typename T>
	T inner_product(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, T init) {
		for (; first1 != last1; ++first1, ++first2)
			init = init +  *first1 * *first2;
		return init;
	}

	template <typename InputIterator1, typename InputIterator2, typename T, 
		typename BinaryOperation1, typename BinaryOperation2>
	T inner_product(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, T init, BinaryOperation1 binary_op1, 
		BinaryOperation2 binary_op2) {
		for (; first1 != last1; ++first1, ++first2)
			init = binary_op1(init, binary_op2(*first1, *first2));
		return init;
	}


	/* partial_sum：计算并输出局部元素之和 */
	template <typename InputIterator, typename OutputIterator>
	OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result) {
		if (first == last)
			return result;
		*result = *first;
		auto value = *first;
		while (++first != last) {
			value = value + *first;
			*++result = value;
		}
		return ++result;
	}

	template <typename InputIterator, typename OutputIterator, typename BinaryOperation>
	OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result,
		BinaryOperation binary_op) {
		if (first == last)
			return result;
		*result = *first;
		auto value = *first;
		while (++first != last) {
			value = binary_op(value, *first);
			*++result = value;	// 优先级为先++后*
		}
		return ++result;
	}

	// 前置声明
	template <typename T, typename Integer, typename MonoidOperation>
	T power(T x, Integer n, MonoidOperation op);

	/* power：幂方运算 */
	template <typename T, typename Integer>
	inline T power(T x, Integer n) {
		return power(x, n, multiplies<T>());
	}

	template <typename T, typename Integer, typename MonoidOperation>
	T power(T x, Integer n, MonoidOperation op) {
		if (n == 0)
			return identity_element(op);
		else {
			// 若n为奇数，则转换为偶数
			while ((n & 1) == 0) {
				n >>= 1;
				x = op(x, x);
			}

			T result = x;
			n >>= 1;
			while (n != 0) {
				x = op(x, x);
				// 这里感觉奇怪，有这一步的原因是循环之外右移了一次，但怎么还会有奇数情况出现
				if ((n & 1) != 0)
					result = op(result, x);
				n >>= 1;
			}
			return result;
		}
	}


	/* iota：将指定区间元素设置为从某个值开始递增 */
	template <typename ForwardIterator, typename T>
	void iota(ForwardIterator first, ForwardIterator last, T value) {
		while (first != last) *first++ = value++;
	}

}