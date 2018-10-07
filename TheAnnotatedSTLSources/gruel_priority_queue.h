#pragma once


#include "gruel_vector.h"
#include "gruel_heap.h"
#include <functional>


namespace gruel {
	using namespace gruel;

	// 优先队列
	template <typename T, typename Sequence = gruel::vector<T>, 
		typename Compare = std::less<typename Sequence::value_type>>
	class priority_queue {
	public:
		using value_type = typename Sequence::value_type;
		using size_type = typename Sequence::size_type;
		using reference = typename Sequence::reference;
		using const_reference = typename Sequence::const_reference;
	protected:
		Sequence c;
		Compare comp;
	public:
		priority_queue() : c() {}
		explicit priority_queue(const Compare &x) : c(), comp(x) {}

		template <typename InputIterator>
		priority_queue(InputIterator first, InputIterator last, const Compare &x)
			: c(first, last), comp(x) {
			make_heap(c.begin(), c.end(), comp);
		}
		template <typename InputIterator>
		priority_queue(InputIterator first, InputIterator last) : c(first, last) {
			make_heap(c.begin(), c.end(), comp);
		}

		~priority_queue() {}


		bool empty() const { return c.empty(); }
		size_type size() const { return c.size(); }
		const_reference top() const { return c.front(); }

		void push(const value_type &x) {
			c.push_back(x);
			// 很奇怪，这里一定要指定是gruel
			gruel::push_heap(c.begin(), c.end(), comp);
		}

		void pop() {
			gruel::pop_heap(c.begin(), c.end(), comp);
			c.pop_back();
		}
	};

}