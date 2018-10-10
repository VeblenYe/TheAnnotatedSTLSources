#pragma once


namespace gruel {

	template <typename Key>
	struct hash {};

	inline std::size_t _gruel_hash_string(const char *s) {
		unsigned long h = 0;
		for (; *s; ++s)
			h = 5 * h + *s;
		return std::size_t(h);
	}


	template <>
	struct hash<char *> {
		std::size_t operator()(const char *s) const { return _gruel_hash_string(s); }
	};

	template <>
	struct hash<const char *> {
		std::size_t operator()(const char *s) const { return _gruel_hash_string(s); }
	};

	template <>
	struct hash<char> {
		std::size_t operator()(char x) const { return x; }
	};


	template <>
	struct hash<unsigned char> {
		std::size_t operator()(char x) const { return x; }
	};


	template <>
	struct hash<signed char> {
		std::size_t operator()(unsigned char x) const { return x; }
	};


	template <>
	struct hash<short> {
		std::size_t operator()(short x) const { return x; }
	};


	template <>
	struct hash<unsigned short> {
		std::size_t operator()(unsigned char x) const { return x; }
	};


	template <>
	struct hash<int> {
		std::size_t operator()(int x) const { return x; }
	};


	template <>
	struct hash<unsigned int> {
		std::size_t operator()(unsigned int x) const { return x; }
	};


	template <>
	struct hash<long> {
		std::size_t operator()(long x) const { return x; }
	};


	template <>
	struct hash<unsigned long> {
		std::size_t operator()(unsigned long x) const { return x; }
	};
	
}