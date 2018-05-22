#include "SMClass.h"

map<int, int> newTileMapping;
void SMClass::UpdateTileTable(unsigned short oldMapping, unsigned short newMapping)
{

}
//((tile& 0xC000)>>4 
unsigned short SMClass::snes2gba_tilemap(uint16_t tile)
{
	unsigned short newtile = tile & 0x3FF;
	char buffer[256] = { 0 };
	int mappedTile = newTileMapping[newtile];
	
	//If there's too many tiles, blank it out. 
	if ((signed int)(mappedTile + 0xC0) > (signed int)0x3FF)
	{
		sprintf(buffer, "Tile value is %x + 0xc0 > 0x3FF setting to 0x40", mappedTile);
		Logger::log->LogIt(Logger::DEBUG, buffer);
		//GBAtroid tile, no need to rebase
		mappedTile = 0x40;
	}
	else {
		sprintf(buffer, "TileEncoded %x -> Tile %x -> Mapped -> %x", tile, newtile, mappedTile);
		Logger::log->LogIt(Logger::DEBUG, buffer);
		mappedTile += 0xC0;
	}
	

	return ((mappedTile) & 0x3FF | ((tile&0xC000)>>4) |  (((tile & 0x1C00) >> 10) + 2) * 0x1000 + 0x1000) & 0xFFFF;
}



bool SMClass::CompareTiles(unsigned char* srcTile, unsigned char* chkTile)
{
	return memcmp(srcTile, chkTile, 32) == 0;
}

unsigned short SMClass::TileExists(unsigned char* srcTile, vector<unsigned char*>*theTiles)
{
	for (int tileCount = 0; tileCount < theTiles->size(); tileCount++)
	{
		if (CompareTiles(srcTile, theTiles->at(tileCount)))
		{
			return tileCount;
		}
	}

	return 0xBAAD;
}

int SMClass::ExportTileset()
{

	vector<unsigned char*> rawTiles;
	vector<unsigned char*> newTiles;

	////Blank tile to use later
	//unsigned char* thisTile = new unsigned char[32];
	//memset(thisTile, 0, 32);
	//rawTiles.push_back(thisTile);

	for (int i = 0; i < Tiles.size() / 32; i++)
	{
		unsigned char* thisTile = new unsigned char[32];
		memcpy(thisTile, &Tiles[i * 32], 32);
		rawTiles.push_back(thisTile);
	}
	
	Logger::log->LogIt(Logger::DEBUG, "Begin remapping");
	for (int rawTileIndex = 0; rawTileIndex < rawTiles.size()-1; rawTileIndex++)
	{

		unsigned char* thisTile= new unsigned char[32];
		memcpy(thisTile, rawTiles.at(rawTileIndex), 32);

		
		int tileIndex = TileExists(thisTile, &newTiles);
		char debugtext[256] = { 0 };
		if (tileIndex==0XBAAD)
		{	
			newTiles.push_back(thisTile);			
			//adjust tilemap
			newTileMapping[rawTileIndex] = newTiles.size()-1;
		
			sprintf(debugtext, "Inserting tile %x -> %x\n", rawTileIndex,rawTileIndex, newTiles.size() - 1);
			Logger::log->LogIt(Logger::DEBUG, debugtext);
		}
		else 
		{
			
		
			int keyIndex = 0xBAAD;
			for (auto &i : newTileMapping) {
				if (i.second == tileIndex) {
					keyIndex = i.first;
					break; // to stop searching
				}
			}
		
			if (keyIndex == 0xBAAD)
			{
				keyIndex = keyIndex + 1;
			}
			newTileMapping[rawTileIndex] = keyIndex;
			sprintf(debugtext, "Remapping from tile %x -> lookedup index %x -> %x\n", rawTileIndex, tileIndex, newTileMapping[rawTileIndex]);
			Logger::log->LogIt(Logger::DEBUG, debugtext);

		}
		
	}
	
	char moreDebugText[512] = { 0 };
	int oldTileCount = rawTiles.size();
	int newTileCount = newTiles.size();;
	int difference = oldTileCount - newTileCount;
	sprintf(moreDebugText, "Found %d duplicate tiles", difference);
	Logger::log->LogIt(Logger::DEBUG, moreDebugText);
	//
	Tiles.clear();
	for (int i = 0; i < newTiles.size(); i++)
	{
		for (int y = 0; y < 32; y++)
		{
			Tiles.push_back(newTiles.at(i)[y]);
		}
	}


	FILE*	fp2 = fopen("tmpgfx.gfx", "wb");
	if (fp2)
	{
		fwrite(&Tiles[0],1, Tiles.size(), fp2);
		fclose(fp2);
	}

	for (int i = 0; i < rawTiles.size(); i++)
	{
		delete[] rawTiles[i];
	}
	for (int i = 0; i < newTiles.size(); i++)
	{
		delete[] newTiles[i];
	}
	return true;
}


int SMClass::ExportTileTable()
{

	
	tTSA gbaTroid;
	gbaTroid.max = TSA.nTSA.size() / 8;
	gbaTroid.nTSA.resize(TSA.nTSA.size());
	FILE* fp2 = fopen("zmcretiletable", "r+b");
	if (fp2)
	{
		fread((unsigned char*)&gbaTroid.nTSA[0], 2, 0x140, fp2);
		fclose(fp2);
	}
	char moreDebugText[512] = { 0 };
	sprintf(moreDebugText, "Begin tile table remapping");
	Logger::log->LogIt(Logger::DEBUG, moreDebugText);
	for (int tsaCounter = 0; tsaCounter < TSA.nTSA.size(); tsaCounter++)
	{
		unsigned short pftt = TSA.nTSA[tsaCounter];
		int flipX = pftt & 0x4000 ? 0x400 : 0;
		int flipY = pftt & 0x8000 ? 0x800 : 0;
		int pal = (((pftt & 0x1C00) >> 8) * 0x1000);
		int tile = (pftt + 0xC0) & 0x3FF;

		//gbaTroid.nTSA[tsaCounter] = pal+ 0x4000 | tile;
		gbaTroid.nTSA[tsaCounter] = snes2gba_tilemap(pftt);
	}
	DeleteFileA("gbatroid.tiletable");
	FILE* fp = fopen("gbatroid.tiletable", "w+b");
	if (fp)
	{
		fwrite(&gbaTroid.nTSA[0], 1, gbaTroid.nTSA.size(), fp);
		fclose(fp);
	}
	
	
	//MemFile* tileTable = new MemFile();
	//tileTable->ZeroSize();
	//for (int tsaCounter = 0; tsaCounter < TSA.nTSA.size(); tsaCounter++)
	//{
	//	unsigned short pftt = TSA.nTSA[tsaCounter];
	//	int flipX = pftt & 0x4000 ? 0x400 : 0;
	//	int flipY = pftt & 0x8000 ? 0x800 : 0;
	//	int pal = (((pftt & 0x1C00) >> 8) * 0x1000);
	//	int tile = (pftt + 0xC0) & 0x3FF;

	//	//gbaTroid.nTSA[tsaCounter] = pal+ 0x4000 | tile;
	//	//gbaTroid.nTSA[tsaCounter] = snes2gba_tilemap(pftt);
	//	tileTable->fput16(snes2gba_tilemap(pftt), true);
	//}
	//tileTable->Save("gbatroid.tiletable");
	//delete tileTable;
	return 0;
}
int SMClass::MageExport(int Area, int Room, bool IsMf)
{
	ExportTileset();
	ExportTileTable();
	MemFile*thisFile = new MemFile(1000);
	thisFile->WriteStr("MAGE 1.0 ROOM", true);
	thisFile->seek(16);
	thisFile->fputc((unsigned char)(IsMf), true);
	thisFile->fputc((unsigned char)Area, true);
	thisFile->fputc((unsigned char)Room, true);
	thisFile->seek(100);
	Backgrounds* theBgs = new Backgrounds(RoomHeader.Width * 16, RoomHeader.Height * 16);
	int  Width = RoomHeader.Width * 16;
	int Height = RoomHeader.Height * 16;
	int ThisY = 0;
	int ThisX = 0;
	RECT srcRect = { 0,0,0,0 };
	RECT dstRect = { 0,0,0,0 };;
	u16* TileBuf2D = &Map[1];
	//imgMap.Create(Width*16, Height*16);

	//Image* pic=&imgMap;

	//pic->SetPalette(&Pal[0]);
	for (int thisY = 0; thisY < Height; thisY++) {

		for (int thisX = 0; thisX < (Width); thisX++) {// from here if something is enabled then draw it 
			unsigned short	hflip = 1;
			unsigned short		vflip = 1;
			unsigned short		curTile = TileBuf2D[thisX + (thisY * Width)];
			unsigned short		TILE = (curTile & 0x3ff);
			unsigned short		flip = (curTile & 0xC00) >> 8;
			unsigned short clipdata = curTile & 0x1000 >> 0xC;
			//theBgs->clip->blocks[thisX + (thisY * Width)] = clipdata;
			theBgs->bg1->blocks[thisX + (thisY * Width)] = TILE;
			switch (flip) {

				//case 0x4:

				//	hflip = -1; dstRect.left = 15;
				//	break;
				//case 0x8:
				//	vflip = -1; dstRect.top = 15;
				//	break;
				//case 0xC:
				//	hflip = -1; dstRect.left = 15;
				//	vflip = -1; dstRect.top = 15;
				//	break;

			}


		}
	}
	thisFile->seek(100);
	int bgPointers[5] = { 0 };
	theBgs->Export(thisFile, bgPointers);



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
