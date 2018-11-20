#pragma once


#include <cstddef>
#include <iostream>


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
		return static_cast<typename iterator_traits<Iterator>::difference_type *>(nullptr);
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


	/* 以下的类称为iterator adapters，迭代器适配器 */
	/* Adapter（适配器）的定义为：将一个class的接口转换为另一个class的接口，使原本因接口不兼容而不能合作的classes可以一起运作 */
	// 如以下就将只有遍历功能的迭代器修改成为了具有插入功能的迭代器适配器——insert iterators
	template <typename Container>
	class back_insert_iterator {
	protected:
		using self = back_insert_iterator<Container>;
		Container *container;

	public:
		// 该迭代器适配器将操作符=从赋值改为插入尾部操作，并且关闭了自增等功能，所以类型为output iterator
		using iterator_category = output_iterator_tag;
		using value_type = void;
		using difference_type = void;
		using pointer = void;
		using reference = void;

		explicit back_insert_iterator(Container &x) : container(&x) {}
		// 将操作符=重载为在尾部插入
		self &operator=(const typename Container::value_type &value) {
			container->push_back(value);
			return *this;
		}

		// 关闭功能
		self &operator*() { return *this; }
		self &operator++() { return *this; }
		self &operator++(int) { return *this; }
	};

	// 辅助函数，方便使用上述适配器
	template <typename Container>
	inline back_insert_iterator<Container> back_inserter(Container &x) {
		return back_insert_iterator<Container>(x);
	}


	template <typename Container>
	class front_insert_iterator {
	protected:
		using self = front_insert_iterator<Container>;
		Container *container;

	public:
		using iterator_category = output_iterator_tag;
		using value_type = void;
		using difference_type = void;
		using pointer = void;
		using reference = void;

		explicit front_insert_iterator(Container &x) : container(&x) {}
		self &operator=(const typename Container::value_type &value) {
			container->push_front(value);
			return *this;
		}

		self &operator*() { return *this; }
		self &operator++() { return *this; }
		self &operator++(int) { return *this; }
	};

	template <typename Container>
	inline front_insert_iterator<Container> front_inserter(Container &x) {
		return front_insert_iterator<Container>(x);
	}


	template <typename Container>
	class insert_iterator {
	protected:
		using self = insert_iterator<Container>;
		Container *container;
		typename Container::iterator iter;

	public:
		using iterator_category = output_iterator_tag;
		using value_type = void;
		using difference_type = void;
		using pointer = void;
		using reference = void;

		explicit insert_iterator(Container &x, typename Container::iterator i) : container(&x), iter(i) {}
		self &operator=(const typename Container::value_type &value) {
			iter = container->insert(iter, value);
			++iter;
			return *this;
		}

		self &operator*() { return *this; }
		self &operator++() { return *this; }
		self &operator++(int) { return *this; }
	};

	template <typename Container, typename Iterator>
	inline insert_iterator<Container> inserter(Container &x, Iterator i) {
		using iter = typename Container::iterator;
		return insert_iterator<Container>(x, iter(i));
	}


	// 将迭代器配接为其对应的逆向迭代器——reverse iterator
	template <typename Iterator>
	class reverse_iterator {
	protected:
		Iterator current;
		
	public:
		using iterator_category = typename Iterator::iterator_category;
		using value_type = typename Iterator::value_type;
		using difference_type = typename Iterator::difference_type;
		using pointer = typename Iterator::pointer;
		using reference = typename Iterator::reference;

		using iterator_type = Iterator;
		using self = reverse_iterator<Iterator>;

	public:
		reverse_iterator() {}
		explicit reverse_iterator(iterator_type x) : current(x) {}
		reverse_iterator(const self &x) : current(x.current) {}

		iterator_type base() const { return current; }
		reference operator*() const {
			// 注意这里对逆向迭代器取值就是将对应正向迭代器后退一格取值
			auto tmp = current;
			return *--tmp;
		}

		pointer operator->() const { return &(operator*()); }

		// 注意一下前进和后退的操作完全相反
		self &operator++() {
			--current;
			return *this;
		}

		self operator++(int) {
			auto tmp = *this;
			++*this;
			return tmp;
		}

		self &operator--() {
			++current;
			return *this;
		}

		self operator--(int) {
			auto tmp = *this;
			--*this;
			return tmp;
		}

		self operator+(difference_type n) const {
			return self(current - n);
		}

		self &operator+=(difference_type n) {
			current -= n;
			return *this;
		}

		self operator-(difference_type n) const {
			return self(current + n);
		}

		self &operator-=(difference_type n) {
			current += n;
			return *this;
		}

		reference operator[](difference_type n) const {
			return *(*this + n);
		}
	};


	template <typename T, typename Distance = std::ptrdiff_t>
	class istream_iterator {
	protected:
		std::istream *stream;
		T value;
		bool end_marker;
		void read() {
			end_marker = (*stream) ? true : false;
			if (end_marker)
				*stream >> value;
			end_marker = (*stream) ? true : false;
		}

		using self = istream_iterator<T, Distance>;

	public:
		using iterator_category = input_iterator_tag;
		using value_type = T;
		using difference_type = Distance;
		using pointer = const T *;
		using reference = const T &;

		istream_iterator() : stream(&std::cin), end_marker(false) {}
		istream_iterator(std::istream &s) : stream(&s) { read(); }

		reference operator*() const { return value; }
		pointer operator->() const { return &(operator*()); }

		self &operator++() {
			read();
			return *this;
		}

		self &operator++(int) {
			auto tmp = *this;
			++*this;
			return tmp;
		}
	};

	template <typename T, typename Distance>
	bool operator==(const istream_iterator<T, Distance> &lhs, const istream_iterator<T, Distance> &rhs) {
		return (lhs.end_marker == rhs.end_marker) && (!lhs.end_marker || lhs.stream == rhs.stream);
	}

	template <typename T, typename Distance>
	bool operator!=(const istream_iterator<T, Distance> &lhs, const istream_iterator<T, Distance> &rhs) {
		return !(lhs == rhs);
	}


	template <typename T>
	class ostream_iterator {
	protected:
		std::ostream *stream;
		const char *string;

		using self = ostream_iterator<T>;
	public:
		using iterator_category = output_iterator_tag;
		using value_type = void;
		using difference_type = void;
		using pointer = void;
		using reference = void;

		ostream_iterator(std::ostream &s) : stream(&s), string(nullptr) {}
		ostream_iterator(std::ostream &s, const char *c) : stream(&s), string(c) {}

		self &operator=(const T &value) {
			*stream << value;
			if (string) *stream << string;
			return *this;
		}

		self &operator*() { return *this; }
		self &operator++() { return *this; }
		self &operator++(int) { return *this; }
	};
}
