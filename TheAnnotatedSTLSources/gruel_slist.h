#pragma once


#include "gruel_memory.h"


namespace gruel {

	/* 综述：slist，单向链表，呃，差不多是list低配版，但在有些操作和空间占用上表现更好 */

	/* 动用继承关系的好处是可以节省空间（我认为） */
	// 节点基类只有指向下一节点的指针
	struct _slist_node_base {
		_slist_node_base *next;
	};


	// 节点类有数据域
	template <typename T>
	struct _slist_node : public _slist_node_base {
		T data;
	};


	// 全局函数，将new_node插入到prev_node之前
	inline _slist_node_base *_slist_make_link(
		_slist_node_base *prev_node, _slist_node_base *new_node) {
		new_node->next = prev_node->next;
		prev_node->next = new_node;
		return new_node;
	}


	// 全局函数，计算以node为头的单链表的长度
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

		// 因为是单向链表，所以只需要incr
		void incr() { node = node->next; }

		bool operator==(const _slist_iterator_base &x) const {
			return node == x.node;
		}

		bool operator!=(const _slist_iterator_base &x) const {
			return node != x.node;
		}
	};


	// 迭代器类
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


	// 单向链表，该类省略了很多功能，但这些功能函数和list相近，可以借鉴
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
		// 头哨兵节点
		list_node_base head;

	public:
		slist() {
			head.next = nullptr;
		}
		~slist() { clear(); }

	public:
		iterator begin() { return iterator((list_node *)head.next); }
		const_iterator begin() const { return const_iterator((list_node *)head.next); }
		iterator end() { return iterator(nullptr); }
		const_iterator end() const { return const_iterator(nullptr); }
		size_type size() const { return _slist_size(head.next); }
		bool empty() const { return head.next == nullptr; }

		void swap(slist &lhs) {
			auto tmp = head.next;
			head.next = lhs.head.next;
			lhs.head.next = tmp;
		}

	public:
		reference front() { return ((list_node *)head.next)->data; }

		// 注意新插入的元素在position之后
		iterator insert_after(iterator position, const value_type &x) {
			return iterator((list_node *)_slist_make_link(position.node, crete_node(x)));
		}

		// 注意这个erase删除的是传入迭代器的后继节点
		iterator erase_after(iterator position) {
			iterator old = prev;
			++prev;
			old.node->next = prev.node->next;
			destory_node(prev);
			return ++old;
		}

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
				// 删除一个指针时，可以用第三方变量记住，后面挺常用的
				auto tmp = cur;
				cur = cur->next;
				destory_node(tmp);
			}
			head.next = nullptr;
		}
	};

}

