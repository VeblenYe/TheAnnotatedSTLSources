#pragma once


#include <new>
#include "gruel_iterator.h"
#include "gruel_type_traits.h"


namespace gruel {

	/* ȫ�ֹ��캯����������ָ��λ�ù������ */
	template <typename T1, typename T2>
	inline void construct(T1 *p, const T2 &value) {
		new (p) T1(value);	// placement new
	}


	/* destory��һ�汾������һ��ָ�� */
	template <typename T>
	inline void destory(T *pointer) {
		pointer->~T();
	}
	
	
	/* û��trivial destructor����Ҫ��һ������������ */
	template <typename ForwardIterator>
	inline void _destory_aux(ForwardIterator first, ForwardIterator last, _false_type) {
		for (; first < last; ++first)
			destory(&*first);
	}


	/* ��trivial destructor��������в��� */
	template <typename ForwardIterator>
	inline void _destory_aux(ForwardIterator first, ForwardIterator last, _true_type) {}


	/* ��ͨ��_type_traits��ȡ���������Ƿ���trivial destructor */
	template <typename ForwardIterator, typename T>
	inline void _destory(ForwardIterator first, ForwardIterator last, T *) {
		using trivial_destructor = typename _type_traits<T>::has_trivial_destructor;
		_destory_aux(first, last, trivial_destructor());
	}


	/* destory�ڶ��汾������һ�Ե����� */
	/* ��ͨ��value_type��ȡ��������ָ�������� */
	template <typename ForwardIterator>
	inline void destory(ForwardIterator first, ForwardIterator last) {
		_destory(first, last, value_type(first));
	}


	/* ��Ե�����Ϊchar *�� wchar_t *���ػ��棬������� */
	inline void destory(char *, char *) {}
	inline void destory(wchar_t *, wchar_t *) {}

}