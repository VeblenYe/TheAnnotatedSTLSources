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
		reference operator[](size_type n) { return *(begin() + n); }
		reference front() { return *begin(); }
		reference back() { return *(end() - 1); }
		void insert(iterator position, size_type n, const T &x);
		void push_back(const T &x) {
			if (finish != end_of_storage) {
				construct(finish, x);
				++finish;
			}
			else
				insert_aux(end(), x);
		}
		void pop_back() {
			--finish;
			destory(finish);
		}

		iterator erase(iterator position) {
			if (position + 1 != end())
				std::copy(position + 1, finish, position);
			--finish;
			destory(finish);
			return position;	
		}

		iterator erase(iterator first, iterator last) {
			auto i = std::copy(last, finish, first);
			destory(i, finish);
		}

		void resize(size_type new_size, const T &x) {
			if (new_size < size())
				erase(begin() + new_size, end());
			else
				insert(end(), new_size - size(), x);
		}

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
			new_finish = uninitialized_copy(start, position, new_start);
			construct(new_finish, x);
			++new_finish;
			new_finish = uninitialized_copy(position, finish, new_finish);

			destory(begin(), end());
			deallocate();

			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}


	template <typename T, typename Alloc>
	void vector<T, Alloc>::insert(iterator position, size_type n, const T &x) {
		if (n != 0) {
			if (size_type(end_of_storage - finish) >= n) {
				auto x_copy = x;
				const size_type elems_after = finish - position;
				iterator old_finish = finish;
				if (elems_after > n) {
					uninitialized_copy(finish - n, finish, finish);
					finish += n;
					std::copy_backward(position, old_finish - n, old_finish);
					std::fill(position, position + n, x_copy);
				}
				else {
					uninitialized_fill_n(finish, n - elems_after, x_copy);
					finish += n - elems_after;
					uninitialized_copy(position, old_finish, finish);
					finish += elems_after;
					std::fill(position, old_finish, x_copy);
				}
			}
			else {
				const size_type old_size = size();
				const size_type len = old_size + std::max(old_size, n);

				auto new_start = data_allocator::allocate(len);
				auto new_finish = new_start;

				// 以下省略异常机制
				new_finish = uninitialized_copy(start, position, new_start);
				new_finish = uninitialized_fill_n(new_finish, n, x);
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
