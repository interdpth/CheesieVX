#include "MemFile.h"


MemFile* MemFile::currentFile;
MemFile::MemFile()
{
	IncreaseSize(5000);//Make a small footprint.
	fileIndex = 0;
}
void MemFile::ZeroOut()
{
	for (int counter = 0; counter < theFile.size(); counter++)
	{
		theFile[counter] = 0;
	}

}

void MemFile::ZeroSize()
{
	theFile.resize(0);
	fileIndex = 0;	
}
void MemFile::IncreaseSize(int size)
{
	theFile.resize(theFile.size()+size);
	size = theFile.size();
}
MemFile::MemFile(int size)
{
	IncreaseSize(size);
	ZeroOut();
	fileIndex = 0;
}

unsigned char* MemFile::GetFile()
{
	return &theFile.front();
}

MemFile::MemFile(char* file)
{
	FILE* FP = fopen(file, "r+b");
	fileName = file;
	fseek(FP, 0, SEEK_END);
	int len = ftell(FP);
	size = len;
	fseek(FP, 0,SEEK_SET);
	theFile.resize(len);

	::fread(&theFile.front(), 1, len, FP);
	fclose(FP);
	FP = NULL;
	fileIndex = 0;
}


void MemFile::Save()
{
	Save(fileName);
}
void MemFile::Save(char* file)
{
	FILE* FP = fopen(file, "w+b");
	::fseek(FP, 0, SEEK_SET);
	::fwrite(&theFile.front(), theFile.size(), 1, FP);
	::fclose(FP);
}
unsigned char MemFile::fgetc()
{
	return theFile[fileIndex++];
}
void MemFile::seek(unsigned long offset)
{
	fileIndex = offset; 
}
//void MemFile::fread(void* dst, int count, int size,FILE* fp)
//{
//	int readSize = count*size;
//	if (readSize + fileIndex > this->size)
//	{
//		readSize = this->size - readSize + fileIndex;
//	}
//	memcpy(dst, &theFile[fileIndex], readSize);
//	fileIndex += readSize;
//}
void MemFile::fwrite(void* src, int count, int size, bool resize)
{
	int readSize = count*size;

		if (GetIndex() + readSize > theFile.size())
		{
			IncreaseSize(GetIndex() + readSize- theFile.size() + 64);
		}
		

	memcpy(&theFile[fileIndex], src, readSize);
	fileIndex += readSize;
}
void MemFile::fwrite(void* src, int count, int size, FILE* fp)
{
	int readSize = count*size;
	if (GetIndex() + readSize > theFile.size())
	{
		IncreaseSize(GetIndex() + readSize - theFile.size() + 64);
	}
	memcpy(&theFile[fileIndex],src,  readSize);
	fileIndex += readSize;	
}

unsigned char MemFile::fputc(char val)
{
	if (fileIndex + 1 > theFile.size())
	{
		IncreaseSize(1);
	}
	theFile[fileIndex++] = val;
	printf("Writing %x to %x", val, fileIndex);
	return theFile[fileIndex - 1];
}


void MemFile::fput32(unsigned short val, bool increase)
{
	if (increase)
	{
		if (fileIndex + 4 > theFile.size())
		{
			IncreaseSize(4);
		}
	}

	theFile[fileIndex] = (unsigned char)val;
	theFile[fileIndex + 1] = (unsigned char)(val >> 8);
	theFile[fileIndex + 2] = (unsigned char)(val >> 16);
	theFile[fileIndex + 3] = (unsigned char)(val >> 24);
	printf("Writing %x to %x", val, fileIndex);
	fileIndex += 4;
}

void MemFile::fput16(unsigned short val, bool increase)
{

	if (increase)
	{
		if (fileIndex + 2 > theFile.size())
		{
			IncreaseSize(2);
		}
	}
	
	//memcpy(&theFile[fileIndex], &val, 2);

	theFile[fileIndex] = (unsigned char)val;
	theFile[fileIndex + 1] = (unsigned char)(val >> 8);
	printf("Writing %x to %x", val, fileIndex);
	fileIndex += 2;	
}

unsigned char MemFile::fputc(char val, bool increase)
{

	if (increase)
	{
		if (fileIndex + 1 > theFile.size())
		{
			IncreaseSize(1);
		}
	}
	theFile[fileIndex++]=val;
	printf("Writing %x to %x", val, fileIndex);
	return theFile[fileIndex-1];
}
long MemFile::GetIndex()
{
	return fileIndex;
}
void MemFile::Align(int rem)
{
	while (fileIndex % rem != 0)
	{
		fileIndex++;
	}
}
void  MemFile::WriteStr(char* thisString, bool increase)
{
	int slen = strlen(thisString);
	if (increase)
	{
		if (fileIndex + slen > theFile.size())
		{
			IncreaseSize(fileIndex + slen - theFile.size());
		}
	}
	memcpy(&theFile.front() + fileIndex, thisString, slen );
	printf("Writing %s to %x", thisString, fileIndex);
	fileIndex += slen;
}
MemFile::~MemFile()
{
	
}
