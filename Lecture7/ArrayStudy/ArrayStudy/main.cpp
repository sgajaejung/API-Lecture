
#include <iostream>
using namespace std;



void main()
{
	int ar1[10][10] = {
		{1, 2, 3},
		{4, 5, 6},
		{7, 8, 9}
	};

	int **ar2;
	ar2 = new int*[ 10];
	for (int i=0; i < 10; ++i)
	{
		ar2[ i] = new int[ 10];
	}

	ar2[ 0][ 0] = 1;
	ar2[ 0][ 1] = 2;

	cout << ar1[ 0][ 0] << endl;
	cout << ar2[ 0][ 0] << endl;

	cout << *(int*)ar1 << endl;
	cout << *(int*)ar2 << endl;
	cout << *(int*)*(int*)ar2 << endl;
}
