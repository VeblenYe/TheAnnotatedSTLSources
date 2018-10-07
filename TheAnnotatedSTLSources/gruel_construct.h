#pragma once


#include <new>
#include "gruel_iterator.h"
#include "gruel_type_traits.h"


namespace gruel {

	/* 全局构造函数，用于在指定位置构造对象 */
	template <typename T1, typename T2>
	inline void construct(T1 *p, const T2 &value) {
		new (p) T1(value);	// placement new
	}


	/* destory第一版本，接受一个指针 */
	template <typename T>
	inline void destory(T *pointer) {
		pointer->~T();
	}
	
	
	/* 没有trivial destructor就需要逐一调用析构函数 */
	template <typename ForwardIterator>
	inline void _destory_aux(ForwardIterator first, ForwardIterator last, _false_type) {
		for (; first < last; ++first)
			destory(&*first);
	}


	/* 有trivial destructor则无需进行操作 */
	template <typename ForwardIterator>
	inline void _destory_aux(ForwardIterator first, ForwardIterator last, _true_type) {}


	/* 再通过_type_traits萃取出该类型是否有trivial destructor */
	template <typename ForwardIterator, typename T>
	inline void _destory(ForwardIterator first, ForwardIterator last, T *) {
		using trivial_destructor = typename _type_traits<T>::has_trivial_destructor;
		_destory_aux(first, last, trivial_destructor());
	}


	/* destory第二版本，接受一对迭代器 */
	/* 先通过value_type萃取出迭代器指向物类型 */
	template <typename ForwardIterator>
	inline void destory(ForwardIterator first, ForwardIterator last) {
		_destory(first, last, value_type(first));
	}


	/* 针对迭代器为char *和 wchar_t *的特化版，无需操作 */
	inline void destory(char *, char *) {}
	inline void destory(wchar_t *, wchar_t *) {}

}