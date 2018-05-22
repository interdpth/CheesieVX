#pragma once
#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdio.h>
using namespace std;
class MemFile
{
public:

	static MemFile* currentFile;
	MemFile();
	void fwrite(void* src, int count, int size, bool resize);
	MemFile(FILE* fp);
	MemFile(int size);
	MemFile(char* file);
	void Save();
	void Save(char* file);
	void seek(unsigned long offset);

	unsigned char fgetc();
	unsigned char fputc(char val);
	void fwrite(void* dst, int count, int size, FILE* fp);
	int size;
	void IncreaseSize(int newSize);
	unsigned char* GetFile();
	~MemFile();
	void WriteStr(char* thisString, bool increase);
	unsigned char fputc(char val, bool increase);
	void fput16(unsigned short val, bool increase);
		void fput32(unsigned short val, bool increase);
	void ZeroOut();
	long GetIndex();
	void Align(int rem=4);
	void ZeroSize();
private: 
	vector<unsigned char> theFile; 
	long fileIndex; 
	char* fileName;
	FILE* inuseFile;
};

