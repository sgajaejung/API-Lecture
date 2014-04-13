
#include <iostream>
using namespace  std;
class cParent
{
public:
	cParent() {
		cout << "cParent()" << endl;
	}
	virtual ~cParent() {
		cout << "~cParent()" << endl;
	}
	virtual void func() {
		cout << "cParent func()" << endl;
	}
};

class cChild : public cParent
{
public:
	cChild() {
		cout << "cChild()" << endl;
	}
	virtual ~cChild() {
		cout << "~cChild()" << endl;
	}
	virtual void func() {
		cout << "cChild func()" << endl;
	}
};

void main()
{
	cParent *p = new cChild();
	p->func();
	delete p;
}
