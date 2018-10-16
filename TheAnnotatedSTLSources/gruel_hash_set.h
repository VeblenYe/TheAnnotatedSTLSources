#pragma once


#include "gruel_hashtable.h"


namespace gruel {

	/* 综述：hash_set，是以hashtable作为底层的set容器，特性上与set相差无几，但没有自动排序，查询和访问元素效率理论上比set要高 */

	/* 对应标准容器里的unordered_set */

	template <typename Value, typename HashFun = hash<Value>, typename EqualKey = std::equal_to<Value>, typename Alloc = alloc>
	class hash_set {
	private:
		using ht = hashtable<Value, Value, HashFun, std::identity<Value>, EqualKey, Alloc>;
		ht rep;

	public:
		using value_type = typename ht::value_type;
		using key_type = typename ht::key_type;
		using hasher = typename ht::hasher;
		using key_equal = typename ht::key_equal;

		using size_type = typename ht::size_type;
		using difference_type = typename ht::difference_type;
		using pointer = typename ht::const_pointer;
		using const_pointer = typename ht::const_pointer;
		using reference = typename ht::const_reference;
		using const_reference = typename ht::const_reference;

		using iterator = typename ht::const_iterator;
		using const_iterator = typename ht::const_iterator;

		hasher hash_funct() const { return rep.hash_funct(); }
		key_equal key_eq() const { return rep.key_eq(); }
		
	public:
		hash_set() : rep(100, hasher(), key_equal()) {}
		explicit hash_set(size_type n) : rep(n, hasher(), key_equal()) {}
		hash_set(size_type n, const hasher &hf) : rep(n, hf, key_equal()) {}
		hash_set(size_type n, const hasher &hf, const key_equal &eql) : rep(n, hf, eql) {}

		template <typename InputIterator>
		hash_set(InputIterator first, InputIterator last) : rep(100, hasher(), key_equal()) {
			rep.insert_unique(first, last);
		}
		template <typename InputIterator>
		hash_set(InputIterator first, InputIterator last, size_type n) : rep(n, hasher(), key_equal()) {
			rep.insert_unique(first, last);
		}
		template <typename InputIterator>
		hash_set(InputIterator first, InputIterator last, size_type n, const hasher &hf) : rep(n, hf, key_equal()) {
			rep.insert_unique(first, last);
		}
		template <typename InputIterator>
		hash_set(InputIterator first, InputIterator last, size_type n, const hasher &hf, const key_equal &eql) : rep(n, hf, eql) {
			rep.insert_unique(first, last);
		}

	public:
		size_type size() const { return rep.size(); }
		size_type max_size() const { return rep.max_size(); }
		bool empty() const { return rep.empty(); }
		void swap(hash_set &hs) { rep.swap(hs.rep); }

		iterator begin() const { return rep.begin(); }
		iterator end() const { return rep.end(); }

	public:
		std::pair<iterator, bool> insert(const value_type &obj) {
			// 都是别人的定义还转型。。
			auto p = rep.insert_unique(obj);
			return { p.first, p.second };
		}
		
		template <typename InputIterator>
		void insert(InputIterator first, InputIterator last) {
			rep.insert_unique(first, last);
		}

		std::pair<iterator, bool> insert_noresize(const value_type &obj) {
			auto p = rep.insert_unique_noresize(obj);
			return { p.first, p.second };
		}

		iterator find(const key_type &obj) const { return rep.find(obj); }

		size_type count(const key_type &obj) const { return rep.count(obj); }

		std::pair<iterator, iterator> equal_range(const key_type &key) const { return rep.equal_range(key); }

		size_type erase(const key_type &key) { return rep.erase(key); }
		void erase(iterator it) { rep.erase(it); }
		void erase(iterator first, iterator last) { rep.erase(first, last); }
		void clear() { rep.clear(); }

	public:
		void resize(size_type n) { rep.resize(n); }
		size_type bucket_count() const { return rep.bucket_count(); }
		size_type max_bucket_count() const { return rep.max_bucket_count(); }
		size_type elems_in_bucket(size_type n) const { return rep.elems_in_bucket(n); }
	};

	template <typename V, typename HF, typename EK, typename A>
	bool operator==(const hash_set<V, HF, EK, A> &lhs,
		const hash_set<V, HF, EK, A> &rhs) {
		return lhs.rep == rhs.rep;
	}

	template <typename V, typename HF, typename EK, typename A>
	bool operator!=(const hash_set<V, HF, EK, A> &lhs,
		const hash_set<V, HF, EK, A> &rhs) {
		return lhs.rep != rhs.rep;
	}

}
