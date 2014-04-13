
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

struct sVec2
{
	float x,y;
};

struct sFire
{
	int type; // 0=direct, 1=delay, 2=loop
	union
	{
		sVec2 dir;
		float delay;
	};
};

struct sBullet
{
	string name;
	int damage;
	float speed;
	string image;
	sPos bound;
	bool conflict;
	vector<sFire> fires;

	sBullet() {}
	sBullet(const sBullet &rhs) {
		operator=(rhs);
	}
	sBullet& operator=(const sBullet &rhs) {
		if (this != &rhs) {
			name = rhs.name;
			damage = rhs.damage;
			speed = rhs.speed;
			image = rhs.image;
			bound = rhs.bound;
			conflict = rhs.conflict;
			fires = rhs.fires;
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

sVec2 parseVec2( const string &str )
{
	sVec2 vec;
	string first, second;
	splitString(str, ",", first, second);
	vec.x = (float)atof( first.c_str() );
	vec.y = (float)atof( second.c_str() );
	return vec;
}

void parseFireElement( const string &str, sBullet &bullet )
{
	string first, second;
	splitString(str, " ", first, second);
	trim(first);
	trim(second);

	sFire fire;
	if (first == "direct")
	{
		fire.type = 0;
		fire.dir = parseVec2(second);
	}
	else if ("delay" == first)
	{
		fire.type = 1;
		fire.delay = (float)atof(second.c_str());
	}
	else if ("loop" == first)
	{
		fire.type = 2;
	}
	else
	{
		return;
	}

	bullet.fires.push_back(fire);
}


void parseFire( const string &str, sBullet &bullet )
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
		parseFireElement( trim(elems[ i]), bullet );
}


bool parseKeyValue( const string &key, const string &value, sBullet &bullet )
{
	if (key == "damage")
	{
		bullet.damage = atoi(value.c_str());
	}
	else if (key == "speed")
	{
		bullet.speed = (float)atof(value.c_str());
	}
	else if (key == "image")
	{
		bullet.image = value;
	}
	else if (key == "boundingBox")
	{
		bullet.bound = parsePos(value);
	}
	else if (key == "bullet_conflict")
	{
		bullet.conflict = (atoi(value.c_str()) == 0)? false : true;
	}
	else if (key == "fire")
	{
		parseFire(value, bullet);
	}
	else
	{
		return false;
	}

	return true;
}


int parseBullet( const string &script, int startPos, sBullet &bullet )
{
	const string header = "bulletName";
	int nPos = script.find(header, startPos);
	if (string::npos == nPos)
		return nPos;
	nPos += header.length();
	bullet.name = trim(script.substr( nPos, script.find("\n", nPos) - nPos ));

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

		if (!parseKeyValue(trim(first), trim(second), bullet))
			break;
	}

	return pos;
}


void main()
{
	ifstream ifs("bullet.txt");
	string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	cout << str.c_str() << endl;

	vector<sBullet> bullets;
	int pos = 0;
	while (1)
	{
		sBullet bullet;
		if (string::npos != (pos = parseBullet(str, pos, bullet)))
			bullets.push_back(bullet);
		else
			break;
	}
}
