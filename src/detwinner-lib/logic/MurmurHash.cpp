/*
 ===============================================================================
 Name        : MurmurHash.cpp
 Author      : NeatDecisions
 Version     :
 Description : Adaptation of MurmurHash3 was written by Austin Appleby.
 ===============================================================================
 */


#include <logic/MurmurHash.hpp>

#include <fstream>
#include <sstream>
#include <stdint.h>


namespace detwinner {
namespace logic {


#define	FORCE_INLINE __attribute__((always_inline)) inline


//------------------------------------------------------------------------------
inline
uint32_t
rotl32 ( uint32_t x, int8_t r )
{
  return (x << r) | (x >> (32 - r));
}


#define	ROTL32(x,y)	rotl32(x,y)


//------------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

FORCE_INLINE
uint32_t
getblock ( const uint32_t * p, int i )
{
  return p[i];
}


//------------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche
FORCE_INLINE
uint32_t
fmix ( uint32_t h )
{
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}


//------------------------------------------------------------------------------
bool
MurmurHash::GetHash(const std::string & fileName, std::string & hash)
{
	std::ifstream f(fileName.c_str(), std::ifstream::in | std::ifstream::binary);

	if (!f) return false;

	constexpr uint32_t seed = 5;
	constexpr std::streamsize FILE_READ_BUFFER_SIZE = 4096;

	uint8_t key[FILE_READ_BUFFER_SIZE];

	uint32_t out[4];

	size_t len = 0, l = 0;

	uint32_t h1 = seed;
	uint32_t h2 = seed;
	uint32_t h3 = seed;
	uint32_t h4 = seed;

	constexpr uint32_t c1 = 0x239b961b;
	constexpr uint32_t c2 = 0xab0e9789;
	constexpr uint32_t c3 = 0x38b34ae5;
	constexpr uint32_t c4 = 0xa1e38b93;

	int nblocks = 0;// = len / 16;

	uint8_t * data = 0;

	f.read((char*)key, FILE_READ_BUFFER_SIZE);

	while ( (l = f.gcount()) > 0 )
	{
		len += l;
		data = (uint8_t*)key;
		nblocks = l / 16;

		//----------
		// body

		const uint32_t * blocks = (const uint32_t *)(data + nblocks*16);

		for(int i = -nblocks; i; i++)
		{
			uint32_t k1 = getblock(blocks,i*4+0);
			uint32_t k2 = getblock(blocks,i*4+1);
			uint32_t k3 = getblock(blocks,i*4+2);
			uint32_t k4 = getblock(blocks,i*4+3);

			k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;

			h1 = ROTL32(h1,19); h1 += h2; h1 = h1*5+0x561ccd1b;

			k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;

			h2 = ROTL32(h2,17); h2 += h3; h2 = h2*5+0x0bcaa747;

			k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;

			h3 = ROTL32(h3,15); h3 += h4; h3 = h3*5+0x96cd1c35;

			k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;

			h4 = ROTL32(h4,13); h4 += h1; h4 = h4*5+0x32ac3b17;
		}

		if (static_cast<size_t>(nblocks) * 16 != l)
			break;

		f.read((char*)key, FILE_READ_BUFFER_SIZE);
	}

	f.close();

	//----------
	// tail

	const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

	uint32_t k1 = 0;
	uint32_t k2 = 0;
	uint32_t k3 = 0;
	uint32_t k4 = 0;

	switch(len & 15)
	{
	case 15: k4 ^= tail[14] << 16;                                [[fallthrough]];
	case 14: k4 ^= tail[13] << 8;                                 [[fallthrough]];
	case 13: k4 ^= tail[12] << 0;
	         k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;   [[fallthrough]];

	case 12: k3 ^= tail[11] << 24;                                [[fallthrough]];
	case 11: k3 ^= tail[10] << 16;                                [[fallthrough]];
	case 10: k3 ^= tail[ 9] << 8;                                 [[fallthrough]];
	case  9: k3 ^= tail[ 8] << 0;
	         k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;   [[fallthrough]];

	case  8: k2 ^= tail[ 7] << 24;                                [[fallthrough]];
	case  7: k2 ^= tail[ 6] << 16;                                [[fallthrough]];
	case  6: k2 ^= tail[ 5] << 8;                                 [[fallthrough]];
	case  5: k2 ^= tail[ 4] << 0;
	         k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;   [[fallthrough]];

	case  4: k1 ^= tail[ 3] << 24;                                [[fallthrough]];
	case  3: k1 ^= tail[ 2] << 16;                                [[fallthrough]];
	case  2: k1 ^= tail[ 1] << 8;                                 [[fallthrough]];
	case  1: k1 ^= tail[ 0] << 0;
	         k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
	};

	//----------
	// finalization

	h1 ^= len; h2 ^= len; h3 ^= len; h4 ^= len;

	h1 += h2; h1 += h3; h1 += h4;
	h2 += h1; h3 += h1; h4 += h1;

	h1 = fmix(h1);
	h2 = fmix(h2);
	h3 = fmix(h3);
	h4 = fmix(h4);

	h1 += h2; h1 += h3; h1 += h4;
	h2 += h1; h3 += h1; h4 += h1;

	((uint32_t*)out)[0] = h1;
	((uint32_t*)out)[1] = h2;
	((uint32_t*)out)[2] = h3;
	((uint32_t*)out)[3] = h4;

		/*
	 * using a ostringstream to convert the hash in a
	 * hex string
	 */

	unsigned char * d = (unsigned char*) out;

	std::ostringstream os;
	for(int i=0; i<16; ++i)
	{
		/*
		 * set the width to 2
		 */
		os.width(2);

		/*
		 * fill with 0
		 */
		os.fill('0');

		/*
		 * conv to hex
		 */
		os << std::hex << static_cast<unsigned int>(d[i]);
	}

	/*
	 * return as std::string
	 */
	hash = os.str();
	return true;
}


}}
