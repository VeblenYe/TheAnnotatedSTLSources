
#define _SILENCE_IDENTITY_STRUCT_DEPRECATION_WARNING 1

/* �����������std�����ݽṹ������std::pair��iterator_traits���ƶϲ����� */

#include <iostream>
#include <iterator>
#include "gruel_numeric.h"
#include "gruel_vector.h"

using std::cout;
using std::endl;
using std::ends;

using namespace gruel;


int main() {
	
	int ia[5] = { 1,2,3,4,5 };
	vector<int> iv(ia, ia + 5);

	cout << accumulate(iv.begin(), iv.end(), 0) << endl;
	cout << inner_product(iv.begin(), iv.end(), iv.begin(), 10) << endl;

	cout << power(10, 3) << endl;

	getchar();

	return 0;
}
