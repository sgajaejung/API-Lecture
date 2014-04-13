
#include <iostream>
#include <Windows.h>
using namespace std;

void func1()  { cout << "func1" << endl; }
void func2() { cout << "func2" << endl; }
void func3() { cout << "func3" << endl; }
void func4() { cout << "func4" << endl; }
void func5(int n)  { cout << "func5 " << n << endl; }
void func6(int n) { cout << "func6 " << n << endl; }

void main()
{
	void (*ptrFunc)();
	ptrFunc = func4;
	ptrFunc();
	(*ptrFunc)();

	void (*ptrFunc2)() = func1;
	ptrFunc2();
	(*ptrFunc2)();

	void (*ptrFunc3)() = &func2;
	ptrFunc3();

	typedef void (*PtrVoidFunc) (void);
	PtrVoidFunc ptr = func3;
	ptr();

	cout << "function array begin" << endl;
	PtrVoidFunc funcs[] = {func1, func2, func3, func4};
	for (int i=0; i < (sizeof(funcs)/sizeof(PtrVoidFunc)); ++i)
		funcs[ i]();
	cout << "function array end" << endl;

	typedef void (*PtrIntFunc) (int);
	PtrIntFunc ptr2 = func5;
	ptr2(100);
}
