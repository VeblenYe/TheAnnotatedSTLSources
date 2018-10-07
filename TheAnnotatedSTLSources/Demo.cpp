
#define _SILENCE_IDENTITY_STRUCT_DEPRECATION_WARNING 1

#include <iostream>
#include <functional>
#include <random>
#include "gruel_set.h"
#include "gruel_map.h"
#include "gruel_list.h"
#include "gruel_deque.h"
#include "gruel_priority_queue.h"


using std::cout;
using std::endl;
using std::ends;

using namespace gruel;


int main() {

	int ia[9] = { 0,1,2,3,4,5,6,7,8 };
	priority_queue<int> ipq(ia, ia + 9);
	cout << ipq.size() << endl;

	cout << ipq.top() << endl;

	ipq.pop();

	cout << ipq.top() << endl;

	ipq.push(10);

	cout << ipq.top() << endl;

	getchar();

	return 0;
}
