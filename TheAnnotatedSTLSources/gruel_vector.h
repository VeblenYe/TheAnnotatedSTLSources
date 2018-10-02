#pragma once


#include "gruel_memory.h"


namespace gruel {

	// vector
	// vector提供随机访问迭代器，其所有功能都可通过原始指针完成
	template <typename T, typename Alloc = alloc>
	class vector {
	public:

		// vector的嵌套类型定义
		using value_type = T;
		using pointer = value_type * ;
		using iterator = value_type * ;
		using const_iterator = const value_type *;
		using reference = value_type & ;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;


		// 构造函数
		vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr) {};
		vector(size_type n, const T &value) { fill_initialize(n, value); }
		vector(int n, const T &value) { fill_initialize(n, value); }
		vector(long n, const T &value) { fill_initialize(n, value); }
		explicit vector(size_type n) { fill_initialize(n, T()); }

		~vector() {
			destory(start, finish);
			deallocate();
		}


		iterator begin() { return start; }
		const_iterator begin() const { return start; }
		iterator end() { return finish; }
		const_iterator end() const { return finish; }
		size_type size() const { return size_type(end() - begin()); }
		size_type capacity() const {
			return static_cast<size_type>(end_of_storage - begin());
		}
		bool empty() const { return begin() == end(); }
		// 下标操作，若越界，则行为未定义
		reference operator[](size_type n) { return *(begin() + n); }
		reference front() { return *begin(); }
		reference back() { return *(end() - 1); }
		void insert(iterator position, size_type n, const T &x);
		// 在尾端插入新元素
		void push_back(const T &x) {
			// 若剩余空间足够，则直接构造
			if (finish != end_of_storage) {
				construct(finish, x);
				++finish;
			}
			// 否则需要其他操作
			else
				insert_aux(end(), x);
		}
		// 删除尾端元素
		void pop_back() {
			--finish;
			destory(finish);
		}

		// 删除指定位置元素
		iterator erase(iterator position) {
			// 如果删除元素不是尾端元素，则直接将删除元素的后一个元素到尾端元素复制到删除元素的位置即可，也就是前移
			if (position + 1 != end())
				std::copy(position + 1, finish, position);
			// 是尾端元素就调整finish后删除即可
			--finish;
			destory(finish);
			return position;	
		}

		// 删除[first, last)区间内的元素
		iterator erase(iterator first, iterator last) {
			// 直接将[last, finish)的元素前移到first的位置即可
			auto i = std::copy(last, finish, first);
			// 再删除多出来的部分
			destory(i, finish);
		}

		// 调整vector的size大小
		void resize(size_type new_size, const T &x) {
			// 如果新size小于旧size，则需要删除多余的元素
			if (new_size < size())
				erase(begin() + new_size, end());
			// 否则，在多出来的区域构建x
			else
				insert(end(), new_size - size(), x);
		}

		// 构建的元素为空
		void resize(size_type new_size) {
			resize(new_size, T());
		}

		void clear() {
			erase(begin(), end());
		}

	protected:

		using data_allocator = simple_alloc<value_type, Alloc>;

		iterator start;
		iterator finish;
		iterator end_of_storage;

		void insert_aux(iterator position, const T &x);
		void deallocate() {
			if (start)
				data_allocator::deallocate(start, end_of_storage - start);
		}

		// 分配并填充
		iterator allocate_and_fill(size_type n, const T &x) {
			auto result = data_allocator::allocate(n);
			uninitialized_fill_n(result, n, x);
			return result;
		}

		void fill_initialize(size_type n, const T &value) {
			start = allocate_and_fill(n, value);
			finish = start + n;
			end_of_storage = finish;
		}
	};


	template <typename T, typename Alloc>
	void vector<T, Alloc>::insert_aux(iterator position, const T &x) {
		if (finish != end_of_storage) {
			construct(finish, *(finish - 1));
			++finish;
			auto x_copy = x;
			std::copy_backward(position, finish - 1, finish - 2);
			*position = x_copy;
		}
		else {
			const auto old_size = size();
			const size_type len = old_size != 0 ? 2 * old_size : 1;
			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;

			// 以下省略了异常机制
			// 复制之前
			new_finish = uninitialized_copy(start, position, new_start);
			// 构造新元素
			construct(new_finish, x);
			++new_finish;
			// 复制之后
			new_finish = uninitialized_copy(position, finish, new_finish);

			destory(begin(), end());
			deallocate();

			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}


	// 插入操作，参数分别是插入位置，插入元素个数，插入元素
	template <typename T, typename Alloc>
	void vector<T, Alloc>::insert(iterator position, size_type n, const T &x) {
		if (n != 0) {
			// 如果剩余空间充足
			if (size_type(end_of_storage - finish) >= n) {
				auto x_copy = x;
				// 插入位置到尾部的元素个数
				const size_type elems_after = finish - position;
				iterator old_finish = finish;
				// 如果插入的元素个数小于elems_after
				if (elems_after > n) {
					// 则先将多出的元素复制到finish之后
					uninitialized_copy(finish - n, finish, finish);
					finish += n;
					// 再将剩余的原区域元素后移至原finish之前
					std::copy_backward(position, old_finish - n, old_finish);
					// 最后填充
					std::fill(position, position + n, x_copy);
				}
				// 如果插入的元素个数大于elems_after
				else {
					// 则先将多出的插入元素复制到finish之后
					uninitialized_fill_n(finish, n - elems_after, x_copy);
					finish += n - elems_after;
					// 再将原区域元素移到新finish之后
					uninitialized_copy(position, old_finish, finish);
					finish += elems_after;
					// 填充剩余的插入元素
					std::fill(position, old_finish, x_copy);
				}
			}
			// 如果剩余空间不足
			else {
				const size_type old_size = size();
				const size_type len = old_size + std::max(old_size, n);

				auto new_start = data_allocator::allocate(len);
				auto new_finish = new_start;

				// 以下省略异常机制
				// 先复制插入位置之前的
				new_finish = uninitialized_copy(start, position, new_start);
				// 插入元素
				new_finish = uninitialized_fill_n(new_finish, n, x);
				// 复制插入位置之后的
				new_finish = uninitialized_copy(position, finish, new_finish);

				destory(start, finish);
				deallocate();

				start = new_start;
				finish = new_finish;
				end_of_storage = new_start + len;
			}
		}
	}

}
