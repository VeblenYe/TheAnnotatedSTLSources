#pragma once



namespace gruel {

	// 一元仿函数
	template <typename Arg, typename Result>
	struct unary_function {
		// 定义相应类型，以便于在STL中配接
		using argument_type = Arg;
		using result_type = Result;
	};


	// 二元仿函数
	template <typename Arg1, typename Arg2, typename Result>
	struct binary_function {
		using first_argument_type = Arg1;
		using seconde_argument_type = Arg2;
		using result_type = Result;
	};


	// 算术类（Arithmetic）仿函数
	template <typename T>
	struct plus : public binary_function<T, T, T> {
		T operator()(const T &x, const T &y) const { return x + y; }
	};

	template <typename T>
	struct minus : public binary_function<T, T, T> {
		T operator()(const T &x, const T &y) const { return x - y; }
	};

	template <typename T>
	struct multiplies : public binary_function<T, T, T> {
		T operator()(const T &x, const T &y) const { return x * y; }
	};

	template <typename T>
	struct divides : public binary_function<T, T, T> {
		T operator()(const T &x, const T &y) const { return x / y; }
	};

	template <typename T>
	struct modulus : public binary_function<T, T, T> {
		T operator()(const T &x, const T &y) const { return x % y; }
	};

	template <typename T>
	struct negate : public unary_function<T, T> {
		T operator()(const T &x) const { return -x; }
	};


	// 证同元素：意思为数值A与该元素做op运算，会得到A自己
	template <typename T>
	inline T identity_element(plus<T>) {
		// 加法的证同元素为0
		return T(0);
	}

	template <typename T>
	inline T identity_element(multiplies<T>) {
		// 乘法的证同元素为1
		return T(1);
	}


	// 关系运算类（Relational）仿函数
	template <typename T>
	struct equal_to : public binary_function<T, T, bool> {
		bool operator()(const T &x, const T &y) const { return x == y; }
	};

	template <typename T>
	struct not_equal_to : public binary_function<T, T, bool> {
		bool operator()(const T &x, const T &y) const { return x != y; }
	};

	template <typename T>
	struct greater : public binary_function<T, T, bool> {
		bool operator()(const T &x, const T &y) const { return x > y; }
	};

	template <typename T>
	struct less : public binary_function<T, T, bool> {
		bool operator()(const T &x, const T &y) const { return x < y; }
	};

	template <typename T>
	struct greater_equal : public binary_function<T, T, bool> {
		bool operator()(const T &x, const T &y) const { return x >= y; }
	};

	template <typename T>
	struct less_equal : public binary_function<T, T, bool> {
		bool operator()(const T &x, const T &y) const { return x <= y; }
	};


	// 逻辑运算类（Logical）仿函数
	template <typename T>
	struct logical_and : public binary_function<T, T, bool> {
		bool operator()(const T &x, const T &y) const { return x && y; }
	};

	template <typename T>
	struct logical_or : public binary_function<T, T, bool> {
		bool operator()(const T &x, const T &y) const { return x || y; }
	};

	template <typename T>
	struct logical_not : public unary_function<T, bool> {
		bool operator()(const T &x) const { return !x; }
	};


	// 证同（identity)、选择（select）、投射（project)
	template <typename T>
	struct identity : public unary_function<T, T> {
		const T &operator()(const T &x) const { return x; }
	};

	template <typename Pair>
	struct select1st : public unary_function<Pair, typename Pair::first_type> {
		const typename Pair::first_type &operator()(const Pair &x) const { return x.first; }
	};

	template <typename Pair>
	struct select2nd : public unary_function<Pair, typename Pair::second_type> {
		const typename Pair::second_type &operator()(const Pair &x) const { return x.second; }
	};

	template <typename Arg1, typename Arg2>
	struct project1st : public binary_function<Arg1, Arg2, Arg1> {
		Arg1 operator()(const Arg1 &x, const Arg2 &) const { return x; }
	};

	template <typename Arg1, typename Arg2>
	struct project2nd : public binary_function<Arg1, Arg2, Arg2> {
		Arg2 operator()(const Arg1 &, const Arg2 &y) const { return y; }
	};


	template <typename First, typename Second>
	struct pair {
		using first_type = First;
		using second_type = Second;

		First first;
		Second second;

		pair() : first(First()), second(Second()) {}
		pair(const First &f, const Second &s) : first(f), second(s) {}
	};
}