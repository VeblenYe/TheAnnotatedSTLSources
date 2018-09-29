#pragma once


/* д��һ�ܣ�������ʵ��������д���� */


#include "gruel_memory.h"


namespace gruel {

	// �����Ķ�������Ϊ��Ϊ���Ժ���޸ķ��㣬�и���ĵ���
	using _rb_tree_color_type = bool;
	const _rb_tree_color_type _rb_tree_red = false;
	const _rb_tree_color_type _rb_tree_black = true;


	// RB-Tree�Ľڵ���࣬û�а���������
	struct _rb_tree_node_base {
		// ��������
		using color_type = _rb_tree_color_type;
		// ָ������͵�ָ��
		using base_ptr = _rb_tree_node_base * ;

		color_type color;	// �ڵ���ɫ
		base_ptr parent;	// ���ڵ�ָ��
		base_ptr left;		// ����ָ��
		base_ptr right;		// �Һ���ָ��

		// ����xΪ����������С�ڵ�
		static base_ptr minimum(base_ptr x) {
			while (x->left)
				x = x->left;
			return x;
		}

		// ����xΪ�����������ڵ�
		static base_ptr maximum(base_ptr x) {
			while (x->right)
				x = x->right;
			return x;
		}
	};


	// RB-Tree�Ľڵ�ģ���࣬����������
	template <typename T>
	struct _rb_tree_node : public _rb_tree_node_base {
		using link_type = _rb_tree_node<T> *;
		T value_field;
	};


	//RB-Tree�ĵ��������࣬��ָ���Ϊ�ڵ����
	struct _rb_tree_iterator_base {
		using base_ptr = typename _rb_tree_node_base::base_ptr;
		// �õ���������Ϊ˫�������
		using iterator_category = bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t;

		base_ptr node;	// ָ��ڵ���࣬���ϵĻ�˵������������֮�����һ�������ϵ
	};


	inline bool operator==(const _rb_tree_iterator_base &lhs,
		const _rb_tree_iterator_base &rhs) {
		return lhs.node == rhs.node;
	}


	inline bool operator!=(const _rb_tree_iterator_base &lhs,
		const _rb_tree_iterator_base &rhs) {
		return lhs.node != rhs.node;
	}


	//RB-Tree�ĵ�����ģ���࣬��ָ���Ϊ�ڵ�ģ����
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
		// ע��������������󸳸�����ָ��
		_rb_tree_iterator(link_type x) { node = x; }
		_rb_tree_iterator(const iterator &it) { node = it.node; }

		// ������Խ���ǿ��ת��
		reference operator*() const { return link_type(node)->value_field; }
		pointer operator->() const { return &(operator*()); }

		// ǰ������
		self &operator++() {
			// ��һ������������ӽڵ�
			if (node->right) {
				node = node->right;
				while (node->left)
					node = node->left;
			}
			// �ڶ��������û�����ӽڵ�
			else {
				// �ҳ����ڵ�
				auto y = node->parent;
				// �����ڵ�ҲΪ���ӽڵ㣬��һֱ���ݣ�ֱ����Ϊ���ӽڵ�Ϊֹ
				while (y->right == node) {
					node = y;
					y = y->parent;
				}
				// ״��3����Ϊ�����RB-Treeʵ���е�header�ڵ�
				if (node->right != y)
					node = y;
				// �Ҿ������������⣬Ӧ���У�û�У��������ˣ���һ�����Ѿ�OK�ˣ�
				// node = y;
			}
			return *this;
		}

		// ��������
		self operator++(int) {
			auto tmp = *this;
			++*this;
			return tmp;
		}

		// ǰ���Լ�
		self &operator--() {
			// ���һ����nodeΪheaderʱ�����˻ص����ڵ㣬��header->right
			if (node->color == _rb_tree_red &&
				node->parent->parent == node) {
				node = node->parent;
			}
			// ���������������ӽڵ�
			else if (node->left) {
				node = node->left;
				while (node->right)
					node = node->right;
			}
			// �������û�����ӽڵ�
			else {
				// �ҳ����ڵ�
				auto y = node->parent;
				// �������ݣ������нڵ㲻Ϊ���ӽڵ�ʱ���丸�ڵ㼴Ϊ��
				while (node == y->left) {
					node = y;
					y = y->parent;
				}
				node = y;
			}
			return *this;
		}

		// �����Լ�
		self operator--(int) {
			auto tmp = *this;
			--*this;
			return tmp;
		}
	};


	// RB-Tree�����ݽṹ
	// KeyOfValue���࣬�������ɼ�ֵ
	template <typename Key, typename Value, typename KeyOfValue,
		typename Compare, typename Alloc = alloc>
		class rb_tree {
		protected:
			using base_ptr = _rb_tree_node_base * ;
			using rb_tree_node = _rb_tree_node<Value>;
			// �Խڵ�Ϊ��λ�����ڴ�
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
			// ��ȡһ���ڵ�ռ�
			link_type get_node() { return rb_tree_node_allocator::allocate(); }
			// �ͷ�ָ��λ���ڴ�
			void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }

			// �����ڵ�
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
			link_type header;		// ����ڵ㣬��parent��root��ָ
			size_type node_count;	// ��ǰ�ڵ���Ŀ
			Compare key_compare;	// ע��������һ����ʵ�������Ժ��治��ʵ����

			link_type &root() const { return (link_type &)header->parent; }
			link_type &leftmost() const { return (link_type &)header->left; }
			link_type &rightmost() const { return (link_type &)header->right; }

			// ����˵����������������������ȡ�ýڵ�x�ĳ�Ա
			// �ܾ���ûʲô��Ҫ
			static link_type &left(link_type x) { return (link_type &)(x->left); }
			static link_type &right(link_type x) { return (link_type &)(x->right); }
			static link_type &parent(link_type x) { return (link_type &)(x->parent); }

			static reference value(link_type x) { return x->value_field; }
			static const Key &key(link_type x) { return KeyOfValue()(value(x)); }
			static color_type &color(link_type x) { return (color_type &)(x->color); }

			// ����Ϊ�����ػ���ָ��
			static link_type &left(base_ptr x) { return (link_type &)(x->left); }
			static link_type &right(base_ptr x) { return (link_type &)(x->right); }
			static link_type &parent(base_ptr x) { return (link_type &)(x->parent); }
			static reference value(base_ptr x) { return ((link_type)x)->value_field; }
			static const Key &key(base_ptr x) { return KeyOfValue()(value((link_type)x)); }
			static color_type &color(base_ptr x) { return (color_type &)(link_type(x)->color); }


			// ��������������ʵ��
			static link_type minimum(link_type x) { return (link_type)_rb_tree_node_base::minimum(x); }
			static link_type maximum(link_type x) { return (link_type)_rb_tree_node_base::maximum(x); }

		private:
			iterator _insert(base_ptr x, base_ptr y, const value_type &v);
			// ע��������ɾ����xΪ���ڵ������������ı����ʣ��ʲ���Ҫ����
			// ����ĵݹ����ֵ��ѧϰ
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
			// �ݹ鸴������
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
			// ע�������comp
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
				// �ݹ鸴������
				link_type tmp = _copy(x.root(), header);
				// ���Ƴɹ���ɾ��ԭ��
				clear();
				// ����header��root�ڵ��ϵ
				root() = tmp;
				header->parent = root();
				header->left = minimum(root());
				header->right = minimum(root());
				// ��������С
				node_count = x.size();
				return *this;
			}

			
			Compare key_comp() const { return key_compare; }
			// ע������������ת��
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
			// ��Ҫ��erase(begin(), end())����Ϊ������Ҫ����
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


	// ����������ڵ��ֵ�����ظ�
	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const value_type &v) {

		link_type y = header;
		link_type x = root();
		bool comp = true;

		while (x) {
			y = x;
			// vС�ڵ���x�����󣬴���x������
			comp = key_compare(KeyOfValue()(v), key(x));
			x = comp ? left(x) : right(x);
		}

		iterator j = iterator(y);
		// ��compΪtrue����yֵ��v��
		if (comp)
			if (j == begin())
				return { _insert(x, y, v), true };
			else
				--j;

		// ����compΪfalse����yֵС�ڵ���v����vֵû��j����������
		if (key_compare(key(j.node), KeyOfValue()(v)))
			return { _insert(x, y, v), true };

		return { j, false };
	}


	// ���뵽ָ��������λ��֮ǰ�����������Ҫ�������������봦��
	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(iterator position, const value_type &x) {

		// ���һ������λ��Ϊbegin()
		if (position.node == header->left) {
			// ���Ԫ�ظ��������㣬��begin()��Ϊ�գ���x�ļ�ֵС��begin()�ļ�ֵ������뵽begin()֮ǰ
			if (size() > 0 && key_compare(KeyOfValue()(x), key(position.node)))
				return _insert(position.node, position.node, x);
			// ���򣬷���insert_unique(x)��ĵ�����
			else
				return insert_unique(x).first;
		}
		// �����������λ��Ϊend()
		else if (position.node == header) {
			// ������ڵ�ļ�ֵС��x�ļ�ֵ������뵽end()֮ǰ�����ڵ�֮�������������
			if (key_compare(key(header->right), KeyOfValue()(x)))
				return _insert(nullptr, position.node, x);
			else
				return insert_unique(x).first;
		}
		// �����������λ��Ϊ��ͷβ�������λ��
		else {
			iterator before = position;
			--before;
			// ���x�ļ�ֵ��position��ǰ���󲢱�positionС���������������
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


	// ����������ڵ��ֵ�����ظ�
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


	// ���뵽ָ��������λ��֮ǰ�����������Ҫ�������������뺯������
	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(iterator position, const value_type &x) {

		// ���һ������λ��Ϊbegin()
		if (position.node == header->left) {
			// ���Ԫ�ظ��������㣬��begin()��Ϊ�գ���x�ļ�ֵС��begin()�ļ�ֵ������뵽begin()֮ǰ
			if (size() > 0 && key_compare(KeyOfValue()(x), key(position.node)))
				return _insert(position.node, position.node, x);
			// ���򣬷���insert_unique(x)��ĵ�����
			else
				return insert_equal(x);
		}
		// �����������λ��Ϊend()
		else if (position.node == header) {
			// ������ڵ�ļ�ֵС��x�ļ�ֵ������뵽end()֮ǰ�����ڵ�֮�������������
			if (key_compare(key(header->right), KeyOfValue()(x)))
				return _insert(nullptr, position.node, x);
			else
				return insert_equal(x);
		}
		// �����������λ��Ϊ��ͷβ�������λ��
		else {
			iterator before = position;
			--before;
			// ���x�ļ�ֵ��position��ǰ���󲢱�positionС���������������
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


	// �����Ĳ��������x_Ϊ��ֵ����㣬y_Ϊ�����ĸ��ڵ�
	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::_insert(base_ptr x_, base_ptr y_, const value_type &v) {

		link_type x = (link_type)x_;
		link_type y = (link_type)y_;
		link_type z;

		// ��yΪheader������x�ǿգ��Ҿ���Ҳ��������
		// ����v�ļ�ֵС��y�ļ�ֵ
		if (y == header || x || key_compare(KeyOfValue()(v), key(y))) {
			z = create_node(v);
			left(y) = z;	// ��yΪheader����leftmostΪz
			// ά��rightmost
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

		// ����ƽ�����
		// ע�����ﲻ����root()
		_rb_tree_rebalance(z, header->parent);
		++node_count;
		return iterator(z);
	}


	// ���Ҳ���
	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator 
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key & k) {
		link_type y = header;
		link_type x = root();

		//int i = 0;
		// �������Ϊ�գ���һֱ������
		while (x) {
			//std::cout << "����Ǳ" << i++ << "��" << std::endl;
			// x�ڵ��ֵ���ڻ����kʱ������
			if (!key_compare(key(x), k)) {
				y = x;
				x = left(x);
			}
			// ���������ߣ����Կ��������߲��������y
			else
				x = right(x);
		}

		iterator j = iterator(y);
		// �Ƚ϶����Ƿ����
		return (j == end() || key_compare(k, key(j.node))) ? end() : j;
	}


	// ���Ҳ���
	template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key &k) const {
		link_type y = header;
		link_type x = root();

		// �������Ϊ�գ���һֱ������
		while (x) {
			// x�ڵ��ֵ���ڻ����kʱ������
			if (!key_compare(key(x), k)) {
				y = x;
				x = left(x);
			}
			// ���������ߣ����Կ��������߲��������y
			else
				x = right(x);
		}

		const_iterator j = const_iterator(y);
		// �Ƚ϶����Ƿ����
		return (j == end() || key_compare(k, key(j.node))) ? end() : j;
	}


	// ���ҵ�һ����С��k�Ľڵ�
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


	// ���ҵ�һ������k�Ľڵ�
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


	// �������Լ���ͼ�ٿ���������
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


	// ����
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


	// ȫ�ֺ��������ڵ���������Ԫ�غ������������������Ҫ��
	inline void _rb_tree_rebalance(_rb_tree_node_base *x, _rb_tree_node_base *&root) {
		x->color = _rb_tree_red;
		// ��x��Ϊ���ڵ���x�ĸ��ڵ�Ϊ��ɫʱ�����ϵ���
		while (x != root && x->parent->color == _rb_tree_red) {
			// ��x�ĸ��ڵ�Ϊ���游�ڵ�����ӽڵ�ʱ
			if (x->parent == x->parent->parent->left) {
				// yΪ�����ڵ�
				auto y = x->parent->parent->right;
				// ���һ����y����������ɫΪ��ɫʱ��������ɫ
				if (y && y->color == _rb_tree_red) {
					x->parent->color = _rb_tree_black;	// ���ڵ���Ϊ��ɫ
					y->color = _rb_tree_black;			// �����ڵ���Ϊ��ɫ
					x->parent->parent->color = _rb_tree_red;	// �游�ڵ���Ϊ��ɫ
					x = x->parent->parent;	// ����
				}
				// ��y�����ڻ�����ɫΪ��ɫʱ
				else {
					// ���������xΪ���ڵ���ڲ����ʱ������һ�ε���ת������Ϊ������
					if (x == x->parent->right) {
						x = x->parent;	// ע�������x����
						_rb_tree_rotate_left(x, root);	// ����
					}
					// ���������xΪ���ڵ��������ʱ
					x->parent->color = _rb_tree_black;	// ���ڵ���Ϊ��ɫ
					x->parent->parent->color = _rb_tree_red;	// �游�ڵ���Ϊ��ɫ
					_rb_tree_rotate_right(x->parent->parent, root);	// ����
				}
			}
			// ��x�ĸ��ڵ�Ϊ���游�ڵ�����ӽڵ�ʱ�����϶Գƣ�
			else {
				auto y = x->parent->parent->left;
				if (y && y->color == _rb_tree_red) {
					x->parent->color = _rb_tree_black;	// ���ڵ���Ϊ��ɫ
					y->color = _rb_tree_black;			// �����ڵ���Ϊ��ɫ
					x->parent->parent->color = _rb_tree_red;	// �游�ڵ���Ϊ��ɫ
					x = x->parent->parent;	// ����
				}
				// ��y�����ڻ�����ɫΪ��ɫʱ
				else {
					// ��xΪ���ڵ���ڲ����ʱ������һ�ε���ת������Ϊ������
					if (x == x->parent->left) {
						x = x->parent;	// ע�������x����
						_rb_tree_rotate_right(x, root);	// ����
					}
					x->parent->color = _rb_tree_black;	// ���ڵ���Ϊ��ɫ
					x->parent->parent->color = _rb_tree_red;	// �游�ڵ���Ϊ��ɫ
					_rb_tree_rotate_left(x->parent->parent, root);	// ����
				}
			}
		}
		// x�п������ݵ����ڵ�ı���ɫ��������е���
		root->color = _rb_tree_black;
	}


	// ȫ�ֺ��������ڵ���ɾ��Ԫ�غ����������������Ҫ����ʱû�㶨�����㶨����������Ҫɾ���Ľڵ�
	inline _rb_tree_node_base *_rb_tree_rebalance_for_erase(_rb_tree_node_base *z,
		_rb_tree_node_base *&root, _rb_tree_node_base *&leftmost, _rb_tree_node_base *&rightmost) {
		_rb_tree_node_base *y = z;  // ����һϵ�в�����y������Ҫɾ���Ľڵ�
		_rb_tree_node_base *x = nullptr;	// x����ָ������ɾ���ڵ�ĺ�̽ڵ�
		_rb_tree_node_base *x_parent = nullptr;	// ����˼�壬ָ��x�ĸ��ڵ�

		// ע����һ�δ�����Ϊ�˽�yָ������Ҫɾ���Ľڵ㣬��û�н���ά�ֺ�������ʵĲ���
		// ���һ�����z��������Ϊ�գ�������ֻ�轫�����ӽڵ㣨����Ϊnullptr)ȡ��z�ڵ㼴��
		if (y->left == nullptr)
			x = y->right;
		else {
			// ���������z��������Ϊ�գ���ʱ��������Ϊ�գ���ͬ��ֻ�轫�����ӽڵ�ȡ��z�ڵ㼴��
			if (y->right == nullptr)
				x = y->left;
			// ���������z��������������Ϊ��ʱ������Ҫ�ҳ����̽ڵ㣨ʵ�����������Ҳ���Һ�̽ڵ㣩
			else {
				y = y->right;
				while (y->left)
					y = y->left;
				x = y->right;
			}
		}

		// �������ʵ��ɾ����������ʱy�����ӽڵ�һ��Ϊ�գ�x����л�գ��Ҵ�ʱyΪz�ĺ�̽ڵ�
		if (y != z) {
			// ����ʵ���Ͼ��ǽ�yȡ��z��λ�ã��ٵ�����������Ĺ�ϵ����
			// �����������z��������һ��Ҫ���ӵ�y�����ӽڵ�
			z->left->parent = y;
			y->left = z->left;
			// ���y����z�����ӽڵ㣬��z�����ӽڵ�����ӽڵ�һ����Ϊ�գ���ʱ�漰���������������Ҫ����
			if (y != z->right) {
				x_parent = y->parent;	// ���ȼ�¼y�ĸ��ڵ㣬��Ϊ������xҪȡ��y��λ��
				if (x)
					x->parent = y->parent;
				// yΪ�丸�ڵ�����ӽڵ�
				y->parent->left = x;
				// ������x�󣬴�ʱy�����ӽڵ������ã���z�����������ӵ�y�����ӽڵ���
				y->right = z->right;
				z->right->parent = y;
			}
			// ���y��z�����ӽڵ㣬��z�����ӽڵ��������һ��Ϊ�գ�ֱ�Ӵ���z���ɣ�����Ҫ���е���
			else
				x_parent = y;	// ����x_parent
			// ������z���ڵ�Ķ�Ӧ�ӽڵ�ָ��y
			if (root == z)
				root = y;
			else if (z == z->parent->left)
				z->parent->left = y;
			else
				z->parent->right = y;
			y->parent = z->parent;
			// ����y��z����ɫ
			using std::swap;
			swap(y->color, z->color);
			// ��ʱ�ѵ�����ϣ�z�Ѿ����ã���yָ������Ҫɾ����z����
			y = z;
		}
		// ���һ������ʵ��ɾ��������ע���ʱy = z��������������Ϊ1
		else {
			// ����ʵ���Ͼ��ǽ�yȡ��z��λ�ã��ٵ�����������Ĺ�ϵ����
			x_parent = y->parent;	// ��¼y�ĸ��ڵ�
			if (x)	// ���x���ڣ���z����������Ϊ�գ��������ӵ�y�ĸ��ڵ�
				x->parent = y->parent;
			if (root == z)	// ���ɾ����zΪ���ڵ㣬��������ڵ�
				root = x;
			else {	//�����ж�zΪ�丸�ڵ�����ӽڵ㻹�����ӽڵ�
				if (z == z->parent->left)
					z->parent->left = x;
				else
					z->parent->right = x;
			}
			if (z == leftmost)	// ����leftmost
				if (z->right == nullptr)	// ���z��������ҲΪ�գ����丸�ڵ��Ϊɾ��z���leftmost
					leftmost = z->parent;
				else	// ����������leftmost��ע������ֻ����x�����е�leftmost����
					leftmost = _rb_tree_node_base::minimum(x);
			if (z == rightmost)	// ����rightmost������ͬ��
				if (z->left == nullptr)
					rightmost = z->parent;
				else
					rightmost = _rb_tree_node_base::maximum(x);
		}


		// ���²�������ά�ֺ����������
		// ��ɾ���ڵ����ɫ��Ϊ��ɫʱ����Ҫά�ֺ��������
		// ��Ҫ�ƻ����ǡ��κνڵ㵽�����з�֦Ҷ�ӵļ�·���ϵĺڽڵ������ͬ����һ����
		if (y->color != _rb_tree_red) {
			// ��ɾ���ڵ�ĺ�̽ڵ㲻Ϊroot������ɫΪ�ڣ�����ѭ������Ϊ��ʱΥ���˺�������ʣ��ɻ�ͼ���
			while (x != root && (x == nullptr || x->color == _rb_tree_black)) {
				if (x == x_parent->left) {
					// wΪɾ���ڵ�ĺ�̽ڵ���ֵܽڵ㣬ʵ��������ĺ�̽ڵ��Ѿ�ȡ��ɾ���ڵ���
					_rb_tree_node_base *w = x_parent->right;
					// ���һ��w����ɫΪ��ɫ
					// ˼·��ͨ��Ⱦɫ��ת�������һתΪ�����������
					// ʵ�ʲ����󣬸��º��w��ɫΪ�ڣ�Ҳ����˵��һ����������������w�ض�Ϊ��
					if (w->color == _rb_tree_red) {
						w->color = _rb_tree_black;
						x_parent->color = _rb_tree_red;
						_rb_tree_rotate_left(x_parent, root);
						w = x_parent->right;	// ����w
					}
					// �������w�����Һ��Ӷ������ڻ��Ϊ��ɫ
					// ˼·��������������������p�Ǻ�ɫ����w�ض�Ϊ�ڣ���p��Ϊ��ɫ��w��Ϊ��ɫ���ɣ���ʱ����·���ĺڽڵ�����ͬ
					// ���p�Ǻ�ɫ����������ε���p��w��û�ã���ʱ���ǽ�w��Ϊ��ɫ��������
					if ((w->left == nullptr || w->left->color == _rb_tree_black)
						&& (w->right == nullptr || w->right->color == _rb_tree_black)) {
						w->color = _rb_tree_red;
						x = x_parent;	// ��ѭ���ж�p�Ƿ�Ϊ�죬��Ϊ������
						x_parent = x_parent->parent;
					}
					else {
						// �������w���Һ��Ӳ����ڻ�Ϊ��ɫ�����Ӵ�����Ϊ��ɫ��
						// ˼·������Ϊ�����
						// ע��������ת����w�ڵ�
						if (w->right == nullptr || w->right->color == _rb_tree_black) {
							if (w->left)	// ��w->leftȾΪ�ڣ���ΪҪȡ��w��λ�ã�Ҫ��w��ɫһ��
								w->left->color = _rb_tree_black;
							w->color = _rb_tree_red;	// ��wȾΪ�죬������õ������
							_rb_tree_rotate_right(w, root);
							w = x_parent->right;	// ����w
						}
						// ����ģ�w�Һ��Ӵ�����Ϊ��ɫ����������
						// ˼·�����������ɫ�Һ��ӣ������������ٵ��ĺ�ɫ�ڵ�
						w->color = x_parent->color;	// ��w��ɫ����Ϊ���ڵ���ɫ
						x_parent->color = _rb_tree_black;	// ���ڵ���ɫ��Ϊ��ɫ�������󲹳��������ڽڵ�
						if (w->right)	// ��w->rightȾΪ��ɫ���Բ�����ת��ȱʧ��w�ڵ�
							w->right->color = _rb_tree_black;
						_rb_tree_rotate_left(x_parent, root);
						break;
					}
				}
				// ����Ϊ�Գ����
				else {
					_rb_tree_node_base *w = x_parent->left;
					if (w->color == _rb_tree_red) {
						w->color = _rb_tree_black;
						x_parent->color = _rb_tree_red;
						_rb_tree_rotate_right(x_parent, root);
						w = x_parent->left;	// ����w
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
			// ���xΪ���ڵ��Ϊ��ɫ����ֻ������Ⱦɫ����
			if (x)
				x->color = _rb_tree_black;
		}

		return y;
	}

}
