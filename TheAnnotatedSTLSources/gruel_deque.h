#pragma once


#include "gruel_memory.h"


namespace gruel {

	/* 综述：deque，双端队列，也是序列式容器，其与vector最大的区别在于deque允许在常数时间内
	对其头端进行插入删除操作，且没有容量的概念，嘛差不多是这样诞生的。除此之外，deque还作为
	其他数据结构的底层容器。 */


	// 用来决定缓冲区大小
	// 就是计算每个缓冲区的元素个数，sz为每个元素所占字节大小
	std::size_t _deque_buf_size(std::size_t n, std::size_t sz) {
		return n != 0 ? n : (sz < 512 ? std::size_t(512 / sz) : std::size_t(1));
	}


	// deque的迭代器，BufSiz为指定缓冲区元素个数，0为默认
	template <typename T, typename Ptr, typename Ref, std::size_t BufSiz = 0>
	struct _deque_iterator {

		// 我其实不太理解为啥迭代器里也要定义迭代器类型
		using iteartor = _deque_iterator<T, T *, T &, BufSiz>;
		// 学到了，常量迭代器要这样设计
		using const_iterator = _deque_iterator<T, const T *, const T &, BufSiz>;

		// 也就是一个缓冲区里有几个元素
		static std::size_t buffer_size() {
			return _deque_buf_size(BufSiz, sizeof(T));
		}

		// 迭代器需要的定义
		// deque提供随机访问迭代器
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using pointer = Ptr;
		using reference = Ref;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		// 用于指向管控中心的类型
		using map_pointer = T * *;

		// self这样设定就可以了
		using self = _deque_iterator;

		// 保持与容器的联结
		T *cur;				// 当前指向的元素位置
		T *first;			// 当前缓冲区的头
		T *last;			// 当前缓冲区的尾
		map_pointer node;	// 指向管控中心

		// 该函数用来跳到新缓冲区
		void set_node(map_pointer new_node) {
			node = new_node;
			first = *new_node;
			last = first + difference_type(buffer_size());
		}

		// 重载运算符
		reference operator*() const { return *cur; }
		pointer operator->() const { return &(operator*()); }

		// 计算两个迭代器之间的距离，根据图示更容易理解
		// 先计算相距几个缓冲区，在计算元素在各自缓冲区的距离
		difference_type operator-(const self &x) const {
			return difference_type(buffer_size() * (node - x.node - 1)) +
				(cur - first) + (x.last - x.cur);
		}


		// 注意前置式返回的是引用
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

		// 实现迭代器随机存取
		self &operator+=(difference_type n) {
			// 偏移量
			difference_type offset = n + cur - first;
			if (offset > 0 && offset < difference_type(buffer_size()))
				// 目标位置在一个缓冲区内
				cur += n;
			else {
				// 不在一个缓冲区内，就要调整缓冲区了
				// 注意是随机访问迭代器，既可以往前也可以往后
				difference_type node_offset =
					offset > 0 ? offset / difference_type(buffer_size())
					: -difference_type((-offset - 1) / buffer_size()) - 1;
				// 注意这里已经调整了缓冲区
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

		// 这里的下标操作符是通过操作符+完成的
		reference operator[](difference_type n) const { return *(*this + n); }
		bool operator==(const self &x) const { return cur == x.cur; }
		bool operator!=(const self &x) const { return !(*this == x); }
		bool operator<(const self &x) const {
			return (node == x.node) ? (cur < x.cur) : (node < x.node);
		}

	};


	// 双端队列
	template <typename T, typename Alloc = alloc, std::size_t BufSiz = 0>
	class deque {
	public:

		// 内嵌类型
		using value_type = T;
		using pointer = T * ;
		using reference = T & ;
		using const_reference = const T &;
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;

		using iterator = _deque_iterator<T, T *, T &, BufSiz>;
		using const_iterator = _deque_iterator<T, const T *, const T &, BufSiz>;


		// 构造函数
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
				// 当缓冲区有两个以上备用空间时
				construct(finish.cur, x);
				++finish.cur;
			}
			else
				push_back_aux(x);
		}


		void push_front(const value_type &x) {
			if (start.cur != start.first) {
				// 注意跟后端插入不同
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
			// 目标位置的后一位
			auto next = pos;
			++next;
			// pos前有多少元素
			auto index = pos - start;
			if (index < (size() >> 1)) {
				// 如果pos前的元素少于总元素一半，则将pos前元素后移，并删除头元素
				std::copy_backward(start, pos, next);
				pop_front();
			}
			else {
				// 否则，将pos后的元素前移，并删除尾元素
				std::copy(next, finish, pos);
				pop_back();
			}
			// 返回相应位置迭代器
			return start + index;
		}


		iterator erase(iterator first, iterator last);


		iterator insert(iterator pos, const value_type &x) {
			// 在头插入
			if (pos.cur == start.cur) {
				push_front(x);
				return start;
			}
			// 在尾插入
			else if (pos.cur == finish.cur) {
				push_back(x);
				// finish不可改变
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

		// 元素指针的指针
		using map_pointer = pointer * ;

		// map专属配置器，每次配置一个指针大小，返回是T **
		using map_allocator = gruel::simple_alloc<pointer, Alloc>;

		// 数据专属配置器，每次配置一个元素大小，返回T *
		using data_allocator = gruel::simple_alloc<value_type, Alloc>;

		// 头迭代器
		iterator start;
		// 尾迭代器
		iterator finish;

		// 管控中心
		map_pointer map;

		size_type map_size;

		// 配置节点
		pointer allocate_node() {
			return data_allocator::allocate(buffer_size());
		}

		// 释放节点
		void deallocate_node(pointer p) {
			data_allocator::deallocate(p, buffer_size());
		}

		// 重新整治map
		void reallocate_map(size_type nodes_to_add, bool add_at_front);

		// 在尾端添加node
		void reserve_map_at_back(size_type nodes_to_add = 1) {
			// 如果map中尾部node的可用个数小于1
			if (nodes_to_add > map_size - (finish.node - map + 1))
				reallocate_map(nodes_to_add, false);
		}

		// 在头部添加node
		void reserve_map_at_front(size_type nodes_to_add = 1) {
			// 如果map中头部node的可用个数小于1
			if (nodes_to_add > start.node - map)
				reallocate_map(nodes_to_add, true);
		}

		// 根据num_elements配置合适的map和node
		void create_map_and_nodes(size_type num_elements);

		// 填充n个value
		void fill_initialize(size_type n, const value_type &value) {
			create_map_and_nodes(n);
			map_pointer cur;
			// 以下省略异常机制
			// [start.node, finish.node)中充满元素
			for (cur = start.node; cur < finish.node; ++cur)
				gruel::uninitialized_fill(*cur, *cur + buffer_size(), value);
			// 将剩余的元素填充
			gruel::uninitialized_fill(finish.first, finish.cur, value);
		}


		void push_back_aux(const value_type &x) {
			auto x_copy = x;
			reserve_map_at_back();
			// 增加了map却并未配置节点
			*(finish.node + 1) = allocate_node();
			// 以下省略异常机制
			construct(finish.cur, x_copy);
			// 更新迭代器
			finish.set_node(finish.node + 1);
			finish.cur = finish.first;
		}

		
		void push_front_aux(const value_type &x) {
			auto x_copy = x;
			reserve_map_at_front();
			*(start.node - 1) = allocate_node();
			// 以下省略异常机制
			start.set_node(start.node - 1);
			start.cur = start.last - 1;
			construct(start.cur, x_copy);
		}


		void pop_back_aux() {
			// 实际需要析构的元素在finish.node - 1的节点上
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
			// pos前元素
			auto index = pos - start;
			auto x_copy = x;
			// pos前元素少于总元素一半
			if (index < (size() / 2)) {
				// 该技巧在vector中也有使用
				push_front(front());
				auto front1 = start;
				++front1;
				auto front2 = front1;
				++front2;
				// 调整pos位置
				pos = start + index;
				auto pos1 = pos;
				++pos1;
				// copy多用前移，主要针对同一内存区域
				std::copy(front2, pos1, front1);
			}
			else {
				push_back(back());
				auto back1 = finish;
				--back1;
				auto back2 = back1;
				--back2;
				// 这里感觉没有必要
				pos = start + index;
				// copy_backward多用后移
				std::copy_backward(pos, back2, back1);
			}
			*pos = x_copy;
			return pos;
		}
	};


	template <typename T, typename Alloc, std::size_t BufSiz>
	void deque<T, Alloc, BufSiz>::create_map_and_nodes(size_type num_elements) {
		// 创建map并配置好相应节点

		// 所需节点数，可能会比实际需求多一
		size_type num_nodes = num_elements / buffer_size() + 1;

		// 一个map管理的节点数最少是8个，最多是所需节点数加2
		map_size = std::max(size_type(8), num_nodes + 2);
		map = map_allocator::allocate(map_size);

		// 将map的中间node分配出去
		map_pointer nstart = map + (map_size - num_nodes) / 2;
		map_pointer nfinish = nstart + num_nodes - 1;

		// 以下省略异常机制
		// 这里需要小于等于
		// 一般0xCDCDCDCD是没有分配的空间指针，实际上应是随机的，但VS给出该值用于方便调试
		map_pointer cur;
		for (cur = nstart; cur <= nfinish; ++cur)
			*cur = allocate_node();

		// 设置迭代器
		start.set_node(nstart);
		finish.set_node(nfinish);
		start.cur = start.first;
		finish.cur = finish.first + num_elements % buffer_size();
	}


	template <typename T, typename Alloc, std::size_t BufSiz>
	void deque<T, Alloc, BufSiz>::reallocate_map(size_type nodes_to_add, bool add_at_front) {
		// 旧map节点数
		size_type old_num_nodes = finish.node - start.node + 1;
		// 新map节点数为旧map节点数加上nodes_to_add
		size_type new_num_nodes = old_num_nodes + nodes_to_add;

		map_pointer new_nstart;
		// 接下来这段没懂，怎么会有这种情况呢？
		// 懂了，就是配置数为8的时候
		if (map_size > 2 * new_num_nodes) {
			// 好奇怪，在头部配置为啥加呢
			new_nstart = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
			if (new_nstart < start.node)
				std::copy(start.node, finish.node, new_nstart);
			else
				std::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
		}
		else {
			size_type new_map_size = map_size + std::max(map_size, nodes_to_add) + 2;
			map_pointer new_map = map_allocator::allocate(new_map_size);

			// 还是设置在map中间区域的node
			new_nstart = new_map + (new_map_size - new_num_nodes) / 2
				+ (add_at_front ? nodes_to_add : 0);

			// 将原数据拷贝到新区域，注意这里拷贝[start, finish + 1)
			std::copy(start.node, finish.node + 1, new_nstart);
			// 删除原数据并更新状态
			map_allocator::deallocate(map, map_size);
			map = new_map;
			map_size = new_map_size;
		}
		// 重新设定迭代器
		start.set_node(new_nstart);
		finish.set_node(new_nstart + old_num_nodes - 1);
	}



	template <typename T, typename Alloc, std::size_t BufSiz>
	void deque<T, Alloc, BufSiz>::clear() {
		// 注意clear()是清除所有元素，将deque设置为初始状态，初始状态有一个缓冲区

		// [start.node + 1, finish.node)区间里的元素都存在
		for (map_pointer node = start.node + 1; node < finish.node; ++node) {
			destory(*node, *node + buffer_size());
			deallocate_node(*node);
		}

		// 还剩余两个以上节点
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
		// 如果清除区间是整个queue，则调用clear()；
		if (first == start && last == finish) {
			clear();
			return finish;
		}
		else {
			// 需要删除的元素个数
			auto n = last - first;
			// 删除区间前的元素个数
			auto elems_before = first - start;
			// 如果删除区间前的元素个数小于删除后剩余元素的一半
			if (elems_before < (size() - n) / 2) {
				// 将删除区间前的元素后移
				std::copy_backward(start, first, last);
				// 设置新头迭代器，删除多余元素和节点
				iterator new_start = start + n;
				destory(start, new_start);
				for (auto cur = start.node; cur < new_start.node; ++cur)
					deallocate_node(*cur);
				start = new_start;
			}
			// 如果删除区间前的元素个数大于删除后剩余元素的一半
			else {
				// 将删除区间前的元素前移
				std::copy(last, finish, first);
				// 设置新尾迭代器，删除多余元素和节点
				iterator new_finish = finish - n;
				destory(new_finish, finish);
				for (auto cur = new_finish.node + 1; cur <= finish.node; ++cur)
					deallocate_node(*cur);
				finish = new_finish;
			}
			// 注意返回的是start加上原删除区间前的元素个数
			return start + elems_before;
		}
	}

}
