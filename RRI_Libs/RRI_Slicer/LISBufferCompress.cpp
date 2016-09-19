//---------------------------------------------------------------------
// LISBufferCompress.cpp


//---------------------------------------------------------------------
//#include "stdafx.h"
#include "LISBufferCompress.h"
#include "zlib.h"

//---------------------------------------------------------------------
// Decompress
//---------------------------------------------------------------------
unsigned char* LISBufferCompress::decompress(unsigned char *output_buffer, unsigned char *input_buffer, long maxOut)
{
	if( output_buffer )
	{
		long* compressedSizeInHeader		= (long*)input_buffer;
		long* originalSizeInHeader			= (long*)(input_buffer+sizeof(long));
		long* crcInHeader					= (long*)(input_buffer+(sizeof(long)*2));
		unsigned char* startOfInputData		= input_buffer+(sizeof(long)*3);
		
		z_stream z;
		int status;

		z.zalloc = 0;
		z.zfree = 0; 
		z.opaque = 0;

		inflateInit(&z);

		z.next_in = (unsigned char*)startOfInputData;	// location of input
		z.avail_in = *compressedSizeInHeader;
		z.next_out = (unsigned char*)output_buffer;
		z.avail_out = maxOut;

		status = inflate( &z, Z_SYNC_FLUSH );

		// NOTE: It gets data errors for some reason! Is this too small a
		// buffer (off by one?) No, it checks out. This seems to be an unreliable
		// failure in ZLIB, so we'll do our own CRC to verify the buffer.


		//if( status == Z_OK )
		if( *crcInHeader == calcBufferCRC((const char*)output_buffer, *originalSizeInHeader))
		{
			deflateEnd(&z);
			return output_buffer;
		}
		deflateEnd(&z);
	}
	return 0;
}

//---------------------------------------------------------------------
// Compress
//
// Contains:
//   COMPRESSED SIZE (4 bytes)
//   ORIG SIZE (4 bytes)
//   CRC  (4 bytes)
//   DATA (zippedSize bytes)
//
//---------------------------------------------------------------------
unsigned char* LISBufferCompress::compress(unsigned char *input_buffer, long max, long& zippedSize)
{
	unsigned char* output_buffer = new unsigned char[max+(sizeof(long)*3)+1];
	if( output_buffer )
	{
		unsigned char* startOfOutputData = output_buffer+(sizeof(long)*3);

		long* compressedSizeInHeader	= (long*)output_buffer;
		long* origSizeInHeader			= (long*)(output_buffer+sizeof(long));
		long* crcInHeader				= (long*)(output_buffer+(sizeof(long)*2));

		*origSizeInHeader = max;

		z_stream z;
		int status;

		z.zalloc = 0;
		z.zfree = 0; 
		z.opaque = 0;

		deflateInit(&z, m_compressionLevel);

		z.next_in = (unsigned char*)input_buffer;	// location of input
		z.avail_in = max;
		z.next_out = (unsigned char*)startOfOutputData;
		z.avail_out = max-1;

		status = deflate( &z, Z_SYNC_FLUSH );

		if( status == Z_OK)
		{
			zippedSize				= z.total_out;
			*compressedSizeInHeader = z.total_out;
			deflateEnd(&z);

			*crcInHeader = calcBufferCRC((const char*)startOfOutputData, zippedSize);

			return output_buffer;
		}
		deflateEnd(&z);
		crcInHeader = 0;
		*compressedSizeInHeader = 0;
		delete output_buffer;
	}
	zippedSize = 0;
	return 0;
}

//---------------------------------------------------------------------
// calcBufferCRC()
// - allows integrity checks
//---------------------------------------------------------------------
unsigned long LISBufferCompress::calcBufferCRC(	const char *input_buffer, long maxIn)
{
	static LISCrc32 crc;
	return crc.calculate(0, input_buffer, maxIn);
}
