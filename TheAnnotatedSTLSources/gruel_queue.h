#pragma once


#include "gruel_deque.h"


namespace gruel {

	/* ������queue�����У���ײ�����Ϊdeque����Ԫ�ش�ȡ˳��Ϊ�Ƚ��ȳ�FIFO */

	template <typename T, typename Sequence = gruel::deque<T>>
	class queue {
		friend bool operator==(const queue &, const queue &);
		friend bool operator<(const queue &, const queue &);
	public:
		using value_type = typename Sequence::value_type;
		using size_type = typename Sequence::size_type;
		using reference = typename Sequence::reference;
		using const_reference = typename Sequence::const_reference;
	protected:
		Sequence c;
	public:
		bool empty() const { return c.empty(); }
		size_type size() const { return c.size(); }
		reference front() { return c.front(); }
		const_reference front() const { return c.front(); }
		reference back() { return c.back(); }
		const_reference back() const { return c.back(); }
		// queue��ĩ�˽���ǰ�˳�
		void push(const value_type &x) { c.push_back(x); }
		void pop() { c.pop_front(); }
	};


	template <typename T,typename Sequence>
	bool operator==(const queue<T, Sequence> &lhs, const queue<T, Sequence> &rhs) {
		return lhs.c == rhs.c;
	}


	template <typename T, typename Sequence>
	bool operator<(const queue<T, Sequence> &lhs, const queue<T, Sequence> &rhs) {
		return lhs.c < rhs.c;
	}
}
