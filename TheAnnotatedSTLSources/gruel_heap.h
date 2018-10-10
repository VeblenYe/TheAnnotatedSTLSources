#pragma once


#include "gruel_iterator.h"


namespace gruel {

	/* 综述：binary_heap，二叉堆（这里为最大堆），实际上就是一个完全二叉树，其性质为根节点为
	所有节点中的最大节点，且可通过隐式表述，即通过一系列函数和其他容器实现 */

	/* 该头文件包含了一系列堆操作的函数 */

	template <typename RandomAccessIterator, typename Distance,
		typename T, typename Compare>
		void _push_heap(RandomAccessIterator first, Distance holeIndex,
			Distance topIndex, T value, const Compare &comp) {
		auto parent = (holeIndex - 1) / 2;
		while (holeIndex > topIndex && comp(*(first + parent), value)) {
			*(first + holeIndex) = *(first + parent);
			holeIndex = parent;
			parent = (holeIndex - 1) / 2;
		}
		*(first + holeIndex) = value;
	}


	template <typename RandomAccessIterator, typename Distance,
		typename T, typename Compare>
		inline void _push_heap_aux(RandomAccessIterator first,
			RandomAccessIterator last, Distance *, T *, const Compare &comp) {
		_push_heap(first, Distance(last - first - 1), Distance(0), T
		(*(last - 1)), comp);
	}


	// 这一组函数允许指定比较规则
	// 与下一组函数差不多，不过指定了比较规则
	// 出现问题：对重载函数的调用不明确，原因没有指定命名空间从而与std中的函数发生冲突
	template <typename RandomAccessIterator, typename Compare>
	inline void push_heap(RandomAccessIterator first,
		RandomAccessIterator last, const Compare &comp) {
		_push_heap_aux(first, last, difference_type(first),
			value_type(first), comp);
	}


/* ---------------------------------------------------------------------------------------------------- */


	/* 该函数的参数意义为：first为底部容器的头；
	holeIndex为插入的节点（实为尾节点）离头节点的距离；
	topIndex为顶部节点离头节点的距离；
	value为新插入元素的值 */
	template <typename RandomAccessIterator, typename Distance, typename T>
	void _push_heap(RandomAccessIterator first, Distance holeIndex,
		Distance topIndex, T value) {
		auto parent = (holeIndex - 1) / 2;	// 根据二叉堆的性质找出新插入节点之父节点
		// 如果插入节点没有到达顶端，且父节点值小于新值（不符合最大堆性质）
		// 具体思路，不断同父节点比较，不满足就与父节点值交换继续上溯比较
		while (holeIndex > topIndex && *(first + parent) < value) {
			*(first + holeIndex) = *(first + parent);	// 令插入新节点的值为父值
			holeIndex = parent;	// 上溯
			parent = (holeIndex - 1) / 2;	// 新插入位置的父节点
		}
		// 找到合适位置，插入设置为新值
		*(first + holeIndex) = value;
	}


	template <typename RandomAccessIterator, typename Distance, typename T>
	inline void _push_heap_aux(RandomAccessIterator first,
		RandomAccessIterator last, Distance *, T *) {
		// 根据推断出的类型构建一个新插入的元素
		_push_heap(first, Distance(last - first - 1), Distance(0),
			T(*(last - 1)));
	}


	// 将元素插入堆并维持最大堆的性质     
	// 这一组函数不允许指定比较规则
	/* 该函数接受两个迭代器，用来表现一个heap底部容器（vector)的头尾，
	并且新元素已经插入到底部容器的最尾端，如果不符合这两个条件，push_heap
	的执行结果为未定义 */
	template <typename RandomAccessIterator>
	inline void push_heap(RandomAccessIterator first,
		RandomAccessIterator last) {
		// 这里运用了一些类型推断的手法，找出该迭代器的距离类型和指向物类型
		_push_heap_aux(first, last, difference_type(first),
			value_type(first));
	}


/* -------------------------------------------------------------------------------------------- */


	// 重新调整heap，洞号为0，欲调整值为value（原尾值）
	// 具体思路，先找出合适的位置，插入value,再经由_push_heap调整
	template <typename RandomAccessIterator, typename Distance, typename T>
	void _adjust_heap(RandomAccessIterator first, Distance holeIndex,
		Distance len, T value) {
		auto topIndex = holeIndex;	// 此时改变的洞为头节点
		auto rightChild = 2 * holeIndex + 2;	// 洞节点的右子节点
		// 当右子节点还在堆内
		while (rightChild < len) {
			// 左右节点比较，以rightChild表示较大子节点
			if (*(first + rightChild) < *(first + rightChild - 1))
				--rightChild;
			// 将洞值设为较大子节点值，然后下行
			*(first + holeIndex) = *(first + rightChild);
			holeIndex = rightChild;
			rightChild = 2 * rightChild + 2;
		}
		// 没有右子节点只有左子节点
		if (rightChild == len) {
			// 将左子节点设为洞值
			*(first + holeIndex) = *(first + rightChild - 1);
			holeIndex = rightChild - 1;	// 洞号下移
		}
		// 上溯
		_push_heap(first, holeIndex, topIndex, value);
	}


	template <typename RandomAccessIterator, typename Distance, typename T>
	inline void _pop_heap(RandomAccessIterator first,
		RandomAccessIterator last, RandomAccessIterator result, 
		T value, Distance *) {
		*result = *first;	// 设定尾值为首值
		_adjust_heap(first, Distance(0), Distance(last - first), value);
	}


	// 思路为首先设定欲调整值为尾值，然后将首值调至尾节点
	// 然后重整[first, last-1)
	template <typename RandomAccessIterator, typename T>
	inline void _pop_heap_aux(RandomAccessIterator first,
		RandomAccessIterator last, T *) {
		_pop_heap(first, last - 1, last - 1, T(*(last - 1)),
			difference_type(first));
	}


	// 将最大的元素弹出堆，并维持最大堆的性质
	// 最大元素即为顶端元素
	template <typename RandomAccessIterator>
	inline void pop_heap(RandomAccessIterator first,
		RandomAccessIterator last) {
		_pop_heap_aux(first, last, value_type(first));
	}


/* ---------------------------------------------------------------------------------------------- */

	// 一下版本可以指定比较函数
	// 重新调整heap，洞号为0，欲调整值为value（原尾值）
	// 具体思路，先找出合适的位置，插入value,再经由_push_heap调整
	template <typename RandomAccessIterator, typename Distance, typename T, typename Compare>
	void _adjust_heap(RandomAccessIterator first, Distance holeIndex,
		Distance len, T value, const Compare &comp) {
		auto topIndex = holeIndex;	// 此时改变的洞为头节点
		auto rightChild = 2 * holeIndex + 2;	// 洞节点的右子节点
		// 当右子节点还在堆内
		while (rightChild < len) {
			// 左右节点比较，以rightChild表示较大子节点
			if (comp(*(first + rightChild), *(first + rightChild - 1)))
				--rightChild;
			// 将洞值设为较大子节点值，然后下行
			*(first + holeIndex) = *(first + rightChild);
			holeIndex = rightChild;
			rightChild = 2 * rightChild + 2;
		}
		// 没有右子节点只有左子节点
		if (rightChild == len) {
			// 将左子节点设为洞值
			*(first + holeIndex) = *(first + rightChild - 1);
			holeIndex = rightChild - 1;	// 洞号下移
		}
		// 上溯
		_push_heap(first, holeIndex, topIndex, value, comp);
	}


	template <typename RandomAccessIterator, typename Distance, typename T, typename Compare>
	inline void _pop_heap(RandomAccessIterator first,
		RandomAccessIterator last, RandomAccessIterator result,
		T value, Distance *, const Compare &comp) {
		*result = *first;	// 设定尾值为首值
		_adjust_heap(first, Distance(0), Distance(last - first), value, comp);
	}


	// 思路为首先设定欲调整值为尾值，然后将首值调至尾节点
	// 然后重整[first, last-1)
	template <typename RandomAccessIterator, typename T, typename Compare>
	inline void _pop_heap_aux(RandomAccessIterator first,
		RandomAccessIterator last, T *, const Compare &comp) {
		_pop_heap(first, last - 1, last - 1, T(*(last - 1)),
			difference_type(first), comp);
	}


	// 将最大的元素弹出堆，并维持最大堆的性质
	// 最大元素即为顶端元素
	template <typename RandomAccessIterator, typename Compare>
	inline void pop_heap(RandomAccessIterator first,
		RandomAccessIterator last, const Compare &comp) {
		_pop_heap_aux(first, last, value_type(first), comp);
	}

	
/* -------------------------------------------------------------------------------------- */


	// 排序只需不停弹出根节点即可
	template <typename RandomAccessIterator>
	void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
		while ((last - first) > 1)
			pop_heap(first, last);
	}


/* --------------------------------------------------------------------------------------- */
	

	template <typename RandomAccessIterator, typename Distance, typename T>
	void _make_heap(RandomAccessIterator first,
		RandomAccessIterator last, T *, Distance *) {
		// 长度为0或1不需要重派
		if (last - first < 2) return;
		// 记录元素个数
		auto len = last - first;
		// 找到第一个要重排的子树，不晓得为啥这样算
		auto holeIndex = (len - 2) / 2;
		while (true) {
			// 调整以holeIndex为首的子树，可以看到需要调整的元素即为holeIndex的值
			_adjust_heap(first, holeIndex, len, T(*(first + holeIndex)));
			// 调整完毕就返回
			if (holeIndex == 0) return;
			// 重排完成子树的上一个节点
			--holeIndex;
		}
	}
	
	
	// 根据[first, last)的元素生成一个最大堆
	template <typename RandomAccessIterator>
	inline void make_heap(RandomAccessIterator first,
		RandomAccessIterator last) {
		_make_heap(first, last, value_type(first), difference_type(first));
	}

}
