
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

struct sPos
{
	int x,y;
};

struct sTime
{
	float time;
	string enemy;
	sPos pos;
};

struct sScenario
{
	string name;
	vector<sTime> times;

	sScenario() {}
	sScenario(const sScenario &rhs) {
		operator=(rhs);
	}
	sScenario& operator=(const sScenario &rhs) {
		if (this != &rhs) {
			name = rhs.name;
			times = rhs.times;
		}
		return *this;
	}
};


void replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if(from.empty())
		return;
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

string& trim(string &str)
{
	for (int i=0; i < (int)str.length(); ++i)
	{
		if ((str[ i] == '\n') || (str[ i] == '\t') || (str[ i] == ' '))
			str[ i] = '$';
		else
			break;
	}

	for (int i=str.length()-1; i >= 0; --i)
	{
		if ((str[ i] == '\n') || (str[ i] == '\t') || (str[ i] == ' '))
			str[ i] = '$';
		else
			break;
	}

	replaceAll(str, "$", "");
	return str;
}

void splitString(const string &src, const string &tok, string &first, string &second)
{
	const int pos = src.find(tok);
	first = src.substr(0, pos);
	if (string::npos != pos)
		second = src.substr(pos+1);
}

sPos parsePos( const string &str )
{
	sPos position;
	string first, second;
	splitString(str, ",", first, second);
	position.x = atoi( first.c_str() );
	position.y = atoi( second.c_str() );
	return position;
}


bool parseKeyValue( const string &key, const string &value, sScenario &scene )
{
	if (key == "time")
	{
		sTime info;
		string time, enemy, posX, posY, second;
		splitString(value, ",", time, second);
		info.time = (float)atof(time.c_str());
		splitString(second, ",", enemy, second);
		info.enemy = trim(enemy);
		info.pos = parsePos(second);

		scene.times.push_back(info);
	}
	else
	{
		return false;
	}

	return true;
}


int parseScenario( const string &script, int startPos, sScenario &scene )
{
	const string header = "scenarioName";
	int nPos = script.find(header, startPos);
	if (string::npos == nPos)
		return nPos;
	nPos += header.length();
	scene.name = trim(script.substr( nPos, script.find("\n", nPos) - nPos ));

	int pos = script.find("{", startPos) + 1;
	while (1)
	{
		const int fidx = script.find(";", pos);
		if ((string::npos == pos) || (string::npos == fidx))
			break;

		const string line = script.substr(pos, fidx-pos);
		string first, second;
		splitString(line, "=", first, second);

		pos = fidx+1;

		if (!parseKeyValue(trim(first), trim(second), scene))
			break;
	}

	return pos;
}


void main()
{
	ifstream ifs("scenario.txt");
	string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	cout << str.c_str() << endl;

	vector<sScenario> scenarios;
	int pos = 0;
	while (1)
	{
		sScenario scene;
		if (string::npos != (pos = parseScenario(str, pos, scene)))
			scenarios.push_back(scene);
		else
			break;
	}
}
