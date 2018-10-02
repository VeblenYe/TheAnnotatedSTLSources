#pragma once


#include "gruel_tree.h"


namespace gruel {

	template <typename Key, typename Compare = std::less<Key>, typename Alloc = alloc>
	class multiset {
		friend bool operator==(const multiset &, const multiset &);
		friend bool operator<(const multiset &, const multiset &);
	public:
		using key_type = Key;
		using value_type = Key;
		using key_compare = Compare;
		using value_compare = Compare;

	private:
		// set的底层容器为RB-Tree
		using rep_type = rb_tree<key_type, value_type, std::identity<value_type>, key_compare, Alloc>;
		using rep_iterator = typename rep_type::iterator;
		rep_type t;

	public:
		// 一些嵌套定义，注意，均为const
		using pointer = typename rep_type::const_pointer;
		using const_pointer = typename rep_type::const_pointer;
		using reference = typename rep_type::const_reference;
		using const_reference = typename rep_type::const_reference;
		using iterator = typename rep_type::const_iterator;
		using const_iterator = typename rep_type::const_iterator;
		using size_type = typename rep_type::size_type;
		using difference_type = typename rep_type::difference_type;
		using self = multiset<Key, Compare, Alloc>;

		multiset() : t(Compare()) {}
		explicit multiset(const Compare &comp) : t(comp) {}

		template <typename InputIterator>
		multiset(InputIterator first, InputIterator last) : t(Compare()) { t.insert_equal(first, last); }

		template <typename InputIterator>
		multiset(InputIterator first, InputIterator last, const Compare &comp) : t(comp) {
			t.insert_equal(first, last);
		}

		multiset(const self &x) : t(x.t) {}

		self &operator=(const self &x) {
			t = x.t;
			return *this;
		}

		key_compare key_comp() const { return t.key_comp(); }
		value_compare value_comp() const { return t.key_comp(); }
		iterator begin() const { return t.begin(); }
		iterator end() const { return t.end(); }
		bool empty() const { return t.empty(); }
		size_type size() const { return t.size(); }
		size_type max_size() const { return t.max_size(); }

		std::pair<iterator, bool> insert(const Key &k) {
			return t.insert_equal(k);
		}

		iterator insert(iterator position, const Key &k) {
			return t.insert_equal((rep_iterator)position, k);
		}

		template <typename InputIterator>
		void insert(InputIterator first, InputIterator last) {
			t.insert_equal(first, last);
		}

		void erase(iterator position) {
			t.erase((rep_iterator)position);
		}

		size_type erase(const Key &x) {
			return t.erase(x);
		}

		void erase(iterator first, iterator last) {
			t.erase((rep_iterator)first, (rep_iterator)last);
		}

		void clear() {
			t.clear();
		}

		iterator find(const key_type &x) const { return t.find(x); }
		size_type count(const key_type &x) const { return t.count(x); }
		iterator lower_bound(const key_type &x) const { return t.lower_bound(x); }
		iterator upper_bound(const key_type &x) const { return t.upper_bound(x); }
		std::pair<iterator, iterator> equal_range(const key_type &x) const {
			return t.equal_range(x);
		}
	};


	template <typename Key, typename Compare, typename Alloc>
	inline bool operator==(const multiset<Key, Compare, Alloc> &lhs,
		const multiset<Key, Compare, Alloc> &rhs) {
		return lhs.t == rhs.t;
	}


	template <typename Key, typename Compare, typename Alloc>
	inline bool operator<(const multiset<Key, Compare, Alloc> &lhs,
		const multiset<Key, Compare, Alloc> &rhs) {
		return lhs.t < rhs.t;
	}

}
