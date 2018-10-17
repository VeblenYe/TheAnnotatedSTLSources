
#define _SILENCE_IDENTITY_STRUCT_DEPRECATION_WARNING 1

/* 发现如果用了std的数据结构，比如std::pair，iterator_traits就推断不出来 */

#include <iostream>
#include "gruel_hash_map.h"
#include "gruel_hash_set.h"
#include "gruel_vector.h"

using std::cout;
using std::endl;
using std::ends;

using namespace gruel;


int main() {
	
	hash_map<int, int> hm;

	hm.insert({ 3,3 });

	for (auto i : hm)
		cout << i.first << i.second << ends;

	getchar();

	return 0;
}
