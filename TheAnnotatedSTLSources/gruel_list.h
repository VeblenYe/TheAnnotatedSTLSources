#pragma once


#include "gruel_memory.h"


namespace gruel {

	// list节点
	template <typename T>
	struct _list_node {
		_list_node<T> *prev;
		_list_node<T> *next;
		T data;
	};

	// list的迭代器，继承自iterator基类
	template <typename T, typename Reference, typename Pointer>
	struct _list_iterator : public iterator<bidirectional_iterator_tag, T> {
		using iterator = _list_iterator<T, T &, T *>;

		// 不知道这个干嘛用，懂得了，方便
		using self = _list_iterator<T, Reference, Pointer>;

		// 指向list节点的普通指针
		using link_type = _list_node<T> *;
		link_type node;

		// 构造函数
		_list_iterator(link_type x) : node(x) {}
		_list_iterator() : node(nullptr) {}
		_list_iterator(const _list_iterator &x) : node(x.node) {}

		// 重载操作符
		bool operator==(const self &x) const { return node == x.node; }
		bool operator!=(const self &x) const { return node != x.node; }

		Reference operator*() const { return (*node).data; }

		// 迭代器成员存取运算子的标准做法？？不是很理解
		Pointer operator->() const { return &(operator*()); }

		self &operator++() {
			node = node->next;
			return *this;
		}

		self operator++(int) {
			self temp = *this;
			++*this;
			return temp;
		}

		self &operator--() {
			node = node->prev;
			return *this;
		}

		self operator--(int) {
			self temp = *this;
			--*this;
			return temp;
		}

	};

	template <typename T, typename Alloc = alloc>
	class list {
	protected:
		// 嵌套定义的声明有先后顺序
		using list_node = _list_node<T>;
		using list_iterator = _list_iterator<T, T &, T *>;
	public:

		// 嵌套定义
		using link_type = list_node * ;
		using value_type = T;
		using iterator = _list_iterator<T, T &, T *>;
		using const_iterator = _list_iterator<T, const T &, const T *>;
		using reference = value_type & ;
		using pointer = value_type * ;
		using difference_type = typename list_iterator::difference_type;
		using size_type = std::size_t;


		iterator begin() { return node->next; }
		const_iterator begin() const { return node->next; }
		iterator end() { return node; }
		const_iterator end() const { return node; }
		
		bool empty() const { return node->next == node; }
		size_type size() const {
			return size_type(distance(begin(), end()));
		}

		reference front() { return *begin(); }
		reference back() { return *--end(); }

		
		// 构造函数
		list() { empty_initialize(); }


		void push_back(const T &x) {
			insert(end(), x);
		}

		void push_front(const T &x) {
			insert(begin(), x);
		}

		iterator insert(iterator position, const T &x) {
			auto temp = create_node(x);
			temp->next = position.node;
			temp->prev = position.node->prev;
			position.node->prev->next = temp;
			position.node->prev = temp;
			return temp;
		}

		iterator erase(iterator position) {
			auto prev_node = position.node->prev;
			auto next_node = position.node->next;
			prev_node->next = next_node;
			next_node->prev = prev_node;
			destory_node(position.node);
			// 创建并返回一个迭代器对象
			return iterator(next_node);
		}

		void pop_front() { erase(begin()); }

		void pop_back() {
			auto temp = end();
			erase(--temp);
		}

		void clear();
		void remove(const T &value);
		void unique();

		// x必须不同于*this
		void splice(iterator position, list &x) {
			if (!x.empty())
				transfer(position, x.begin(), x.end());
		}

		void splice(iterator position, list &, iterator i) {
			auto j = i;
			++j;
			if (position == i || position == j)
				return;
			transfer(position, i, j);
		}

		void splice(iterator position, list &, iterator first, iterator last) {
			if (first != last)
				transfer(position, first, last);
		}


		void merge(list<T, Alloc> &x);
		void reverse();
		// STL的sort()需要接受RandomAccessIterator
		void sort();

		void swap(list<T, Alloc> &x) noexcept {
			auto temp = node;
			node = x.node;
			x.node = temp;
		}
		
		
	protected:

		// 只需要一个指针就可以表示整个环状双向链表
		link_type node;

		// 专属空间配置器，每次配置一个节点大小
		using list_node_allocate = simple_alloc<list_node, Alloc>;

		// 配置一个空节点空间
		link_type get_node() { return list_node_allocate::allocate(); }
		
		// 释放节点空间
		void put_node(link_type p) { list_node_allocate::deallocate(p); }

		link_type create_node(const T &x) {
			auto p = get_node();
			construct(&p->data, x);
			return p;
		}

		void destory_node(link_type p) {
			destory(&p->data);
			put_node(p);
		}

		void empty_initialize() {
			node = get_node();
			node->next = node;
			node->prev = node;
		}


		// 将[first, last)中的内容迁移至position前
		void transfer(iterator position, iterator first, iterator last) {
			last.node->prev->next = position.node;
			first.node->prev->next = last.node;
			position.node->prev->next = first.node;
			auto temp = position.node->prev;
			position.node->prev = last.node->prev;
			last.node->prev = first.node->prev;
			first.node->prev = temp;
		}

	};


	// 删除所有节点
	template <typename T, typename Alloc>
	void list<T, Alloc>::clear() {
		/* 不能这样写
		for (auto it = begin(); it != end(); ++it) {
			destory_node(it.node);
		} */

		// 这里不用迭代器的原因我认为是全部删除，所以直接删除节点不用配置
		auto cur = node->next;
		while (cur != node) {
			auto temp = cur;
			cur = cur->next;
			destory_node(temp);
		}
		// 重置node节点
		node->prev = node;
		node->next = node;
	}

	
	// 将数值为value的节点擦除
	template <typename T, typename Alloc>
	void list<T, Alloc>::remove(const T &value) {
		/* 不能用迭代器 
		for (auto it = begin(); it != end(); ++it)
			if (*it == value)
				erase(it);
				*/

		auto first = begin();
		auto last = end();	// 能这样写的原因是node节点不变化
		while (first != last) {
			auto next = first;
			++next;
			if (*first == value)
				erase(first);
			first = next;
		}
	}


	template <typename T, typename Alloc>
	void list<T, Alloc>::unique() {
		if (empty()) return;
		auto first = begin();
		auto last = end();
		auto next = first;
		while (++next != last) {
			if (*first == *next)
				erase(next);
			else
				first = next;
			// 修正区段范围
			next = first;
		}
	}


	template <typename T, typename Alloc>
	void list<T, Alloc>::merge(list<T, Alloc> &x) {
		auto first1 = begin();
		auto last1 = end();
		auto first2 = x.begin();
		auto last2 = x.end();
		
		while (first1 != last1 && first2 != last2)
			if (*first2 < *first1) {
				auto next = first2;
				transfer(first1, first2, ++next);
				first2 = next;
				// transfer(first1, first2, ++first2);
			}
			else
				++first1;
		if (first2 != last2) transfer(first1, first2, last2);
	}


	template <typename T, typename Alloc>
	void list<T, Alloc>::reverse() {
		if (node->next == node || node->next->next == node) return;

		auto first = begin();
		++first;
		while (first != end()) {
			auto old = first;
			++first;
			transfer(begin(), old, first);
		}
	}


	template <typename T, typename Alloc>
	void list<T, Alloc>::sort() {
		if (node->next == node || node->next->next == node)
			return;

		// 以下是快排代码，没看懂。。
		list<T, Alloc> carry;
		list<T, Alloc> counter[64];
		int fill = 0;
		while (!empty()) {
			carry.splice(carry.begin(), *this, begin());
			int i = 0;
			while (i < fill && !counter[i].empty()) {
				counter[i].merge(carry);
				carry.swap(counter[i++]);
			}
			carry.swap(counter[i]);
			if (i == fill)
				++fill;
		}

		for (int i = 1; i < fill; ++i)
			counter[i].merge(counter[i - 1]);
		swap(counter[fill - 1]);
	}

}