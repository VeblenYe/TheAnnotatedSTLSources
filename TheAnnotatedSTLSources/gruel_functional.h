#pragma once



namespace gruel {

	// һԪ�º���
	template <typename Arg, typename Result>
	struct unary_function {
		// ������Ӧ���ͣ��Ա�����STL�����
		using argument_type = Arg;
		using result_type = Result;
	};


	// ��Ԫ�º���
	template <typename Arg1, typename Arg2, typename Result>
	struct binary_function {
		using first_argument_type = Arg1;
		using seconde_argument_type = Arg2;
		using result_type = Result;
	};


	// �����ࣨArithmetic���º���
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


	// ֤ͬԪ�أ���˼Ϊ��ֵA���Ԫ����op���㣬��õ�A�Լ�
	template <typename T>
	inline T identity_element(plus<T>) {
		// �ӷ���֤ͬԪ��Ϊ0
		return T(0);
	}

	template <typename T>
	inline T identity_element(multiplies<T>) {
		// �˷���֤ͬԪ��Ϊ1
		return T(1);
	}


	// ��ϵ�����ࣨRelational���º���
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


	// �߼������ࣨLogical���º���
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


	// ֤ͬ��identity)��ѡ��select����Ͷ�䣨project)
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