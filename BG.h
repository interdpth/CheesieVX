#pragma once
#include "Layer.h"
#include <vector>
#include "MemFile.h"
#include "RomClass.h"
using namespace std;
class BG
{
public:
	 unsigned char width;

	 unsigned char height;

	 vector<unsigned short> blocks;

	 unsigned char size;

	 vector<unsigned short> tileTable;

	 Layer layer;

	 int origLen;

	 unsigned char prop;

	 void Export(MemFile* theFile);
	BG(int width,int height);
	~BG();
};

