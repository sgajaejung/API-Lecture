/**
 @filename Block.h
 
*/
#pragma once


enum Type {
	BLANK,
	MINE,
	NUMBER,
	MAX_TYPE,
};

enum Mask {
	BLOCK = 1 << 8,
	CHECK = 1 << 9,
};

inline Type GetBlockType(int blockType)
{
	return (Type)(blockType & 0xF);
}

inline bool IsBlock(int blockType)
{
	return (blockType & BLOCK)? true : false;
}

inline bool IsCheck(int blockType)
{
	return (blockType & CHECK)? true : false;
}

inline int SetBlock(int blockType, bool isBlock)
{
	if (isBlock)
		return (blockType | BLOCK);
	return (blockType & ~BLOCK);
}

inline int SetCheck(int blockType, bool isCheck)
{
	if (isCheck)
		return (blockType | CHECK);
	return (blockType & ~CHECK);
}
