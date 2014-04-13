
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

struct sPath
{
	int type; // 0=pos, 1=fire, 2=delay, 3=loop
	union
	{
		sPos pos;
		float delay;
	};
};

struct sEnemy
{
	string name;
	int life;
	vector<string> bullets;
	vector<sPos> bulletPos;
	sPos bound;
	float fireRate;
	vector<sPath> path;

	sEnemy() {}
	sEnemy(const sEnemy &rhs) {
		operator=(rhs);
	}
	sEnemy& operator=(const sEnemy &rhs) {
		if (this != &rhs) {
			name = rhs.name;
			life = rhs.life;
			bullets = rhs.bullets;
			bulletPos = rhs.bulletPos;
			bound = rhs.bound;
			fireRate = rhs.fireRate;
			path = rhs.path;
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


sPos parsePos( const string &str )
{
	sPos position;
	const int pos = str.find(",");
	position.x = atoi( str.substr(0, pos).c_str() );
	position.y = atoi( str.substr(pos+1).c_str() );
	return position;
}


void parsePathElement( const string &str, sEnemy &enemy )
{
	const int spaceIdx = str.find(" ");
	string first = str.substr(0, spaceIdx);
	trim(first);
	string second = str.substr(spaceIdx+1);
	trim(second);

	sPath path;
	if (first == "pos")
	{
		path.type = 0;
		path.pos = parsePos(second);
	}
	else if ("fire" == first)
	{
		path.type = 1;
	}
	else if ("delay" == first)
	{
		path.type = 2;
		path.delay = (float)atof(second.c_str());

	}
	else if ("loop" == first)
	{
		path.type = 3;
	}
	else
	{
		return;
	}

	enemy.path.push_back( path);
}


void parsePath( const string &str, sEnemy &enemy )
{
	string line = str;
	replaceAll(line, "{", "");
	replaceAll(line, "}", "");
	replaceAll(line, ";", "");
	trim(line);

	vector<string> elems;
	int pos=0;
	while (line.find("\n", pos) != string::npos)
	{
		const int next = line.find("\n", pos);
		elems.push_back( line.substr(pos, next-pos) );
		pos += elems.back().length() + 1;
	}
	elems.push_back( line.substr(pos) );

	for (int i=0; i < (int)elems.size(); ++i)
		parsePathElement( trim(elems[ i]), enemy );
}


bool parseKeyValue( const string &key, const string &value, sEnemy &enemy )
{
	if (key == "life")
	{
		enemy.life = atoi(value.c_str());
	}
	else if (key == "bullet")
	{
		enemy.bullets.push_back(value);
	}
	else if (key == "bulletPos")
	{
		enemy.bulletPos.push_back( parsePos(value) );
	}
	else if (key == "boundingBox")
	{
		enemy.bound = parsePos(value);
	}
	else if (key == "fireRate")
	{
		enemy.fireRate = (float)atof(value.c_str());
	}
	else if (key == "path")
	{
		parsePath(value, enemy);
	}
	else
	{
		return false;
	}

	return true;
}


int parseEnemy( const string &script, int startPos, sEnemy &enemy )
{
	const string header = "EnemyName";
	int nPos = script.find(header, startPos);
	if (string::npos == nPos)
		return nPos;
	nPos += header.length();
	enemy.name = script.substr( nPos, script.find("\n") - nPos );

	int pos = script.find("{", startPos) + 1;
	while (1)
	{
		const int fidx = script.find(";", pos);
		if ((string::npos == pos) || (string::npos == fidx))
			break;

		const string line = script.substr(pos, fidx-pos);

		const int equalIdx = line.find("=");
		string first = line.substr(0, equalIdx);
		trim(first);

		string second = line.substr(equalIdx+1);
		trim(second);

		pos = fidx+1;

		if (!parseKeyValue(first, second, enemy))
			break;
	}

	return pos;
}


void main()
{
	ifstream ifs("enemy.txt");
	string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	cout << str.c_str() << endl;
	
	vector<sEnemy> enemies;
	int pos = 0;
	while (1)
	{
		sEnemy enemy;
		if (string::npos != (pos = parseEnemy(str, pos, enemy)))
			enemies.push_back(enemy);
		else
			break;
	}

}
