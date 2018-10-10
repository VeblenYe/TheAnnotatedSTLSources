#pragma once


#include "gruel_memory.h"
#include "gruel_hash_fun.h"
#include "gruel_vector.h"


namespace gruel {
	
	/* ������hashtable���ṩ���κ������named item)�Ĵ�ȡ������ɾ�����������Ա���Ϊһ��
	�ֵ�ṹ��dictionary)�����ֽṹ�����������ṩ����ʱ��Ļ������� */

	/* ��hashtableʵ�ֵķ����ǿ�������separate chaining)����Ҫ��˼·������һ���ײ�����
	�䵱��Ͱ��bucket������ÿ��bucket��װ�����ɸ����ڵ㣨node������Ҳ����Ԫ�ء������ǲ���node
	�Ƿ������hashtable��ʱ�������ͨ��hash�������ٶ�λbucket��Ȼ����bucket�в��ҡ�����
	��ÿ��Ͱ�е�Ԫ���������Ƶľ���ƽ���������Ĳ���Ч�ʾ��ܽӽ��������Ӷȡ�ÿ��bucket�нڵ�
	����֯��ʽΪ������û�������ṩ���������ǵ���ά�� */


	// �ڵ㣨node��ģ����
	template <typename Value>
	struct _hashtable_node {
		_hashtable_node *next;
		Value val;
	};

	
	// hashtable��ǰ������
	template <typename Value, typename Key, typename HashFun, typename ExtractKey,
		typename EqualKey, typename Alloc>
		class hashtable;


	/* hashtable�ĵ�������HashFun��ExtractKey��EqualKey���Ƿº���������ֱ�Ϊ
	��ϣ��������ȡ��ֵ���жϼ�ֵ�Ƿ���� */
	// ��ʵ�������ﲢû����ô�õ��������ͣ��²���Ϊ�˶���hashtable����
	template <typename Value, typename Key, typename HashFun, typename ExtractKey, 
		typename EqualKey, typename Alloc = alloc>
		struct _hashtable_iterator {

		// Ƕ�׶���
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

		node *cur;	// ָ��ǰ�ڵ�
		hashtable *ht;	// ��hashtable�������ӣ���Ϊ��ʱҪ�����µ�bucket

		_hashtable_iterator() {}
		_hashtable_iterator(node *n, hashtable *tab) : cur(n), ht(tab) {}

		reference operator*() { return cur->val; }
		pointer operator->() { return &(operator*()); }
		iterator &operator++();
		iterator operator++(int);
		// �е�
		bool operator==(const iterator &it) const { return cur == it.cur; }
		bool operator!=(const iterator &it) const { return cur != it.cur; }
	};


	// _hashtable_iterator��ǰ��������ע�������bucket�����
	template <typename V, typename K, typename HF, typename ExK, typename EqK, typename A>
	typename _hashtable_iterator<V, K, HF, ExK, EqK, A>::iterator &
		_hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++() {

		const node *old = cur;	// ��¼��ǰ�ڵ㣬����ȷ��"bucket"
		cur = cur->next;	// ��һ�ڵ�
		if (!cur) {
			// �����һ�ڵ㲻���ڣ�����Ҫ��bucket
			size_type bucket = ht->bkt_num(old->val);	// �ҵ���ǰ�ڵ��bucket
			while (!cur && ++bucket < ht->buckets.size())
				// ������һ����node���ڵ�bucket
				cur = ht->buckets[bucket];
		}

		return *this;
	}


	// ��������
	template <typename V, typename K, typename HF, typename ExK, typename EqK, typename A>
	typename _hashtable_iterator<V, K, HF, ExK, EqK, A>::iterator 
		_hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++(int) {
		auto tmp = *this;
		++*this;
		return tmp;
	}


	// ��˵������������Ҫ�������������ṩ����������Ͱ
	// ����28��������ÿ��������С���Ϊǰһ����������
	// ��֪bucket��������Ϊ429496729
	static const int _gruel_num_primes = 28;
	static const unsigned long _gruel_prime_list[_gruel_num_primes] = {
		53, 97, 193, 389, 769,
		1543, 3079, 6151, 12289, 24593,
		49157, 98317, 1996613, 393241, 786433,
		1572869, 3145739, 6291469, 12582917, 25165843,
		50331653, 100663319, 201326611, 402653189, 805306457,
		1610612741, 3221225473ul, 429496729ul
	};


	// ����n�ƶϳ����ʵ�Ͱ�ĸ���
	inline unsigned long _gruel_next_prime(unsigned long n) {
		auto first = _gruel_prime_list;
		auto last = _gruel_prime_list + _gruel_num_primes;
		// ���÷����㷨lower_bound
		auto pos = std::lower_bound(first, last, n);
		return (pos == last) ? *(last - 1) : *pos;
	}


	// hashtable���ݽṹ�����Ͳ��������������ͬ
	template <typename Value, typename Key, typename HashFun, typename ExtractKey, 
		typename EqualKey, typename Alloc = alloc>
		class hashtable {
		// ��Ԫ������
		friend _hashtable_iterator<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>;
		public:
			// ��Ƕ����
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
			// ע�����ﶼ����ʵ��
			hasher hash;
			key_equal equals;
			ExtractKey get_key;

			// ��nodeΪ��λ����ռ�
			using node = _hashtable_node<Value>;
			using node_allocator = simple_alloc<node, Alloc>;

			// bucket��ʵ�ֲ���vector
			vector<node *, Alloc> buckets;
			size_type num_elements;	// �ڵ����

			// ����һ���½ڵ�
			node *new_node(const value_type &obj) {
				node *n = node_allocator::allocate();
				n->next = nullptr;
				construct(&n->val, obj);
				return n;
			}

			// ɾ���ڵ�
			void delete_node(node *n) {
				destory(n->val);
				node_allocator::deallocate(n);
			}

			// ��ʼ��buckets
			void initialize_buckets(size_type n) {
				const size_type n_buckets = next_size(n);	// �ҳ�����Ͱ��
				// �����ռ䲢����Ϊnullptr
				buckets.reserve(n_buckets);	// reserveֻ���ÿռ䲻����Ԫ��
				buckets.insert(buckets.end(), n_buckets, nullptr);
				num_elements = 0;
			}

			std::pair<iterator, bool> insert_unique_noresize(const value_type &obj);

			iterator insert_equal_noresize(const value_type &obj);

			// ����һϵ�к���ȷ���ĸ�Ͱ����ʵ����ȡ�࣬�и����ӵ�hash�㷨
			// ������ȫ��ϣ�ȵȣ�������û�в��ã������˺ܶ��Ѷ�
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

			// ���캯����ע��û��Ĭ�Ϲ��캯��
			hashtable(size_type n, const HashFun &hf, const EqualKey &eql) :
				hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0) {
				initialize_buckets(n);
			}

			size_type size() const { return num_elements; }
			// ����Ͱ�ĸ���
			size_type bucket_count() const { return buckets.size(); }
			// ����ÿ��Ͱ�еĽڵ����
			size_type elems_in_bucket(size_type i) const {
				size_type result = 0;
				for (node *n = buckets[i]; n; ++result, n = n->next);
				return result;
			}
			// Ͱ��������
			size_type max_bucket_count() const { return _gruel_prime_list[_gruel_num_primes - 1]; }
			// ��һ�����ʵ�Ͱ����
			size_type next_size(size_type n) const { return _gruel_next_prime(n); }
			// ���·���ռ䣬һ������Ҫ�������ǽ��͸������Ӧ�
			void resize(size_type num_elements_hint);

			// ͷβ���������Լ�д�ģ�Ӧ��������
			iterator begin() { return iterator(*buckets.begin(), this); }
			const_iterator begin() const { return iterator(*buckets.begin(), this); }
			iterator end() { return iterator(nullptr, this); }
			const_iterator end() const { return iterator(nullptr, this); }

			// ���룬�����ظ�
			std::pair<iterator, bool> insert_unique(const value_type &obj) {
				resize(num_elements + 1);
				return insert_unique_noresize(obj);
			}

			// ���룬���ظ�
			iterator insert_equal(const value_type &obj) {
				resize(num_elements + 1);
				return insert_equal_noresize(obj);
			}

			// ��սڵ�
			void clear();

			// ����һ��hashtable����
			void copy_from(const hashtable &ht);

			// ����Ԫ��
			iterator find(const key_type &key) {
				const size_type n = bkt_num(key);	// ȷ��Ͱ
				node *first;
				// �ڸ�Ͱ�в���
				for (first = buckets[n]; first && !equals(get_key(first->val), key); first = first->next);
				return { first, this };
			}

			// �������Ԫ�ظ���
			size_type count(const key_type &key) const {
				const size_type n = bkt_num(key);	// ȷ��Ͱ
				size_type result = 0;
				// �ڸ�Ͱ�в���ͳ��
				for (const node *cur = buckets[n]; cur; cur = cur->next)
					if (equals(get_key(cur->val), key))
						++result;
				return result;
			}
	};


	// ��������hashtable������Ҫ��������¿ռ䣬�ƶ�Ԫ�أ�ɾ���ɿռ�Ĳ�����ʵ�ʵ�ʵ��������һЩ���ɣ�ֵ��ѧϰ
	template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
	void hashtable<V, K, HF, Ex, Eq, A>::resize(size_type num_elements_hint) {
		// ע��������ж���׼�Ǻ�Ͱ�ĸ������бȽϣ�Ϊ��ά�ֽ�С�Ħ�������߲��Ч��
		const size_type old_n = buckets.size();	// �ɿռ��С
		// �¿ռ��С���ھɿռ��С�Ž�������
		if (num_elements_hint > old_n) {
			const size_type n = next_size(num_elements_hint);	// �ҳ�����Ͱ��
			if (n > old_n) {
				// ����һ����ʱvector����ռ��СΪn������Ԫ��ȫ����nullptr
				vector<node *, A> tmp(n, nullptr);
				// һ��Ͱһ��Ͱ����Ԫ�أ�������һЩ����
				for (size_type bucket = 0; bucket < old_n; ++bucket) {
					node *first = buckets[bucket];	// ��ǰͰ���׽ڵ�
					while (first) {
						size_type new_bucket = bkt_num(first->val, n);	// ȷ���ýڵ�����buckets��λ��
						buckets[bucket] = first->next;	// ����Ͱ�׽ڵ�ָ����һ�ڵ�
						first->next = tmp[new_bucket];	// ��ԭ�׽ڵ�ָ����buckets����Ӧ�ڵ�
						tmp[new_bucket] = first;	// ��buckets����Ӧ�ڵ��׽ڵ�ָ��ԭ�׽ڵ㣬��ɽڵ�ת��
						first = buckets[bucket];	// �׽ڵ㻻Ϊ��һ�ڵ��������
					}
				}
				// ���ֱ�ӽ�����ʱvector��tmp�ں�������������������������
				// ��ֱ����������ԭ���Ҿ�����begin()��end()��ʵ���й�
				buckets.swap(tmp);
			}
		}
	}


	// ���벻�ظ�Ԫ�ص�ʵ�ʲ�����NoResize��Ϊ���ﲻ�ı�hashtableͰ����
	template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
	std::pair<typename hashtable<V, K, HF, Ex, Eq, A>::iterator, bool>
		hashtable<V, K, HF, Ex, Eq, A>::insert_unique_noresize(const value_type &obj) {
		const size_type n = bkt_num(obj);	// ���Ҷ�ӦͰ
		node *first = buckets[n];
		// ���Ͱ���м�ֵ��ȵĽڵ㣬�����ʧ��
		for (node *cur = first; cur; cur = cur->next)
			if (equals(get_key(cur->val), get_key(obj)))
				return { iterator(cur, this), false };

		// û����Ƚڵ㣬��ֱ�Ӳ��뵽Ͱ����ʼλ��
		node *tmp = new_node(obj);
		tmp->next = first;
		buckets[n] = tmp;
		++num_elements;
		return { iterator(tmp, this), true };
	}


	// �����ظ�Ԫ�ص�ʵ�ʲ���
	template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
	typename hashtable<V, K, HF, Ex, Eq, A>::iterator
		hashtable<V, K, HF, Ex, Eq, A>::insert_equal_noresize(const value_type &obj) {
		const size_type n = bkt_num(obj);	// ������Ӧ��Ͱ
		node *first = buckets[n];
		// ��Ͱ�в��ң����м�ֵ��Ƚڵ㣬����뵽�ýڵ�֮��
		for (node *cur = first; cur; cur = cur->next)
			if (equals(get_key(cur->val), get_key(obj))) {
				node *tmp = new_node(obj);
				tmp->next = cur->next;
				cur->next = tmp;
				++num_elements;
				return { tmp, this };
			}
		
		// û����Ƚڵ���ֱ�Ӳ��뵽Ͱ��ʼλ��
		node *tmp = new_node(obj);
		tmp->next = first;
		buckets[n] = tmp;
		++num_elements;
		return { tmp, this };
	}


	// ��սڵ㣬ע������굫buckets���ڣ���Ͱ������
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


	// ����һhashtable����
	template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
	void hashtable<V, K, HF, Ex, Eq, A>::copy_from(const hashtable &ht) {
		// ���ԭhashtable
		buckets.clear();
		size_type ht_size = ht.buckets.size();
		// ƥ��ht��Ͱ����
		buckets.reserve(ht_size);
		buckets.insert(buckets.end(), ht_size, nullptr);
		// ���Ͱ����
		for (size_type i = 0; i < ht_size; ++i) {
			const node *cur = ht.buckets[i];
			// ���жϵ�ǰͰ�Ƿ���ڽڵ�
			if (cur) {
				node *copy = new_node(cur->val);
				buckets[i] = copy;

				// ����ڵ㸴��
				for (node *next = cur->next; next; cur = next, next = cur->next) {
					copy->next = new_node(next->val);
					copy = copy->next;
				}
			}
		}
		// ���ýڵ����
		num_elements = ht.num_elements;
	}
}
