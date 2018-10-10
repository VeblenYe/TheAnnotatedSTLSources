
#define _SILENCE_IDENTITY_STRUCT_DEPRECATION_WARNING 1

#include <iostream>
#include <functional>
#include <random>
#include "gruel_set.h"
#include "gruel_map.h"
#include "gruel_list.h"
#include "gruel_deque.h"
#include "gruel_priority_queue.h"
#include "gruel_hashtable.h"


using std::cout;
using std::endl;
using std::ends;

using namespace gruel;


int main() {

	hashtable<int, int, hash<int>, std::identity<int>, std::equal_to<int>, alloc> iht(50, hash<int>(), std::equal_to<int>());

	cout << iht.size() << endl;
	cout << iht.bucket_count() << endl;
	cout << iht.max_bucket_count() << endl;

	
	
	iht.insert_unique(59);
	iht.insert_unique(63);
	iht.insert_unique(108);
	iht.insert_unique(2);
	iht.insert_unique(53);
	iht.insert_unique(55);
	cout << iht.size() << endl;

	for (auto it : iht)
		cout << it << ends;
	cout << endl;
	
	for (int i = 0; i < iht.bucket_count(); ++i) {
		int n = iht.elems_in_bucket(i);
		if (n != 0)
			cout << "桶" << i << "拥有" << n << "个元素" << endl;
	}
	
	for (int i = 0; i <= 47; ++i)
		iht.insert_equal(i);
	cout << iht.size() << endl;
	cout << iht.bucket_count() << endl;

	for (int i = 0; i < iht.bucket_count(); ++i) {
		int n = iht.elems_in_bucket(i);
		if (n != 0)
			cout << "桶" << i << "拥有" << n << "个元素" << endl;
	}

	for (auto it : iht)
		cout << it << ends;
	cout << endl;

	cout << *(iht.find(2)) << endl;
	cout << iht.count(2) << endl;

	getchar();

	return 0;
}
