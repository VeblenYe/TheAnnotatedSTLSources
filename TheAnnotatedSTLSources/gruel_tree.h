#pragma once


/* 写了一周，不过其实就晚上在写。。 */


#include "gruel_memory.h"


namespace gruel {

	// 这样的定义我认为是为了以后的修改方便，有更大的弹性
	using _rb_tree_color_type = bool;
	const _rb_tree_color_type _rb_tree_red = false;
	const _rb_tree_color_type _rb_tree_black = true;


	// RB-Tree的节点基类，没有包含数据域
	struct _rb_tree_node_base {
		// 简化类型名
		using color_type = _rb_tree_color_type;
		// 指向该类型的指针
		using base_ptr = _rb_tree_node_base * ;

		color_type color;	// 节点颜色
		base_ptr parent;	// 父节点指针
		base_ptr left;		// 左孩子指针
		base_ptr right;		// 右孩子指针

		// 求以x为根的树的最小节点
		static base_ptr minimum(base_ptr x) {
			while (x->left)
				x = x->left;
			return x;
		}

		// 求以x为根的树的最大节点
		static base_ptr maximum(base_ptr x) {
			while (x->right)
				x = x->right;
			return x;
		}
	};


	// RB-Tree的节点模板类，包含数据域
	template <typename T>
	struct _rb_tree_node : public _rb_tree_node_base {
		using link_type = _rb_tree_node<T> *;
		T value_field;
	};


	//RB-Tree的迭代器基类，其指向的为节点基类
	struct _rb_tree_iterator_base {
		using base_ptr = typename _rb_tree_node_base::base_ptr;
		// 该迭代器类型为双向迭代器
		using iterator_category = bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t;

		base_ptr node;	// 指向节点基类，书上的话说：用于与容器之间产生一个连结关系
	};


	inline bool operator==(const _rb_tree_iterator_base &lhs,
		const _rb_tree_iterator_base &rhs) {
		return lhs.node == rhs.node;
	}


	inline bool operator!=(const _rb_tree_iterator_base &lhs,
		const _rb_tree_iterator_base &rhs) {
		return lhs.node != rhs.node;
	}


	//RB-Tree的迭代器模板类，其指向的为节点模板类
	template <typename Value, typename Ref, typename Ptr>
	struct _rb_tree_iterator : public _rb_tree_iterator_base {
		using value_type = Value;
		using reference = Ref;
		using pointer = Ptr;
		using iterator = _rb_tree_iterator<Value, Value &, Value *>;
		using const_iterator = _rb_tree_iterator<Value, const Value &, const Value *>;
		using self = _rb_tree_iterator<Value, Ref, Ptr>;
		using link_type = _rb_tree_node<Value> *;

		_rb_tree_iterator() {}
		// 注意这里派生类对象赋给基类指针
		_rb_tree_iterator(link_type x) { node = x; }
		_rb_tree_iterator(const iterator &it) { node = it.node; }

		// 这里可以进行强制转换
		reference operator*() const { return link_type(node)->value_field; }
		pointer operator->() const { return &(operator*()); }

		// 前置自增
		self &operator++() {
			// 第一种情况，有右子节点
			if (node->right) {
				node = node->right;
				while (node->left)
					node = node->left;
			}
			// 第二种情况，没有右子节点
			else {
				// 找出父节点
				auto y = node->parent;
				// 若父节点也为右子节点，则一直上溯，直到不为右子节点为止
				while (y->right == node) {
					node = y;
					y = y->parent;
				}
				// 状况3，是为了配合RB-Tree实现中的header节点
				if (node->right != y)
					node = y;
				// 我觉得这里有问题，应该有（没有，我理解错了，上一步就已经OK了）
				// node = y;
			}
			return *this;
		}

		// 后置自增
		self operator++(int) {
			auto tmp = *this;
			++*this;
			return tmp;
		}

		// 前置自减
		self &operator--() {
			// 情况一，当node为header时，则退回到最大节点，即header->right
			if (node->color == _rb_tree_red &&
				node->parent->parent == node) {
				node = node->parent;
			}
			// 情况二：如果右左子节点
			else if (node->left) {
				node = node->left;
				while (node->right)
					node = node->right;
			}
			// 情况三：没有左子节点
			else {
				// 找出父节点
				auto y = node->parent;
				// 不断上溯，当现行节点不为左子节点时，其父节点即为答案
				while (node == y->left) {
					node = y;
					y = y->parent;
				}
				node = y;
			}
			return *this;
		}

		// 后置自减
		self operator--(int) {
			auto tmp = *this;
			--*this;
			return tmp;
		}
	};


	// RB-Tree的数据结构
	// KeyOfValue是类，用来生成键值
	template <typename Key, typename Value, typename KeyOfValue,
		typename Compare, typename Alloc = alloc>
		class rb_tree {
		protected:
			using base_ptr = _rb_tree_node_base * ;
			using rb_tree_node = _rb_tree_node<Value>;
			// 以节点为单位分配内存
			using rb_tree_node_allocator = gruel::simple_alloc<rb_tree_node, Alloc>;
			using color_type = _rb_tree_color_type;
			using self = rb_tree<Key, Value, KeyOfValue, Compare, Alloc>;

		public:
			using value_type = Value;
			using key_type = Key;
			using pointer = value_type * ;
			using const_pointer = const value_type *;
			using reference = value_type & ;
			using const_reference = const value_type &;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;
			using link_type = rb_tree_node * ;
			using iterator = _rb_tree_iterator<value_type, reference, pointer>;
			using const_iterator = _rb_tree_iterator<value_type, const_reference, const_pointer>;

		protected:
			// 获取一个节点空间
			link_type get_node() { return rb_tree_node_allocator::allocate(); }
			// 释放指定位置内存
			void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }

			// 创建节点
			link_type create_node(const value_type &x) {
				link_type tmp = get_node();
				construct(&tmp->value_field, x);
				return tmp;
			}

			link_type clone_node(link_type x) {
				link_type tmp = create_node(x->value_field);
				tmp->color = x->color;
				tmp->parent = nullptr;
				tmp->left = nullptr;
				tmp->right = nullptr;
				return tmp;
			}

			void destory_node(link_type p) {
				destory(&p->value_field);
				put_node(p);
			}

		protected:
			link_type header;		// 特殊节点，其parent与root互指
			size_type node_count;	// 当前节点数目
			Compare key_compare;	// 注意这里是一个类实例，所以后面不用实例化

			link_type &root() const { return (link_type &)header->parent; }
			link_type &leftmost() const { return (link_type &)header->left; }
			link_type &rightmost() const { return (link_type &)header->right; }

			// 书上说：以下六个函数用来方便取得节点x的成员
			// 总觉得没什么必要
			static link_type &left(link_type x) { return (link_type &)(x->left); }
			static link_type &right(link_type x) { return (link_type &)(x->right); }
			static link_type &parent(link_type x) { return (link_type &)(x->parent); }

			static reference value(link_type x) { return x->value_field; }
			static const Key &key(link_type x) { return KeyOfValue()(value(x)); }
			static color_type &color(link_type x) { return (color_type &)(x->color); }

			// 以下为了重载基类指针
			static link_type &left(base_ptr x) { return (link_type &)(x->left); }
			static link_type &right(base_ptr x) { return (link_type &)(x->right); }
			static link_type &parent(base_ptr x) { return (link_type &)(x->parent); }
			static reference value(base_ptr x) { return ((link_type)x)->value_field; }
			static const Key &key(base_ptr x) { return KeyOfValue()(value((link_type)x)); }
			static color_type &color(base_ptr x) { return (color_type &)(link_type(x)->color); }


			// 以下两个函数已实现
			static link_type minimum(link_type x) { return (link_type)_rb_tree_node_base::minimum(x); }
			static link_type maximum(link_type x) { return (link_type)_rb_tree_node_base::maximum(x); }

		private:
			iterator _insert(base_ptr x, base_ptr y, const value_type &v);
			// 注意这里是删除以x为根节点的子树，不会改变性质，故不需要调整
			// 这里的递归调用值得学习
			void _erase(link_type x) {
				while (x) {
					_erase(right(x));
					link_type y = left(x);
					destory_node(x);
					x = y;
					--node_count;
				}
			}
			void init() {
				header = get_node();
				color(header) = _rb_tree_red;

				root() = nullptr;
				leftmost() = header;
				rightmost() = header;
			}
			// 递归复制子树
			link_type _copy(link_type x, link_type p) {
				if (!x)
					return nullptr;
				link_type tmp = clone_node(x);
				tmp->left = _copy((link_type)(x->left), tmp);
				tmp->right = _copy((link_type)(x->right), tmp);
				tmp->parent = p;
				return tmp;
			}

		public:
			// 注意这里的comp
			rb_tree(const Compare &comp = Compare()) : node_count(0), key_compare(comp) { init(); }
			rb_tree(const self &x) : node_count(x.size()), key_compare(x.key_comp()) {
				header = clone_node(x.header);
				header->parent = _copy(x.root(), header);
				header->left = minimum(root());
				header->right = maximum(root());
			}
			~rb_tree() {
				clear();
				put_node(header);
			}

			self &operator=(const self &x) {
				// 递归复制子树
				link_type tmp = _copy(x.root(), header);
				// 复制成功后删除原树
				clear();
				// 修正header和root节点关系
				root() = tmp;
				header->parent = root();
				header->left = minimum(root());
				header->right = minimum(root());
				// 修正树大小
				node_count = x.size();
				return *this;
			}

			
			Compare key_comp() const { return key_compare; }
			// 注意这里有类型转换
			iterator begin() { return leftmost(); }
			const_iterator begin() const { return leftmost(); }
			iterator end() { return header; }
			const_iterator end() const { return header; }
			bool empty() const { return node_count == 0; }
			size_type size() const { return node_count; }
			size_type max_size() const { return (size_type)(-1); }
			iterator find(const Key &k);
			const_iterator find(const Key &k) const;
			iterator lower_bound(const Key &k);
			const_iterator lower_bound(const Key &k) const;
			iterator upper_bound(const Key &k);
			const_iterator upper_bound(const Key &k) const;
			std::pair<iterator, iterator> equal_range(const Key &k) {
				return { lower_bound(k), upper_bound(k) };
			}
			std::pair<const_iterator, const_iterator> equal_range(const Key &k) const {
				return { lower_bound(k), upper_bound(k) };
			}
			size_type count(const Key &k) const {
				std::pair<const_iterator, const_iterator> p = equal_range(k);
				return (size_type)distance(p.first, p.second);
			}
			void erase(link_type x) { _erase(x); }
			size_type erase(const Key &x) {
				std::pair<const_iterator, const_iterator> p = equal_range(x);
				size_type n = distance(p.first, p.second);
				erase(p.first, p.second);
				return n;
			}
			void erase(iterator position) {
				link_type y = (link_type)_rb_tree_rebalance_for_erase(position.node, header->parent,
					header->left, header->right);
				destory_node(y);
				--node_count;
			}
			void erase(iterator first, iterator last) {
				if (first == begin() && last == end())
					clear();
				else
					while (first != last)
						erase(first++);
			}
			std::pair<iterator, bool> insert_unique(const value_type &x);
			iterator insert_unique(iterator position, const value_type &x);
			template <typename InputIterator>
			void insert_unique(InputIterator first, InputIterator last) {
				for (; first != last; ++first)
					insert_unique(*first);
			}
			iterator insert_equal(const value_type &v);
			iterator insert_equal(iterator position, const value_type &x);
			template <typename InputIterator>
			void insert_equal(InputIterator first, InputIterator last) {
				for (; first != last; ++first)
					insert_equal(*first);
			}
			// 不要用erase(begin(), end())，因为并不需要调整
			void clear() { 
				_erase(root());
			}
	};


	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	inline bool operator==(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &lhs,
			const rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &rhs) {
		if (lhs.size() != rhs.size())
			return false;
		auto first1 = lhs.begin();
		auto last1 = lhs.end();
		auto first2 = rhs.begin();
		for (; first1 != last1 && *first1 == *first2; ++first1, ++first2);
		if (first1 != last1)
			return false;
		return true;
	}


	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	inline bool operator<(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &lhs,
			const rb_tree<Key, Value, KeyOfValue, Compare, Alloc> &rhs) {
		if (lhs.size() != rhs.size())
			return lhs.size() < rhs.size();
		auto first1 = lhs.begin();
		auto last1 = lhs.end();
		auto first2 = rhs.begin();
		while (first1 != last1) {
			if (*first1 < *first2)
				return true;
			if (*first2 < *first1)
				return false;
			++first1, ++first2;
		}
		return false;
	}


	// 插入操作，节点键值不能重复
	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const value_type &v) {

		link_type y = header;
		link_type x = root();
		bool comp = true;

		while (x) {
			y = x;
			// v小于等于x则往左，大于x则往右
			comp = key_compare(KeyOfValue()(v), key(x));
			x = comp ? left(x) : right(x);
		}

		iterator j = iterator(y);
		// 若comp为true，即y值比v大
		if (comp)
			if (j == begin())
				return { _insert(x, y, v), true };
			else
				--j;

		// 这里comp为false，即y值小于等于v，若v值没有j大，则二者相等
		if (key_compare(key(j.node), KeyOfValue()(v)))
			return { _insert(x, y, v), true };

		return { j, false };
	}


	// 插入到指定迭代器位置之前，如果不符合要求，则由其他插入处理
	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(iterator position, const value_type &x) {

		// 情况一：插入位置为begin()
		if (position.node == header->left) {
			// 如果元素个数大于零，即begin()不为空，且x的键值小于begin()的键值，则插入到begin()之前
			if (size() > 0 && key_compare(KeyOfValue()(x), key(position.node)))
				return _insert(position.node, position.node, x);
			// 否则，返回insert_unique(x)后的迭代器
			else
				return insert_unique(x).first;
		}
		// 情况二：插入位置为end()
		else if (position.node == header) {
			// 如果最大节点的键值小于x的键值，则插入到end()之前，最大节点之后，满足插入条件
			if (key_compare(key(header->right), KeyOfValue()(x)))
				return _insert(nullptr, position.node, x);
			else
				return insert_unique(x).first;
		}
		// 情况三：插入位置为除头尾外的任意位置
		else {
			iterator before = position;
			--before;
			// 如果x的键值比position的前驱大并比position小，则满足插入条件
			if (key_compare(key(before.node), KeyOfValue()(x))
				&& key_compare(KeyOfValue()(x), key(position.node))) {
				if (right(before.node) == nullptr)
					return _insert(nullptr, before.node, x);
				else
					return _insert(position.node, position.node, x);
			}
			else
				return insert_unique(x).first;
		}
	}


	// 插入操作，节点键值可以重复
	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const value_type & v) {
		link_type y = header;
		link_type x = root();

		while (x) {
			y = x;
			x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
		}
		return _insert(x, y, v);
	}


	// 插入到指定迭代器位置之前，如果不符合要求，则交由其他插入函数处理
	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(iterator position, const value_type &x) {

		// 情况一：插入位置为begin()
		if (position.node == header->left) {
			// 如果元素个数大于零，即begin()不为空，且x的键值小于begin()的键值，则插入到begin()之前
			if (size() > 0 && key_compare(KeyOfValue()(x), key(position.node)))
				return _insert(position.node, position.node, x);
			// 否则，返回insert_unique(x)后的迭代器
			else
				return insert_equal(x);
		}
		// 情况二：插入位置为end()
		else if (position.node == header) {
			// 如果最大节点的键值小于x的键值，则插入到end()之前，最大节点之后，满足插入条件
			if (key_compare(key(header->right), KeyOfValue()(x)))
				return _insert(nullptr, position.node, x);
			else
				return insert_equal(x);
		}
		// 情况三：插入位置为除头尾外的任意位置
		else {
			iterator before = position;
			--before;
			// 如果x的键值比position的前驱大并比position小，则满足插入条件
			if (key_compare(key(before.node), KeyOfValue()(x))
				&& key_compare(KeyOfValue()(x), key(position.node))) {
				if (right(before.node) == nullptr)
					return _insert(nullptr, before.node, x);
				else
					return _insert(position.node, position.node, x);
			}
			else
				return insert_equal(x);
		}
	}


	// 真正的插入操作，x_为新值插入点，y_为插入点的父节点
	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::_insert(base_ptr x_, base_ptr y_, const value_type &v) {

		link_type x = (link_type)x_;
		link_type y = (link_type)y_;
		link_type z;

		// 当y为header，或者x非空，我觉得也是这个情况
		// 或者v的键值小于y的键值
		if (y == header || x || key_compare(KeyOfValue()(v), key(y))) {
			z = create_node(v);
			left(y) = z;	// 若y为header，则leftmost为z
			// 维护rightmost
			if (y == header) {
				root() = z;
				rightmost() = z;
			}
			else if (y == leftmost())
				leftmost() = z;
		}
		else {
			z = create_node(v);
			right(y) = z;
			if (y == rightmost())
				rightmost() = z;
		}

		parent(z) = y;
		left(z) = nullptr;
		right(z) = nullptr;

		// 进行平衡操作
		// 注意这里不能用root()
		_rb_tree_rebalance(z, header->parent);
		++node_count;
		return iterator(z);
	}


	// 查找操作
	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator 
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key & k) {
		link_type y = header;
		link_type x = root();

		//int i = 0;
		// 如果树不为空，则一直往下走
		while (x) {
			//std::cout << "已下潜" << i++ << "层" << std::endl;
			// x节点键值大于或等于k时往左走
			if (!key_compare(key(x), k)) {
				y = x;
				x = left(x);
			}
			// 否则往右走，可以看到往右走并不会更新y
			else
				x = right(x);
		}

		iterator j = iterator(y);
		// 比较二者是否相等
		return (j == end() || key_compare(k, key(j.node))) ? end() : j;
	}


	// 查找操作
	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key &k) const {
		link_type y = header;
		link_type x = root();

		// 如果树不为空，则一直往下走
		while (x) {
			// x节点键值大于或等于k时往左走
			if (!key_compare(key(x), k)) {
				y = x;
				x = left(x);
			}
			// 否则往右走，可以看到往右走并不会更新y
			else
				x = right(x);
		}

		const_iterator j = const_iterator(y);
		// 比较二者是否相等
		return (j == end() || key_compare(k, key(j.node))) ? end() : j;
	}


	// 查找第一个不小于k的节点
	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::lower_bound(const Key &k) {
		link_type y = header;
		link_type x = root();

		while (x) {
			if (!key_compare(key(x), k)) {
				y = x;
				x = left(x);
			}
			else
				x = right(x);
		}

		return iterator(y);
	}


	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::lower_bound(const Key &k) const {
		link_type y = header;
		link_type x = root();

		while (x) {
			if (!key_compare(key(x), k)) {
				y = x;
				x = left(x);
			}
			else
				x = right(x);
		}

		return const_iterator(y);
	}


	// 查找第一个大于k的节点
	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::upper_bound(const Key &k) {
		link_type y = header;
		link_type x = root();

		while (x) {
			if (!key_compare(k, key(x))) {
				y = x;
				x = left(x);
			}
			else
				x = right(x);
		}

		return iterator(y);
	}


	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::upper_bound(const Key &k) const {
		link_type y = header;
		link_type x = root();

		while (x) {
			if (!key_compare(k, key(x))) {
				y = x;
				x = left(x);
			}
			else
				x = right(x);
		}

		return const_iterator(y);
	}


	// 左旋，自己画图再看代码好理解
	inline void _rb_tree_rotate_left(_rb_tree_node_base *x, _rb_tree_node_base *&root) {
		auto y = x->right;
		x->right = y->left;
		if (y->left)
			y->left->parent = x;
		y->parent = x->parent;
		if (x == root)
			root = y;
		else if (x == x->parent->left)
			x->parent->left = y;
		else
			x->parent->right = y;
		y->left = x;
		x->parent = y;
	}


	// 右旋
	inline void _rb_tree_rotate_right(_rb_tree_node_base *x, _rb_tree_node_base *&root) {
		auto y = x->left;
		x->left = y->right;
		if (y->right)
			y->right->parent = x;
		y->parent = x->parent;

		if (x == root)
			root = y;
		else if (x == x->parent->left)
			x->parent->left = y;
		else
			x->parent->right = y;
		y->right = x;
		x->parent = y;
	}


	// 全局函数，用于调整插入新元素后的树，以满足红黑树的要求
	inline void _rb_tree_rebalance(_rb_tree_node_base *x, _rb_tree_node_base *&root) {
		x->color = _rb_tree_red;
		// 当x不为根节点且x的父节点为红色时，不断调整
		while (x != root && x->parent->color == _rb_tree_red) {
			// 当x的父节点为其祖父节点的左子节点时
			if (x->parent == x->parent->parent->left) {
				// y为伯父节点
				auto y = x->parent->parent->right;
				// 情况一：当y存在且其颜色为红色时，调整颜色
				if (y && y->color == _rb_tree_red) {
					x->parent->color = _rb_tree_black;	// 父节点设为黑色
					y->color = _rb_tree_black;			// 伯父节点设为黑色
					x->parent->parent->color = _rb_tree_red;	// 祖父节点设为红色
					x = x->parent->parent;	// 上溯
				}
				// 当y不存在或其颜色为黑色时
				else {
					// 情况二：当x为父节点的内侧插入时，进行一次单旋转，调整为外侧插入
					if (x == x->parent->right) {
						x = x->parent;	// 注意这里的x变了
						_rb_tree_rotate_left(x, root);	// 左旋
					}
					// 情况三：当x为父节点的外侧插入时
					x->parent->color = _rb_tree_black;	// 父节点设为黑色
					x->parent->parent->color = _rb_tree_red;	// 祖父节点设为红色
					_rb_tree_rotate_right(x->parent->parent, root);	// 右旋
				}
			}
			// 当x的父节点为其祖父节点的右子节点时（与上对称）
			else {
				auto y = x->parent->parent->left;
				if (y && y->color == _rb_tree_red) {
					x->parent->color = _rb_tree_black;	// 父节点设为黑色
					y->color = _rb_tree_black;			// 伯父节点设为黑色
					x->parent->parent->color = _rb_tree_red;	// 祖父节点设为红色
					x = x->parent->parent;	// 上溯
				}
				// 当y不存在或其颜色为黑色时
				else {
					// 当x为父节点的内侧插入时，进行一次单旋转，调整为外侧插入
					if (x == x->parent->left) {
						x = x->parent;	// 注意这里的x变了
						_rb_tree_rotate_right(x, root);	// 右旋
					}
					x->parent->color = _rb_tree_black;	// 父节点设为黑色
					x->parent->parent->color = _rb_tree_red;	// 祖父节点设为红色
					_rb_tree_rotate_left(x->parent->parent, root);	// 左旋
				}
			}
		}
		// x有可能上溯到根节点改变颜色，这里进行调整
		root->color = _rb_tree_black;
	}


	// 全局函数，用于调整删除元素后的树，以满足红黑树要求（暂时没搞定）（搞定），返回需要删除的节点
	inline _rb_tree_node_base *_rb_tree_rebalance_for_erase(_rb_tree_node_base *z,
		_rb_tree_node_base *&root, _rb_tree_node_base *&leftmost, _rb_tree_node_base *&rightmost) {
		_rb_tree_node_base *y = z;  // 经过一系列操作的y是真正要删除的节点
		_rb_tree_node_base *x = nullptr;	// x总是指向真正删除节点的后继节点
		_rb_tree_node_base *x_parent = nullptr;	// 顾名思义，指向x的父节点

		// 注意这一段代码是为了将y指向真正要删除的节点，并没有进行维持红黑树性质的操作
		// 情况一：如果z的左子树为空，那我们只需将其右子节点（可能为nullptr)取代z节点即可
		if (y->left == nullptr)
			x = y->right;
		else {
			// 情况二：当z的右子树为空（此时左子树不为空），同样只需将其左子节点取代z节点即可
			if (y->right == nullptr)
				x = y->left;
			// 情况三：当z的左右子树都不为空时，则需要找出其后继节点（实际上以上情况也是找后继节点）
			else {
				y = y->right;
				while (y->left)
					y = y->left;
				x = y->right;
			}
		}

		// 情况三的实际删除操作，此时y的左子节点一定为空，x则或有或空，且此时y为z的后继节点
		if (y != z) {
			// 操作实际上就是将y取代z的位置，再调整好子树间的关系即可
			// 首先无论如何z的左子树一定要连接到y的左子节点
			z->left->parent = y;
			y->left = z->left;
			// 如果y不是z的右子节点，则z的右子节点的左子节点一定不为空，此时涉及到更深的子树，需要调整
			if (y != z->right) {
				x_parent = y->parent;	// 首先记录y的父节点，因为接下来x要取代y的位置
				if (x)
					x->parent = y->parent;
				// y为其父节点的左子节点
				y->parent->left = x;
				// 调整完x后，此时y的右子节点已无用，则将z的右子树连接到y的右子节点上
				y->right = z->right;
				z->right->parent = y;
			}
			// 如果y是z的右子节点，则z的右子节点的左子树一定为空，直接处理z即可，不需要进行调整
			else
				x_parent = y;	// 更新x_parent
			// 以下让z父节点的对应子节点指向y
			if (root == z)
				root = y;
			else if (z == z->parent->left)
				z->parent->left = y;
			else
				z->parent->right = y;
			y->parent = z->parent;
			// 交换y和z的颜色
			using std::swap;
			swap(y->color, z->color);
			// 此时已调整完毕，z已经无用，将y指向真正要删除的z即可
			y = z;
		}
		// 情况一、二的实际删除操作，注意此时y = z，且其儿子数最多为1
		else {
			// 操作实际上就是将y取代z的位置，再调整好子树间的关系即可
			x_parent = y->parent;	// 记录y的父节点
			if (x)	// 如果x存在，即z的右子树不为空，则将其连接到y的父节点
				x->parent = y->parent;
			if (root == z)	// 如果删除的z为根节点，则调整根节点
				root = x;
			else {	//否则，判断z为其父节点的左子节点还是右子节点
				if (z == z->parent->left)
					z->parent->left = x;
				else
					z->parent->right = x;
			}
			if (z == leftmost)	// 修正leftmost
				if (z->right == nullptr)	// 如果z的右子树也为空，则其父节点必为删除z后的leftmost
					leftmost = z->parent;
				else	// 否则，重新求leftmost，注意这里只需求x子树中的leftmost即可
					leftmost = _rb_tree_node_base::minimum(x);
			if (z == rightmost)	// 修正rightmost，其余同上
				if (z->left == nullptr)
					rightmost = z->parent;
				else
					rightmost = _rb_tree_node_base::maximum(x);
		}


		// 以下操作用于维持红黑树的性质
		// 当删除节点的颜色不为红色时才需要维持红黑树性质
		// 主要破坏的是“任何节点到其所有分枝叶子的简单路径上的黑节点个数相同”这一性质
		if (y->color != _rb_tree_red) {
			// 当删除节点的后继节点不为root且其颜色为黑，不断循环，因为此时违反了红黑树性质，可画图理解
			while (x != root && (x == nullptr || x->color == _rb_tree_black)) {
				if (x == x_parent->left) {
					// w为删除节点的后继节点的兄弟节点，实际上这里的后继节点已经取代删除节点了
					_rb_tree_node_base *w = x_parent->right;
					// 情况一：w的颜色为红色
					// 思路：通过染色旋转，将情况一转为其他情况处理
					// 实际操作后，更新后的w颜色为黑，也就是说这一步操作后下面的情况w必定为黑
					if (w->color == _rb_tree_red) {
						w->color = _rb_tree_black;
						x_parent->color = _rb_tree_red;
						_rb_tree_rotate_left(x_parent, root);
						w = x_parent->right;	// 更新w
					}
					// 情况二：w的左右孩子都不存在或均为黑色
					// 思路：这里有两种情况，如果p是红色，则w必定为黑，将p改为黑色，w改为红色即可，这时两条路径的黑节点数相同
					// 如果p是黑色，则无论如何调整p和w都没用，这时我们将w改为红色，再上溯
					if ((w->left == nullptr || w->left->color == _rb_tree_black)
						&& (w->right == nullptr || w->right->color == _rb_tree_black)) {
						w->color = _rb_tree_red;
						x = x_parent;	// 由循环判断p是否为红，不为则上溯
						x_parent = x_parent->parent;
					}
					else {
						// 情况三：w的右孩子不存在或为黑色（左孩子存在且为红色）
						// 思路：调整为情况四
						// 注意这里旋转的是w节点
						if (w->right == nullptr || w->right->color == _rb_tree_black) {
							if (w->left)	// 将w->left染为黑，因为要取代w的位置，要和w颜色一致
								w->left->color = _rb_tree_black;
							w->color = _rb_tree_red;	// 将w染为红，右旋后得到情况四
							_rb_tree_rotate_right(w, root);
							w = x_parent->right;	// 更新w
						}
						// 情况四：w右孩子存在且为红色，左孩子随意
						// 思路：利用这个红色右孩子，补上左子树少掉的黑色节点
						w->color = x_parent->color;	// 将w颜色更新为父节点颜色
						x_parent->color = _rb_tree_black;	// 父节点颜色设为黑色，左旋后补充左子树黑节点
						if (w->right)	// 将w->right染为黑色，以补充旋转后缺失的w节点
							w->right->color = _rb_tree_black;
						_rb_tree_rotate_left(x_parent, root);
						break;
					}
				}
				// 以下为对称情况
				else {
					_rb_tree_node_base *w = x_parent->left;
					if (w->color == _rb_tree_red) {
						w->color = _rb_tree_black;
						x_parent->color = _rb_tree_red;
						_rb_tree_rotate_right(x_parent, root);
						w = x_parent->left;	// 更新w
					}
					if ((w->left == nullptr || w->left->color == _rb_tree_black)
						&& (w->right == nullptr || w->right->color == _rb_tree_black)) {
						w->color = _rb_tree_red;
						x = x_parent;
						x_parent = x_parent->parent;
					}
					else {
						if (w->left == nullptr || w->left->color == _rb_tree_black) {
							if (w->right)
								w->right->color = _rb_tree_black;
							w->color = _rb_tree_red;
							_rb_tree_rotate_left(w, root);
							w = x_parent->left;
						}
						w->color = x_parent->color;
						x_parent->color = _rb_tree_black;
						if (w->left)
							w->left->color = _rb_tree_black;
						_rb_tree_rotate_right(x_parent, root);
						break;
					}
				}
			}
			// 如果x为根节点或为红色，则只需重新染色即可
			if (x)
				x->color = _rb_tree_black;
		}

		return y;
	}

}
