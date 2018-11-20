
#define _SILENCE_IDENTITY_STRUCT_DEPRECATION_WARNING 1

/* 发现如果用了std的数据结构，比如std::pair，iterator_traits就推断不出来 */

#include <iostream>
#include "gruel_algorithm.h"
#include "gruel_functional.h"
#include "gruel_vector.h"

using std::cout;
using std::endl;
using std::ends;

using namespace gruel;


int main() {

	int ia[5] = { 1,2,3,4,5 };
	vector<int> iv(ia, ia + 5);

	cout << accumulate(iv.begin(), iv.end(), 0) << endl;

	cout << accumulate(iv.begin(), iv.end(), 0, minus<int>()) << endl;

	cout << inner_product(iv.begin(), iv.end(), iv.begin(), 0) << endl;

	cout << inner_product(iv.begin(), iv.end(), iv.begin(), 0, minus<int>(), plus<int>()) << endl;

	ostream_iterator<int> oite(cout, " ");

	partial_sum(iv.begin(), iv.end(), oite);

	cout << endl;

	partial_sum(iv.begin(), iv.end(), oite, minus<int>());

	cout << endl;

	adjacent_difference(iv.begin(), iv.end(), oite);

	cout << endl;

	adjacent_difference(iv.begin(), iv.end(), oite, plus<int>());

	cout << endl;

	cout << power(10, 3) << endl;

	cout << power(10, 3, plus<int>()) << endl;

	getchar();

	return 0;
}
