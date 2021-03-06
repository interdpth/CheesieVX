#include "SMClass.h"
#include <stdio.h>
int SMClass::ReduceToVisibleTiles()
{
	return 0;
}
int SMClass::MageExport(int Area, int Room, bool IsMf)
{
	


	newTileTableMappingIndexes.clear();
	newMapData.clear();

	ExportTileset();

	ExportTileTable();
	vector<unsigned short*> oldMapData;
	int gbaTroidsize = gbaTroid.nTSA.size();
	int oldBlockCount = gbaTroidsize / 4;
	for (int blockCounter = 0; blockCounter < oldBlockCount; blockCounter++)
	{
		unsigned short* theseBlocks = new unsigned short[4];
		memcpy(theseBlocks, &gbaTroid.nTSA[blockCounter * 4], 8);
		oldMapData.push_back(theseBlocks);
	}
	newMapData.clear();
	//automap
	for (int blockCounter = 0; blockCounter < 0x100; blockCounter++)
	{
		unsigned short* thisTileMapblock = new unsigned short[4];
		memcpy(thisTileMapblock, oldMapData[blockCounter], 8);
		newMapData.push_back(thisTileMapblock);
		newTileTableMappingIndexes[blockCounter] = newMapData.size() - 1;
	}
	QuantifyMapTiles(theBgs->bg1);
	QuantifyTable();
	Remap(theBgs->bg1);
	char path[1024];
	sprintf(path, "%s\\gbatroid.tiletable", exporthPath);

	DeleteFileA(path);
	FILE* fp = fopen(path, "w+b");
	int len = gbaTroid.nTSA.size();
	if (fp)
	{
		fwrite(&gbaTroid.nTSA[0], 2, len, fp);
		fclose(fp);
	}

	MemFile*thisFile = new MemFile(1000);
	thisFile->WriteStr("MAGE 1.0 ROOM", true);
	thisFile->seek(16);
	thisFile->fputc((unsigned char)(IsMf), true);
	thisFile->fputc((unsigned char)Area, true);
	thisFile->fputc((unsigned char)Room, true);
	thisFile->seek(100);
	

	
	thisFile->seek(100);
	int bgPointers[5] = { 0 };
	theBgs->Export(thisFile, bgPointers,exporthPath);



	int position = thisFile->GetIndex();
	//
	thisFile->fputc(0xFF); thisFile->fputc(0xFF); thisFile->fputc(0xFF);
	//this.enemyLists[0].Export(byteStream);
	int val = 0;
	/*if (this.header.spriteset1event > 0)
	{
	val = byteStream.Position;
	this.enemyLists[1].Export(byteStream);
	}*/
	int val2 = 0;
	/*if (this.header.spriteset2event > 0)
	{
	val2 = byteStream.Position;
	this.enemyLists[2].Export(byteStream);
	}*/
	int position2 = thisFile->GetIndex();
	thisFile->fputc(0x0);
	/*this.doorList.Export(byteStream);*/

	int position3 = thisFile->GetIndex();
	thisFile->fputc(0x0);
	//this.scrollList.Export(byteStream);
	RHeader newHeader;
	memset(&newHeader, 0, 0x3c);
	newHeader.bTileset = 8;

	newHeader.bBg0 = 0;
	newHeader.bBg1 = 16;
	newHeader.bBg2 = 0;
	newHeader.lBg3 = 0;
	thisFile->seek(32);
	thisFile->fwrite(&newHeader, 1, sizeof(RHeader), true);
	thisFile->seek(40);
	for (int i = 0; i < 5; i++)
	{
		thisFile->fput32(bgPointers[i], true);
	}
	thisFile->seek(64);
	thisFile->fput32(position, true);
	thisFile->seek(72);
	thisFile->fput32(val, true);
	thisFile->seek(80);
	thisFile->fput32(val2, true);
	thisFile->seek(92);
	thisFile->fput32(position2, true);
	thisFile->fput32(position3, true);
	thisFile->Save("test.mgr");

	return 0;
}
