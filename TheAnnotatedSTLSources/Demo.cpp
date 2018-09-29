
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
	for (int i = 0; i < 10; ++i)
		itree.insert_equal(dist(e));
	for (auto i : itree)
		std::cout << i << std::ends;
	std::cout << std::endl;

	auto q = itree;
	q.erase(q.begin());
	//q.insert_equal(99);

	cout << (q < itree) << endl;


	getchar();

	return 0;
}
