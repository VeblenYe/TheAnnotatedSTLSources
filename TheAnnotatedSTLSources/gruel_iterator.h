#pragma once


#include <cstddef>


namespace gruel {

	/* 定义五种迭代器类型 */
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag : public input_iterator_tag {};
	struct bidirectional_iterator_tag : public forward_iterator_tag {};
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};


	/* 定义迭代器的基类，该类包含了设计迭代器时必须定义的成员 */
	// 嘛，_t是int的意思吗
	// 可继承也可不继承
	template <typename Category, typename T, typename Distance = std::ptrdiff_t,
		typename Pointer = T * , typename Reference = T &>
	struct iterator {
		using iterator_category = Category;	// 迭代器类型
		using value_type = T;				// 迭代器指向物类型
		using difference_type = Distance;	// 迭代器之间的距离类型
		using pointer = Pointer;			// 迭代器指向物指针类型
		using reference = Reference;		// 迭代器指向物引用类型
	};


	/* 迭代器“榨汁机” */
	// 增加一层间接性的好处是可以对不同的Iterator进行偏特化
	template <typename Iterator>
	struct iterator_traits {
		using iterator_category = typename Iterator::iterator_category;
		using value_type = typename Iterator::value_type;
		using difference_type = typename Iterator::difference_type;
		using pointer = typename Iterator::pointer;
		using reference = typename Iterator::reference;
	};


	/* 针对原生指针设计的偏特化版 */
	template <typename T>
	struct iterator_traits<T *> {
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T * ;
		using reference = T & ;
	};


	/* 针对常量指针设计的偏特化版 */
	template <typename T>
	struct iterator_traits<const T *> {
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = const T * ;
		using reference = const T & ;
	};


	/* 以下函数可以方便地萃取出迭代器的特性 */
	/* 注意，萃取迭代器类型时返回的是一个临时对象 */
	template <typename Iterator>
	inline auto iterator_category(const Iterator &) 
		-> typename iterator_traits<Iterator>::iterator_category {
		using category = typename iterator_traits<Iterator>::iterator_category;
		return category();
	}


	/* 萃取迭代器指向物类型返回的是一个强转为该类型的空指针，空间占用少还效率还高，学习了 */
	template <typename Iterator>
	inline auto value_type(const Iterator &)
		-> typename iterator_traits<Iterator>::value_type * {
		return static_cast<typename iterator_traits<Iterator>::value_type *>(nullptr);
	}


	/* 同上 */
	template <typename Iterator>
	inline auto difference_type(const Iterator &)
		-> typename iterator_traits<Iterator>::difference_type * {
		return static_cast<typename iterator_traits<Iterator>::difference *>(nullptr);
	}


	/* 输入迭代器，要递增计算 */
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


	/* 随机访问迭代器，直接相减 */
	template <typename RandomAccessIterator>
	inline auto _distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
		-> typename iterator_traits<RandomAccessIterator>::difference_type {
		return last - first;
	}


	// 用于计算两个迭代器之间的距离，根据迭代器的类型特化
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


	// 将迭代器前进或后退相应个单位
	template <typename InputIterator, typename Distance>
	inline void advance(InputIterator &it, Distance n) {
		_advance(it, n, iterator_category(it));
	}

}
