#pragma once

enum CompressionType
{
	NONE=0,
	RLE=16,
	LZ77=0x40,
};

class Compression
{
public:
	long   RLECompress(unsigned char cmp[], unsigned char uncmp[], unsigned long sze);
	Compression();
	~Compression();
};

