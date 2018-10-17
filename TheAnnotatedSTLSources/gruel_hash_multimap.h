#pragma once


#include "gruel_hashtable.h"
#include "gruel_functional.h"


namespace gruel {

	template <typename Key, typename T, typename HashFun = hash<Key>,
		typename EqualKey = std::equal_to<Key>, typename Alloc = alloc>
		class hash_multimap {
		private:
			using ht = hashtable<std::pair<const Key, T>, select1st<std::pair<const Key, T>>, HashFun, const Key, EqualKey, Alloc>;
			ht rep;

		public:
			using hasher = typename ht::hasher;
			using key_equal = typename ht::key_equal;

			using data_type = T;
			using mapped_type = T;
			using value_type = typename ht::value_type;
			using key_type = typename ht::key_type;
			using pointer = typename ht::pointer;
			using const_pointer = typename ht::const_pointer;
			using reference = typename ht::reference;
			using const_reference = typename ht::const_reference;
			using size_type = typename ht::size_type;
			using difference_type = typename ht::difference_type;

			using iterator = typename ht::iterator;
			using const_iterator = typename ht::const_iterator;

			hasher hash_funct() const { return rep.hash_funct(); }
			key_equal key_eq() const { return rep.key_eq(); }

		public:
			hash_multimap() : rep(100, hasher(), key_equal()) {}
			explicit hash_multimap(size_type n) : rep(n, hasher(), key_equal()) {}
			hash_multimap(size_type n, const hasher &hf) : rep(n, hf, key_equal()) {}
			hash_multimap(size_type n, const hasher &hf, const key_equal &eql) : rep(n, hf, eql) {}

			template <typename InputIterator>
			hash_multimap(InputIterator first, InputIterator last) : rep(100, hasher(), key_equal()) {
				rep.insert_equal(first, last);
			}
			template <typename InputIterator>
			hash_multimap(InputIterator first, InputIterator last, size_type n) : rep(n, hasher(), key_equal()) {
				rep.insert_equal(first, last);
			}
			template <typename InputIterator>
			hash_multimap(InputIterator first, InputIterator last, size_type n, const hasher &hf) : rep(n, hf, key_equal()) {
				rep.insert_equal(first, last);
			}
			template <typename InputIterator>
			hash_multimap(InputIterator first, InputIterator last, size_type n, const hasher &hf, const key_equal &eql) : rep(n, hf, eql) {
				rep.insert_equal(first, last);
			}

		public:
			size_type size() const { return rep.size(); }
			size_type max_size() const { return rep.max_size(); }
			bool empty() const { return rep.empty(); }
			void swap(hash_multimap &hs) { rep.swap(hs.rep); }

			iterator begin() { return rep.begin(); }
			const_iterator begin() const { return rep.begin(); }
			iterator end() { return rep.end(); }
			const_iterator end() const { return rep.end(); }

			std::pair<iterator, bool> insert(const value_type &obj) {
				// ���Ǳ��˵Ķ��廹ת�͡���
				return rep.insert_equal(obj);
			}

			template <typename InputIterator>
			void insert(InputIterator first, InputIterator last) {
				rep.insert_equal(first, last);
			}

			std::pair<iterator, bool> insert_noresize(const value_type &obj) {
				auto p = rep.insert_equal_noresize(obj);
				return { p.first, p.second };
			}

			iterator find(const key_type &obj) { return rep.find(obj); }
			const_iterator find(const key_type &obj) const { return rep.find(obj); }

			size_type count(const key_type &obj) const { return rep.count(obj); }

			std::pair<iterator, iterator> equal_range(const key_type &key) { return rep.equal_range(key); }
			std::pair<const_iterator, bool> equal_range(const key_type &key) const { return rep.equal_range(key); }

			size_type erase(const key_type &key) { return rep.erase(key); }
			void erase(iterator it) { rep.erase(it); }
			void erase(iterator first, iterator last) { rep.erase(first, last); }
			void clear() { rep.clear(); }
			void resize(size_type n) { rep.resize(n); }
			size_type bucket_count() const { return rep.bucket_count(); }
			size_type max_bucket_count() const { return rep.max_bucket_count(); }
			size_type elems_in_bucket(size_type n) const { return rep.elems_in_bucket(n); }

	};


	template <typename K, typename T, typename HF, typename EK, typename A>
	bool operator==(const hash_multimap<K, T, HF, EK, A> &lhs,
		const hash_multimap<K, T, HF, EK, A> &rhs) {
		return lhs.rep == rhs.rep;
	}

	template <typename K, typename T, typename HF, typename EK, typename A>
	bool operator!=(const hash_multimap<K, T, HF, EK, A> &lhs,
		const hash_multimap<K, T, HF, EK, A> &rhs) {
		return lhs.rep != rhs.rep;
	}

}