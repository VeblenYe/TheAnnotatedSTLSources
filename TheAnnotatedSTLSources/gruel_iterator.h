#pragma once


#include <cstddef>


namespace gruel {

	/* �������ֵ��������� */
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag : public input_iterator_tag {};
	struct bidirectional_iterator_tag : public forward_iterator_tag {};
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};


	/* ����������Ļ��࣬�����������Ƶ�����ʱ���붨��ĳ�Ա */
	// �_t��int����˼��
	// �ɼ̳�Ҳ�ɲ��̳�
	template <typename Category, typename T, typename Distance = std::ptrdiff_t,
		typename Pointer = T * , typename Reference = T &>
	struct iterator {
		using iterator_category = Category;	// ����������
		using value_type = T;				// ������ָ��������
		using difference_type = Distance;	// ������֮��ľ�������
		using pointer = Pointer;			// ������ָ����ָ������
		using reference = Reference;		// ������ָ������������
	};


	/* ��������ե֭���� */
	// ����һ�����Եĺô��ǿ��ԶԲ�ͬ��Iterator����ƫ�ػ�
	template <typename Iterator>
	struct iterator_traits {
		using iterator_category = typename Iterator::iterator_category;
		using value_type = typename Iterator::value_type;
		using difference_type = typename Iterator::difference_type;
		using pointer = typename Iterator::pointer;
		using reference = typename Iterator::reference;
	};


	/* ���ԭ��ָ����Ƶ�ƫ�ػ��� */
	template <typename T>
	struct iterator_traits<T *> {
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T * ;
		using reference = T & ;
	};


	/* ��Գ���ָ����Ƶ�ƫ�ػ��� */
	template <typename T>
	struct iterator_traits<const T *> {
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = const T * ;
		using reference = const T & ;
	};


	/* ���º������Է������ȡ�������������� */
	/* ע�⣬��ȡ����������ʱ���ص���һ����ʱ���� */
	template <typename Iterator>
	inline auto iterator_category(const Iterator &) 
		-> typename iterator_traits<Iterator>::iterator_category {
		using category = typename iterator_traits<Iterator>::iterator_category;
		return category();
	}


	/* ��ȡ������ָ�������ͷ��ص���һ��ǿתΪ�����͵Ŀ�ָ�룬�ռ�ռ���ٻ�Ч�ʻ��ߣ�ѧϰ�� */
	template <typename Iterator>
	inline auto value_type(const Iterator &)
		-> typename iterator_traits<Iterator>::value_type * {
		return static_cast<typename iterator_traits<Iterator>::value_type *>(nullptr);
	}


	/* ͬ�� */
	template <typename Iterator>
	inline auto difference_type(const Iterator &)
		-> typename iterator_traits<Iterator>::difference_type * {
		return static_cast<typename iterator_traits<Iterator>::difference *>(nullptr);
	}


	/* �����������Ҫ�������� */
	template <typename InputIterator>
	inline auto _distance(InputIterator first, InputIterator last, input_iterator_tag)
		-> typename iterator_traits<InputIterator>::difference_type {
		typename iterator_traits<InputIterator>::difference_type n = 0;
		while (first != last) {
			++first;
			++n;
		}
		return n;
	}


	/* ������ʵ�������ֱ����� */
	template <typename RandomAccessIterator>
	inline auto _distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
		-> typename iterator_traits<RandomAccessIterator>::difference_type {
		return last - first;
	}


	// ���ڼ�������������֮��ľ��룬���ݵ������������ػ�
	template <typename InputIterator>
	inline auto distance(InputIterator first, InputIterator last)
		-> typename iterator_traits<InputIterator>::difference_type {
		return _distance(first, last, iterator_category(first));
	}


	template <typename InputIterator, typename Distance>
	inline void _advance(InputIterator &it, Distance n, input_iterator_tag) {
		while (n--)
			++it;
	}


	template <typename BidirectionalIterator, typename Distance>
	inline void _advance(BidirectionalIterator &it, Distance n, bidirectional_iterator_tag) {
		if (n >= 0)
			while (n--)
				++it;
		else
			while (n++)
				--it;
	}


	template <typename RandomAccessIterator, typename Distance>
	inline void _advance(RandomAccessIterator &it, Distance n, random_access_iterator_tag) {
		it += n;
	}


	// ��������ǰ���������Ӧ����λ
	template <typename InputIterator, typename Distance>
	inline void advance(InputIterator &it, Distance n) {
		_advance(it, n, iterator_category(it));
	}

}
