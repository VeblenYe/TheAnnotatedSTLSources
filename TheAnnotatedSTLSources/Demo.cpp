
#define _SILENCE_IDENTITY_STRUCT_DEPRECATION_WARNING 1

#include <iostream>
#include <functional>
#include <random>
#include "gruel_set.h"


using std::cout;
using std::endl;
using std::ends;


int main() {

	gruel::set<int> iset;

	std::random_device r;
	std::default_random_engine e(r());
	std::uniform_int_distribution<int> dist(1, 1000);
	for (int i = 0; i < 10; ++i)
		iset.insert(dist(e));
	for (auto i : iset)
		std::cout << i << std::ends;
	std::cout << std::endl;


	getchar();

	return 0;
}
