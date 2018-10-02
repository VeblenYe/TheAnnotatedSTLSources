
#define _SILENCE_IDENTITY_STRUCT_DEPRECATION_WARNING 1

#include <iostream>
#include <functional>
#include <random>
#include "gruel_set.h"
#include "gruel_map.h"


using std::cout;
using std::endl;
using std::ends;


int main() {

	gruel::map<int, int> iset;

	std::random_device r;
	std::default_random_engine e(r());
	std::uniform_int_distribution<int> dist(1, 1000);
	for (int i = 0; i < 10; ++i)
		iset.insert({ i, dist(e) });
	for (auto i : iset)
		std::cout << i.second << std::ends;
	std::cout << std::endl;


	getchar();

	return 0;
}
