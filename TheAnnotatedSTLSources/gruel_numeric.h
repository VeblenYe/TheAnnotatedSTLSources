#pragma once


#include "gruel_functional.h"


namespace gruel {


	/* accumulate����ÿ��Ԫ��ֵͨ��ָ������������init�� */
	template <typename InputIterator, typename T>
	T accumulate(InputIterator first, InputIterator last, T init) {
		for (; first != last; ++first)
			init = init + *first;	// ���ﲻд+=�����ǿ����Զ��������û������+=
		return init;
	}

	template <typename InputIterator, typename T, typename BinaryOperation>
	T accumulate(InputIterator first, InputIterator last, T init, BinaryOperation binary_op) {
		for (; first != last; ++first)
			init = binary_op(init, *first);
		return init;
	}


	/* adjacent_difference�����㲢�������Ԫ�صĲ�ֵ */
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
			*++result = binary_op(tmp, value);	// ���ȼ�Ϊ��++��*
			value = tmp;
		}
		return ++result;
	}


	/* inner_product��������������Ԫ�ص�һ���ڻ� */
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


	/* partial_sum�����㲢����ֲ�Ԫ��֮�� */
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
			*++result = value;	// ���ȼ�Ϊ��++��*
		}
		return ++result;
	}

	// ǰ������
	template <typename T, typename Integer, typename MonoidOperation>
	T power(T x, Integer n, MonoidOperation op);

	/* power���ݷ����� */
	template <typename T, typename Integer>
	inline T power(T x, Integer n) {
		return power(x, n, multiplies<T>());
	}

	template <typename T, typename Integer, typename MonoidOperation>
	T power(T x, Integer n, MonoidOperation op) {
		if (n == 0)
			return identity_element(op);
		else {
			// ��nΪ��������ת��Ϊż��
			while ((n & 1) == 0) {
				n >>= 1;
				x = op(x, x);
			}

			T result = x;
			n >>= 1;
			while (n != 0) {
				x = op(x, x);
				// ����о���֣�����һ����ԭ����ѭ��֮��������һ�Σ�����ô�����������������
				if ((n & 1) != 0)
					result = op(result, x);
				n >>= 1;
			}
			return result;
		}
	}


	/* iota����ָ������Ԫ������Ϊ��ĳ��ֵ��ʼ���� */
	template <typename ForwardIterator, typename T>
	void iota(ForwardIterator first, ForwardIterator last, T value) {
		while (first != last) *first++ = value++;
	}

}