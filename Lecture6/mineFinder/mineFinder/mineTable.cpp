
#include "stdafx.h"
#include "mineTable.h"
#include <sstream>
#include <string>
#include "particle.h"
using namespace std;

#define SETRECT(r, x,y,w,h) {SetRect(&r, x, y, x+w, y+h);}

extern cParticleManager g_particleMng;


cMineTable::cMineTable()
{
	const COLORREF red = RGB(255,0,0);
	const COLORREF white = RGB(255,255,255);
	const COLORREF gray = RGB(200,200,200);

	m_brushes[ MINE] = CreateSolidBrush(red);
	m_brushes[ NUMBER] = CreateSolidBrush(white);
	m_brushes[ BLANK] = CreateSolidBrush(white);
	m_blockBrush = CreateSolidBrush(gray);
	m_tableOffset.x = 200;
	m_tableOffset.y = 100;
}

cMineTable::~cMineTable()
{
	for (int i=0; i < MAX_TYPE; ++i)
		DeleteObject(m_brushes[ i]);
}


/**
 @brief 
 @date 2014-04-15
*/
void cMineTable::Init(bool newTable) // newTable=true
{
	if (newTable)
	{
		for (int i=0; i < MAX_TABLE_W; ++i)
		{
			for (int k=0; k < MAX_TABLE_H; ++k)
				m_table[ i][ k].type = SetBlock(BLANK, true);
		}

		// init mine
		int cnt = 0;
		while (cnt < MAX_MINE_COUNT)
		{
			const int x = rand() % MAX_TABLE_W;
			const int y = rand() % MAX_TABLE_W;
		
			if (BLOCK == m_table[ x][ y].type)
			{
				m_table[ x][ y].type = SetBlock(MINE, true);
				++cnt;
			}
		}
	}
	else
	{
		for (int i=0; i < MAX_TABLE_W; ++i)
		{
			for (int k=0; k < MAX_TABLE_H; ++k)
				m_table[ i][ k].type = SetBlock(GetBlockType(m_table[ i][ k].type), true);
		}
	}

	// init mine count
	for (int i=0; i < MAX_TABLE_W; ++i)
	{
		for (int k=0; k < MAX_TABLE_H; ++k)
		{
			if (BLOCK == m_table[ i][ k].type)
			{
				const int mineCnt = GetMineCount(i, k);
				if (mineCnt > 0)
				{
					m_table[ i][ k].type = SetBlock(NUMBER, true);
					m_table[ i][ k].value = mineCnt;
				}
			}
		}
	}

}


/**
 @brief 
 @date 2014-04-15
*/
void cMineTable::Render(HDC hdc)
{
	for (int i=0; i < 11; ++i)
	{
		MoveToEx(hdc, m_tableOffset.x, m_tableOffset.y+i*WIDTH, NULL);
		LineTo(hdc, m_tableOffset.x+WIDTH*10, m_tableOffset.y+i*WIDTH);
	}

	for (int i=0; i < 11; ++i)
	{
		MoveToEx(hdc, m_tableOffset.x + i*WIDTH, m_tableOffset.y, NULL);
		LineTo(hdc, m_tableOffset.x + i*WIDTH, m_tableOffset.y + WIDTH*10);
	}

	
	for (int i=0; i < MAX_TABLE_W; ++i)
	{
		for (int k=0; k < MAX_TABLE_H; ++k)
		{
			RECT r;
			SETRECT(r, m_tableOffset.x + i*WIDTH, m_tableOffset.y + k*WIDTH, WIDTH, WIDTH);

			const Type type = GetBlockType(m_table[ i][ k].type);
			if (IsBlock(m_table[ i][ k].type))
			{
				HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, m_blockBrush);
				Rectangle(hdc, r.left, r.top, r.right, r.bottom);
				SelectObject(hdc, oldBrush);

				if (IsCheck(m_table[ i][ k].type))
				{
					OffsetRect(&r, 0, 10);
					SetBkMode(hdc, TRANSPARENT);
					DrawTextA(hdc, "!", 1, &r, DT_CENTER);
				}
			}
			else
			{
				HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, m_brushes[ type] );
				Rectangle(hdc, r.left, r.top, r.right, r.bottom);
				SelectObject(hdc, oldBrush);

				if ((NUMBER == type) || (MINE == type))
				{
					std::stringstream ss;
					switch (type)
					{
					case NUMBER:
						ss << m_table[ i][ k].value;
						break;
					case MINE:
						ss << "@";
						break;
					}
					string str = ss.str();

					OffsetRect(&r, 0, 10);
					SetBkMode(hdc, TRANSPARENT);
					DrawTextA(hdc, str.c_str(), str.length(), &r, DT_CENTER);		
				}
			}
		}
	}
}


/**
 @brief 
 @date 2014-04-15
*/
int cMineTable::GetMineCount(int x, int y)
{
	POINT offset[] = { {-1,0}, {-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1} };

	int cnt = 0;
	for (int i=0; i < sizeof(offset)/sizeof(POINT); ++i)
	{
		POINT xy = {x+offset[ i].x, y+offset[ i].y};
		if (CheckRange(xy.x, xy.y))
		{
			if (MINE == GetBlockType(m_table[ xy.x][ xy.y].type))
				++cnt;
		}
	}
	return cnt;
}
 

/**
 @brief 
 @date 2014-04-17
*/
int cMineTable::GetCheckCount(int x, int y)
{
	POINT offset[] = { {-1,0}, {-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1} };
	int cnt = 0;
	for (int i=0; i < sizeof(offset)/sizeof(POINT); ++i)
	{
		POINT xy = {x+offset[ i].x, y+offset[ i].y};
		if (CheckRange(xy.x, xy.y))
		{
			if (IsCheck(m_table[ xy.x][ xy.y].type))
				++cnt;
		}
	}
	return cnt;
}


/**
 @brief 
 @date 2014-04-17
*/
bool cMineTable::Click(const POINT &pos)
{
	const int x = (pos.x-m_tableOffset.x) / WIDTH;
	const int y = (pos.y-m_tableOffset.y) / WIDTH;
	if (!CheckRange(x,y))
		return false;

	if (!IsBlock(m_table[ x][ y].type))
		return false;

	if (IsCheck(m_table[ x][ y].type))
		return false;

	const Type type = GetBlockType(m_table[ x][ y].type);
	if (BLANK == type)
		ExpandBlank(x, y);
	else
		m_table[ x][ y].type = SetBlock(type, false);

	g_particleMng.AddParticle( new cParticle(pos) );

	return true;
}


/**
 @brief 
 @date 2014-04-17
*/
bool cMineTable::Check(const POINT &pos)
{
	const int x = (pos.x-m_tableOffset.x) / WIDTH;
	const int y = (pos.y-m_tableOffset.y) / WIDTH;
	if (!CheckRange(x,y))
		return false;

	if (!IsBlock(m_table[ x][ y].type))
		return false;

	const bool toggle = !(IsCheck(m_table[ x][ y].type));
	m_table[ x][ y].type = SetCheck(m_table[ x][ y].type, toggle);
	return true;
}


/**
 @brief  check 된 블럭을 지뢰라고 가정하고, 나머지 블럭들을 제거한다.
 @date 2014-04-17
*/
bool cMineTable::Erase(const POINT &pos)
{
	const int x = (pos.x-m_tableOffset.x) / WIDTH;
	const int y = (pos.y-m_tableOffset.y) / WIDTH;
	if (!CheckRange(x,y))
		return false;

	if (IsBlock(m_table[ x][ y].type))
		return false;

	const Type type = GetBlockType(m_table[ x][ y].type);
	if (type == NUMBER)
	{
		if (GetCheckCount(x,y) == m_table[ x][ y].value)
		{
			POINT offset[] = { {-1,0}, {-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1} };
			for (int i=0; i < sizeof(offset)/sizeof(POINT); ++i)
			{
				POINT xy = {x+offset[ i].x, y+offset[ i].y};
				if (!CheckRange(xy.x, xy.y))
					continue;

				if (IsBlock(m_table[ xy.x][ xy.y].type) && !IsCheck(m_table[ xy.x][ xy.y].type))
				{
					ExpandBlank(xy.x, xy.y);
				}
			}
		}
	}

	return true;
}


/**
 @brief 
 @date 2014-04-17
*/
bool cMineTable::CheckRange(int x, int y)
{
	if ((x < 0) || (x >= MAX_TABLE_W))
		return false;
	if ((y < 0) || (y >= MAX_TABLE_H))
		return false;
	return true;
}


/**
 @brief 
 @date 2014-04-17
*/
void cMineTable::ExpandBlank(int x, int y)
{
	if (!CheckRange(x,y))
		return;

	if (!IsBlock(m_table[ x][ y].type))
		return;

	const Type type = GetBlockType(m_table[ x][ y].type);
	switch (type)
	{
	case BLANK:
	case NUMBER:
		{
			m_table[ x][ y].type = SetBlock(type, false);
			POINT pos = {m_tableOffset.x+x*WIDTH, m_tableOffset.y+y*WIDTH};
			g_particleMng.AddParticle( new cParticle(pos) );
		}
		break;

	default:
		return;
	}

	if (type == NUMBER)
		return; // *** return *** 

	POINT offset[] = { {-1,0}, {-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1} };
	for (int i=0; i < sizeof(offset)/sizeof(POINT); ++i)
		ExpandBlank(x+offset[ i].x, y+offset[ i].y);
}
