#pragma once


#include "gruel_memory.h"


namespace gruel {

	// vector
	// vector�ṩ������ʵ������������й��ܶ���ͨ��ԭʼָ�����
	template <typename T, typename Alloc = alloc>
	class vector {
	public:

		// vector��Ƕ�����Ͷ���
		using value_type = T;
		using pointer = value_type * ;
		using iterator = value_type * ;
		using const_iterator = const value_type *;
		using reference = value_type & ;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;


		// ���캯��
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
		// �±��������Խ�磬����Ϊδ����
		reference operator[](size_type n) { return *(begin() + n); }
		reference front() { return *begin(); }
		reference back() { return *(end() - 1); }
		void insert(iterator position, size_type n, const T &x);
		// ��β�˲�����Ԫ��
		void push_back(const T &x) {
			// ��ʣ��ռ��㹻����ֱ�ӹ���
			if (finish != end_of_storage) {
				construct(finish, x);
				++finish;
			}
			// ������Ҫ��������
			else
				insert_aux(end(), x);
		}
		// ɾ��β��Ԫ��
		void pop_back() {
			--finish;
			destory(finish);
		}

		// ɾ��ָ��λ��Ԫ��
		iterator erase(iterator position) {
			// ���ɾ��Ԫ�ز���β��Ԫ�أ���ֱ�ӽ�ɾ��Ԫ�صĺ�һ��Ԫ�ص�β��Ԫ�ظ��Ƶ�ɾ��Ԫ�ص�λ�ü��ɣ�Ҳ����ǰ��
			if (position + 1 != end())
				std::copy(position + 1, finish, position);
			// ��β��Ԫ�ؾ͵���finish��ɾ������
			--finish;
			destory(finish);
			return position;	
		}

		// ɾ��[first, last)�����ڵ�Ԫ��
		iterator erase(iterator first, iterator last) {
			// ֱ�ӽ�[last, finish)��Ԫ��ǰ�Ƶ�first��λ�ü���
			auto i = std::copy(last, finish, first);
			// ��ɾ��������Ĳ���
			destory(i, finish);
		}

		// ����vector��size��С
		void resize(size_type new_size, const T &x) {
			// �����sizeС�ھ�size������Ҫɾ�������Ԫ��
			if (new_size < size())
				erase(begin() + new_size, end());
			// �����ڶ���������򹹽�x
			else
				insert(end(), new_size - size(), x);
		}

		// ������Ԫ��Ϊ��
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

		// ���䲢���
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

			// ����ʡ�����쳣����
			// ����֮ǰ
			new_finish = uninitialized_copy(start, position, new_start);
			// ������Ԫ��
			construct(new_finish, x);
			++new_finish;
			// ����֮��
			new_finish = uninitialized_copy(position, finish, new_finish);

			destory(begin(), end());
			deallocate();

			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}


	// ��������������ֱ��ǲ���λ�ã�����Ԫ�ظ���������Ԫ��
	template <typename T, typename Alloc>
	void vector<T, Alloc>::insert(iterator position, size_type n, const T &x) {
		if (n != 0) {
			// ���ʣ��ռ����
			if (size_type(end_of_storage - finish) >= n) {
				auto x_copy = x;
				// ����λ�õ�β����Ԫ�ظ���
				const size_type elems_after = finish - position;
				iterator old_finish = finish;
				// ��������Ԫ�ظ���С��elems_after
				if (elems_after > n) {
					// ���Ƚ������Ԫ�ظ��Ƶ�finish֮��
					uninitialized_copy(finish - n, finish, finish);
					finish += n;
					// �ٽ�ʣ���ԭ����Ԫ�غ�����ԭfinish֮ǰ
					std::copy_backward(position, old_finish - n, old_finish);
					// ������
					std::fill(position, position + n, x_copy);
				}
				// ��������Ԫ�ظ�������elems_after
				else {
					// ���Ƚ�����Ĳ���Ԫ�ظ��Ƶ�finish֮��
					uninitialized_fill_n(finish, n - elems_after, x_copy);
					finish += n - elems_after;
					// �ٽ�ԭ����Ԫ���Ƶ���finish֮��
					uninitialized_copy(position, old_finish, finish);
					finish += elems_after;
					// ���ʣ��Ĳ���Ԫ��
					std::fill(position, old_finish, x_copy);
				}
			}
			// ���ʣ��ռ䲻��
			else {
				const size_type old_size = size();
				const size_type len = old_size + std::max(old_size, n);

				auto new_start = data_allocator::allocate(len);
				auto new_finish = new_start;

				// ����ʡ���쳣����
				// �ȸ��Ʋ���λ��֮ǰ��
				new_finish = uninitialized_copy(start, position, new_start);
				// ����Ԫ��
				new_finish = uninitialized_fill_n(new_finish, n, x);
				// ���Ʋ���λ��֮���
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
