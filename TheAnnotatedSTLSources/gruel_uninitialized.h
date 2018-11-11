#pragma once


#include "gruel_iterator.h"
#include "gruel_type_traits.h"
#include "gruel_construct.h"
#include "gruel_algorithm.h"


namespace gruel {

	// T�����Ƿ���POD����
	template <typename InputIterator, typename ForwardIterator>
	inline ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last,
		ForwardIterator result, _true_type) {
		return copy(first, last, result);
	}


	template <typename InputIterator, typename ForwardIterator>
	inline ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last,
		ForwardIterator result, _false_type) {
		auto cur = result;
		for (; first != last; ++first, ++cur)
			construct(&*cur, *first);
		return cur;
	}


	// �Ƶ���T������
	template <typename InputIterator, typename ForwardIterator, typename T>
	inline ForwardIterator _uninitialized_copy(InputIterator first, InputIterator last,
		ForwardIterator result, T *) {
		using is_POD = typename _type_traits<T>::is_POD_type;
		return _uninitialized_copy_aux(first, last, result, is_POD());
	}


	// copy�ǽ�first��last�е����ݿ�����result����ֻ��Ҫ��ȡ���ݣ�����Ҫ�޸����ݣ����������������
	template <typename InputIterator, typename ForwardIterator>
	inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
		ForwardIterator result) {
		return _uninitialized_copy(first, last, result, value_type(result));
	}


	// �ػ��汾��ֱ�ӵ���memmove���ٶȼ���
	template <>
	inline char *uninitialized_copy(const char *first, const char *last,
		char *result) {
		std::memmove(result, first, last - first);
		return result + (last - first);
	}


	// �ػ��汾��ֱ�ӵ���memmove���ٶȼ��죬ע����������ͳ���
	template <>
	inline wchar_t *uninitialized_copy(const wchar_t *first, const wchar_t *last,
		wchar_t *result) {
		std::memmove(result, first, sizeof(wchar_t) * (last - first));
		return result + (last - first);
	}


	// �ж�X�Ƿ�ΪPOD
	template <typename ForwardIterator, typename T>
	void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
		const T &x, _true_type) {
		fill(first, last, x);
	}


	template <typename ForwardIterator, typename T>
	inline void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
		const T &x, _false_type) {
		auto cur = first;
		for (; cur != last; ++cur)
			construct(&*cur, x);
	}


	// �Ƶ���X������
	template <typename ForwardIterator, typename T, typename X>
	inline void _uninitialized_fill(ForwardIterator first, ForwardIterator last,
		const T &x, X *) {
		using is_POD = typename _type_traits<X>::is_POD_type;
		_uninitialized_fill_aux(first, last, x, is_POD());
	}


	// fill����first��last�ڲ���x�ĸ���Ʒ��Ҫ����д�룬�����õ��������
	template <typename ForwardIterator, typename T>
	inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x) {
		_uninitialized_fill(first, last, x, value_type(first));
	}
	

	// �ж�X�Ƿ�ΪPOD
	template <typename ForwardIterator, typename Size, typename T>
	inline ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n,
		const T &x, _true_type) {
		return fill_n(first, n, x);
	}


	template <typename ForwardIterator, typename Size, typename T>
	inline ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n,
		const T &x, _false_type) {
		auto cur = first;
		for (; n > 0; --n, ++cur)
			construct(&*cur, x);
		return cur;
	}


	// �Ƶ�X
	template <typename ForwardIterator, typename Size, typename T, typename X>
	inline ForwardIterator _uninitialized_fill_n(ForwardIterator first, Size n,
		const T &x, X *) {
		using is_POD = typename _type_traits<X>::is_POD_type;
		return _uninitialized_fill_n_aux(first, n, x, is_POD());
	}


	// fill_n����[first, first + n)�Ϲ���x
	template <typename ForwardIterator, typename Size, typename T>
	inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n,
		const T &x) {
		return _uninitialized_fill_n(first, n, x, value_type(first));
	}
}
