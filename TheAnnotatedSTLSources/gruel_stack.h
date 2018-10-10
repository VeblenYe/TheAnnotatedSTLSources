#pragma once


#include "gruel_deque.h"


namespace gruel {

	/* 综述：stack，堆栈，以deque为底层容器，其元素存取顺序为后进先出LIFO */

	template <typename T, typename Sequence>
	bool operator==(const stack<T, Sequence> &lhs, const stack<T, Sequence> &rhs) {
		return lhs.c == rhs.c;
	}


	template <typename T, typename Sequence>
	bool operator<(const stack<T, Sequence> &lhs, const stack<T, Sequence> &rhs) {
		return lhs.c < rhs.c;
	}

	// stack由底部容器deque实现
	// 这种修改某物接口，形成另一种风貌的容器，成为adapter
	template <typename T, typename Sequence = gruel::deque<T>>
	class stack {
		// 跟书上写得有出入，这里并不需要<>，也或者是我哪里没理解弄错了
		friend bool operator==(const stack &, const stack &);
		friend bool operator<(const stack &, const stack &);
	public:
		using value_type = typename Sequence::value_type;
		using reference = typename Sequence::reference;
		using const_reference = typename Sequence::const_reference;
		using size_type = typename Sequence::size_type;
		
		bool empty() const { return c.empty(); }
		size_type size() const { return c.size(); }
		reference top() { return c.back(); }
		const_reference top() const { return c.back(); }
		void push(const value_type &x) { c.push_back(x); }
		void pop() { c.pop_back(); }

	protected:
		Sequence c;	// 底层容器，在effective C++ 中称这种类为复合类
	};

}