#pragma once


#include "gruel_memory.h"
#include "gruel_hash_fun.h"
#include "gruel_vector.h"
#include "gruel_functional.h"


namespace gruel {
	
	/* 综述：hashtable可提供对任何有名项（named item)的存取操作和删除操作，可以被视为一种
	字典结构（dictionary)，这种结构的用意在于提供常数时间的基本操作 */

	/* 该hashtable实现的方法是开链法（separate chaining)，简要的思路是利用一个底层容器
	充当“桶（bucket）”，每个bucket里装有若干个“节点（node）”，也就是元素。当我们查找node
	是否存在于hashtable里时，则可以通过hash函数快速定位bucket，然后在bucket中查找。我们
	将每个桶中的元素数量控制的尽量平均，这样的查找效率就能接近常数复杂度。每个bucket中节点
	的组织方式为链表，且没有利用提供的容器而是单独维护 */


	// 节点（node）模板类
	template <typename Value>
	struct _hashtable_node {
		_hashtable_node *next;
		Value val;
	};

	
	// 前置声明
	template <typename Value, typename Key, typename HashFun, typename ExtractKey,
		typename EqualKey, typename Alloc>
		struct _hashtable_iterator;

	template <typename Value, typename Key, typename HashFun, typename ExtractKey,
		typename EqualKey, typename Alloc>
		struct _hashtable_const_iterator;

	template <typename Value, typename Key, typename HashFun, typename ExtractKey,
		typename EqualKey, typename Alloc>
		class hashtable;


	/* hashtable的迭代器，HashFun、ExtractKey和EqualKey都是仿函数，意义分别为
	哈希函数、提取键值和判断键值是否相等 */
	// 其实迭代器里并没有怎么用到上述类型，猜测是为了定义hashtable类型
	template <typename Value, typename Key, typename HashFun, typename ExtractKey, 
		typename EqualKey, typename Alloc = alloc>
		struct _hashtable_iterator {

		// 嵌套定义
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

		node *cur;	// 指向当前节点
		hashtable *ht;	// 与hashtable保持连接，因为有时要跳到新的bucket

		_hashtable_iterator() {}
		_hashtable_iterator(node *n, hashtable *tab) : cur(n), ht(tab) {}

		reference operator*() const { return cur->val; }
		pointer operator->() const { return &(operator*()); }
		// 注意没有定义后退操作
		iterator &operator++();
		iterator operator++(int);
		// 判等
		bool operator==(const iterator &it) const { return cur == it.cur; }
		bool operator!=(const iterator &it) const { return cur != it.cur; }
	};


	// _hashtable_iterator的前置自增，注意会有跳bucket的情况
	template <typename V, typename K, typename HF, typename ExK, typename EqK, typename A>
	typename _hashtable_iterator<V, K, HF, ExK, EqK, A>::iterator &
		_hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++() {

		const node *old = cur;	// 记录当前节点，用于确定"bucket"
		cur = cur->next;	// 下一节点
		if (!cur) {
			// 如果下一节点不存在，则需要跳bucket
			size_type bucket = ht->bkt_num(old->val);	// 找到当前节点的bucket
			while (!cur && ++bucket < ht->buckets.size())
				// 查找下一个有node存在的bucket
				cur = ht->buckets[bucket];
		}

		return *this;
	}


	// 后置自增
	template <typename V, typename K, typename HF, typename ExK, typename EqK, typename A>
	typename _hashtable_iterator<V, K, HF, ExK, EqK, A>::iterator 
		_hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++(int) {
		auto tmp = *this;
		++*this;
		return tmp;
	}


	/* hashtable的常量迭代器 */
	template <typename Value, typename Key, typename HashFun, typename ExtractKey,
		typename EqualKey, typename Alloc = alloc>
		struct _hashtable_const_iterator {

		// 嵌套定义
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

		const node *cur;	// 指向当前节点
		const hashtable *ht;	// 与hashtable保持连接，因为有时要跳到新的bucket

		_hashtable_const_iterator() {}
		_hashtable_const_iterator(const node *n, const hashtable *tab) : cur(n), ht(tab) {}
		// 注意这个类型转换构造函数
		_hashtable_const_iterator(const iterator &it) : cur(it.cur), ht(it.ht) {}

		reference operator*() const { return cur->val; }
		pointer operator->() const { return &(operator*()); }
		// 注意没有定义后退操作
		const_iterator &operator++();
		const_iterator operator++(int);
		// 判等
		bool operator==(const const_iterator &it) const { return cur == it.cur; }
		bool operator!=(const const_iterator &it) const { return cur != it.cur; }
	};


	template <typename V, typename K, typename HF, typename ExK, typename EqK, typename A>
	typename _hashtable_const_iterator<V, K, HF, ExK, EqK, A>::const_iterator &
		_hashtable_const_iterator<V, K, HF, ExK, EqK, A>::operator++() {

		const node *old = cur;	// 记录当前节点，用于确定"bucket"
		cur = cur->next;	// 下一节点
		if (!cur) {
			// 如果下一节点不存在，则需要跳bucket
			size_type bucket = ht->bkt_num(old->val);	// 找到当前节点的bucket
			while (!cur && ++bucket < ht->buckets.size())
				// 查找下一个有node存在的bucket
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


	// 虽说开链法并不需要质数，但还是提供质数数量的桶
	// 共有28个质数，每个质数大小差不多为前一个质数两倍
	// 可知bucket的最大个数为429496729
	static const int _gruel_num_primes = 28;
	static const unsigned long _gruel_prime_list[_gruel_num_primes] = {
		53, 97, 193, 389, 769,
		1543, 3079, 6151, 12289, 24593,
		49157, 98317, 1996613, 393241, 786433,
		1572869, 3145739, 6291469, 12582917, 25165843,
		50331653, 100663319, 201326611, 402653189, 805306457,
		1610612741, 3221225473ul, 429496729ul
	};


	// 根据n推断出合适的桶的个数
	inline unsigned long _gruel_next_prime(unsigned long n) {
		auto first = _gruel_prime_list;
		auto last = _gruel_prime_list + _gruel_num_primes;
		// 调用泛型算法lower_bound
		auto pos = std::lower_bound(first, last, n);
		return (pos == last) ? *(last - 1) : *pos;
	}


	// hashtable数据结构，类型参数意义与迭代器同
	template <typename Value, typename Key, typename HashFun, typename ExtractKey, 
		typename EqualKey, typename Alloc = alloc>
		class hashtable {
		// 友元类声明
		friend _hashtable_iterator<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>;
		friend _hashtable_const_iterator<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>;
		public:
			// 内嵌定义
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
			// 注意这里都是类实例
			hasher hash;
			key_equal equals;
			ExtractKey get_key;

			// 以node为单位分配空间
			using node = _hashtable_node<Value>;
			using node_allocator = simple_alloc<node, Alloc>;

			using self = hashtable;

			// bucket的实现采用vector
			vector<node *, Alloc> buckets;
			size_type num_elements;	// 节点个数

			// 创建一个新节点
			node *new_node(const value_type &obj) {
				node *n = node_allocator::allocate();
				n->next = nullptr;
				construct(&n->val, obj);
				return n;
			}

			// 删除节点
			void delete_node(node *n) {
				destory(&n->val);
				node_allocator::deallocate(n);
			}

			// 初始化buckets
			void initialize_buckets(size_type n) {
				const size_type n_buckets = next_size(n);	// 找出合适桶数
				// 保留空间并设置为nullptr
				buckets.reserve(n_buckets);	// reserve只配置空间不构造元素
				buckets.insert(buckets.end(), n_buckets, nullptr);
				num_elements = 0;
			}

			pair<iterator, bool> insert_unique_noresize(const value_type &obj);

			iterator insert_equal_noresize(const value_type &obj);

			// 以下一系列函数确定哪个桶，其实就是取余，有更复杂的hash算法
			// 比如完全哈希等等，但这里没有采用，降低了很多难度
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
					// 找到first
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

			// 构造函数，注意没有默认构造函数
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
			// 返回桶的个数
			size_type bucket_count() const { return buckets.size(); }
			// 返回每个桶中的节点个数
			size_type elems_in_bucket(size_type i) const {
				size_type result = 0;
				for (node *n = buckets[i]; n; ++result, n = n->next);
				return result;
			}
			// 桶的最大个数
			size_type max_bucket_count() const { return _gruel_prime_list[_gruel_num_primes - 1]; }
			// 下一个合适的桶个数
			size_type next_size(size_type n) const { return _gruel_next_prime(n); }
			// 重新分配空间，一个很重要的作用是降低负载因子α
			void resize(size_type num_elements_hint);

			// 头尾迭代器，自己写的，应该是这样
			iterator begin() {
				for (size_type n = 0; n < buckets.size(); ++n)
					if (buckets[n])
						return iterator(buckets[n], this);
				return end();
			}
			const_iterator begin() const {
				for (size_type n = 0; n < buckets.size(); ++n)
					if (buckets[n])
						// 出现错误：无法从"initializer list"转换为"gruel::_hashtable_iterator<Value, Key, HashFun, ExtractKey, EqualKey, Alloc>
						// 什么鬼，这里是返回一个临时对象啊
						// 原因是传入的参量为const，迭代器没有对应的构造函数
						// 解决方法为定义一个新的const迭代器
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

			// 插入，不可重复
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

			// 插入，可重复
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

			// 删除指定键值元素，并返回元素个数
			// 学习rb_tree中利用equal_range（注：可不能学习，这是无序容器）
			// 思路看代码即可
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
			// 有点类似单向链表的删除，不过这样子效率好低啊
			void erase(const iterator &it) {
				node *p = it.cur;
				if (p) {
					const size_type n = bkt_num(p->val);
					node *cur = buckets[n];

					// 如果要删除的节点为头节点
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
			// 这个删除也蛮好玩的，有点类似deque
			void erase(iterator first, iterator last) {
				// 先找到各自在的桶
				size_type first_bucket = first.cur ? get_key(first.cur->val) : buckets.size();
				size_type last_bucket = last.cur ? get_key(last.cur->val) : buckets.size();

				if (first == last)
					return;
				// 在同一个桶
				else if (first_bucket == last_bucket)
					erase_bucket(first_bucket, first.cur, last.cur);
				// 不同的桶
				else {
					erase_bucket(first_bucket, first.cur, nullptr);
					for (size_type n = first_bucket + 1; n < last_bucket; ++n)
						erase_bucket(n, nullptr);
					if (last_bucket == buckets.size())
						erase_bucket(last_bucket, last.cur);
				}
			}

			// 这里就转型后再转调用了，注意只有非const转const才可以
			void erase(const const_iterator &it) {
				erase(iterator(const_cast<node *>(it.cur), const_cast<hashtable *>(it.ht)));
			}
			void erase(const_iterator first, const_iterator last) {
				erase(iterator(const_cast<node *>(first.cur), const_cast<hashtable *>(first.ht)),
					iterator(const_cast<node *>(last.cur), const_cast<hashtable *>(last.ht)));
			}

			// 清空节点
			void clear();

			// 从另一个hashtable复制
			void copy_from(const hashtable &ht);

			// 查找元素
			iterator find(const key_type &key) {
				const size_type n = bkt_num_key(key);	// 确定桶
				node *first;
				// 在该桶中查找
				for (first = buckets[n]; first && !equals(get_key(first->val), key); first = first->next);
				return { first, this };
			}

			// 计算给定元素个数
			size_type count(const key_type &key) const {
				const size_type n = bkt_num_key(key);	// 确定桶
				size_type result = 0;
				// 在该桶中查找统计
				for (const node *cur = buckets[n]; cur; cur = cur->next)
					if (equals(get_key(cur->val), key))
						++result;
				return result;
			}

			// 跟我原来想的差不多
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
				// 依旧不清楚怎么交换配置器，而且也觉得没有必要交换
			}
	};


	// 重新配置hashtable，按理要完成配置新空间，移动元素，删除旧空间的操作，实际的实现运用了一些技巧，值得学习
	template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
	void hashtable<V, K, HF, Ex, Eq, A>::resize(size_type num_elements_hint) {
		// 注意这里的判定标准是和桶的个数进行比较，为了维持较小的α，以提高查表效率
		const size_type old_n = buckets.size();	// 旧空间大小
		// 新空间大小大于旧空间大小才进行配置
		if (num_elements_hint > old_n) {
			const size_type n = next_size(num_elements_hint);	// 找出合适桶数
			if (n > old_n) {
				// 创建一个临时vector，其空间大小为n，并且元素全部置nullptr
				vector<node *, A> tmp(n, nullptr);
				// 一个桶一个桶复制元素，运用了一些技巧
				for (size_type bucket = 0; bucket < old_n; ++bucket) {
					node *first = buckets[bucket];	// 当前桶的首节点
					while (first) {
						size_type new_bucket = bkt_num(first->val, n);	// 确定该节点在新buckets的位置
						buckets[bucket] = first->next;	// 将该桶首节点指向下一节点
						first->next = tmp[new_bucket];	// 将原首节点指向新buckets的相应节点
						tmp[new_bucket] = first;	// 新buckets的相应节点首节点指向原首节点，完成节点转移
						first = buckets[bucket];	// 首节点换为下一节点继续操作
					}
				}
				// 最后直接交换临时vector，tmp在函数结束后自行析构，厉害了
				// 能直接这样做的原因我觉得与begin()和end()的实现有关
				// 一开始马虎写在for循环里。。
				buckets.swap(tmp);
			}
		}
	}


	// 插入不重复元素的实际操作，NoResize意为这里不改变hashtable桶个数
	template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
	pair<typename hashtable<V, K, HF, Ex, Eq, A>::iterator, bool>
		hashtable<V, K, HF, Ex, Eq, A>::insert_unique_noresize(const value_type &obj) {
		const size_type n = bkt_num(obj);	// 查找对应桶
		node *first = buckets[n];
		// 如果桶中有键值相等的节点，则插入失败
		for (node *cur = first; cur; cur = cur->next)
			if (equals(get_key(cur->val), get_key(obj)))
				return { iterator(cur, this), false };

		// 没有相等节点，则直接插入到桶的起始位置
		node *tmp = new_node(obj);
		tmp->next = first;
		buckets[n] = tmp;
		++num_elements;
		return { iterator(tmp, this), true };
	}


	// 插入重复元素的实际操作
	template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
	typename hashtable<V, K, HF, Ex, Eq, A>::iterator
		hashtable<V, K, HF, Ex, Eq, A>::insert_equal_noresize(const value_type &obj) {
		const size_type n = bkt_num(obj);	// 查找相应的桶
		node *first = buckets[n];
		// 在桶中查找，若有键值相等节点，则插入到该节点之后
		for (node *cur = first; cur; cur = cur->next)
			if (equals(get_key(cur->val), get_key(obj))) {
				node *tmp = new_node(obj);
				tmp->next = cur->next;
				cur->next = tmp;
				++num_elements;
				return { tmp, this };
			}
		
		// 没有相等节点则直接插入到桶开始位置
		node *tmp = new_node(obj);
		tmp->next = first;
		buckets[n] = tmp;
		++num_elements;
		return { tmp, this };
	}


	// 清空节点，注意清空完但buckets还在，且桶数不变
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


	// 从另一hashtable复制
	template <typename V, typename K, typename HF, typename Ex, typename Eq, typename A>
	void hashtable<V, K, HF, Ex, Eq, A>::copy_from(const hashtable &ht) {
		// 清空原hashtable
		buckets.clear();
		size_type ht_size = ht.buckets.size();
		// 匹配ht的桶个数
		buckets.reserve(ht_size);
		buckets.insert(buckets.end(), ht_size, nullptr);
		// 逐个桶复制
		for (size_type i = 0; i < ht_size; ++i) {
			const node *cur = ht.buckets[i];
			// 先判断当前桶是否存在节点
			if (cur) {
				node *copy = new_node(cur->val);
				buckets[i] = copy;

				// 逐个节点复制
				for (node *next = cur->next; next; cur = next, next = cur->next) {
					copy->next = new_node(next->val);
					copy = copy->next;
				}
			}
		}
		// 设置节点个数
		num_elements = ht.num_elements;
	}
}
