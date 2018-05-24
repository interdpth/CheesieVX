#include "SMClass.h"

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


	return ((mappedTile) & 0x3FF | ((tile & 0xC000) >> 4) | (((tile & 0x1C00) >> 10) + 2) * 0x1000 + 0x1000) & 0xFFFF;
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
	for (int rawTileIndex = 0; rawTileIndex < rawTiles.size() - 1; rawTileIndex++)
	{

		unsigned char* thisTile = new unsigned char[32];
		memcpy(thisTile, rawTiles.at(rawTileIndex), 32);


		int tileIndex = TileExists(thisTile, &newTiles);
		char debugtext[256] = { 0 };
		if (tileIndex == 0XBAAD)
		{
			newTiles.push_back(thisTile);
			//adjust tilemap
			newTileMapping[rawTileIndex] = newTiles.size() - 1;

			sprintf(debugtext, "Inserting tile %x -> %x\n", rawTileIndex, rawTileIndex, newTiles.size() - 1);
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
		fwrite(&Tiles[0], 1, Tiles.size(), fp2);
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
