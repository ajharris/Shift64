// Copyright (c) Robarts Research Institute 2006

//---------------------------------------------------------------------
// crc32.h -- compute the CRC-32 of a data stream
// Table generation Copyright (C) 1995-1998 Mark Adler (zlib)
//---------------------------------------------------------------------

#if !defined( __Crc32_Life_H )
#define __Crc32_Life_H

class LISCrc32
{
public:
	LISCrc32();
	unsigned long calculate(unsigned long crc, const char* buf, unsigned int len);

private:
	void InitTable();
	unsigned long crc_table[256];
};


#endif //__Crc32_Life_H
//---------------------------------------------------------------------
