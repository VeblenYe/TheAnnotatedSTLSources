
#define _SILENCE_IDENTITY_STRUCT_DEPRECATION_WARNING 1

#include <iostream>
#include <functional>
#include <random>
#include "gruel_tree.h"


using std::cout;
using std::endl;
using std::ends;


int main() {

	gruel::rb_tree<int, int, std::identity<int>, std::less<int>> itree;

	std::random_device r;
	std::default_random_engine e(r());
	std::uniform_int_distribution<int> dist(1, 1000);
	for (int i = 0; i < 500; ++i)
		itree.insert_equal(dist(e));
	for (auto i : itree)
		std::cout << i << std::ends;
	std::cout << std::endl;

	auto p = itree.lower_bound(78);
	cout << *p << endl;

	auto q = itree.lower_bound(900);
	cout << *q << endl;

	itree.erase(p, q);
	for (auto i : itree)
		std::cout << i << std::ends;


	getchar();

	return 0;
}
