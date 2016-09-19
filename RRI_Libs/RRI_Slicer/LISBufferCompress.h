// Copyright (c) Robarts Research Institute 2006

//---------------------------------------------------------------------
// LISBufferCompress
//---------------------------------------------------------------------

#if !defined( __LISBufferCompress_h )
#define __LISBufferCompress_h

//---------------------------------------------------------------------

#include "LISCrc32.h"

//---------------------------------------------------------------------

class LISBufferCompress
{
public:
	LISBufferCompress(int aCompressionLevel=1) { setCompressionLevel(aCompressionLevel); }
	virtual ~LISBufferCompress() {}

	//-----------------------------------------------------------------
	// Note that compress() hands you a new buffer that is YOUR 
	// responsibility to delete.
	// This uses the aCompressionLevel to determine how far to search
	// for code matches. Basically, 1 is fastest/worst, 9 is slow/best
	// Also note that the first 32 bits are dedicated to storing the
	// size of the buffer. 
	//-----------------------------------------------------------------
	unsigned char* compress(		unsigned char *input_buffer, 
									long maxIn, 
									long& zippedSize);

	//-----------------------------------------------------------------
	// decompress()
	// Note that the first 32 bits are dedicated to storing the
	// size of the buffer. 
	//-----------------------------------------------------------------
	unsigned char* decompress(		unsigned char *output_buffer, 
									unsigned char *input_buffer, 
									long maxOut );

	//-----------------------------------------------------------------
	// Does a quick checksum of the file. Note that it is table based,
	// so if you keep the LISBufferCompress alive longer the table
	// creation will be once only (that is, faster)
	//-----------------------------------------------------------------
	unsigned long calcBufferCRC(	const char *input_buffer, 
									long maxIn);

	//-----------------------------------------------------------------
	// setCompressionLevel()
	// 1 = poor compression, but fast (works well with volume data)
	// 9 = great compression, but sloooow (works REALLY well with volume data)
	//-----------------------------------------------------------------
	void setCompressionLevel(int aCompressionLevel)
	{ 
		if( m_compressionLevel < 0 )
			m_compressionLevel = 0;
		if( m_compressionLevel > 9 )
			m_compressionLevel = 9;
		m_compressionLevel = aCompressionLevel; 
	}

private:
	int m_compressionLevel;
};

#endif //__LISBufferCompress_h