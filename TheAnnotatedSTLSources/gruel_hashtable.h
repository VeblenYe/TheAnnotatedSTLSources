#pragma once


#include "gruel_memory.h"
#include "gruel_hash_fun.h"
#include "gruel_vector.h"
#include "gruel_functional.h"


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

	
	// ǰ������
	template <typename Value, typename Key, typename HashFun, typename ExtractKey,
		typename EqualKey, typename Alloc>
		struct _hashtable_iterator;

	template <typename Value, typename Key, typename HashFun, typename ExtractKey,
		typename EqualKey, typename Alloc>
		struct _hashtable_const_iterator;

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
		using const_iterator = _hashtable_const_iterator<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>;
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

		reference operator*() const { return cur->val; }
		pointer operator->() const { return &(operator*()); }
		// ע��û�ж�����˲���
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


	/* hashtable�ĳ��������� */
	template <typename Value, typename Key, typename HashFun, typename ExtractKey,
		typename EqualKey, typename Alloc = alloc>
		struct _hashtable_const_iterator {

		// Ƕ�׶���
		using hashtable = hashtable<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>;
		using iterator = _hashtable_iterator<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>;
		using const_iterator = _hashtable_const_iterator<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>;
		using node = _hashtable_node<Value>;

		using iterator_category = forward_iterator_tag;
		using value_type = Value;
		using pointer = const value_type * ;
		using reference = const value_type & ;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		const node *cur;	// ָ��ǰ�ڵ�
		const hashtable *ht;	// ��hashtable�������ӣ���Ϊ��ʱҪ�����µ�bucket

		_hashtable_const_iterator() {}
		_hashtable_const_iterator(const node *n, const hashtable *tab) : cur(n), ht(tab) {}
		// ע���������ת�����캯��
		_hashtable_const_iterator(const iterator &it) : cur(it.cur), ht(it.ht) {}

		reference operator*() const { return cur->val; }
		pointer operator->() const { return &(operator*()); }
		// ע��û�ж�����˲���
		const_iterator &operator++();
		const_iterator operator++(int);
		// �е�
		bool operator==(const const_iterator &it) const { return cur == it.cur; }
		bool operator!=(const const_iterator &it) const { return cur != it.cur; }
	};


	template <typename V, typename K, typename HF, typename ExK, typename EqK, typename A>
	typename _hashtable_const_iterator<V, K, HF, ExK, EqK, A>::const_iterator &
		_hashtable_const_iterator<V, K, HF, ExK, EqK, A>::operator++() {

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

	template <typename V, typename K, typename HF, typename ExK, typename EqK, typename A>
	typename _hashtable_const_iterator<V, K, HF, ExK, EqK, A>::const_iterator
		_hashtable_const_iterator<V, K, HF, ExK, EqK, A>::operator++(int) {
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
		friend _hashtable_const_iterator<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>;
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
			using const_reference = const value_type &;
			using difference_type = std::ptrdiff_t;

			using iterator = _hashtable_iterator<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>;
			using const_iterator = _hashtable_const_iterator<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>;

		private:
			// ע�����ﶼ����ʵ��
			hasher hash;
			key_equal equals;
			ExtractKey get_key;

			// ��nodeΪ��λ����ռ�
			using node = _hashtable_node<Value>;
			using node_allocator = simple_alloc<node, Alloc>;

			using self = hashtable;

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
				destory(&n->val);
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

			pair<iterator, bool> insert_unique_noresize(const value_type &obj);

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

			void erase_bucket(const size_type n, node *first, node *last) {
				node *cur = buckets[n];
				if (cur == first)
					return erase_bucket(n, last);
				else {
					node *next;
					// �ҵ�first
					for (next = cur->next; next != first; cur = next, next = cur->next);
					while (next != last) {
						cur->next = next->next;
						delete_node(next);
						next = cur->next;
						--num_elements;
					}
				}
			}
			void erase_bucket(const size_type n, node *last) {
				node *cur = buckets[n];
				while (cur != last) {
					node *next = cur->next;
					delete_node(cur);
					cur = next;
					buckets[n] = cur;
					--num_elements;
				}
			}

		public:

			// ���캯����ע��û��Ĭ�Ϲ��캯��
			hashtable(size_type n, const HashFun &hf, const EqualKey &eql) :
				hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0) {
				initialize_buckets(n);
			}

			hashtable(const hashtable &ht) : hash(ht.hash_funct()), equals(ht.key_eq()), get_key(ht.get_key) {
				copy_from(ht);
			}

			self &operator=(const self &ht) {
				if (this != &ht) {
					clear();
					hash = ht.hash;
					equals = ht.equals;
					get_key = ht.get_key;
					copy_from(ht);
				}
				return *this;
			}

			~hashtable() { clear(); }

			hasher hash_funct() const { return hash; }
			key_equal key_eq() const { return equals; }

			size_type size() const { return num_elements; }
			size_type max_size() const { return size_type(-1); }
			bool empty() const { return num_elements == 0; }
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
			iterator begin() {
				for (size_type n = 0; n < buckets.size(); ++n)
					if (buckets[n])
						return iterator(buckets[n], this);
				return end();
			}
			const_iterator begin() const {
				for (size_type n = 0; n < buckets.size(); ++n)
					if (buckets[n])
						// ���ִ����޷���"initializer list"ת��Ϊ"gruel::_hashtable_iterator<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>
						// ʲô�������Ƿ���һ����ʱ����
						// ԭ���Ǵ���Ĳ���Ϊconst��������û�ж�Ӧ�Ĺ��캯��
						// �������Ϊ����һ���µ�const������
						return const_iterator(buckets[n], this);
				return end();
			}
			iterator end() { return iterator(nullptr, this); }
			const_iterator end() const { return const_iterator(nullptr, this); }

			reference find_or_insert(const value_type &obj) {
				resize(num_elements + 1);

				const size_type n = bkt_num(obj);
				node *first = buckets[n];

				for (node *cur = first; cur; cur = cur->next)
					if (equals(get_key(cur->val)), get_key(obj))
						return cur->val;

				node *tmp = new_node(obj);
				tmp->next = first;
				buckets[n] = tmp;
				++num_elements;
				return tmp->val;
			}

			// ���룬�����ظ�
			pair<iterator, bool> insert_unique(const value_type &obj) {
				resize(num_elements + 1);
				return insert_unique_noresize(obj);
			}

			template <typename InputIterator>
			void insert_unique(InputIterator first, InputIterator last) {
				size_type n = distance(first, last);
				resize(num_elements + n);
				while (first != last)
					insert_unique_noresize(*first++);
			}

			// ���룬���ظ�
			iterator insert_equal(const value_type &obj) {
				resize(num_elements + 1);
				return insert_equal_noresize(obj);
			}

			template <typename InputIterator>
			void insert_equal(InputIterator first, InputIterator last) {
				size_type n = distance(first, last);
				resize(num_elements + n);
				while (first != last)
					insert_equal_noresize(*first++);
			}

			// ɾ��ָ����ֵԪ�أ�������Ԫ�ظ���
			// ѧϰrb_tree������equal_range��ע���ɲ���ѧϰ����������������
			// ˼·�����뼴��
			size_type erase(const key_type &key) {
				const size_type n = bkt_num_key(key);
				node *first = buckets[n];
				size_type erased = 0;

				if (first) {
					node *cur = first;
					node *next = cur->next;
					while (next) {
						if (equals(get_key(next->val), key)) {
							cur->next = next->next;
							delete_node(next);
							next = cur->next;
							++erased;
							--num_elements;
						}
						else {
							cur = next;
							next = cur->next;
						}
					}
					if (equals(get_key(first->val), key)) {
						buckets[n] = first->next;
						delete_node(first);
						++erased;
						--num_elements;
					}
				}
				return erased;
			}
			// �е����Ƶ��������ɾ��������������Ч�ʺõͰ�
			void erase(const iterator &it) {
				node *p = it.cur;
				if (p) {
					const size_type n = bkt_num(p->val);
					node *cur = buckets[n];

					// ���Ҫɾ���Ľڵ�Ϊͷ�ڵ�
					if (cur == p) {
						buckets[n] = cur->next;
						delete_node(cur);
						--num_elements;
					}
					else {
						node *next = cur->next;
						while (next) {
							if (next == p) {
								cur->next = next->next;
								delete_node(next);
								--num_elements;
								break;
							}
							else {
								cur = next;
								next = cur->next;
							}
						}
					}
				}
			}
			// ���ɾ��Ҳ������ģ��е�����deque
			void erase(iterator first, iterator last) {
				// ���ҵ������ڵ�Ͱ
				size_type first_bucket = first.cur ? get_key(first.cur->val) : buckets.size();
				size_type last_bucket = last.cur ? get_key(last.cur->val) : buckets.size();

				if (first == last)
					return;
				// ��ͬһ��Ͱ
				else if (first_bucket == last_bucket)
					erase_bucket(first_bucket, first.cur, last.cur);
				// ��ͬ��Ͱ
				else {
					erase_bucket(first_bucket, first.cur, nullptr);
					for (size_type n = first_bucket + 1; n < last_bucket; ++n)
						erase_bucket(n, nullptr);
					if (last_bucket == buckets.size())
						erase_bucket(last_bucket, last.cur);
				}
			}

			// �����ת�ͺ���ת�����ˣ�ע��ֻ�з�constתconst�ſ���
			void erase(const const_iterator &it) {
				erase(iterator(const_cast<node *>(it.cur), const_cast<hashtable *>(it.ht)));
			}
			void erase(const_iterator first, const_iterator last) {
				erase(iterator(const_cast<node *>(first.cur), const_cast<hashtable *>(first.ht)),
					iterator(const_cast<node *>(last.cur), const_cast<hashtable *>(last.ht)));
			}

			// ��սڵ�
			void clear();

			// ����һ��hashtable����
			void copy_from(const hashtable &ht);

			// ����Ԫ��
			iterator find(const key_type &key) {
				const size_type n = bkt_num_key(key);	// ȷ��Ͱ
				node *first;
				// �ڸ�Ͱ�в���
				for (first = buckets[n]; first && !equals(get_key(first->val), key); first = first->next);
				return { first, this };
			}

			// �������Ԫ�ظ���
			size_type count(const key_type &key) const {
				const size_type n = bkt_num_key(key);	// ȷ��Ͱ
				size_type result = 0;
				// �ڸ�Ͱ�в���ͳ��
				for (const node *cur = buckets[n]; cur; cur = cur->next)
					if (equals(get_key(cur->val), key))
						++result;
				return result;
			}

			// ����ԭ����Ĳ��
			pair<iterator, iterator> equal_range(const key_type &key) {
				const size_type n = bkt_num_key(key);

				for (node *first = buckets[n]; first; first = first->next)
					if (equals(get_key(first->val), key)) {
						for (node *cur = first; cur; cur = cur->next)
							if (!equals(get_key(first->val), key))
								return { iterator(first, this), iterator(cur, this) };
						for (size_type m = n + 1; m < buckets.size(); ++m)
							if (buckets[m])
								return { iterator(first, this), iterator(buckets[m], this) };
						return { iterator(first, this), end() };
					}

				return { end(), end() };
			}

			pair<const_iterator, const_iterator> equal_range(const key_type &key) const {
				const size_type n = bkt_num_key(key);

				for (node *first = buckets[n]; first; first = first->next)
					if (equals(get_key(first->val), key)) {
						for (node *cur = first; cur; cur = cur->next)
							if (!equals(get_key(first->val), key))
								return { const_iterator(first, this), const_iterator(cur, this) };
						for (size_type m = n + 1; m < buckets.size(); ++m)
							if (buckets[m])
								return { const_iterator(first, this), const_iterator(buckets[m], this) };
						return { const_iterator(first, this), end() };
					}

				return { end(), end() };
			}

			void swap(hashtable &ht) noexcept {
				using std::swap;
				buckets.swap(ht.buckets);
				swap(num_elements, ht.num_elements);
				swap(hash, ht.hash);
				swap(equals, ht.equals);
				swap(get_key, ht.get_key);
				// ���ɲ������ô����������������Ҳ����û�б�Ҫ����
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
				// һ��ʼ��д��forѭ�����
				buckets.swap(tmp);
			}
		}
	}


	// ���벻�ظ�Ԫ�ص�ʵ�ʲ�����NoResize��Ϊ���ﲻ�ı�hashtableͰ����
	template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
	pair<typename hashtable<V, K, HF, Ex, Eq, A>::iterator, bool>
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
