#pragma once


#include "gruel_deque.h"


namespace gruel {

	/* 综述：queue，队列，其底层容器为deque，其元素存取顺序为先进先出FIFO */

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
		// queue是末端进、前端出
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
