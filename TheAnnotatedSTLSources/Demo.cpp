
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
#include "gruel_hash_set.h"


using std::cout;
using std::endl;
using std::ends;

using namespace gruel;


int main() {

	hash_set<int> myHashSet;

	cout << myHashSet.size() << endl;
	cout << myHashSet.bucket_count() << endl;
	cout << myHashSet.max_bucket_count() << endl;

	for (int i = 0; i < 1000; ++i)
		myHashSet.insert(i);

	cout << myHashSet.size() << endl;

	for (auto it : myHashSet)
		cout << it << ends;
	cout << endl;
	

	getchar();

	return 0;
}
