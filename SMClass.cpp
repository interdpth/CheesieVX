#include "SMClass.h"
#include <wx/bitmap.h>

void          EncodePal(unsigned short *palGBA, COLOR *palPC, int numpals, char palpos)
{

	int             i, ii, s = 0;

	// unsigned short clr;
	unsigned long             clr = 0;;
	long            index = 0;

	for (ii = 0; ii < numpals; ii++)
	{
		index = 0x10 * ii;

		for (i = 0; i < 16; i++)
		{
			/*	clr = palPC[index + i] >> 3;
				palGBA[index + i] = (unsigned short)((clr & 0x1F) | (clr & 0x1F00) >> 3 | (clr & 0x1F0000) >> 6);
	*/
	// clr = (palPC[index + i]>>3);
			palGBA[index + i] = (unsigned short)(((palPC[index + i].r >> 3) & 0x1F) * 8 | (((palPC[index + i].g >> 3) & 0x1F00) >> 3) * 8 | (((palPC[index + i].b >> 3) & 0x1F0000) >> 6)) * 8;
		}
	}
	return;
}


//----- (00410DB8) --------------------------------------------------------
int __stdcall LunarSNEStoPC(unsigned long RomPointer)
{
	// converts CPU address to ROM addresses (assumes the ROM doesn't have a copier header)

	RomPointer &= 0x7FFFFF;

	if ((RomPointer & 0x8000) == 0)
		throw std::exception();

	RomPointer = ((RomPointer & 0x7F0000) >> 1) + (RomPointer & 0x7FFF);

	return RomPointer;
}
//----- (00410E6C) --------------------------------------------------------


int __stdcall LunarPCtoSNES(int RomPointer) {
	// converts a ROM Address to a CPU Address

	RomPointer &= 0x7FFFFF;

	RomPointer = ((RomPointer & 0x3F8000) << 1) + (RomPointer & 0x7FFF) | 0x8000;

	// take us to the upper banks if the game uses FastROM
	return (RomPointer);
}

u32 SMClass::Pnt2Off(unsigned long pointer) {
	// pointer &= 0x7FFFFF;

	// pointer = ( (pointer & 0x3F8000) << 1 ) + ( pointer & 0x7FFF ) | 0x8000;
	// Seperate the numbers into two groups: the right 4 numbers and the left 
	//however-many numbers. Multiply the left numbers by 2, then add 80. 
	//If the right numbers are less than 8000, add 8000 to them. If they are more than 8000, add 1 to the left numbers.
	//<LusterVanguard> 7E500 => 7 E500 => 8E E500 => 8F E500
	//LunarLoadDLL();

	pointer = LunarSNEStoPC(pointer);//I'm horrible. 
									 // LunarUnloadDLL();
	return pointer;
}
SMClass::SMClass(char* FilePath)
{
	Logger::log = new Logger("DebugFile.txt");
	strcpy(System.RomFilePath, FilePath);
	System.ROM = fopen(FilePath, "r+b");


	Setup();
}
SMClass::~SMClass() {}
int SMClass::Setup() {
	this->Tiles.resize(0x20000);
	this->Pal.resize(0x200);
	this->TSA.nTSA.resize(0x2080);
	gfxSet = 0;
	return 0;
}
int SMClass::SNES2GBA() {
	int i = 0;
	for (; i < Tiles.size(); i += 32) {
		SNES2GBA(&Tiles[i]);
	}
	return 0;
}
u32 SMClass::ReadPointer(u32 offset) {
	FILE* fp = System.ROM;
	u32 checker;
	char bytes[3];
	if (fp) {
		fseek(fp, offset, SEEK_SET);
		fread(bytes, 3, 1, fp);

	}

	checker = (int)bytes[0] & 0xFF;
	checker |= (int)(bytes[1] << 8 & 0x00FF00);
	checker |= (int)(bytes[2] << 16 & 0xFF0000);

	return Pnt2Off(checker);




}


int SMClass::GrabTileset(int GraphicsSet) {
	u32 TableOff = 0;
	u32 PalOff = 0;
	u32 TileOff = 0;
	int tsaDet = 0;
	int tiledst = 0;
	int size = 0;
	vector<u8> buffer;
	vector<u8> buffer2;
	int i = 0;
	u8* src;
	u8* dst;
	int table = 0x7E6A2 + ((GraphicsSet) * 9);
	TableOff = ReadPointer(table);
	TileOff = ReadPointer(table + 3);//0x1D4629
	PalOff = ReadPointer(table + 6);
	//Take two buffers and just combine them.
	tiledst = SMDecomp(TileOff, &buffer);

	tiledst = 20480;
	size = SMDecomp(0x1C8000, &buffer2);
	Tiles.resize(tiledst + size);
	memcpy(&Tiles[0], &buffer[0], tiledst);
	memcpy(&Tiles[tiledst], &buffer2[0], size);
	SNES2GBA();


	//SNES.DecodePal();//0x212d7c

	size = SMDecomp(PalOff, &buffer);


	Pal.resize(buffer.size());
	System.DecodeSNESPal(PalOff, &Pal[0], 8, 0, size, &buffer);
	char path[1024];
	sprintf(path, "%s\\roompal", exporthPath);

	FILE* fp2 = fopen(path, "wb");
	if (fp2)
	{
		fseek(fp2, 0x10*2, SEEK_SET);
		fwrite(&buffer.front(), 1, size, fp2);
		fclose(fp2);
	}


	//EncodePal(gbapal, &Pal[0], 16, 16);


	vector<u8> tsa1;
	vector<u8> tsa2;

	int fullsize;
	if (RoomHeader.Area != 6) {
		tsaDet = SMDecomp(0x1CA09D, &tsa1);
	}
	else {
		tsaDet = 0;
	}

	SMDecomp(TableOff, &tsa2);
	int tsacount = tsa1.size() / 8;
	TSA.nTSA.resize(tsa1.size() + tsa2.size());
	TSA.max = TSA.nTSA.size() / 8;
	src = &tsa1[0];
	dst = (unsigned char*)&TSA.nTSA[0];
	memcpy(dst, src, 0x800);
	//TSA.max=0x200+tsa2.size()/2;
	src = &tsa2[0];
	dst = (unsigned char*)&TSA.nTSA[0];//0x800*2?
	memcpy(&dst[0x800], src, tsa2.size());

	return 0;
}
int SMClass::SNES2GBA(u8* pnt) {
	//unsigned char src[32],dst[32];
	std::vector<u8> src, dst, rdst;
	src.resize(64);
	dst.resize(128);
	rdst.resize(64);

	int srcpnt = 0;
	int dstpnt = 0;
	int i = 0;
	int j = 0;
	memcpy(&src[0], pnt, 32);

	unsigned char line[4];
	int x, y, shift;
	unsigned long *dest_4bpp;
	dest_4bpp = (unsigned long*)&rdst[0];
	for (y = 0; y < 8; y++)
	{
		dest_4bpp[y] = 0;      // empty line
		line[0] = src[y * 2];   // plane 1
		line[1] = src[y * 2 + 1];   // plane 2
		line[2] = src[y * 2 + 16];// plane 3
		line[3] = src[y * 2 + 17];// plane 4
		for (x = 0; x < 8; x++)
		{
			shift = (7 - x) * 4;
			dest_4bpp[y] |= ((line[0] & 1) << shift) + ((line[1] & 1) << (shift + 1)) + ((line[2] & 1) << (shift + 2)) + ((line[3] & 1) << (shift + 3));
			line[0] >>= 1;
			line[1] >>= 1;
			line[2] >>= 1;
			line[3] >>= 1;
		}
	}
	memcpy(pnt, &rdst[0], 32);
	return 0;



}




int ValidType(int type) {
	if (type == 0xE5FF ||
		type == 0xE640 ||
		type == 0xE652 ||
		type == 0xE669 ||
		type == 0xE678) {
		return 2;
	}
	else if (type == 0xE612 ||
		type == 0xE629) {
		return 3;
	}
	else if (type == 0xE5EB) {
		return 4;
	}
	else if (type == 0xE5E6) {
		return 0;
	}
	else {
		return -1;
	}
}
int  SMClass::LoadMDB_StateSelect(u32 Address) {

	// max 10 roomstates / 52 bytes

	long pointer = 0;
	u8 Destination[52];
	int Counter = 0, RSCounter = 0;
	int E6Location = 0, FrabLocation = 0;
	u8 Frab = 0; // First Roommap Address Byte
	int TempBytes[3];
	u32 Pointer = 0;
	u16 type;
	u16 typepointer;
	long Offset = Address;
	char buffer[512] = { 0 };
	if (System.ROM) {
		while (RoomStatePointers.size() != 10) {
			sprintf(buffer, "Seeking %x", Offset);
			Logger::log->LogIt(Logger::DEBUG, buffer);
			fseek(System.ROM, Offset, SEEK_SET);
			Offset = ftell(System.ROM);
			fread(&type, 1, 2, System.ROM);
			sprintf(buffer, "Type is %x", type);
			Logger::log->LogIt(Logger::DEBUG, buffer);
			if (ValidType(type) == 0) {     // Standard state
				Pointer = Offset + 0x880002;    // Add 880002 to get SNES pointer (07XXXX + 880002 = 8FXXXX + 2)

				sprintf(buffer, "Raw Pointer is %x", Pointer);


				Logger::log->LogIt(Logger::DEBUG, buffer);

				sprintf(buffer, "Converted is %x", Pnt2Off(Pointer));

				Logger::log->LogIt(Logger::DEBUG, buffer);
				RoomStatePointers.push_back(Pnt2Off(Pointer));
			}
			else if (ValidType(type) == -1) {
				////Invalid room state. Do something here?
				//Offset--;
			}
			else {
				Offset = Offset + ValidType(type) + 2;      // If not a standard state
				fseek(System.ROM, Offset - 2, SEEK_SET);      // Set offset to next state header - 2 (location of the state pointer)
				fread(&typepointer, 1, 2, System.ROM);      // Get state pointer
				Pointer = typepointer + 0x8F0000;       // Add 8F0000 to get SNES pointer
				sprintf(buffer, "Raw Pointer is %x", Pointer);


				Logger::log->LogIt(Logger::DEBUG, buffer);

				sprintf(buffer, "Converted is %x", Pnt2Off(Pointer));

				Logger::log->LogIt(Logger::DEBUG, buffer);
				RoomStatePointers.push_back(Pnt2Off(Pointer));
				break;  // I dont know what this does?
			}
		}
		//fclose(System.ROM);
	}
	return RoomStatePointers.size();
}
void SMClass::GrabBG(vector<u8>* buffer)
{
	unsigned long bgdata_pointer = RoomStates[iRoomState].Bgdata_p;
	theBgs->bg2->layer = Layer::BG2;
	if (RoomStates[iRoomState].Layer2_Scr & 0x0101) {
		unsigned int LastROMPosition;
		return;
		long size = 0;
		std::vector<u8> cmpBuffer;
		std::vector<u8> dcmpBuffer;
		cmpBuffer.resize(0x10000);
		dcmpBuffer.resize(0x10000);
		unsigned char* src, dst;
		unsigned char dataDump[512];
		char debug[512];
		FILE* fp = fopen(System.RomFilePath, "r+b");
		if (fp) {
			int offset = Pnt2Off(bgdata_pointer);
			fseek(fp, offset, SEEK_SET);

			vector<unsigned char> data;
			unsigned char byteCode = fgetc(fp);
			unsigned long curAddr;

			unsigned short instruction;
			fread(&instruction, 1, 2, fp);
			unsigned long dataOffset = 0;
			unsigned long dstOffset = 0;
			unsigned short vramAddr = 0;
			unsigned short size = 0;
			unsigned short doorInfo = 0xBAAD;
			u32 checker;
			char bytes[3];

			while (instruction != 0)
			{
				dataOffset = 0;
				dstOffset = 0;
				vramAddr = 0;
				size = 0;
				doorInfo = 0xBAAD;
				switch (instruction)
				{
				case 0xe:
					fread(&doorInfo, 2, 1, fp);
				case 8:
				case 2:
					fread(bytes, 3, 1, fp);
					fread(&vramAddr, 1, 2, fp);
					fread(&size, 1, 2, fp);
					curAddr = ftell(fp);

					checker = (int)bytes[0] & 0xFF;
					checker |= (int)(bytes[1] << 8 & 0x00FF00);
					checker |= (int)(bytes[2] << 16 & 0xFF0000);
					checker = Pnt2Off(checker);
					sprintf(debug, "Would read bg data via instruction %x from %x to %x size %x", instruction, checker, vramAddr, size);
					if (doorInfo != 0xBaad)
					{
						sprintf(debug, "%s source door %x", debug, doorInfo);
					}
					Logger::log->LogIt(Logger::DEBUG, debug);
					break;
				case 4:
					fread(bytes, 3, 1, fp);
					fread(&dstOffset, 1, 2, fp);
					curAddr = ftell(fp);

					checker = (int)bytes[0] & 0xFF;
					checker |= (int)(bytes[1] << 8 & 0x00FF00);
					checker |= (int)(bytes[2] << 16 & 0xFF0000);
					checker = Pnt2Off(checker);
					sprintf(debug, "Would read bg data via instruction %x from %x to %x", instruction, checker, dstOffset);
					Logger::log->LogIt(Logger::DEBUG, debug);
					break;
					break;
				default:
					sprintf(debug, "Unsupported BG_DATA instruction %x", instruction);
					Logger::log->LogIt(Logger::ERRORZ, debug);
					break;
				}

				fread(&instruction, 1, 2, fp);
			}
			fclose(fp);
		}
	}
	else
	{
		//Load bg from level data


		int size = buffer->size();
		int possibles = size / 3;
		memcpy(&theBgs->bg2->blocks[0], &buffer->at(RoomHeader.Width * 16 * RoomHeader.Height * 16 * 3 + 2), RoomHeader.Width * 16 * RoomHeader.Height * 16 * 2);

	}
}
int SMClass::GrabRoom() {
	theBgs = new Backgrounds(RoomHeader.Width * 16, RoomHeader.Height * 16);
	vector<u8> buffer;
	u32 size = SMDecomp(RoomStates[iRoomState].Roommap_p, &buffer);

	Map.resize(size / 2);
	memcpy(&Map[0], &buffer[0], size);
	theBgs->bg1->layer = Layer::BG1;
	theBgs->bg1->blocks.resize(RoomHeader.Width * 16 * RoomHeader.Height * 16);

	memcpy(&theBgs->bg1->blocks.front(), &Map.front() + 1, RoomHeader.Width * 16 * RoomHeader.Height * 16 * 2);
	GrabBG(&buffer);
	return 0;
}

int SMClass::FindHeaders() {/*
							//make some Sanity Algo
							MDB_Header tmp;
							FILE* fp = fopen(SNES.FileName,"r+b");
							if(fp){
							while(!feof(fp)){


							}


							}*/
	return 0;
}
int SMClass::LoadHeader(u32 Address) {
	iRoomState = 0;
	_currentAddress = Address;
	FILE* fp = fopen(System.RomFilePath, "r+b");
	if (fp) {
		fseek(fp, Address, SEEK_SET);
		fread(&RoomHeader.unknown1, 1, sizeof(RoomHeader.unknown1), fp);
		fread(&RoomHeader.Area, 1, sizeof(RoomHeader.Area), fp);
		fread(&RoomHeader.Xpos, 1, sizeof(RoomHeader.Xpos), fp);
		fread(&RoomHeader.Ypos, 1, sizeof(RoomHeader.Ypos), fp);
		fread(&RoomHeader.Width, 1, sizeof(RoomHeader.Width), fp);

		fread(&RoomHeader.Height, 1, sizeof(RoomHeader.Height), fp);

		fread(&RoomHeader.Unknown1, 1, sizeof(RoomHeader.Unknown1), fp);
		fread(&RoomHeader.Unknown2, 1, sizeof(RoomHeader.Unknown1), fp);
		fread(&RoomHeader.Unknown3, 1, sizeof(RoomHeader.Unknown2), fp);

		fread(&RoomHeader.Pdoorout, 1, sizeof(RoomHeader.Pdoorout), fp);
		fclose(fp);
	}
	return RoomHeader.Pdoorout;
}



int SMClass::DrawRoom(wxMemoryDC* dst, wxMemoryDC* src, BG* bg, wxRasterOperationMode rop) {
	int thisX = 0, thisY = 0, mX = 0, mY = 0;
	u16 TILE = 0;
	int curTile = 0;
	int dstX = 0;
	int dstY = 0;
	int hflip = 1, vflip = 1, dstHeight, dstWidth;
	int i = 0;
	int flip = 0;
	int  Width = RoomHeader.Width * 16;
	int Height = RoomHeader.Height * 16;
	int ThisY = 0;
	int ThisX = 0;
	RECT srcRect = { 0,0,0,0 };
	RECT dstRect = { 0,0,0,0 };;
	u16* TileBuf2D = &bg->blocks.front();
	//imgMap.Create(Width*16, Height*16);

	//Image* pic=&imgMap;

	//bg->SetPalette(&Pal[0]);
	for (thisY = 0; thisY < Height; thisY++) {

		for (thisX = 0; thisX < (Width); thisX++) {// from here if something is enabled then draw it 
			hflip = 1;
			vflip = 1;
			curTile = TileBuf2D[thisX + (thisY * Width)];
			TILE = (curTile & 0x3ff);
			flip = (curTile & 0xC00) >> 8;


			dstRect.left = 0;
			dstRect.top = 0;
			dstRect.right = 0;
			dstRect.bottom = 0;
			switch (flip) {

			case 0x4:

				hflip = -1; dstRect.left = 15;
				break;
			case 0x8:
				vflip = -1; dstRect.top = 15;
				break;
			case 0xC:
				hflip = -1; dstRect.left = 15;
				vflip = -1; dstRect.top = 15;
				break;

			}

			//Upon reviewing this code I am only halfway sure of how it works. XD
			dstRect.left += thisX * 16;
			dstRect.top += thisY * 16;
			dstRect.right = hflip * 16;
			dstRect.bottom = vflip * 16;

			srcRect.left = ((TILE) % 32) * 16;
			srcRect.top = ((TILE) / 32) * 16;
			srcRect.right = 16;
			srcRect.bottom = 16;



			dst->StretchBlit(dstRect.left, dstRect.top, dstRect.right, dstRect.bottom, src, srcRect.left, srcRect.top, 16, 16, rop);
		}
	}
	//BlitToBB();
	return 0;
}
int SMClass::LoadMDB_Roomstate(u32 Address, MDB_Roomstate* OutputMDB_Roomstate) 
{
	FILE* fp = fopen(System.RomFilePath, "r+b");
	if (fp) {
		fseek(fp, Address, SEEK_SET);	
		u32 checker;
		char bytes[3];
		fread(bytes, 3, 1, fp);
		checker = (int)bytes[0] & 0xFF;
		checker |= (int)(bytes[1] << 8 & 0x00FF00);
		checker |= (int)(bytes[2] << 16 & 0xFF0000);
		OutputMDB_Roomstate->Roommap_p = Pnt2Off(checker);
		fread(&OutputMDB_Roomstate->GraphicsSet, 1, 1, fp);
		fread(&OutputMDB_Roomstate->MusicTrack, 1, 1, fp);
		fread(&OutputMDB_Roomstate->MusicControl, 1, 1, fp);
		fread(&OutputMDB_Roomstate->Fx1_p, 2, 1, fp);
		fread(&OutputMDB_Roomstate->Roompop_p, 2, 1, fp);
		fread(&OutputMDB_Roomstate->Enemyset_p, 2, 1, fp);
		fread(&OutputMDB_Roomstate->Layer2_Scr, 2, 1, fp);
		fread(&OutputMDB_Roomstate->Mdb_Scroll_p, 2, 1, fp);
		fread(&OutputMDB_Roomstate->Unknown, 2, 1, fp);
		fread(&OutputMDB_Roomstate->Fx2_p, 2, 1, fp);
		fread(&OutputMDB_Roomstate->Plm_p, 2, 1, fp);
		fread(&OutputMDB_Roomstate->Bgdata_p, 2, 1, fp);
		fread(&OutputMDB_Roomstate->LayersHandling, 2, 1, fp);
		fclose(fp);
	}
	return OutputMDB_Roomstate->GraphicsSet;
}



long SMClass::SMDecomp(u32 offset, vector<u8>* buffer) {
	unsigned int LastROMPosition;
	long size = 0;
	std::vector<u8> cmpBuffer;
	std::vector<u8> dcmpBuffer;
	cmpBuffer.resize(0x10000);
	dcmpBuffer.resize(0x10000);
	unsigned char* src, dst;
	FILE* fp = fopen(System.RomFilePath, "r+b");
	if (fp) {
		fseek(fp, offset, SEEK_SET);
		fread(&cmpBuffer[0], 1, cmpBuffer.size(), fp);
		size = Compression.Main_Decompress(&dcmpBuffer[0], &cmpBuffer[0], cmpBuffer.size());
		/// size=LunarDecompress(buffer,offset,0x10000,4,0,&LastROMPosition);
		buffer->resize(size);
		memcpy(&buffer->front(), &dcmpBuffer[0], size);

		fclose(fp);
	}
	//if (!size)
	// {
	//    printf("\nEither decompression failed, the format isn't supported, or the compressed structure has a size of 0.\n");
	//    return 0;
	// }


	return size;
}

