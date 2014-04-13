
#include <iostream>
using namespace std;

class Overflow { };
class Underflow {};

class cA
{
public:
	void func1() throw(Overflow)
	{
		cout << "cA::func1() " << endl;
		throw Overflow();
	}
	void func2() throw()
	{
		cout << "cA::func2() " << endl;
	}
};


void main()
{
	cA a;
	try 
	{
		a.func1();
	}
	catch (...)
	{
		cout << "exception catch " << endl;
	}
}
