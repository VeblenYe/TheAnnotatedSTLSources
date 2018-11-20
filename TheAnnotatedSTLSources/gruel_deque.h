#pragma once


#include "gruel_memory.h"


namespace gruel {

	/* ������deque��˫�˶��У�Ҳ������ʽ����������vector������������deque�����ڳ���ʱ����
	����ͷ�˽��в���ɾ����������û�������ĸ�����������������ġ�����֮�⣬deque����Ϊ
	�������ݽṹ�ĵײ������� */


	// ����������������С
	// ���Ǽ���ÿ����������Ԫ�ظ�����szΪÿ��Ԫ����ռ�ֽڴ�С
	std::size_t _deque_buf_size(std::size_t n, std::size_t sz) {
		return n != 0 ? n : (sz < 512 ? std::size_t(512 / sz) : std::size_t(1));
	}


	// deque�ĵ�������BufSizΪָ��������Ԫ�ظ�����0ΪĬ��
	template <typename T, typename Ptr, typename Ref, std::size_t BufSiz = 0>
	struct _deque_iterator {

		// ����ʵ��̫���Ϊɶ��������ҲҪ�������������
		using iteartor = _deque_iterator<T, T *, T &, BufSiz>;
		// ѧ���ˣ�����������Ҫ�������
		using const_iterator = _deque_iterator<T, const T *, const T &, BufSiz>;

		// Ҳ����һ�����������м���Ԫ��
		static std::size_t buffer_size() {
			return _deque_buf_size(BufSiz, sizeof(T));
		}

		// ��������Ҫ�Ķ���
		// deque�ṩ������ʵ�����
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using pointer = Ptr;
		using reference = Ref;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		// ����ָ��ܿ����ĵ�����
		using map_pointer = T * *;

		// self�����趨�Ϳ�����
		using self = _deque_iterator;

		// ����������������
		T *cur;				// ��ǰָ���Ԫ��λ��
		T *first;			// ��ǰ��������ͷ
		T *last;			// ��ǰ��������β
		map_pointer node;	// ָ��ܿ�����

		// �ú������������»�����
		void set_node(map_pointer new_node) {
			node = new_node;
			first = *new_node;
			last = first + difference_type(buffer_size());
		}

		// ���������
		reference operator*() const { return *cur; }
		pointer operator->() const { return &(operator*()); }

		// ��������������֮��ľ��룬����ͼʾ���������
		// �ȼ�����༸�����������ڼ���Ԫ���ڸ��Ի������ľ���
		difference_type operator-(const self &x) const {
			return difference_type(buffer_size() * (node - x.node - 1)) +
				(cur - first) + (x.last - x.cur);
		}


		// ע��ǰ��ʽ���ص�������
		self &operator++() {
			++cur;
			if (cur == last) {
				set_node(node + 1);
				cur = first;
			}
			return *this;
		}

		self operator++(int) {
			self tmp = *this;
			++*this;
			return tmp;
		}

		self &operator--() {
			--cur;
			if (cur == first) {
				set_node(node - 1);
				cur = last;
			}
			return *this;
		}

		self operator--(int) {
			self tmp = *this;
			--*this;
			return tmp;
		}

		// ʵ�ֵ����������ȡ
		self &operator+=(difference_type n) {
			// ƫ����
			difference_type offset = n + cur - first;
			if (offset > 0 && offset < difference_type(buffer_size()))
				// Ŀ��λ����һ����������
				cur += n;
			else {
				// ����һ���������ڣ���Ҫ������������
				// ע����������ʵ��������ȿ�����ǰҲ��������
				difference_type node_offset =
					offset > 0 ? offset / difference_type(buffer_size())
					: -difference_type((-offset - 1) / buffer_size()) - 1;
				// ע�������Ѿ������˻�����
				set_node(node + node_offset);
				cur = first + (offset - node_offset * difference_type(buffer_size()));
			}
			return *this;
		}

		self operator+(difference_type n) const {
			self tmp = *this;
			return tmp += n;
		}

		self &operator-=(difference_type n) { return *this += -n; }

		self operator-(difference_type n) const {
			self tmp = *this;
			return tmp -= n;
		}

		// ������±��������ͨ��������+��ɵ�
		reference operator[](difference_type n) const { return *(*this + n); }
		bool operator==(const self &x) const { return cur == x.cur; }
		bool operator!=(const self &x) const { return !(*this == x); }
		bool operator<(const self &x) const {
			return (node == x.node) ? (cur < x.cur) : (node < x.node);
		}

	};


	// ˫�˶���
	template <typename T, typename Alloc = alloc, std::size_t BufSiz = 0>
	class deque {
	public:

		// ��Ƕ����
		using value_type = T;
		using pointer = T * ;
		using reference = T & ;
		using const_reference = const T &;
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;

		using iterator = _deque_iterator<T, T *, T &, BufSiz>;
		using const_iterator = _deque_iterator<T, const T *, const T &, BufSiz>;


		// ���캯��
		deque() : start(), finish(), map(nullptr), map_size(0) { create_map_and_nodes(0); }
		deque(int n, const value_type &value) : start(), finish(), map(nullptr), map_size(0) {
			fill_initialize(n, value);
		}


		iterator begin() { return start; }
		const_iterator begin() const { return start; }
		iterator end() { return finish; }
		const_iterator end() const { return finish; }

		reference operator[](size_type n) {
			return start[difference_type(n)];
		}

		reference front() { return *start; }
		reference back() { return *(finish - 1); }

		size_type size() const { return finish - start; }
		size_type max_size() const { return size_type(-1); }
		bool empty() const { return start == finish; }


		void push_back(const value_type &x) {
			if (finish.cur != finish.last - 1) {
				// �����������������ϱ��ÿռ�ʱ
				construct(finish.cur, x);
				++finish.cur;
			}
			else
				push_back_aux(x);
		}


		void push_front(const value_type &x) {
			if (start.cur != start.first) {
				// ע�����˲��벻ͬ
				construct(start.cur, x);
				--start.cur;
			}
			else
				push_front_aux(x);
		}


		void pop_back() {
			if (finish.cur != finish.first) {
				--finish.cur;
				destory(finish.cur);
			}
			else
				pop_back_aux();
		}

		void pop_front() {
			if (start.cur != start.last - 1) {
				destory(start.cur);
				++start.cur;
			}
			else
				pop_front_aux();
		}


		void clear();


		iterator erase(iterator pos) {
			// Ŀ��λ�õĺ�һλ
			auto next = pos;
			++next;
			// posǰ�ж���Ԫ��
			auto index = pos - start;
			if (index < (size() >> 1)) {
				// ���posǰ��Ԫ��������Ԫ��һ�룬��posǰԪ�غ��ƣ���ɾ��ͷԪ��
				std::copy_backward(start, pos, next);
				pop_front();
			}
			else {
				// ���򣬽�pos���Ԫ��ǰ�ƣ���ɾ��βԪ��
				std::copy(next, finish, pos);
				pop_back();
			}
			// ������Ӧλ�õ�����
			return start + index;
		}


		iterator erase(iterator first, iterator last);


		iterator insert(iterator pos, const value_type &x) {
			// ��ͷ����
			if (pos.cur == start.cur) {
				push_front(x);
				return start;
			}
			// ��β����
			else if (pos.cur == finish.cur) {
				push_back(x);
				// finish���ɸı�
				auto tmp = finish;
				--tmp;
				return tmp;
			}
			else
				return insert_aux(pos, x);
		}

	protected:

		static std::size_t buffer_size() {
			return _deque_buf_size(BufSiz, sizeof(T));
		}

		// Ԫ��ָ���ָ��
		using map_pointer = pointer * ;

		// mapר����������ÿ������һ��ָ���С��������T **
		using map_allocator = gruel::simple_alloc<pointer, Alloc>;

		// ����ר����������ÿ������һ��Ԫ�ش�С������T *
		using data_allocator = gruel::simple_alloc<value_type, Alloc>;

		// ͷ������
		iterator start;
		// β������
		iterator finish;

		// �ܿ�����
		map_pointer map;

		size_type map_size;

		// ���ýڵ�
		pointer allocate_node() {
			return data_allocator::allocate(buffer_size());
		}

		// �ͷŽڵ�
		void deallocate_node(pointer p) {
			data_allocator::deallocate(p, buffer_size());
		}

		// ��������map
		void reallocate_map(size_type nodes_to_add, bool add_at_front);

		// ��β�����node
		void reserve_map_at_back(size_type nodes_to_add = 1) {
			// ���map��β��node�Ŀ��ø���С��1
			if (nodes_to_add > map_size - (finish.node - map + 1))
				reallocate_map(nodes_to_add, false);
		}

		// ��ͷ�����node
		void reserve_map_at_front(size_type nodes_to_add = 1) {
			// ���map��ͷ��node�Ŀ��ø���С��1
			if (nodes_to_add > start.node - map)
				reallocate_map(nodes_to_add, true);
		}

		// ����num_elements���ú��ʵ�map��node
		void create_map_and_nodes(size_type num_elements);

		// ���n��value
		void fill_initialize(size_type n, const value_type &value) {
			create_map_and_nodes(n);
			map_pointer cur;
			// ����ʡ���쳣����
			// [start.node, finish.node)�г���Ԫ��
			for (cur = start.node; cur < finish.node; ++cur)
				gruel::uninitialized_fill(*cur, *cur + buffer_size(), value);
			// ��ʣ���Ԫ�����
			gruel::uninitialized_fill(finish.first, finish.cur, value);
		}


		void push_back_aux(const value_type &x) {
			auto x_copy = x;
			reserve_map_at_back();
			// ������mapȴ��δ���ýڵ�
			*(finish.node + 1) = allocate_node();
			// ����ʡ���쳣����
			construct(finish.cur, x_copy);
			// ���µ�����
			finish.set_node(finish.node + 1);
			finish.cur = finish.first;
		}

		
		void push_front_aux(const value_type &x) {
			auto x_copy = x;
			reserve_map_at_front();
			*(start.node - 1) = allocate_node();
			// ����ʡ���쳣����
			start.set_node(start.node - 1);
			start.cur = start.last - 1;
			construct(start.cur, x_copy);
		}


		void pop_back_aux() {
			// ʵ����Ҫ������Ԫ����finish.node - 1�Ľڵ���
			deallocate_node(finish.first);
			finish.set_node(finish.node - 1);
			finish.cur = finish.last - 1;
			destory(finish.cur);
		}

		
		void pop_front_aux() {
			destory(start.cur);
			deallocate_node(start.first);
			start.set_node(start.node + 1);
			start.cur = start.first;
		}


		iterator insert_aux(iterator pos, const value_type &x) {
			// posǰԪ��
			auto index = pos - start;
			auto x_copy = x;
			// posǰԪ��������Ԫ��һ��
			if (index < (size() / 2)) {
				// �ü�����vector��Ҳ��ʹ��
				push_front(front());
				auto front1 = start;
				++front1;
				auto front2 = front1;
				++front2;
				// ����posλ��
				pos = start + index;
				auto pos1 = pos;
				++pos1;
				// copy����ǰ�ƣ���Ҫ���ͬһ�ڴ�����
				std::copy(front2, pos1, front1);
			}
			else {
				push_back(back());
				auto back1 = finish;
				--back1;
				auto back2 = back1;
				--back2;
				// ����о�û�б�Ҫ
				pos = start + index;
				// copy_backward���ú���
				std::copy_backward(pos, back2, back1);
			}
			*pos = x_copy;
			return pos;
		}
	};


	template <typename T, typename Alloc, std::size_t BufSiz>
	void deque<T, Alloc, BufSiz>::create_map_and_nodes(size_type num_elements) {
		// ����map�����ú���Ӧ�ڵ�

		// ����ڵ��������ܻ��ʵ�������һ
		size_type num_nodes = num_elements / buffer_size() + 1;

		// һ��map����Ľڵ���������8�������������ڵ�����2
		map_size = std::max(size_type(8), num_nodes + 2);
		map = map_allocator::allocate(map_size);

		// ��map���м�node�����ȥ
		map_pointer nstart = map + (map_size - num_nodes) / 2;
		map_pointer nfinish = nstart + num_nodes - 1;

		// ����ʡ���쳣����
		// ������ҪС�ڵ���
		// һ��0xCDCDCDCD��û�з���Ŀռ�ָ�룬ʵ����Ӧ������ģ���VS������ֵ���ڷ������
		map_pointer cur;
		for (cur = nstart; cur <= nfinish; ++cur)
			*cur = allocate_node();

		// ���õ�����
		start.set_node(nstart);
		finish.set_node(nfinish);
		start.cur = start.first;
		finish.cur = finish.first + num_elements % buffer_size();
	}


	template <typename T, typename Alloc, std::size_t BufSiz>
	void deque<T, Alloc, BufSiz>::reallocate_map(size_type nodes_to_add, bool add_at_front) {
		// ��map�ڵ���
		size_type old_num_nodes = finish.node - start.node + 1;
		// ��map�ڵ���Ϊ��map�ڵ�������nodes_to_add
		size_type new_num_nodes = old_num_nodes + nodes_to_add;

		map_pointer new_nstart;
		// ���������û������ô������������أ�
		// ���ˣ�����������Ϊ8��ʱ��
		if (map_size > 2 * new_num_nodes) {
			// ����֣���ͷ������Ϊɶ����
			new_nstart = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
			if (new_nstart < start.node)
				std::copy(start.node, finish.node, new_nstart);
			else
				std::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
		}
		else {
			size_type new_map_size = map_size + std::max(map_size, nodes_to_add) + 2;
			map_pointer new_map = map_allocator::allocate(new_map_size);

			// ����������map�м������node
			new_nstart = new_map + (new_map_size - new_num_nodes) / 2
				+ (add_at_front ? nodes_to_add : 0);

			// ��ԭ���ݿ�����������ע��������[start, finish + 1)
			std::copy(start.node, finish.node + 1, new_nstart);
			// ɾ��ԭ���ݲ�����״̬
			map_allocator::deallocate(map, map_size);
			map = new_map;
			map_size = new_map_size;
		}
		// �����趨������
		start.set_node(new_nstart);
		finish.set_node(new_nstart + old_num_nodes - 1);
	}



	template <typename T, typename Alloc, std::size_t BufSiz>
	void deque<T, Alloc, BufSiz>::clear() {
		// ע��clear()���������Ԫ�أ���deque����Ϊ��ʼ״̬����ʼ״̬��һ��������

		// [start.node + 1, finish.node)�������Ԫ�ض�����
		for (map_pointer node = start.node + 1; node < finish.node; ++node) {
			destory(*node, *node + buffer_size());
			deallocate_node(*node);
		}

		// ��ʣ���������Ͻڵ�
		if (start.node != finish.node) {
			destory(start.cur, start.last);
			destory(finish.first, finish.cur);
			deallocate_node(finish.first);
		}
		else
			destory(start.cur, finish.cur);

		finish = start;
	}


	template <typename T, typename Alloc, std::size_t BufSiz>
	typename deque<T, Alloc, BufSiz>::iterator 
		deque<T, Alloc, BufSiz>::erase(iterator first, iterator last) {
		// ����������������queue�������clear()��
		if (first == start && last == finish) {
			clear();
			return finish;
		}
		else {
			// ��Ҫɾ����Ԫ�ظ���
			auto n = last - first;
			// ɾ������ǰ��Ԫ�ظ���
			auto elems_before = first - start;
			// ���ɾ������ǰ��Ԫ�ظ���С��ɾ����ʣ��Ԫ�ص�һ��
			if (elems_before < (size() - n) / 2) {
				// ��ɾ������ǰ��Ԫ�غ���
				std::copy_backward(start, first, last);
				// ������ͷ��������ɾ������Ԫ�غͽڵ�
				iterator new_start = start + n;
				destory(start, new_start);
				for (auto cur = start.node; cur < new_start.node; ++cur)
					deallocate_node(*cur);
				start = new_start;
			}
			// ���ɾ������ǰ��Ԫ�ظ�������ɾ����ʣ��Ԫ�ص�һ��
			else {
				// ��ɾ������ǰ��Ԫ��ǰ��
				std::copy(last, finish, first);
				// ������β��������ɾ������Ԫ�غͽڵ�
				iterator new_finish = finish - n;
				destory(new_finish, finish);
				for (auto cur = new_finish.node + 1; cur <= finish.node; ++cur)
					deallocate_node(*cur);
				finish = new_finish;
			}
			// ע�ⷵ�ص���start����ԭɾ������ǰ��Ԫ�ظ���
			return start + elems_before;
		}
	}

}
