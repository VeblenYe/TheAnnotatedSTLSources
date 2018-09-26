#pragma once


#include "gruel_memory.h"


namespace gruel {


	/* 动用继承关系的好处是可以节省空间（我认为） */
	struct _slist_node_base {
		_slist_node_base *next;
	};


	template <typename T>
	struct _slist_node : public _slist_node_base {
		T data;
	};


	inline _slist_node_base *_slist_make_link(
		_slist_node_base *prev_node, _slist_node_base *new_node) {
		new_node->next = prev_node->next;
		prev_node->next = new_node;
		return new_node;
	}


	inline std::size_t _slist_size(_slist_node_base *node) {
		std::size_t result = 0;
		for (; node != nullptr; node = node->next)
			++result;
		return result;
	}


	/* 之所以在这个基类里重载==和!=，是因为该操作符只需返回bool，
	而不需要返回迭代器本身，也就是说原则是能在该类中实现的函数尽量
	在该类中实现，也就是基类的抽取共性 */
	struct _slist_iterator_base {
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using iterator_category = forward_iterator_tag;

		_slist_node_base *node;

		_slist_iterator_base(_slist_node_base *x) : node(x) {}

		void incr() { node = node->next; }

		bool operator==(const _slist_iterator_base &x) const {
			return node == x.node;
		}

		bool operator!=(const _slist_iterator_base &x) const {
			return node != x.node;
		}
	};


	template <typename T, typename Ref, typename Ptr>
	struct _slist_iterator : public _slist_iterator_base {

		using iterator = _slist_iterator<T, T &, T *>;
		using const_iterator = _slist_iterator<T, const T &, const T *>;
		using self = _slist_iterator<T, Ref, Ptr>;

		using value_type = T;
		using pointer = Ptr;
		using reference = Ref;
		using list_node = _slist_node<T>;

		_slist_iterator(list_node *x) : _slist_iterator_base(x) {}
		_slist_iterator() : _slist_iterator_base(nullptr) {}
		_slist_iterator(const _slist_iterator &x) : _slist_iterator_base(x.node) {}

		reference operator*() const { return ((list_node *)node)->data; }
		pointer operator->() const { return &(operator*()); }

		self &operator++() {
			incr();
			return *this;
		}

		self *operator++(int) {
			self tmp = *this;
			++tmp;
			return tmp;
		}
	};


	template <typename T, typename Alloc = alloc>
	class slist {
	public:
		using value_type = T;
		using pointer = value_type * ;
		using const_pointer = const value_type *;
		using reference = value_type & ;
		using const_reference = const value_type &;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		using iterator = _slist_iterator<T, T &, T *>;
		using const_iterator = _slist_iterator<T, const T &, const T *>;
	private:
		using list_node = _slist_node<T>;
		using list_node_base = _slist_node_base;
		using iterator_base = _slist_iterator_base;
		using list_node_allocator = simple_alloc<list_node, Alloc>;

		static list_node *create_node(const value_type &x) {
			list_node *node = list_node_allocator::allocate();
			// 以下省略异常机制
			construct(&node->data, x);
			node->next = nullptr;
			return node;
		}

		static void destory_node(list_node *node) {
			destory(&node->data);
			list_node_allocator::deallocate(node);
		}
	private:
		list_node_base head;

	public:
		slist() {
			head.next = nullptr;
		}
		~slist() { clear(); }

	public:
		iterator begin() { return iterator((list_node *)head.next); }
		iterator end() { return iterator(nullptr); }
		size_type size() const { return _slist_size(head.next); }
		bool empty() const { return head.next == nullptr; }

		void swap(slist &lhs) {
			auto tmp = head.next;
			head.next = lhs.head.next;
			lhs.head.next = tmp;
		}

	public:
		reference front() { return ((list_node *)head.next)->data; }

		void push_front(const value_type &x) {
			_slist_make_link(&head, create_node(x));
		}

		void pop_front() {
			auto node = (list_node *)head.next;
			head.next = node->next;
			destory_node(node);
		}

		void clear() {
			auto cur = (list_node *)head.next;
			while (cur != nullptr) {
				auto tmp = cur;
				cur = cur->next;
				destory_node(tmp);
			}
			head.next = nullptr;
		}
	};

}

