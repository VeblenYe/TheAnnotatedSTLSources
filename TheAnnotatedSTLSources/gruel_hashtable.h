#pragma once


#include "gruel_memory.h"
#include "gruel_hash_fun.h"
#include "gruel_vector.h"


namespace gruel {


	template <typename Value>
	struct _hashtable_node {
		_hashtable_node *next;
		Value val;
	};

	
	template <typename Value, typename Key, typename HashFun, typename ExtractKey,
		typename EqualKey, typename Alloc>
		class hashtable;


	template <typename Value, typename Key, typename HashFun, typename ExtractKey, 
		typename EqualKey, typename Alloc = alloc>
		struct _hashtable_iterator {

		using hashtable = hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>;
		using iterator = _hashtable_iterator<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>;
		using const_iterator = _hashtable_iterator<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>;
		using node = _hashtable_node<Value>;

		using iterator_category = forward_iterator_tag;
		using value_type = Value;
		using pointer = value_type * ;
		using reference = value_type & ;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		node *cur;
		hashtable *ht;

		_hashtable_iterator() {}
		_hashtable_iterator(node *n, hashtable *tab) : cur(n), ht(tab) {}

		reference operator*() { return cur->val; }
		pointer operator->() { return &(operator*()); }
		iterator &operator++();
		iterator operator++(int);
		bool operator==(const iterator &it) const { return cur == it.cur; }
		bool operator!=(const iterator &it) const { return cur != it.cur; }
	};


	template <typename V, typename K, typename HF, typename ExK, typename EqK, typename A>
	typename _hashtable_iterator<V, K, HF, ExK, EqK, A>::iterator &
		_hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++() {

		const node *old = cur;
		cur = cur->next;
		if (!cur) {
			size_type bucket = ht->bkt_num(old->val);
			while (!cur && ++bucket < ht->buckets.size())
				cur = ht->buckets[bucket];
		}

		return *this;
	}


	template <typename V, typename K, typename HF, typename ExK, typename EqK, typename A>
	typename _hashtable_iterator<V, K, HF, ExK, EqK, A>::iterator 
		_hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++(int) {
		auto tmp = *this;
		++*this;
		return tmp;
	}


	static const int _gruel_num_primes = 28;
	static const unsigned long _gruel_prime_list[_gruel_num_primes] = {
		53, 97, 193, 389, 769,
		1543, 3079, 6151, 12289, 24593,
		49157, 98317, 1996613, 393241, 786433,
		1572869, 3145739, 6291469, 12582917, 25165843,
		50331653, 100663319, 201326611, 402653189, 805306457,
		1610612741, 3221225473ul, 429496729ul
	};


	inline unsigned long _gruel_next_prime(unsigned long n) {
		auto first = _gruel_prime_list;
		auto last = _gruel_prime_list + _gruel_num_primes;
		auto pos = std::lower_bound(first, last, n);
		return (pos == last) ? *(last - 1) : *pos;
	}


	template <typename Value, typename Key, typename HashFun, typename ExtractKey, 
		typename EqualKey, typename Alloc = alloc>
		class hashtable {
		friend _hashtable_iterator<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>;
		public:
			using hasher = HashFun;
			using key_equal = EqualKey;
			using size_type = std::size_t;
			using value_type = Value;
			using key_type = Key;
			using pointer = value_type * ;
			using const_pointer = const value_type *;
			using reference = value_type & ;
			using iterator = _hashtable_iterator<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>;
			using const_iterator = _hashtable_iterator<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>;
			using const_reference = const value_type &;
			using difference_type = std::ptrdiff_t;

		private:
			hasher hash;
			key_equal equals;
			ExtractKey get_key;

			using node = _hashtable_node<Value>;
			using node_allocator = simple_alloc<node, Alloc>;

			vector<node *, Alloc> buckets;
			size_type num_elements;

			node *new_node(const value_type &obj) {
				node *n = node_allocator::allocate();
				n->next = nullptr;
				construct(&n->val, obj);
				return n;
			}

			void delete_node(node *n) {
				destory(n->val);
				node_allocator::deallocate(n);
			}

			void initialize_buckets(size_type n) {
				const size_type n_buckets = next_size(n);
				buckets.reserve(n_buckets);
				buckets.insert(buckets.end(), n_buckets, nullptr);
				num_elements = 0;
			}

			std::pair<iterator, bool> insert_unique_noresize(const value_type &obj);

			iterator insert_equal_noresize(const value_type &obj);

			size_type bkt_num(const value_type &obj, std::size_t n)const {
				return bkt_num_key(get_key(obj), n);
			}

			size_type bkt_num(const value_type &obj) const {
				return bkt_num_key(get_key(obj));
			}

			size_type bkt_num_key(const key_type &key) const {
				return bkt_num_key(key, buckets.size());
			}

			size_type bkt_num_key(const key_type &key, std::size_t n) const {
				return hash(key) % n;
			}

		public:

			hashtable(size_type n, const HashFun &hf, const EqualKey &eql) :
				hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0) {
				initialize_buckets(n);
			}

			size_type size() const { return num_elements; }
			size_type bucket_count() const { return buckets.size(); }
			size_type elems_in_bucket(size_type i) const {
				size_type result = 0;
				for (node *n = buckets[i]; n; ++result, n = n->next);
				return result;
			}
			size_type max_bucket_count() const { return _gruel_prime_list[_gruel_num_primes - 1]; }
			size_type next_size(size_type n) const { return _gruel_next_prime(n); }
			void resize(size_type num_elements_hint);

			iterator begin() { return iterator(*buckets.begin(), this); }
			const_iterator begin() const { return iterator(*buckets.begin(), this); }
			iterator end() { return iterator(nullptr, this); }
			const_iterator end() const { return iterator(nullptr, this); }


			std::pair<iterator, bool> insert_unique(const value_type &obj) {
				resize(num_elements + 1);
				return insert_unique_noresize(obj);
			}

			iterator insert_equal(const value_type &obj) {
				resize(num_elements + 1);
				return insert_equal_noresize(obj);
			}

			void clear();

			void copy_from(const hashtable &ht);

			iterator find(const key_type &key) {
				const size_type n = bkt_num(key);
				node *first;
				for (first = buckets[n]; first && !equals(get_key(first->val), key); first = first->next);
				return { first, this };
			}

			size_type count(const key_type &key) const {
				const size_type n = bkt_num(key);
				size_type result = 0;
				for (const node *cur = buckets[n]; cur; cur = cur->next)
					if (equals(get_key(cur->val), key))
						++result;
				return result;
			}
	};


	template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
	void hashtable<V, K, HF, Ex, Eq, A>::resize(size_type num_elements_hint) {
		const size_type old_n = buckets.size();
		if (num_elements_hint > old_n) {
			const size_type n = next_size(num_elements_hint);
			if (n > old_n) {
				vector<node *, A> tmp(n, nullptr);
				for (size_type bucket = 0; bucket < old_n; ++bucket) {
					node *first = buckets[bucket];
					while (first) {
						size_type new_bucket = bkt_num(first->val, n);
						buckets[bucket] = first->next;
						first->next = tmp[new_bucket];
						tmp[new_bucket] = first;
						first = buckets[bucket];
					}
				}
				buckets.swap(tmp);
			}
		}
	}


	template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
	std::pair<typename hashtable<V, K, HF, Ex, Eq, A>::iterator, bool>
		hashtable<V, K, HF, Ex, Eq, A>::insert_unique_noresize(const value_type &obj) {
		const size_type n = bkt_num(obj);
		node *first = buckets[n];
		for (node *cur = first; cur; cur = cur->next)
			if (equals(get_key(cur->val), get_key(obj)))
				return { iterator(cur, this), false };

		node *tmp = new_node(obj);
		tmp->next = first;
		buckets[n] = tmp;
		++num_elements;
		return { iterator(tmp, this), true };
	}


	template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
	typename hashtable<V, K, HF, Ex, Eq, A>::iterator
		hashtable<V, K, HF, Ex, Eq, A>::insert_equal_noresize(const value_type &obj) {
		const size_type n = bkt_num(obj);
		node *first = buckets[n];
		for (node *cur = first; cur; cur = cur->next)
			if (equals(get_key(cur->val), get_key(obj))) {
				node *tmp = new_node(obj);
				tmp->next = cur->next;
				cur->next = tmp;
				++num_elements;
				return { tmp, this };
			}

		node *tmp = new_node(obj);
		tmp->next = first;
		buckets[n] = tmp;
		++num_elements;
		return { tmp, this };
	}


	template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
	void hashtable<V, K, HF, Ex, Eq, A>::clear() {
		for (size_type n = 0; n < buckets.size(); ++n) {
			node *cur = buckets[n];
			while (cur) {
				node *next = cur->next;
				delete_node(cur);
				cur = next;
			}
			buckets[n] = nullptr;
		}
		num_elements = 0;
	}


	template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
	void hashtable<V, K, HF, Ex, Eq, A>::copy_from(const hashtable &ht) {
		buckets.clear();
		size_type ht_size = ht.buckets.size();
		buckets.reserve(ht_size);
		buckets.insert(buckets.end(), ht_size, nullptr);
		for (size_type i = 0; i < ht_size; ++i) {
			const node *cur = ht.buckets[i];
			if (cur) {
				node *copy = new_node(cur->val);
				buckets[i] = copy;

				for (node *next = cur->next; next; cur = next, next = cur->next) {
					copy->next = new_node(next->val);
					copy = copy->next;
				}
			}
		}
		num_elements = ht.num_elements;
	}
}
