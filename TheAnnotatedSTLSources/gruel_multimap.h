#pragma once


#include "gruel_tree.h"


namespace gruel {

	/* ������multimap������ӳ�䣨����������Ҳû�취�������ǿ��Դ����ͬ��ֵԪ�ص�map�汾 */

	template <typename Key, typename Value, typename Compare = std::less<Key>, typename Alloc = alloc>
	class multimap {
	public:
		using key_type = Key;	// ��ֵ����
		using data_type = Value;	// ʵֵ����
		using mapped_type = Value;
		using value_type = std::pair<const Key, Value>;	// Ԫ�����ͣ���ֵ/ʵֵ��
		using key_compare = Compare;	// ��ֵ�ȽϺ���

		// ����һ���º���functor���������ǵ��á�Ԫ�رȽϺ�����
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
		// ע����������ͣ�Ԫ��������pair����pair.first��Ϊ��ֵ��Դ��ʵ������select1st������ֱ��ʹ��Key
		using rep_type = rb_tree<key_type, value_type, Key, key_compare, Alloc>;
		rep_type t;

	public:
		// Ƕ������
		using pointer = typename rep_type::pointer;
		using const_pointer = typename rep_type::const_pointer;
		using reference = typename rep_type::reference;
		using const_reference = typename rep_type::const_reference;
		using iterator = typename rep_type::iterator;
		using const_iterator = typename rep_type::const_iterator;
		using size_type = typename rep_type::size_type;
		using difference_type = typename rep_type::difference_type;
		using self = multimap<Key, Value, Compare, Alloc>;

		multimap() : t(Compare()) {}

		explicit multimap(const Compare &comp) : t(comp) {}

		template <typename InputIterator>
		multimap(InputIterator first, InputIterator last) : t(Compare()) {
			t.insert_equal(first, last);
		}

		template <typename InputIterator>
		multimap(InputIterator first, InputIterator last, const Compare &comp) : t(comp) {
			t.insert_equal(first, last);
		}

		multimap(const self &x) : t(x.t) {}

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
			/* ���ȸ��ݼ�ֵ��ʵֵ����һ��Ԫ�أ�����ʵֵδ֪�����Բ���һ����ʵֵ������ͬ����ʱ���������
			�ٽ���Ԫ�ز��뵽map��ȥ�������������һ��pair�����һԪ���Ǹ���������ָ������׵�����Ԫ�أ�
			��ָ�����ʧ�ܵ㣨��ֵ�ظ����ľ�Ԫ�ء�ǰһ��ָ����Ϊ��ֵʹ�ã�������Ϊ��ֵʹ�á� */
			return (*((insert(value_type(k, Value()))).first)).second;
		}
		void swap(self &x) { t.swap(x.t); }

		std::pair<iterator, bool> insert(const value_type &x) {
			return t.insert_equal(x);
		}

		iterator insert(iterator position, const value_type &x) {
			return t.insert_equal(position, x);
		}

		template <typename InputIterator>
		void insert(InputIterator first, InputIterator last) {
			t.insert_equal(first, last);
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
		std::pair<iterator, iterator> equal_range(const key_type &x) {
			return t.equal_range(x);
		}
		std::pair<const_iterator, const_iterator> equal_range(const key_type &x) const {
			return t.equal_range(x);
		}
	};

	template <typename Key, typename Value, typename Compare, typename Alloc>
	inline bool operator==(const multimap<Key, Value, Compare, Alloc> &lhs,
		const multimap<Key, Value, Compare, Alloc> &rhs) {
		return lhs.t == rhs.t;
	}


	template <typename Key, typename Value, typename Compare, typename Alloc>
	inline bool operator<(const multimap<Key, Value, Compare, Alloc> &lhs,
		const multimap<Key, Value, Compare, Alloc> &rhs) {
		return lhs.t < rhs.t;
	}
}
