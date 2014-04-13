
#include <iostream>
#include <map>
#include <string>
using namespace std;

class cParent
{
public:
	cParent(const string &name) : m_name(name) {}
	virtual ~cParent() {}
	virtual void func1() { cout << "cParent func1() " << m_name.c_str() << endl; }
	virtual void func2() { cout << "cParent func2() " << endl; }
	virtual void func3() { cout << "cParent func3() " << endl; }
	virtual void func4() { cout << "cParent func4() " << endl; }
	static void func10() { cout <<"cChild func10()" << endl; }
protected:
	string m_name;
};

class cChild : public cParent
{
public:
	cChild(const string &name) : cParent(name) {}
	virtual void func1() override { cout << "cChild func1() " << m_name.c_str() << endl; }
	void onlyChildFunc() { cout << "onlyChildFunc() " << endl; }
};


map<string, cParent*> variables;
map<string, void (cParent::*)()> operate;
void call_mem_func(const string &var, const string &oper )
{
	(variables[ var]->*operate[ oper])();
}

void main()
{
	cParent *p = new cChild("test1");
	
	// member function pointer
	void (cParent::*ptrFunc) () = &cParent::func1;
	//(*ptrFunc)(); // error
	(p->*ptrFunc)(); // call cChild::func1
	
	variables[ "test1"] = new cChild("test1");
	variables[ "test2"] = new cChild("test2");
	variables[ "test3"] = new cChild("test3");
	variables[ "test4"] = new cChild("test4");

	operate[ "func1"] = &cParent::func1;
	operate[ "func2"] = &cParent::func2;
	operate[ "func3"] = &cParent::func3;
	operate[ "func4"] = &cParent::func4;

	call_mem_func("test1", "func1");
	call_mem_func("test2", "func2");
	call_mem_func("test3", "func3");
	call_mem_func("test4", "func4");

	
	// static member function pointer
	//void (cParent::*ptrFunc2) () = &cParent::func10; // error
	void (*ptrFunc2) () = &cParent::func10; // ok
	(*ptrFunc2)();


	// casting member function pointer
	//void (cParent::*ptrFunc3) () = &cChild::onlyChildFunc;  // error
	void (cChild::*ptrFunc3) () = &cChild::onlyChildFunc; // ok
	void (cChild::*ptrFunc4) () = &cChild::func1;  // ok

	
	
	delete p;
	auto it = variables.begin();
	while (variables.end() != it)
		delete it++->second;
}
