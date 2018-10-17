#pragma once


#include "gruel_tree.h"
#include "gruel_functional.h"


namespace gruel {

	/* 综述：map。图，一种关联性容器（associative containers），其所有元素都会根据元素
	的键值自动排序，map的所有元素都是pair，同时拥有实值（value)和键值(key)，且不允许两
	个元素有相同的键值。其底层容器为红黑树 */

	// C++14的多态lambda
	// auto select1st = [](auto const &pair) { return pair.first; };

	/*
	#define AUTO_RETURN(...) -> decltype(__VA_ARGS__) {return (__VA_ARGS__);}

	template <typename Pair>
	auto select1st() AUTO_RETURN(std::bind(&Pair::first, std::placeholders::_1))
	*/

	// map拥有键值与实值，底层容器也为红黑树，操作大多转调rb_tree完成
	// Key为键值，Value为实值
	template <typename Key, typename Value, typename Compare = std::less<Key>, typename Alloc = alloc>
	class map {
	public:
		using key_type = Key;	// 键值类型
		using data_type = Value;	// 实值类型
		using mapped_type = Value;
		using value_type = pair<const Key, Value>;	// 元素类型（键值/实值）
		using key_compare = Compare;	// 键值比较函数

		// 定义一个仿函数functor，其作用是调用“元素比较函数”
		class value_compare : public std::binary_function<value_type, value_type, bool> {
			friend class map<Key, Value, Compare, Alloc>;
		protected:
			Compare comp;
			value_compare(Compare c) : comp(c) {}
		public:
			bool operator()(const value_type &lhs, const value_type &rhs) const {
				return comp(lhs.first, rhs.first);
			}
		};

	private:
		// 注意这里的类型，元素类型是pair，而pair.first作为键值，源码实现用了select1st，这里直接使用Key
		using rep_type = rb_tree<key_type, value_type, select1st<value_type>, key_compare, Alloc>;
		rep_type t;

	public:
		// 嵌套类型
		using pointer = typename rep_type::pointer;
		using const_pointer = typename rep_type::const_pointer;
		using reference = typename rep_type::reference;
		using const_reference = typename rep_type::const_reference;
		using iterator = typename rep_type::iterator;
		using const_iterator = typename rep_type::const_iterator;
		using size_type = typename rep_type::size_type;
		using difference_type = typename rep_type::difference_type;
		using self = map<Key, Value, Compare, Alloc>;

		map() : t(Compare()) {}

		explicit map(const Compare &comp) : t(comp) {}
		
		template <typename InputIterator>
		map(InputIterator first, InputIterator last) : t(Compare()) {
			t.insert_unique(first, last);
		}

		template <typename InputIterator>
		map(InputIterator first, InputIterator last, const Compare &comp) : t(comp) {
			t.insert_unique(first, last);
		}

		map(const self &x) : t(x.t) {}

		self &operator=(const self &x) {
			t = x.t;
			return *this;
		}

		key_compare key_comp() const { return t.key_comp(); }
		value_compare value_comp() const { return value_compare(t.key_comp()); }
		iterator begin() { return t.begin(); }
		const_iterator begin() const { return t.begin(); }
		iterator end() { return t.end(); }
		const_iterator end() const { return t.end(); }
		bool empty() const { return t.empty(); }
		size_type size() const { return t.size(); }
		size_type max_size() const { return size_type(-1); }
		Value &operator[](const key_type &k) {
			/* 首先根据键值和实值做出一个元素，由于实值未知，所以产生一个与实值类型相同的暂时对象替代，
			再将该元素插入到map中去，插入操作返回一个pair，其第一元素是个迭代器，指向插入妥当的新元素，
			或指向插入失败点（键值重复）的旧元素。前一个指向作为左值使用，后者作为右值使用。 */
			return (*((insert(value_type(k, Value()))).first)).second;
		}
		void swap(self &x) { t.swap(x.t); }

		pair<iterator, bool> insert(const value_type &x) {
			return t.insert_unique(x);
		}

		iterator insert(iterator position, const value_type &x) {
			return t.insert_unique(position, x);
		}

		template <typename InputIterator>
		void insert(InputIterator first, InputIterator last) {
			t.insert_unique(first, last);
		}

		void erase(iterator position) {
			t.erase(position);
		}

		size_type erase(const Key &x) {
			return t.erase(x);
		}

		void erase(iterator first, iterator last) {
			t.erase(first, last);
		}

		void clear() {
			t.clear();
		}

		iterator find(const key_type &x) { return t.find(x); }
		const_iterator find(const key_type &x) const { return t.find(x); }
		size_type count(const key_type &x) const { return t.count(x); }
		iterator lower_bound(const key_type &x) { return t.lower_bound(x); }
		const_iterator lower_bound(const key_type &x) const { return t.lower_bound(x); }
		iterator upper_bound(const key_type &x) { return t.upper_bound(x); }
		const_iterator upper_bound(const key_type &x) const { return t.upper_bound(x); }
		pair<iterator, iterator> equal_range(const key_type &x) {
			return t.equal_range(x);
		}
		pair<const_iterator, const_iterator> equal_range(const key_type &x) const {
			return t.equal_range(x);
		}
	};

	template <typename Key, typename Value, typename Compare, typename Alloc>
	inline bool operator==(const map<Key, Value, Compare, Alloc> &lhs,
		const map<Key, Value, Compare, Alloc> &rhs) {
		return lhs.t == rhs.t;
	}


	template <typename Key, typename Value, typename Compare, typename Alloc>
	inline bool operator<(const map<Key, Value, Compare, Alloc> &lhs,
		const map<Key, Value, Compare, Alloc> &rhs) {
		return lhs.t < rhs.t;
	}
}
