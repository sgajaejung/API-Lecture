
#include <iostream>
#include <string>
using namespace std;

class cA
{
public:
	cA(const string &name) : m_name(name) 
	{ 
		cout << "cA " << m_name.c_str() << endl; 
	}

	cA(const cA &a) : m_name(a.m_name) 
	{ 
		m_name += "0";
		cout << "copy cA " << m_name.c_str() << endl; 
	}

	virtual ~cA() { cout << "~cA " << m_name.c_str() << endl; }
	string m_name;
};


void func( cA a )
{

}

cA func1( cA a )
{
	return cA(a.m_name+"0");
}

cA func2( cA &a )
{
	return cA(a.m_name+"0");
}

cA& func3( cA &a )
{
	return a;
}


void main()
{
	cA a("1");
	cA b = func3( a );
}
