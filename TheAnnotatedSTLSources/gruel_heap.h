#pragma once


#include "gruel_iterator.h"


namespace gruel {

	/* ������binary_heap������ѣ�����Ϊ���ѣ���ʵ���Ͼ���һ����ȫ��������������Ϊ���ڵ�Ϊ
	���нڵ��е����ڵ㣬�ҿ�ͨ����ʽ��������ͨ��һϵ�к�������������ʵ�� */

	/* ��ͷ�ļ�������һϵ�жѲ����ĺ��� */

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


	// ��һ�麯������ָ���ȽϹ���
	// ����һ�麯����࣬����ָ���˱ȽϹ���
	// �������⣺�����غ����ĵ��ò���ȷ��ԭ��û��ָ�������ռ�Ӷ���std�еĺ���������ͻ
	template <typename RandomAccessIterator, typename Compare>
	inline void push_heap(RandomAccessIterator first,
		RandomAccessIterator last, const Compare &comp) {
		_push_heap_aux(first, last, difference_type(first),
			value_type(first), comp);
	}


/* ---------------------------------------------------------------------------------------------------- */


	/* �ú����Ĳ�������Ϊ��firstΪ�ײ�������ͷ��
	holeIndexΪ����Ľڵ㣨ʵΪβ�ڵ㣩��ͷ�ڵ�ľ��룻
	topIndexΪ�����ڵ���ͷ�ڵ�ľ��룻
	valueΪ�²���Ԫ�ص�ֵ */
	template <typename RandomAccessIterator, typename Distance, typename T>
	void _push_heap(RandomAccessIterator first, Distance holeIndex,
		Distance topIndex, T value) {
		auto parent = (holeIndex - 1) / 2;	// ���ݶ���ѵ������ҳ��²���ڵ�֮���ڵ�
		// �������ڵ�û�е��ﶥ�ˣ��Ҹ��ڵ�ֵС����ֵ���������������ʣ�
		// ����˼·������ͬ���ڵ�Ƚϣ���������븸�ڵ�ֵ�����������ݱȽ�
		while (holeIndex > topIndex && *(first + parent) < value) {
			*(first + holeIndex) = *(first + parent);	// ������½ڵ��ֵΪ��ֵ
			holeIndex = parent;	// ����
			parent = (holeIndex - 1) / 2;	// �²���λ�õĸ��ڵ�
		}
		// �ҵ�����λ�ã���������Ϊ��ֵ
		*(first + holeIndex) = value;
	}


	template <typename RandomAccessIterator, typename Distance, typename T>
	inline void _push_heap_aux(RandomAccessIterator first,
		RandomAccessIterator last, Distance *, T *) {
		// �����ƶϳ������͹���һ���²����Ԫ��
		_push_heap(first, Distance(last - first - 1), Distance(0),
			T(*(last - 1)));
	}


	// ��Ԫ�ز���Ѳ�ά�����ѵ�����     
	// ��һ�麯��������ָ���ȽϹ���
	/* �ú���������������������������һ��heap�ײ�������vector)��ͷβ��
	������Ԫ���Ѿ����뵽�ײ���������β�ˣ����������������������push_heap
	��ִ�н��Ϊδ���� */
	template <typename RandomAccessIterator>
	inline void push_heap(RandomAccessIterator first,
		RandomAccessIterator last) {
		// ����������һЩ�����ƶϵ��ַ����ҳ��õ������ľ������ͺ�ָ��������
		_push_heap_aux(first, last, difference_type(first),
			value_type(first));
	}


/* -------------------------------------------------------------------------------------------- */


	// ���µ���heap������Ϊ0��������ֵΪvalue��ԭβֵ��
	// ����˼·�����ҳ����ʵ�λ�ã�����value,�پ���_push_heap����
	template <typename RandomAccessIterator, typename Distance, typename T>
	void _adjust_heap(RandomAccessIterator first, Distance holeIndex,
		Distance len, T value) {
		auto topIndex = holeIndex;	// ��ʱ�ı�Ķ�Ϊͷ�ڵ�
		auto rightChild = 2 * holeIndex + 2;	// ���ڵ�����ӽڵ�
		// �����ӽڵ㻹�ڶ���
		while (rightChild < len) {
			// ���ҽڵ�Ƚϣ���rightChild��ʾ�ϴ��ӽڵ�
			if (*(first + rightChild) < *(first + rightChild - 1))
				--rightChild;
			// ����ֵ��Ϊ�ϴ��ӽڵ�ֵ��Ȼ������
			*(first + holeIndex) = *(first + rightChild);
			holeIndex = rightChild;
			rightChild = 2 * rightChild + 2;
		}
		// û�����ӽڵ�ֻ�����ӽڵ�
		if (rightChild == len) {
			// �����ӽڵ���Ϊ��ֵ
			*(first + holeIndex) = *(first + rightChild - 1);
			holeIndex = rightChild - 1;	// ��������
		}
		// ����
		_push_heap(first, holeIndex, topIndex, value);
	}


	template <typename RandomAccessIterator, typename Distance, typename T>
	inline void _pop_heap(RandomAccessIterator first,
		RandomAccessIterator last, RandomAccessIterator result, 
		T value, Distance *) {
		*result = *first;	// �趨βֵΪ��ֵ
		_adjust_heap(first, Distance(0), Distance(last - first), value);
	}


	// ˼·Ϊ�����趨������ֵΪβֵ��Ȼ����ֵ����β�ڵ�
	// Ȼ������[first, last-1)
	template <typename RandomAccessIterator, typename T>
	inline void _pop_heap_aux(RandomAccessIterator first,
		RandomAccessIterator last, T *) {
		_pop_heap(first, last - 1, last - 1, T(*(last - 1)),
			difference_type(first));
	}


	// ������Ԫ�ص����ѣ���ά�����ѵ�����
	// ���Ԫ�ؼ�Ϊ����Ԫ��
	template <typename RandomAccessIterator>
	inline void pop_heap(RandomAccessIterator first,
		RandomAccessIterator last) {
		_pop_heap_aux(first, last, value_type(first));
	}


/* ---------------------------------------------------------------------------------------------- */

	// һ�°汾����ָ���ȽϺ���
	// ���µ���heap������Ϊ0��������ֵΪvalue��ԭβֵ��
	// ����˼·�����ҳ����ʵ�λ�ã�����value,�پ���_push_heap����
	template <typename RandomAccessIterator, typename Distance, typename T, typename Compare>
	void _adjust_heap(RandomAccessIterator first, Distance holeIndex,
		Distance len, T value, const Compare &comp) {
		auto topIndex = holeIndex;	// ��ʱ�ı�Ķ�Ϊͷ�ڵ�
		auto rightChild = 2 * holeIndex + 2;	// ���ڵ�����ӽڵ�
		// �����ӽڵ㻹�ڶ���
		while (rightChild < len) {
			// ���ҽڵ�Ƚϣ���rightChild��ʾ�ϴ��ӽڵ�
			if (comp(*(first + rightChild), *(first + rightChild - 1)))
				--rightChild;
			// ����ֵ��Ϊ�ϴ��ӽڵ�ֵ��Ȼ������
			*(first + holeIndex) = *(first + rightChild);
			holeIndex = rightChild;
			rightChild = 2 * rightChild + 2;
		}
		// û�����ӽڵ�ֻ�����ӽڵ�
		if (rightChild == len) {
			// �����ӽڵ���Ϊ��ֵ
			*(first + holeIndex) = *(first + rightChild - 1);
			holeIndex = rightChild - 1;	// ��������
		}
		// ����
		_push_heap(first, holeIndex, topIndex, value, comp);
	}


	template <typename RandomAccessIterator, typename Distance, typename T, typename Compare>
	inline void _pop_heap(RandomAccessIterator first,
		RandomAccessIterator last, RandomAccessIterator result,
		T value, Distance *, const Compare &comp) {
		*result = *first;	// �趨βֵΪ��ֵ
		_adjust_heap(first, Distance(0), Distance(last - first), value, comp);
	}


	// ˼·Ϊ�����趨������ֵΪβֵ��Ȼ����ֵ����β�ڵ�
	// Ȼ������[first, last-1)
	template <typename RandomAccessIterator, typename T, typename Compare>
	inline void _pop_heap_aux(RandomAccessIterator first,
		RandomAccessIterator last, T *, const Compare &comp) {
		_pop_heap(first, last - 1, last - 1, T(*(last - 1)),
			difference_type(first), comp);
	}


	// ������Ԫ�ص����ѣ���ά�����ѵ�����
	// ���Ԫ�ؼ�Ϊ����Ԫ��
	template <typename RandomAccessIterator, typename Compare>
	inline void pop_heap(RandomAccessIterator first,
		RandomAccessIterator last, const Compare &comp) {
		_pop_heap_aux(first, last, value_type(first), comp);
	}

	
/* -------------------------------------------------------------------------------------- */


	// ����ֻ�費ͣ�������ڵ㼴��
	template <typename RandomAccessIterator>
	void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
		while ((last - first) > 1)
			pop_heap(first, last);
	}


/* --------------------------------------------------------------------------------------- */
	

	template <typename RandomAccessIterator, typename Distance, typename T>
	void _make_heap(RandomAccessIterator first,
		RandomAccessIterator last, T *, Distance *) {
		// ����Ϊ0��1����Ҫ����
		if (last - first < 2) return;
		// ��¼Ԫ�ظ���
		auto len = last - first;
		// �ҵ���һ��Ҫ���ŵ�������������Ϊɶ������
		auto holeIndex = (len - 2) / 2;
		while (true) {
			// ������holeIndexΪ�׵����������Կ�����Ҫ������Ԫ�ؼ�ΪholeIndex��ֵ
			_adjust_heap(first, holeIndex, len, T(*(first + holeIndex)));
			// ������Ͼͷ���
			if (holeIndex == 0) return;
			// ���������������һ���ڵ�
			--holeIndex;
		}
	}
	
	
	// ����[first, last)��Ԫ������һ������
	template <typename RandomAccessIterator>
	inline void make_heap(RandomAccessIterator first,
		RandomAccessIterator last) {
		_make_heap(first, last, value_type(first), difference_type(first));
	}

}
