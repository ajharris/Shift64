//---------------------------------------------------------------------
// crc32.cpp -- compute the CRC-32 of a data stream
//
// Table generation Copyright (C) 1995-1998 Mark Adler (zlib)
//
//---------------------------------------------------------------------
//#include "stdafx.h"
#include "LISCrc32.h"

#define DO1(buf)  crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

//---------------------------------------------------------------------

LISCrc32::LISCrc32()
{
	InitTable();
}

//---------------------------------------------------------------------

unsigned long LISCrc32::calculate(unsigned long crc, const char* buf, unsigned int len)
{
	if (buf == 0) 
		return 0L;

	crc = crc ^ 0xffffffffL;
	while (len >= 8)
	{
	  DO8(buf);
	  len -= 8;
	}
	if (len) do {
	  DO1(buf);
	} while (--len);
	return crc ^ 0xffffffffL;
}

//---------------------------------------------------------------------

void LISCrc32::InitTable()
{
	unsigned long c;
	int n, k;
	unsigned long poly;            // polynomial exclusive-or pattern
	// terms of polynomial defining this crc (except x^32):
	static const unsigned char p[] = {0,1,2,4,5,7,8,10,11,12,16,22,23,26};

	// make exclusive-or pattern from polynomial (0xedb88320L)
	poly = 0L;
	for (n = 0; n < sizeof(p)/sizeof(unsigned char); n++)
	poly |= 1L << (31 - p[n]);

	for (n = 0; n < 256; n++)
	{
		c = (unsigned long)n;
		for (k = 0; k < 8; k++)
		  c = c & 1 ? poly ^ (c >> 1) : c >> 1;
		crc_table[n] = c;
	}
}
