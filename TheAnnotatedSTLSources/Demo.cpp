
#define _SILENCE_IDENTITY_STRUCT_DEPRECATION_WARNING 1

#include <iostream>
#include <functional>
#include <random>
#include "gruel_set.h"
#include "gruel_map.h"
#include "gruel_list.h"
#include "gruel_deque.h"


using std::cout;
using std::endl;
using std::ends;


int main() {

	gruel::deque<int> ilist;
	std::random_device r;
	std::default_random_engine e(r());
	std::uniform_int_distribution<int> u(0, 100);
	for (int i = 0; i < 13; ++i)
		ilist.push_back(u(e));

	for (auto i : ilist)
		cout << i << ends;
	cout << endl;

	for (auto i : ilist)
		cout << i << ends;


	getchar();

	return 0;
}
