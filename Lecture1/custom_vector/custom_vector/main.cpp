
#include <iostream>
#include <vector>
using namespace std;


template <class T>
class xvector
{
public:
	xvector() :
		m_buffer(NULL),
		m_size(0),
		m_capacity(0)
	{
	}

	xvector(int size) : 
		m_size(size),
		m_capacity(size)
	{
		m_buffer = new T[ size];
	}
	
	~xvector() {
		delete[] m_buffer;
		m_buffer = NULL;
	}

	void push_back(const T &v) {
	}

	void pop_back() {
	}

	int size() { 
		return m_size; 
	}

	void erase(int idx) {
	}

	void reserve(int size) {
		recapable(size);
	}

	void rotate(int first, int n_first, int last)
	{
	}

	bool empty() {
		return (0 == m_size);
	}

	void resize(int size) {
	}

	void recapable(int size) {
	}

	const T& operator[] (int idx) {
		return m_buffer[ idx];
	}


private:
	T *m_buffer;
	int m_size;
	int m_capacity;
};



void main()
{
	xvector<int> vec;
	vec.push_back( 1 );
	vec.push_back( 2 );
	vec.push_back( 3 );
	vec.push_back( 4 );
	vec.erase(1);

	for (int i=0; i < vec.size(); ++i)
	{
		cout << vec[ i] << endl;
	}
}
