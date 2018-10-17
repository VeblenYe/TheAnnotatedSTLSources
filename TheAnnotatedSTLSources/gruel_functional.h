#pragma once



namespace gruel {


	template <typename First, typename Second>
	struct pair {
		using first_type = First;
		using second_type = Second;

		First first;
		Second second;

		pair() : first(First()), second(Second()) {}
		pair(const First &f, const Second &s) : first(f), second(s) {}
	};


	template <typename Pair>
	struct select1st {

		auto operator()(const Pair &p) const->decltype(p.first) {
			return p.first;
		}

	};

}