#pragma once


#include <cstddef>
#include <iostream>


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
		return static_cast<typename iterator_traits<Iterator>::difference_type *>(nullptr);
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


	/* ���µ����Ϊiterator adapters�������������� */
	/* Adapter�����������Ķ���Ϊ����һ��class�Ľӿ�ת��Ϊ��һ��class�Ľӿڣ�ʹԭ����ӿڲ����ݶ����ܺ�����classes����һ������ */
	// �����¾ͽ�ֻ�б������ܵĵ������޸ĳ�Ϊ�˾��в��빦�ܵĵ���������������insert iterators
	template <typename Container>
	class back_insert_iterator {
	protected:
		using self = back_insert_iterator<Container>;
		Container *container;

	public:
		// �õ�������������������=�Ӹ�ֵ��Ϊ����β�����������ҹر��������ȹ��ܣ���������Ϊoutput iterator
		using iterator_category = output_iterator_tag;
		using value_type = void;
		using difference_type = void;
		using pointer = void;
		using reference = void;

		explicit back_insert_iterator(Container &x) : container(&x) {}
		// ��������=����Ϊ��β������
		self &operator=(const typename Container::value_type &value) {
			container->push_back(value);
			return *this;
		}

		// �رչ���
		self &operator*() { return *this; }
		self &operator++() { return *this; }
		self &operator++(int) { return *this; }
	};

	// ��������������ʹ������������
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


	// �����������Ϊ���Ӧ���������������reverse iterator
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
			// ע����������������ȡֵ���ǽ���Ӧ�������������һ��ȡֵ
			auto tmp = current;
			return *--tmp;
		}

		pointer operator->() const { return &(operator*()); }

		// ע��һ��ǰ���ͺ��˵Ĳ�����ȫ�෴
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
