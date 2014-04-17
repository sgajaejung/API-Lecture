/**
 @filename mineTable.h
 
*/
#pragma once

#include "Block.h"


enum {
	MAX_TABLE_W = 10,
	MAX_TABLE_H = 10,
	MAX_MINE_COUNT = 10,
	WIDTH = 30,
};


class cMineTable
{
public:
	cMineTable();
	virtual ~cMineTable();

	void Init(bool newTable=true);
	void Render(HDC hdc);
	bool Click(const POINT &pos);
	bool Check(const POINT &pos);
	bool Erase(const POINT &pos);


protected:
	int GetMineCount(int x, int y);
	int GetCheckCount(int x, int y);
	bool CheckRange(int x, int y);
	void ExpandBlank(int x, int y);


private:
	struct sBlock
	{
		int type;
		int value; //type = NUMBER, value = mine count
	};

	sBlock m_table[ MAX_TABLE_W][ MAX_TABLE_H];
	HBRUSH m_brushes[ MAX_TYPE];
	HBRUSH m_blockBrush;
	POINT m_tableOffset;
};
