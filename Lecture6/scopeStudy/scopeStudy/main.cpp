
#include <iostream>
using namespace std;

namespace test
{
	void func1()
	{
		cout << "test::func()" << endl;
	}
}

void func1()
{
	cout << "global::func1()" << endl;
}

void main()
{
	test::func1();
	func1();

	using namespace test;
	::func1();
}
