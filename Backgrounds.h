#pragma once
#include "BG.h"
#include "MemFile.h"
class Backgrounds
{

public:
	BG* bg0;

	BG* bg1;

	BG* bg2;

	BG* bg3;

	BG* clip;
	void Export(MemFile* thisFile, int* dstArry);
	Backgrounds(int width, int height);
	~Backgrounds();
};

