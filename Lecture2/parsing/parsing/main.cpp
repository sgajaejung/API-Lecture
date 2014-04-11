
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>
using namespace std;

void main()
{
	ifstream ifs("enemy.txt");
	bool r = ifs.good();
	string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	cout << str.c_str() << endl;

	map<string, string> parseData;

	int pos = 0;
	while (1)
	{
		int fidx = str.find(";", pos);
		if ((string::npos == pos) || (fidx < 0))
			break;

		string line = str.substr(pos, fidx-pos);
		//cout << line.c_str() << endl;

		int equalIdx = line.find("=");
		string first = line.substr(0, equalIdx);
		std::remove(first.begin(), first.end(), '\n');
		//cout << "first " << first.c_str() << endl;
		
		string second = line.substr(equalIdx+1);
		//cout << "second " << second.c_str() << endl;

		parseData[ first] = second;
		pos = fidx+1;
	}

	map<string, string>::iterator it = parseData.begin();
	while (it != parseData.end())
	{
		cout << "first = " << it->first.c_str() << ", second = " << it->second.c_str()  << endl;
		++it;
	}

}
