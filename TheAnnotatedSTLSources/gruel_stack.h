#pragma once


#include "gruel_deque.h"


namespace gruel {

	/* ������stack����ջ����dequeΪ�ײ���������Ԫ�ش�ȡ˳��Ϊ����ȳ�LIFO */

	template <typename T, typename Sequence>
	bool operator==(const stack<T, Sequence> &lhs, const stack<T, Sequence> &rhs) {
		return lhs.c == rhs.c;
	}


	template <typename T, typename Sequence>
	bool operator<(const stack<T, Sequence> &lhs, const stack<T, Sequence> &rhs) {
		return lhs.c < rhs.c;
	}

	// stack�ɵײ�����dequeʵ��
	// �����޸�ĳ��ӿڣ��γ���һ�ַ�ò����������Ϊadapter
	template <typename T, typename Sequence = gruel::deque<T>>
	class stack {
		// ������д���г��룬���ﲢ����Ҫ<>��Ҳ������������û���Ū����
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
		Sequence c;	// �ײ���������effective C++ �г�������Ϊ������
	};

}