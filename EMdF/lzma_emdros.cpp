/*
 * LZMA encoding and decoding for Emdros.
 *
 * Created: 3/29-2018.
 * Last update: 10/4-2018.
 *
 * Based on the Public Domain parts of PDLzip by Antonio Diaz Diaz,
 * version 1.9 released 2018-02-05.  Downloaded from:
 * http://download.savannah.gnu.org/releases/lzip/pdlzip/
 *
 * PDLzip, in turn, is based on some Public Domain parts of the LZMA
 * SDK by Igor Pavlov.
 *
 * Modified by Ulrik Sandborg-Petersen for inclusion in Emdros.
 *
 * This file is in the Public Domain.  It does not contain any
 * BSD-licensed code from PDLzip.
 *
 * 
 *
 */

#include <emdros-lconfig.h>

#if defined(USE_BPT) && USE_BPT
	


#define _FILE_OFFSET_BITS 64

#include <cstdlib>
#include <cstring>

#include "lzma_emdros.h"


/* LzFind.c -- Match finder for LZ algorithms
2009-04-22 : Igor Pavlov : Public domain */



#define emdros_lzma_crc32_TABLE_LENGTH (256)
#define emdros_lzma_crc32_POLY (0xEDB88320L)

static uint32_t emdros_lzma_crc32_table[emdros_lzma_crc32_TABLE_LENGTH] = {
  0x00000000L,  0x77073096L,  0xEE0E612CL,  0x990951BAL,
  0x076DC419L,  0x706AF48FL,  0xE963A535L,  0x9E6495A3L,
  0x0EDB8832L,  0x79DCB8A4L,  0xE0D5E91EL,  0x97D2D988L,
  0x09B64C2BL,  0x7EB17CBDL,  0xE7B82D07L,  0x90BF1D91L,
  0x1DB71064L,  0x6AB020F2L,  0xF3B97148L,  0x84BE41DEL,
  0x1ADAD47DL,  0x6DDDE4EBL,  0xF4D4B551L,  0x83D385C7L,
  0x136C9856L,  0x646BA8C0L,  0xFD62F97AL,  0x8A65C9ECL,
  0x14015C4FL,  0x63066CD9L,  0xFA0F3D63L,  0x8D080DF5L,
  0x3B6E20C8L,  0x4C69105EL,  0xD56041E4L,  0xA2677172L,
  0x3C03E4D1L,  0x4B04D447L,  0xD20D85FDL,  0xA50AB56BL,
  0x35B5A8FAL,  0x42B2986CL,  0xDBBBC9D6L,  0xACBCF940L,
  0x32D86CE3L,  0x45DF5C75L,  0xDCD60DCFL,  0xABD13D59L,
  0x26D930ACL,  0x51DE003AL,  0xC8D75180L,  0xBFD06116L,
  0x21B4F4B5L,  0x56B3C423L,  0xCFBA9599L,  0xB8BDA50FL,
  0x2802B89EL,  0x5F058808L,  0xC60CD9B2L,  0xB10BE924L,
  0x2F6F7C87L,  0x58684C11L,  0xC1611DABL,  0xB6662D3DL,
  0x76DC4190L,  0x01DB7106L,  0x98D220BCL,  0xEFD5102AL,
  0x71B18589L,  0x06B6B51FL,  0x9FBFE4A5L,  0xE8B8D433L,
  0x7807C9A2L,  0x0F00F934L,  0x9609A88EL,  0xE10E9818L,
  0x7F6A0DBBL,  0x086D3D2DL,  0x91646C97L,  0xE6635C01L,
  0x6B6B51F4L,  0x1C6C6162L,  0x856530D8L,  0xF262004EL,
  0x6C0695EDL,  0x1B01A57BL,  0x8208F4C1L,  0xF50FC457L,
  0x65B0D9C6L,  0x12B7E950L,  0x8BBEB8EAL,  0xFCB9887CL,
  0x62DD1DDFL,  0x15DA2D49L,  0x8CD37CF3L,  0xFBD44C65L,
  0x4DB26158L,  0x3AB551CEL,  0xA3BC0074L,  0xD4BB30E2L,
  0x4ADFA541L,  0x3DD895D7L,  0xA4D1C46DL,  0xD3D6F4FBL,
  0x4369E96AL,  0x346ED9FCL,  0xAD678846L,  0xDA60B8D0L,
  0x44042D73L,  0x33031DE5L,  0xAA0A4C5FL,  0xDD0D7CC9L,
  0x5005713CL,  0x270241AAL,  0xBE0B1010L,  0xC90C2086L,
  0x5768B525L,  0x206F85B3L,  0xB966D409L,  0xCE61E49FL,
  0x5EDEF90EL,  0x29D9C998L,  0xB0D09822L,  0xC7D7A8B4L,
  0x59B33D17L,  0x2EB40D81L,  0xB7BD5C3BL,  0xC0BA6CADL,
  0xEDB88320L,  0x9ABFB3B6L,  0x03B6E20CL,  0x74B1D29AL,
  0xEAD54739L,  0x9DD277AFL,  0x04DB2615L,  0x73DC1683L,
  0xE3630B12L,  0x94643B84L,  0x0D6D6A3EL,  0x7A6A5AA8L,
  0xE40ECF0BL,  0x9309FF9DL,  0x0A00AE27L,  0x7D079EB1L,
  0xF00F9344L,  0x8708A3D2L,  0x1E01F268L,  0x6906C2FEL,
  0xF762575DL,  0x806567CBL,  0x196C3671L,  0x6E6B06E7L,
  0xFED41B76L,  0x89D32BE0L,  0x10DA7A5AL,  0x67DD4ACCL,
  0xF9B9DF6FL,  0x8EBEEFF9L,  0x17B7BE43L,  0x60B08ED5L,
  0xD6D6A3E8L,  0xA1D1937EL,  0x38D8C2C4L,  0x4FDFF252L,
  0xD1BB67F1L,  0xA6BC5767L,  0x3FB506DDL,  0x48B2364BL,
  0xD80D2BDAL,  0xAF0A1B4CL,  0x36034AF6L,  0x41047A60L,
  0xDF60EFC3L,  0xA867DF55L,  0x316E8EEFL,  0x4669BE79L,
  0xCB61B38CL,  0xBC66831AL,  0x256FD2A0L,  0x5268E236L,
  0xCC0C7795L,  0xBB0B4703L,  0x220216B9L,  0x5505262FL,
  0xC5BA3BBEL,  0xB2BD0B28L,  0x2BB45A92L,  0x5CB36A04L,
  0xC2D7FFA7L,  0xB5D0CF31L,  0x2CD99E8BL,  0x5BDEAE1DL,
  0x9B64C2B0L,  0xEC63F226L,  0x756AA39CL,  0x026D930AL,
  0x9C0906A9L,  0xEB0E363FL,  0x72076785L,  0x05005713L,
  0x95BF4A82L,  0xE2B87A14L,  0x7BB12BAEL,  0x0CB61B38L,
  0x92D28E9BL,  0xE5D5BE0DL,  0x7CDCEFB7L,  0x0BDBDF21L,
  0x86D3D2D4L,  0xF1D4E242L,  0x68DDB3F8L,  0x1FDA836EL,
  0x81BE16CDL,  0xF6B9265BL,  0x6FB077E1L,  0x18B74777L,
  0x88085AE6L,  0xFF0F6A70L,  0x66063BCAL,  0x11010B5CL,
  0x8F659EFFL,  0xF862AE69L,  0x616BFFD3L,  0x166CCF45L,
  0xA00AE278L,  0xD70DD2EEL,  0x4E048354L,  0x3903B3C2L,
  0xA7672661L,  0xD06016F7L,  0x4969474DL,  0x3E6E77DBL,
  0xAED16A4AL,  0xD9D65ADCL,  0x40DF0B66L,  0x37D83BF0L,
  0xA9BCAE53L,  0xDEBB9EC5L,  0x47B2CF7FL,  0x30B5FFE9L,
  0xBDBDF21CL,  0xCABAC28AL,  0x53B39330L,  0x24B4A3A6L,
  0xBAD03605L,  0xCDD70693L,  0x54DE5729L,  0x23D967BFL,
  0xB3667A2EL,  0xC4614AB8L,  0x5D681B02L,  0x2A6F2B94L,
  0xB40BBE37L,  0xC30C8EA1L,  0x5A05DF1BL,  0x2D02EF8DL,
};



static uint32_t emdros_lzma_crc32(uint32_t a, uint32_t b)
{
	uint32_t idx, prev;
	
	prev = (a >> 8) & 0x00FFFFFF;
	idx = (a ^ b) & 0xFF;
	return (prev ^ emdros_lzma_crc32_table[idx] ^ 0xFFFFFFFF);
}

uint32_t emdros_lzma_crc32_calc(const unsigned char *inbuf, int buf_len)
{
	int x;
	uint32_t crc = 0xFFFFFFFFL;
	for (x = 0;
	     x < buf_len;
	     ++x) {
		crc = emdros_lzma_crc32(crc, inbuf[x]);
	}
	return crc;
}

static inline void emdros_lzma_crc32_update_buf(uint32_t *crc, const unsigned char *inbuf, int buf_len)
{
	int x;
	uint32_t mycrc = *crc;
	for (x = 0;
	     x < buf_len;
	     ++x) {
		mycrc = emdros_lzma_crc32(mycrc, inbuf[x]);
	}
	*crc = mycrc;
}


#define Ft_size (20)

/*  0-3  CRC32 of the uncompressed data */
/*  4-11 size of the uncompressed data */
/* 12-19 member size including header and trailer */
typedef uint8_t File_trailer[Ft_size];

static inline unsigned int Ft_get_data_crc(const File_trailer data)
{
	unsigned int result = 0;
	result = data[0]
		| (data[1] << 8)
		| (data[2] << 16)
		| (data[3] << 24);
	return result;
}


static inline void Ft_set_data_crc(File_trailer data, unsigned int crc)
{
	data[0] = crc & 0xFF;
	crc >>= 8;
	data[1] = crc & 0xFF;
	crc >>= 8;
	data[2] = crc & 0xFF;
	crc >>= 8;
	data[3] = crc & 0xFF;
}

static inline unsigned long long Ft_get_data_size(const File_trailer data)
{
	unsigned long long result = 0;
	result = data[11];
	result <<= 8;
	result += data[10];
	result <<= 8;
	result += data[9];
	result <<= 8;
	result += data[8];
	result <<= 8;
	result += data[7];
	result <<= 8;
	result += data[6];
	result <<= 8;
	result += data[5];
	result <<= 8;
	result += data[4];
	return result;
}

static inline void Ft_set_data_size(File_trailer data, unsigned long long sz)
{
	data[4] = sz & 0xFF;
	sz >>= 8;
	data[5] = sz & 0xFF;
	sz >>= 8;
	data[6] = sz & 0xFF;
	sz >>= 8;
	data[7] = sz & 0xFF;
	sz >>= 8;
	data[8] = sz & 0xFF;
	sz >>= 8;
	data[9] = sz & 0xFF;
	sz >>= 8;
	data[10] = sz & 0xFF;
	sz >>= 8;
	data[11] = sz & 0xFF;
}

static inline unsigned long long Ft_get_member_size(const File_trailer data)
{
	unsigned long long result = 0;
	result = data[19];
	result <<= 8;
	result += data[18];
	result <<= 8;
	result += data[17];
	result <<= 8;
	result += data[16];
	result <<= 8;
	result += data[15];
	result <<= 8;
	result += data[14];
	result <<= 8;
	result += data[13];
	result <<= 8;
	result += data[12];
	return result;
}

static inline void Ft_set_member_size(File_trailer data, unsigned long long sz)
{
	data[12] = sz & 0xFF;
	sz >>= 8;
	data[13] = sz & 0xFF;
	sz >>= 8;
	data[14] = sz & 0xFF;
	sz >>= 8;
	data[15] = sz & 0xFF;
	sz >>= 8;
	data[16] = sz & 0xFF;
	sz >>= 8;
	data[17] = sz & 0xFF;
	sz >>= 8;
	data[18] = sz & 0xFF;
	sz >>= 8;
	data[19] = sz & 0xFF;
}

static const uint8_t emdros_lzma_magic_string[4] = { 0x45, 0x4d, 0x4c, 0x5a }; /* "EMLZ" */

static inline void EMDROS_LZMA_Fh_set_magic(EMDROS_LZMA_File_header data)
{
	data[0] = emdros_lzma_magic_string[0];
	data[1] = emdros_lzma_magic_string[1];
	data[2] = emdros_lzma_magic_string[2];
	data[3] = emdros_lzma_magic_string[3];
	data[4] = 2; /* version */
}

bool EMDROS_LZMA_Fh_verify_magic(const EMDROS_LZMA_File_header data)
{
	return data[0] == emdros_lzma_magic_string[0]
		&& data[1] == emdros_lzma_magic_string[1]
		&& data[2] == emdros_lzma_magic_string[2]
		&& data[3] == emdros_lzma_magic_string[3];
}

void EMDROS_LZMA_set_uncompressed_size(size_t uncompressed_size, uint8_t *uncompressed_size_buf)
{
	uncompressed_size_buf[0] = uncompressed_size & 0xFF;
	uncompressed_size >>= 8;
	uncompressed_size_buf[1] = uncompressed_size & 0xFF;
	uncompressed_size >>= 8;
	uncompressed_size_buf[2] = uncompressed_size & 0xFF;
	uncompressed_size >>= 8;
	uncompressed_size_buf[3] = uncompressed_size & 0xFF;
	uncompressed_size >>= 8;
	uncompressed_size_buf[4] = uncompressed_size & 0xFF;
	uncompressed_size >>= 8;
	uncompressed_size_buf[5] = uncompressed_size & 0xFF;
	uncompressed_size >>= 8;
	uncompressed_size_buf[6] = uncompressed_size & 0xFF;
	uncompressed_size >>= 8;
	uncompressed_size_buf[7] = uncompressed_size & 0xFF;
}

size_t EMDROS_LZMA_get_uncompressed_size(uint8_t *uncompressed_size_buf)
{
	size_t uncompressed_size = 0;
	uncompressed_size = uncompressed_size_buf[7];
	uncompressed_size <<= 8;
	uncompressed_size |= uncompressed_size_buf[6];
	uncompressed_size <<= 8;
	uncompressed_size |= uncompressed_size_buf[5];
	uncompressed_size <<= 8;
	uncompressed_size |= uncompressed_size_buf[4];
	uncompressed_size <<= 8;
	uncompressed_size |= uncompressed_size_buf[3];
	uncompressed_size <<= 8;
	uncompressed_size |= uncompressed_size_buf[2];
	uncompressed_size <<= 8;
	uncompressed_size |= uncompressed_size_buf[1];
	uncompressed_size <<= 8;
	uncompressed_size |= uncompressed_size_buf[0];

	return uncompressed_size;
}


inline int get_max_bit(unsigned int value)
{
	int bits = 0;
	for ( ;; ) {
		if (value == 0) {
			break;
		}
		++bits;
		value >>= 1;
	}
	return bits;
}

uint32_t EMDROS_LZMA_Fh_get_dictionary_size(const EMDROS_LZMA_File_header data)
{
	uint8_t d = data[5];
	
	// Get first 5 bits
	int max_bits = d & 0x1F;
	
	// Get remaining 3 bits
	int upper_bits = (d >> 5) & 0x07;
	
	uint32_t dictSize = 1 << max_bits;
	
	if (upper_bits > 0) {
		dictSize -= (dictSize >> 4) * (upper_bits);
	}
	
	return dictSize;
}

static inline bool EMDROS_LZMA_Fh_set_dictionary_size(EMDROS_LZMA_File_header header, unsigned int dictionary_size)
{
	int max_bits = get_max_bit(dictionary_size - 1);
	
	unsigned int base_size = 1 << max_bits;
	unsigned int fraction = base_size >> 4;
	int result = 0;
	int i;
	for (i = 7;
	     i >= 1;
	     --i) {
		if (base_size - (i * fraction) >= dictionary_size) {
			
			result = (i << 5);
			break;
		}
	}
	header[5] = max_bits | result;
	return true;
}



#define kHash2Size (1 << 10)
#define kHash3Size (1 << 16)
#define kHash4Size (1 << 20)

#define kFix3HashSize (kHash2Size)
#define kFix4HashSize (kHash2Size + kHash3Size)

#define HASH2_CALC hashValue = cur[0] | ((uint32_t)cur[1] << 8);

#define HASH3_CALC {				\
uint32_t temp = emdros_lzma_crc32_table[cur[0]] ^ cur[1];	\
hash2Value = temp & (kHash2Size - 1);		\
hashValue = (temp ^ ((uint32_t)cur[2] << 8)) & p->hashMask; }

#define HASH4_CALC { \
  uint32_t temp = emdros_lzma_crc32_table[cur[0]] ^ cur[1]; \
  hash2Value = temp & (kHash2Size - 1); \
  hash3Value = (temp ^ ((uint32_t)cur[2] << 8)) & (kHash3Size - 1); \
  hashValue = (temp ^ ((uint32_t)cur[2] << 8) ^ (emdros_lzma_crc32_table[cur[3]] << 5)) & p->hashMask; }

#define kEmptyHashValue 0
#define kMaxValForNormalize ((uint32_t)0xFFFFFFFF)
#define kNormalizeStepMin (1 << 10) /* it must be power of 2 */
#define kNormalizeMask (~(kNormalizeStepMin - 1))

#define kStartMaxLen 3


static inline uint32_t Mf_GetNumAvailableBytes(CMatchFinder *p)
  { return p->streamPos - p->pos; }

static inline uint8_t Mf_GetIndexByte(CMatchFinder *p, int index)
  { return p->buffer[index]; }

static inline uint8_t * Mf_GetPointerToCurrentPos(CMatchFinder *p)
  { return p->buffer; }


static void Mf_ReadBlock(CMatchFinder *p)
{
  if (p->streamEndWasReached || p->result != EMDROS_SZ_OK)
    return;
  int emdros_errno = 0;
  for (;;)
  {
    uint8_t * const dest = p->buffer + (p->streamPos - p->pos);
    const int size = (p->bufferBase + p->blockSize - dest);
    int rd;
    if (size == 0)
      return;
    rd = p->lzma_readblock(p->pFIN, dest, size, &emdros_errno);
    if (rd != size && emdros_errno)
      { p->result = EMDROS_SZ_ERROR_READ; return; }
    if (rd == 0)
    {
      p->streamEndWasReached = true;
      return;
    }
    emdros_lzma_crc32_update_buf( &p->crc, dest, rd );
    p->streamPos += rd;
    if (p->streamPos - p->pos > p->keepSizeAfter)
      return;
  }
}


static void Mf_CheckAndMoveAndRead(CMatchFinder *p)
{
  if ((uint32_t)(p->bufferBase + p->blockSize - p->buffer) <= p->keepSizeAfter)
    {
    memmove(p->bufferBase,
      p->buffer - p->keepSizeBefore,
      p->streamPos - p->pos + p->keepSizeBefore);
    p->buffer = p->bufferBase + p->keepSizeBefore;
    }
  Mf_ReadBlock(p);
}


void Mf_Free(CMatchFinder *p)
{
  free(p->hash);
  p->hash = 0;
  free(p->bufferBase);
  p->bufferBase = 0;
}

static CLzRef* AllocRefs(uint32_t num)
{
  uint32_t sizeInBytes = num * sizeof(CLzRef);
  if (sizeInBytes / sizeof(CLzRef) != num)
    return 0;
  return (CLzRef *)malloc(sizeInBytes);
}

static void Mf_SetLimits(CMatchFinder *p)
{
  uint32_t limit = kMaxValForNormalize - p->pos;
  uint32_t limit2 = p->cyclicBufferSize - p->cyclicBufferPos;
  if (limit2 < limit)
    limit = limit2;
  limit2 = p->streamPos - p->pos;
  if (limit2 <= p->keepSizeAfter)
  {
    if (limit2 > 0)
      limit2 = 1;
  }
  else
    limit2 -= p->keepSizeAfter;
  if (limit2 < limit)
    limit = limit2;
  {
    uint32_t lenLimit = p->streamPos - p->pos;
    if (lenLimit > p->matchMaxLen)
      lenLimit = p->matchMaxLen;
    p->lenLimit = lenLimit;
  }
  p->posLimit = p->pos + limit;
}


int Mf_Init(CMatchFinder *p, void *pFIN, const int mc, uint32_t historySize,
	    uint32_t keepAddBufferBefore, uint32_t matchMaxLen, uint32_t keepAddBufferAfter,
	    emdros_lzma_readblock_function_ptr match_readblock_func)
{
  const uint32_t sizeReserv = ( historySize >> 1 ) +
    (keepAddBufferBefore + matchMaxLen + keepAddBufferAfter) / 2 + (1 << 19);

  p->hash = 0;
  p->cutValue = mc;
  p->pFIN = pFIN;
  p->btMode = true;
  p->numHashBytes = 4;
  p->crc = 0xFFFFFFFFU;
  p->keepSizeBefore = historySize + keepAddBufferBefore + 1;
  p->keepSizeAfter = matchMaxLen + keepAddBufferAfter;
  /* we need one additional byte, since we use MoveBlock after pos++ and before dictionary using */
  /* keepSizeBefore + keepSizeAfter + sizeReserv must be < 4G) */
  p->blockSize = p->keepSizeBefore + p->keepSizeAfter + sizeReserv;
  p->buffer = p->bufferBase = (uint8_t *)malloc(p->blockSize);
  p->lzma_readblock = match_readblock_func;
  if( p->bufferBase )
  {
    uint32_t newCyclicBufferSize = historySize + 1;
    uint32_t hs;
    p->matchMaxLen = matchMaxLen;
    {
      if (p->numHashBytes == 2)
        hs = (1 << 16) - 1;
      else
      {
        hs = historySize - 1;
        hs |= (hs >> 1);
        hs |= (hs >> 2);
        hs |= (hs >> 4);
        hs |= (hs >> 8);
        hs >>= 1;
        hs |= 0xFFFF; /* don't change it! It's required for Deflate */
        if (hs > (1 << 24))
        {
          if (p->numHashBytes == 3)
            hs = (1 << 24) - 1;
          else
            hs >>= 1;
        }
      }
      p->hashMask = hs;
      hs++;
      if (p->numHashBytes > 2) hs += kHash2Size;
      if (p->numHashBytes > 3) hs += kHash3Size;
      if (p->numHashBytes > 4) hs += kHash4Size;
    }

    {
      uint32_t newSize;
      p->historySize = historySize;
      p->hashSizeSum = hs;
      p->cyclicBufferSize = newCyclicBufferSize;
      p->numSons = (p->btMode ? newCyclicBufferSize * 2 : newCyclicBufferSize);
      newSize = p->hashSizeSum + p->numSons;
      p->hash = AllocRefs(newSize);
      if (p->hash != 0)
      {
        uint32_t i;
        p->son = p->hash + p->hashSizeSum;
        for (i = 0; i < p->hashSizeSum; i++)
          p->hash[i] = kEmptyHashValue;
        p->cyclicBufferPos = 0;
        p->pos = p->streamPos = p->cyclicBufferSize;
        p->result = EMDROS_SZ_OK;
        p->streamEndWasReached = false;
        Mf_ReadBlock(p);
        Mf_SetLimits(p);
        return 1;
      }
    }
  }
  Mf_Free(p);
  return 0;
}

static void Mf_Normalize3(uint32_t subValue, CLzRef *items, uint32_t numItems)
{
  uint32_t i;
  for (i = 0; i < numItems; i++)
  {
    uint32_t value = items[i];
    if (value <= subValue)
      value = kEmptyHashValue;
    else
      value -= subValue;
    items[i] = value;
  }
}

static void Mf_Normalize(CMatchFinder *p)
{
  uint32_t subValue = (p->pos - p->historySize - 1) & kNormalizeMask;
  Mf_Normalize3(subValue, p->hash, p->hashSizeSum + p->numSons);
  p->posLimit -= subValue;
  p->pos -= subValue;
  p->streamPos -= subValue;
}

static void Mf_CheckLimits(CMatchFinder *p)
{
  if (p->pos == kMaxValForNormalize)
    Mf_Normalize(p);
  if (!p->streamEndWasReached && p->keepSizeAfter == p->streamPos - p->pos)
    Mf_CheckAndMoveAndRead(p);
  if (p->cyclicBufferPos == p->cyclicBufferSize)
    p->cyclicBufferPos = 0;
  Mf_SetLimits(p);
}

static uint32_t * Hc_GetMatchesSpec(uint32_t lenLimit, uint32_t curMatch, uint32_t pos, const uint8_t *cur, CLzRef *son,
    uint32_t _cyclicBufferPos, uint32_t _cyclicBufferSize, uint32_t cutValue,
    uint32_t *distances, uint32_t maxLen)
{
  son[_cyclicBufferPos] = curMatch;
  for (;;)
  {
    uint32_t delta = pos - curMatch;
    if (cutValue-- == 0 || delta >= _cyclicBufferSize)
      return distances;
    {
      const uint8_t *pb = cur - delta;
      curMatch = son[_cyclicBufferPos - delta + ((delta > _cyclicBufferPos) ? _cyclicBufferSize : 0)];
      if (pb[maxLen] == cur[maxLen] && *pb == *cur)
      {
        uint32_t len = 0;
        while (++len != lenLimit)
          if (pb[len] != cur[len])
            break;
        if (maxLen < len)
        {
          *distances++ = maxLen = len;
          *distances++ = delta - 1;
          if (len == lenLimit)
            return distances;
        }
      }
    }
  }
}


static uint32_t * GetMatchesSpec1( uint32_t lenLimit, uint32_t curMatch,
    uint32_t pos, const uint8_t *cur, CLzRef *son,
    uint32_t _cyclicBufferPos, uint32_t _cyclicBufferSize, uint32_t cutValue,
    uint32_t *distances, uint32_t maxLen )
{
  CLzRef *ptr0 = son + (_cyclicBufferPos << 1) + 1;
  CLzRef *ptr1 = son + (_cyclicBufferPos << 1);
  uint32_t len0 = 0, len1 = 0;
  for (;;)
  {
    uint32_t delta = pos - curMatch;
    if (cutValue-- == 0 || delta >= _cyclicBufferSize)
    {
      *ptr0 = *ptr1 = kEmptyHashValue;
      return distances;
    }
    {
      CLzRef *pair = son + ((_cyclicBufferPos - delta + ((delta > _cyclicBufferPos) ? _cyclicBufferSize : 0)) << 1);
      const uint8_t *pb = cur - delta;
      uint32_t len = (len0 < len1 ? len0 : len1);
      if (pb[len] == cur[len])
      {
        if (++len != lenLimit && pb[len] == cur[len])
          while (++len != lenLimit)
            if (pb[len] != cur[len])
              break;
        if (maxLen < len)
        {
          *distances++ = maxLen = len;
          *distances++ = delta - 1;
          if (len == lenLimit)
          {
            *ptr1 = pair[0];
            *ptr0 = pair[1];
            return distances;
          }
        }
      }
      if (pb[len] < cur[len])
      {
        *ptr1 = curMatch;
        ptr1 = pair + 1;
        curMatch = *ptr1;
        len1 = len;
      }
      else
      {
        *ptr0 = curMatch;
        ptr0 = pair;
        curMatch = *ptr0;
        len0 = len;
      }
    }
  }
}

static void SkipMatchesSpec(uint32_t lenLimit, uint32_t curMatch, uint32_t pos, const uint8_t *cur, CLzRef *son,
    uint32_t _cyclicBufferPos, uint32_t _cyclicBufferSize, uint32_t cutValue)
{
  CLzRef *ptr0 = son + (_cyclicBufferPos << 1) + 1;
  CLzRef *ptr1 = son + (_cyclicBufferPos << 1);
  uint32_t len0 = 0, len1 = 0;
  for (;;)
  {
    uint32_t delta = pos - curMatch;
    if (cutValue-- == 0 || delta >= _cyclicBufferSize)
    {
      *ptr0 = *ptr1 = kEmptyHashValue;
      return;
    }
    {
      CLzRef *pair = son + ((_cyclicBufferPos - delta + ((delta > _cyclicBufferPos) ? _cyclicBufferSize : 0)) << 1);
      const uint8_t *pb = cur - delta;
      uint32_t len = (len0 < len1 ? len0 : len1);
      if (pb[len] == cur[len])
      {
        while (++len != lenLimit)
          if (pb[len] != cur[len])
            break;
        {
          if (len == lenLimit)
          {
            *ptr1 = pair[0];
            *ptr0 = pair[1];
            return;
          }
        }
      }
      if (pb[len] < cur[len])
      {
        *ptr1 = curMatch;
        ptr1 = pair + 1;
        curMatch = *ptr1;
        len1 = len;
      }
      else
      {
        *ptr0 = curMatch;
        ptr0 = pair;
        curMatch = *ptr0;
        len0 = len;
      }
    }
  }
}

#define MOVE_POS \
  ++p->cyclicBufferPos; \
  p->buffer++; \
  if (++p->pos == p->posLimit) Mf_CheckLimits(p);

#define MOVE_POS_RET MOVE_POS return offset;

static void Mf_MovePos(CMatchFinder *p) { MOVE_POS; }

#define GET_MATCHES_HEADER2(minLen, ret_op) \
  uint32_t lenLimit; uint32_t hashValue; const uint8_t *cur; uint32_t curMatch; \
  lenLimit = p->lenLimit; { if (lenLimit < minLen) { Mf_MovePos(p); ret_op; }} \
  cur = p->buffer;

#define GET_MATCHES_HEADER(minLen) GET_MATCHES_HEADER2(minLen, return 0)
#define SKIP_HEADER(minLen)        GET_MATCHES_HEADER2(minLen, continue)

#define MF_PARAMS(p) p->pos, p->buffer, p->son, p->cyclicBufferPos, p->cyclicBufferSize, p->cutValue

#define GET_MATCHES_FOOTER(offset, maxLen) \
  offset = (uint32_t)(GetMatchesSpec1(lenLimit, curMatch, MF_PARAMS(p), \
  distances + offset, maxLen) - distances); MOVE_POS_RET;

#define SKIP_FOOTER \
  SkipMatchesSpec(lenLimit, curMatch, MF_PARAMS(p)); MOVE_POS;

static uint32_t Bt2_MatchFinder_GetMatches(CMatchFinder *p, uint32_t *distances)
{
  uint32_t offset;
  GET_MATCHES_HEADER(2)
  HASH2_CALC;
  curMatch = p->hash[hashValue];
  p->hash[hashValue] = p->pos;
  offset = 0;
  GET_MATCHES_FOOTER(offset, 1)
}

static uint32_t Bt3_MatchFinder_GetMatches(CMatchFinder *p, uint32_t *distances)
{
  uint32_t hash2Value, delta2, maxLen, offset;
  GET_MATCHES_HEADER(3)

  HASH3_CALC;

  delta2 = p->pos - p->hash[hash2Value];
  curMatch = p->hash[kFix3HashSize + hashValue];

  p->hash[hash2Value] =
  p->hash[kFix3HashSize + hashValue] = p->pos;


  maxLen = 2;
  offset = 0;
  if (delta2 < p->cyclicBufferSize && *(cur - delta2) == *cur)
  {
    for (; maxLen != lenLimit; maxLen++)
      if (cur[(ptrdiff_t)maxLen - delta2] != cur[maxLen])
        break;
    distances[0] = maxLen;
    distances[1] = delta2 - 1;
    offset = 2;
    if (maxLen == lenLimit)
    {
      SkipMatchesSpec(lenLimit, curMatch, MF_PARAMS(p));
      MOVE_POS_RET;
    }
  }
  GET_MATCHES_FOOTER(offset, maxLen)
}

static uint32_t Bt4_MatchFinder_GetMatches(CMatchFinder *p, uint32_t *distances)
{
  uint32_t hash2Value, hash3Value, delta2, delta3, maxLen, offset;
  GET_MATCHES_HEADER(4)

  HASH4_CALC;

  delta2 = p->pos - p->hash[                hash2Value];
  delta3 = p->pos - p->hash[kFix3HashSize + hash3Value];
  curMatch = p->hash[kFix4HashSize + hashValue];

  p->hash[                hash2Value] =
  p->hash[kFix3HashSize + hash3Value] =
  p->hash[kFix4HashSize + hashValue] = p->pos;

  maxLen = 1;
  offset = 0;
  if (delta2 < p->cyclicBufferSize && *(cur - delta2) == *cur)
  {
    distances[0] = maxLen = 2;
    distances[1] = delta2 - 1;
    offset = 2;
  }
  if (delta2 != delta3 && delta3 < p->cyclicBufferSize && *(cur - delta3) == *cur)
  {
    maxLen = 3;
    distances[offset + 1] = delta3 - 1;
    offset += 2;
    delta2 = delta3;
  }
  if (offset != 0)
  {
    for (; maxLen != lenLimit; maxLen++)
      if (cur[(ptrdiff_t)maxLen - delta2] != cur[maxLen])
        break;
    distances[offset - 2] = maxLen;
    if (maxLen == lenLimit)
    {
      SkipMatchesSpec(lenLimit, curMatch, MF_PARAMS(p));
      MOVE_POS_RET;
    }
  }
  if (maxLen < 3)
    maxLen = 3;
  GET_MATCHES_FOOTER(offset, maxLen)
}

static uint32_t Hc4_MatchFinder_GetMatches(CMatchFinder *p, uint32_t *distances)
{
  uint32_t hash2Value, hash3Value, delta2, delta3, maxLen, offset;
  GET_MATCHES_HEADER(4)

  HASH4_CALC;

  delta2 = p->pos - p->hash[                hash2Value];
  delta3 = p->pos - p->hash[kFix3HashSize + hash3Value];
  curMatch = p->hash[kFix4HashSize + hashValue];

  p->hash[                hash2Value] =
  p->hash[kFix3HashSize + hash3Value] =
  p->hash[kFix4HashSize + hashValue] = p->pos;

  maxLen = 1;
  offset = 0;
  if (delta2 < p->cyclicBufferSize && *(cur - delta2) == *cur)
  {
    distances[0] = maxLen = 2;
    distances[1] = delta2 - 1;
    offset = 2;
  }
  if (delta2 != delta3 && delta3 < p->cyclicBufferSize && *(cur - delta3) == *cur)
  {
    maxLen = 3;
    distances[offset + 1] = delta3 - 1;
    offset += 2;
    delta2 = delta3;
  }
  if (offset != 0)
  {
    for (; maxLen != lenLimit; maxLen++)
      if (cur[(ptrdiff_t)maxLen - delta2] != cur[maxLen])
        break;
    distances[offset - 2] = maxLen;
    if (maxLen == lenLimit)
    {
      p->son[p->cyclicBufferPos] = curMatch;
      MOVE_POS_RET;
    }
  }
  if (maxLen < 3)
    maxLen = 3;
  offset = (uint32_t)(Hc_GetMatchesSpec(lenLimit, curMatch, MF_PARAMS(p),
    distances + offset, maxLen) - (distances));
  MOVE_POS_RET
}


static void Bt2_MatchFinder_Skip(CMatchFinder *p, uint32_t num)
{
  do
  {
    SKIP_HEADER(2)
    HASH2_CALC;
    curMatch = p->hash[hashValue];
    p->hash[hashValue] = p->pos;
    SKIP_FOOTER
  }
  while (--num != 0);
}


static void Bt3_MatchFinder_Skip(CMatchFinder *p, uint32_t num)
{
  do
  {
    uint32_t hash2Value;
    SKIP_HEADER(3)
    HASH3_CALC;
    curMatch = p->hash[kFix3HashSize + hashValue];
    p->hash[hash2Value] =
    p->hash[kFix3HashSize + hashValue] = p->pos;
    SKIP_FOOTER
  }
  while (--num != 0);
}

static void Bt4_MatchFinder_Skip(CMatchFinder *p, uint32_t num)
{
  do
  {
    uint32_t hash2Value, hash3Value;
    SKIP_HEADER(4)
    HASH4_CALC;
    curMatch = p->hash[kFix4HashSize + hashValue];
    p->hash[                hash2Value] =
    p->hash[kFix3HashSize + hash3Value] = p->pos;
    p->hash[kFix4HashSize + hashValue] = p->pos;
    SKIP_FOOTER
  }
  while (--num != 0);
}

static void Hc4_MatchFinder_Skip(CMatchFinder *p, uint32_t num)
{
  do
  {
    uint32_t hash2Value, hash3Value;
    SKIP_HEADER(4)
    HASH4_CALC;
    curMatch = p->hash[kFix4HashSize + hashValue];
    p->hash[                hash2Value] =
    p->hash[kFix3HashSize + hash3Value] =
    p->hash[kFix4HashSize + hashValue] = p->pos;
    p->son[p->cyclicBufferPos] = curMatch;
    MOVE_POS
  }
  while (--num != 0);
}


void Mf_CreateVTable(CMatchFinder *p, IMatchFinder *vTable)
{
  if (!p->btMode)
  {
    vTable->GetMatches = (Mf_GetMatches_Func)Hc4_MatchFinder_GetMatches;
    vTable->Skip = (Mf_Skip_Func)Hc4_MatchFinder_Skip;
  }
  else if (p->numHashBytes == 2)
  {
    vTable->GetMatches = (Mf_GetMatches_Func)Bt2_MatchFinder_GetMatches;
    vTable->Skip = (Mf_Skip_Func)Bt2_MatchFinder_Skip;
  }
  else if (p->numHashBytes == 3)
  {
    vTable->GetMatches = (Mf_GetMatches_Func)Bt3_MatchFinder_GetMatches;
    vTable->Skip = (Mf_Skip_Func)Bt3_MatchFinder_Skip;
  }
  else
  {
    vTable->GetMatches = (Mf_GetMatches_Func)Bt4_MatchFinder_GetMatches;
    vTable->Skip = (Mf_Skip_Func)Bt4_MatchFinder_Skip;
  }
}





/* LzmaEnc.c -- LZMA Encoder
2009-11-24 : Igor Pavlov : Public domain */

#define _FILE_OFFSET_BITS 64

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* #define SHOW_STAT */
/* #define SHOW_STAT2 */

#ifdef SHOW_STAT
static int ttt = 0;
#endif

#define kNumTopBits 24
#define kTopValue ((uint32_t)1 << kNumTopBits)

#define kNumBitModelTotalBits 11
#define kBitModelTotal (1 << kNumBitModelTotalBits)
#define kNumMoveBits 5
#define kProbInitValue (kBitModelTotal >> 1)

#define kNumMoveReducingBits 4
#define kNumBitPriceShiftBits 4

#define kNumLogBits (9 + (int)sizeof(uint32_t) / 2)
#define kDicLogSizeMaxCompress ((kNumLogBits - 1) * 2 + 7)

typedef void * CLzmaEncHandle;

CLzmaEncHandle LzmaEnc_Init( const int dict_size, const int match_len_limit,
                             const int infd, const int outfd );
void LzmaEnc_Free(CLzmaEncHandle p);
int LzmaEnc_Encode(CLzmaEncHandle p);


static void LzmaEnc_FastPosInit(uint8_t *g_FastPos)
{
  int c = 2, slotFast;
  g_FastPos[0] = 0;
  g_FastPos[1] = 1;

  for (slotFast = 2; slotFast < kNumLogBits * 2; slotFast++)
  {
    uint32_t k = (1 << ((slotFast >> 1) - 1));
    uint32_t j;
    for (j = 0; j < k; j++, c++)
      g_FastPos[c] = (uint8_t)slotFast;
  }
}

#define BSR2_RET(pos, res) { uint32_t i = 6 + ((kNumLogBits - 1) & \
  (0 - (((((uint32_t)1 << (kNumLogBits + 6)) - 1) - pos) >> 31))); \
  res = p->g_FastPos[pos >> i] + (i * 2); }
/*
#define BSR2_RET(pos, res) { res = (pos < (1 << (kNumLogBits + 6))) ? \
  p->g_FastPos[pos >> 6] + 12 : \
  p->g_FastPos[pos >> (6 + kNumLogBits - 1)] + (6 + (kNumLogBits - 1)) * 2; }
*/

#define GetPosSlot1(pos) p->g_FastPos[pos]
#define GetPosSlot2(pos, res) { BSR2_RET(pos, res); }
#define GetPosSlot(pos, res) { if (pos < kNumFullDistances) res = p->g_FastPos[pos]; else BSR2_RET(pos, res); }


#define LZMA_NUM_REPS 4

typedef struct
{
  uint32_t price;

  State state;

  uint32_t posPrev2;
  uint32_t backPrev2;

  uint32_t posPrev;
  uint32_t backPrev;
  uint32_t backs[LZMA_NUM_REPS];

  bool prev1IsChar;
  bool prev2;
} COptimal;

#define kNumOpts (1 << 12)

#define kNumLenToPosStates 4
#define kNumPosSlotBits 6
#define kDicLogSizeMin 0
#define kDicLogSizeMax 32
#define kDistTableSizeMax (kDicLogSizeMax * 2)


#define kNumAlignBits 4
#define kAlignTableSize (1 << kNumAlignBits)
#define kAlignMask (kAlignTableSize - 1)

#define kStartPosModelIndex 4
#define kEndPosModelIndex 14
#define kNumPosModels (kEndPosModelIndex - kStartPosModelIndex)

#define kNumFullDistances (1 << (kEndPosModelIndex >> 1))

#define LZMA_LC_MAX 8
#define LZMA_LP_MAX 4
#define LZMA_PB_MAX 4

#define LZMA_NUM_PB_STATES_MAX (1 << LZMA_PB_MAX)


#define kLenNumLowBits 3
#define kLenNumLowSymbols (1 << kLenNumLowBits)
#define kLenNumMidBits 3
#define kLenNumMidSymbols (1 << kLenNumMidBits)
#define kLenNumHighBits 8
#define kLenNumHighSymbols (1 << kLenNumHighBits)

#define kLenNumSymbolsTotal (kLenNumLowSymbols + kLenNumMidSymbols + kLenNumHighSymbols)

#define LZMA_MATCH_LEN_MIN 2
#define LZMA_MATCH_LEN_MAX (LZMA_MATCH_LEN_MIN + kLenNumSymbolsTotal - 1)

#define kNumStates 12

typedef struct
{
  int choice;
  int choice2;
  int low[LZMA_NUM_PB_STATES_MAX << kLenNumLowBits];
  int mid[LZMA_NUM_PB_STATES_MAX << kLenNumMidBits];
  int high[kLenNumHighSymbols];
} CLenEnc;

typedef struct
{
  CLenEnc p;
  uint32_t prices[LZMA_NUM_PB_STATES_MAX][kLenNumSymbolsTotal];
  uint32_t tableSize;
  uint32_t counters[LZMA_NUM_PB_STATES_MAX];
} CLenPriceEnc;

typedef struct
{
  uint64_t low;
  uint64_t processed;
  uint8_t *bufBase;
  uint8_t *buf;
  uint8_t *bufLim;
  uint32_t range;
  uint32_t cacheSize;
  void *pFOUT;
  int res;
  uint8_t cache;
  emdros_lzma_writeblock_function_ptr lzma_writeblock;
} CRangeEnc;


#define literal_context_bits (3)
#define pos_state_bits       (2)
#define min_dictionary_size  (1 << 12)

typedef struct
{
  uint64_t nowPos64;
  int *litProbs;
  IMatchFinder matchFinder;
  CMatchFinder matchFinderBase;

  uint32_t optimumEndIndex;
  uint32_t optimumCurrentIndex;

  uint32_t longestMatchLength;
  uint32_t numPairs;
  uint32_t numAvail;
  COptimal opt[kNumOpts];

  uint8_t g_FastPos[1 << kNumLogBits];

  uint32_t ProbPrices[kBitModelTotal >> kNumMoveReducingBits];
  uint32_t matches[LZMA_MATCH_LEN_MAX * 2 + 2 + 1];
  uint32_t numFastBytes;
  uint32_t additionalOffset;
  uint32_t reps[LZMA_NUM_REPS];
  State state;

  uint32_t posSlotPrices[kNumLenToPosStates][kDistTableSizeMax];
  uint32_t distancesPrices[kNumLenToPosStates][kNumFullDistances];
  uint32_t alignPrices[kAlignTableSize];
  uint32_t alignPriceCount;

  uint32_t distTableSize;

  unsigned lc, lp, pb;
  unsigned lpMask, pbMask;

  int isMatch[kNumStates][LZMA_NUM_PB_STATES_MAX];
  int isRep[kNumStates];
  int isRepG0[kNumStates];
  int isRepG1[kNumStates];
  int isRepG2[kNumStates];
  int isRep0Long[kNumStates][LZMA_NUM_PB_STATES_MAX];

  int posSlotEncoder[kNumLenToPosStates][1 << kNumPosSlotBits];
  int posEncoders[kNumFullDistances - kEndPosModelIndex];
  int posAlignEncoder[1 << kNumAlignBits];

  CLenPriceEnc lenEnc;
  CLenPriceEnc repLenEnc;

  CRangeEnc rc;

  uint32_t matchPriceCount;

  int result;
  uint32_t dictSize;
  bool fastMode;
  bool finished;
} CLzmaEnc;


static const int kLiteralNextStates[kNumStates] = {0, 0, 0, 0, 1, 2, 3,  4,  5,  6,  4,  5};
static const int kMatchNextStates[kNumStates]   = {7, 7, 7, 7, 7, 7, 7, 10, 10, 10, 10, 10};
static const int kRepNextStates[kNumStates]     = {8, 8, 8, 8, 8, 8, 8, 11, 11, 11, 11, 11};
static const int kShortRepNextStates[kNumStates]= {9, 9, 9, 9, 9, 9, 9, 11, 11, 11, 11, 11};

#define IsCharState(s) ((s) < 7)

#define GetLenToPosState(len) (((len) < kNumLenToPosStates + 1) ? (len) - 2 : kNumLenToPosStates - 1)

#define kInfinityPrice (1 << 30)

#define RC_BUF_SIZE (1 << 16)


static int RangeEnc_Init( CRangeEnc *p, void *pFOUT,
			  emdros_lzma_writeblock_function_ptr emdros_lzma_writeblock)
  {
  p->low = 0;
  p->processed = 0;
  p->range = 0xFFFFFFFF;
  p->cacheSize = 1;
  p->pFOUT = pFOUT;
  p->res = EMDROS_SZ_OK;
  p->cache = 0;
  p->buf = p->bufBase = (uint8_t *)malloc( RC_BUF_SIZE );
  p->lzma_writeblock = emdros_lzma_writeblock;
  if( !p->bufBase ) return 0;
  p->bufLim = p->bufBase + RC_BUF_SIZE;
  return 1;
  }


static void RangeEnc_Free(CRangeEnc *p)
{
  free(p->bufBase);
  p->bufBase = 0;
}


static void RangeEnc_FlushStream(CRangeEnc *p)
{
  int emdros_errno = 0;
  int num;
  if (p->res != EMDROS_SZ_OK)
    return;
  num = p->buf - p->bufBase;
  if (num != p->lzma_writeblock(p->pFOUT, p->bufBase, num, &emdros_errno))
    p->res = EMDROS_SZ_ERROR_WRITE;
  p->processed += num;
  p->buf = p->bufBase;
}

static void RangeEnc_ShiftLow(CRangeEnc *p)
{
  if ((uint32_t)p->low < (uint32_t)0xFF000000 || (int)(p->low >> 32) != 0)
  {
    uint8_t temp = p->cache;
    do
    {
      uint8_t *buf = p->buf;
      *buf++ = (uint8_t)(temp + (uint8_t)(p->low >> 32));
      p->buf = buf;
      if (buf == p->bufLim)
        RangeEnc_FlushStream(p);
      temp = 0xFF;
    }
    while (--p->cacheSize != 0);
    p->cache = (uint8_t)((uint32_t)p->low >> 24);
  }
  p->cacheSize++;
  p->low = (uint32_t)p->low << 8;
}

static void RangeEnc_FlushData(CRangeEnc *p)
{
  int i;
  for (i = 0; i < 5; i++)
    RangeEnc_ShiftLow(p);
}

static void RangeEnc_EncodeDirectBits(CRangeEnc *p, uint32_t value, int numBits)
{
  do
  {
    p->range >>= 1;
    p->low += p->range & (0 - ((value >> --numBits) & 1));
    if (p->range < kTopValue)
    {
      p->range <<= 8;
      RangeEnc_ShiftLow(p);
    }
  }
  while (numBits != 0);
}

static void RangeEnc_EncodeBit(CRangeEnc *p, int *prob, uint32_t symbol)
{
  uint32_t ttt = *prob;
  uint32_t newBound = (p->range >> kNumBitModelTotalBits) * ttt;
  if (symbol == 0)
  {
    p->range = newBound;
    ttt += (kBitModelTotal - ttt) >> kNumMoveBits;
  }
  else
  {
    p->low += newBound;
    p->range -= newBound;
    ttt -= ttt >> kNumMoveBits;
  }
  *prob = (int)ttt;
  if (p->range < kTopValue)
  {
    p->range <<= 8;
    RangeEnc_ShiftLow(p);
  }
}

static void LitEnc_Encode(CRangeEnc *p, int *probs, uint32_t symbol)
{
  symbol |= 0x100;
  do
  {
    RangeEnc_EncodeBit(p, probs + (symbol >> 8), (symbol >> 7) & 1);
    symbol <<= 1;
  }
  while (symbol < 0x10000);
}

static void LitEnc_EncodeMatched(CRangeEnc *p, int *probs, uint32_t symbol, uint32_t matchByte)
{
  uint32_t offs = 0x100;
  symbol |= 0x100;
  do
  {
    matchByte <<= 1;
    RangeEnc_EncodeBit(p, probs + (offs + (matchByte & offs) + (symbol >> 8)), (symbol >> 7) & 1);
    symbol <<= 1;
    offs &= ~(matchByte ^ symbol);
  }
  while (symbol < 0x10000);
}

static void LzmaEnc_InitPriceTables(uint32_t *ProbPrices)
{
  uint32_t i;
  for (i = (1 << kNumMoveReducingBits) / 2; i < kBitModelTotal; i += (1 << kNumMoveReducingBits))
  {
    const int kCyclesBits = kNumBitPriceShiftBits;
    uint32_t w = i;
    uint32_t bitCount = 0;
    int j;
    for (j = 0; j < kCyclesBits; j++)
    {
      w = w * w;
      bitCount <<= 1;
      while (w >= ((uint32_t)1 << 16))
      {
        w >>= 1;
        bitCount++;
      }
    }
    ProbPrices[i >> kNumMoveReducingBits] = ((kNumBitModelTotalBits << kCyclesBits) - 15 - bitCount);
  }
}


#define GET_PRICE(prob, symbol) \
  p->ProbPrices[((prob) ^ (((-(int)(symbol))) & (kBitModelTotal - 1))) >> kNumMoveReducingBits];

#define GET_PRICEa(prob, symbol) \
  ProbPrices[((prob) ^ ((-((int)(symbol))) & (kBitModelTotal - 1))) >> kNumMoveReducingBits];

#define GET_PRICE_0(prob) p->ProbPrices[(prob) >> kNumMoveReducingBits]
#define GET_PRICE_1(prob) p->ProbPrices[((prob) ^ (kBitModelTotal - 1)) >> kNumMoveReducingBits]

#define GET_PRICE_0a(prob) ProbPrices[(prob) >> kNumMoveReducingBits]
#define GET_PRICE_1a(prob) ProbPrices[((prob) ^ (kBitModelTotal - 1)) >> kNumMoveReducingBits]

static uint32_t LitEnc_GetPrice(const int *probs, uint32_t symbol, uint32_t *ProbPrices)
{
  uint32_t price = 0;
  symbol |= 0x100;
  do
  {
    price += GET_PRICEa(probs[symbol >> 8], (symbol >> 7) & 1);
    symbol <<= 1;
  }
  while (symbol < 0x10000);
  return price;
}

static uint32_t LitEnc_GetPriceMatched(const int *probs, uint32_t symbol, uint32_t matchByte, uint32_t *ProbPrices)
{
  uint32_t price = 0;
  uint32_t offs = 0x100;
  symbol |= 0x100;
  do
  {
    matchByte <<= 1;
    price += GET_PRICEa(probs[offs + (matchByte & offs) + (symbol >> 8)], (symbol >> 7) & 1);
    symbol <<= 1;
    offs &= ~(matchByte ^ symbol);
  }
  while (symbol < 0x10000);
  return price;
}


static void RcTree_Encode(CRangeEnc *rc, int *probs, int numBitLevels, uint32_t symbol)
{
  uint32_t m = 1;
  int i;
  for (i = numBitLevels; i != 0;)
  {
    uint32_t bit;
    i--;
    bit = (symbol >> i) & 1;
    RangeEnc_EncodeBit(rc, probs + m, bit);
    m = (m << 1) | bit;
  }
}

static void RcTree_ReverseEncode(CRangeEnc *rc, int *probs, int numBitLevels, uint32_t symbol)
{
  uint32_t m = 1;
  int i;
  for (i = 0; i < numBitLevels; i++)
  {
    uint32_t bit = symbol & 1;
    RangeEnc_EncodeBit(rc, probs + m, bit);
    m = (m << 1) | bit;
    symbol >>= 1;
  }
}

static uint32_t RcTree_GetPrice(const int *probs, int numBitLevels, uint32_t symbol, uint32_t *ProbPrices)
{
  uint32_t price = 0;
  symbol |= (1 << numBitLevels);
  while (symbol != 1)
  {
    price += GET_PRICEa(probs[symbol >> 1], symbol & 1);
    symbol >>= 1;
  }
  return price;
}

static uint32_t RcTree_ReverseGetPrice(const int *probs, int numBitLevels, uint32_t symbol, uint32_t *ProbPrices)
{
  uint32_t price = 0;
  uint32_t m = 1;
  int i;
  for (i = numBitLevels; i != 0; i--)
  {
    uint32_t bit = symbol & 1;
    symbol >>= 1;
    price += GET_PRICEa(probs[m], bit);
    m = (m << 1) | bit;
  }
  return price;
}


static void LenEnc_Init(CLenEnc *p)
{
  unsigned i;
  p->choice = p->choice2 = kProbInitValue;
  for (i = 0; i < (LZMA_NUM_PB_STATES_MAX << kLenNumLowBits); i++)
    p->low[i] = kProbInitValue;
  for (i = 0; i < (LZMA_NUM_PB_STATES_MAX << kLenNumMidBits); i++)
    p->mid[i] = kProbInitValue;
  for (i = 0; i < kLenNumHighSymbols; i++)
    p->high[i] = kProbInitValue;
}

static void LenEnc_Encode(CLenEnc *p, CRangeEnc *rc, uint32_t symbol, uint32_t posState)
{
  if (symbol < kLenNumLowSymbols)
  {
    RangeEnc_EncodeBit(rc, &p->choice, 0);
    RcTree_Encode(rc, p->low + (posState << kLenNumLowBits), kLenNumLowBits, symbol);
  }
  else
  {
    RangeEnc_EncodeBit(rc, &p->choice, 1);
    if (symbol < kLenNumLowSymbols + kLenNumMidSymbols)
    {
      RangeEnc_EncodeBit(rc, &p->choice2, 0);
      RcTree_Encode(rc, p->mid + (posState << kLenNumMidBits), kLenNumMidBits, symbol - kLenNumLowSymbols);
    }
    else
    {
      RangeEnc_EncodeBit(rc, &p->choice2, 1);
      RcTree_Encode(rc, p->high, kLenNumHighBits, symbol - kLenNumLowSymbols - kLenNumMidSymbols);
    }
  }
}

static void LenEnc_SetPrices(CLenEnc *p, uint32_t posState, uint32_t numSymbols, uint32_t *prices, uint32_t *ProbPrices)
{
  uint32_t a0 = GET_PRICE_0a(p->choice);
  uint32_t a1 = GET_PRICE_1a(p->choice);
  uint32_t b0 = a1 + GET_PRICE_0a(p->choice2);
  uint32_t b1 = a1 + GET_PRICE_1a(p->choice2);
  uint32_t i = 0;
  for (i = 0; i < kLenNumLowSymbols; i++)
  {
    if (i >= numSymbols)
      return;
    prices[i] = a0 + RcTree_GetPrice(p->low + (posState << kLenNumLowBits), kLenNumLowBits, i, ProbPrices);
  }
  for (; i < kLenNumLowSymbols + kLenNumMidSymbols; i++)
  {
    if (i >= numSymbols)
      return;
    prices[i] = b0 + RcTree_GetPrice(p->mid + (posState << kLenNumMidBits), kLenNumMidBits, i - kLenNumLowSymbols, ProbPrices);
  }
  for (; i < numSymbols; i++)
    prices[i] = b1 + RcTree_GetPrice(p->high, kLenNumHighBits, i - kLenNumLowSymbols - kLenNumMidSymbols, ProbPrices);
}

static void LenPriceEnc_UpdateTable(CLenPriceEnc *p, uint32_t posState, uint32_t *ProbPrices)
{
  LenEnc_SetPrices(&p->p, posState, p->tableSize, p->prices[posState], ProbPrices);
  p->counters[posState] = p->tableSize;
}

static void LenPriceEnc_UpdateTables(CLenPriceEnc *p, uint32_t numPosStates, uint32_t *ProbPrices)
{
  uint32_t posState;
  for (posState = 0; posState < numPosStates; posState++)
    LenPriceEnc_UpdateTable(p, posState, ProbPrices);
}

static void LenEnc_Encode2(CLenPriceEnc *p, CRangeEnc *rc, uint32_t symbol, uint32_t posState, bool updatePrice, uint32_t *ProbPrices)
{
  LenEnc_Encode(&p->p, rc, symbol, posState);
  if (updatePrice)
    if (--p->counters[posState] == 0)
      LenPriceEnc_UpdateTable(p, posState, ProbPrices);
}


static void MovePos(CLzmaEnc *p, uint32_t num)
{
  #ifdef SHOW_STAT
  ttt += num;
  printf("\n MovePos %d", num);
  #endif
  if (num != 0)
  {
    p->additionalOffset += num;
    p->matchFinder.Skip(&p->matchFinderBase, num);
  }
}

static uint32_t ReadMatchDistances(CLzmaEnc *p, uint32_t *numDistancePairsRes)
{
  uint32_t lenRes = 0, numPairs;
  p->numAvail = Mf_GetNumAvailableBytes(&p->matchFinderBase);
  numPairs = p->matchFinder.GetMatches(&p->matchFinderBase, p->matches);
  #ifdef SHOW_STAT
  printf("\n i = %d numPairs = %d    ", ttt, numPairs / 2);
  ttt++;
  {
    uint32_t i;
    for (i = 0; i < numPairs; i += 2)
      printf("%2d %6d   | ", p->matches[i], p->matches[i + 1]);
  }
  #endif
  if (numPairs > 0)
  {
    lenRes = p->matches[numPairs - 2];
    if (lenRes == p->numFastBytes)
    {
      const uint8_t *pby = Mf_GetPointerToCurrentPos(&p->matchFinderBase) - 1;
      uint32_t distance = p->matches[numPairs - 1] + 1;
      uint32_t numAvail = p->numAvail;
      if (numAvail > LZMA_MATCH_LEN_MAX)
        numAvail = LZMA_MATCH_LEN_MAX;
      {
        const uint8_t *pby2 = pby - distance;
        for (; lenRes < numAvail && pby[lenRes] == pby2[lenRes]; lenRes++) ;
      }
    }
  }
  p->additionalOffset++;
  *numDistancePairsRes = numPairs;
  return lenRes;
}


#define MakeAsChar(p) (p)->backPrev = (uint32_t)(-1); (p)->prev1IsChar = false;
#define MakeAsShortRep(p) (p)->backPrev = 0; (p)->prev1IsChar = false;
#define IsShortRep(p) ((p)->backPrev == 0)

static uint32_t GetRepLen1Price(CLzmaEnc *p, State state, uint32_t posState)
{
  return
    GET_PRICE_0(p->isRepG0[state]) +
    GET_PRICE_0(p->isRep0Long[state][posState]);
}

static uint32_t GetPureRepPrice(CLzmaEnc *p, uint32_t repIndex, State state, uint32_t posState)
{
  uint32_t price;
  if (repIndex == 0)
  {
    price = GET_PRICE_0(p->isRepG0[state]);
    price += GET_PRICE_1(p->isRep0Long[state][posState]);
  }
  else
  {
    price = GET_PRICE_1(p->isRepG0[state]);
    if (repIndex == 1)
      price += GET_PRICE_0(p->isRepG1[state]);
    else
    {
      price += GET_PRICE_1(p->isRepG1[state]);
      price += GET_PRICE(p->isRepG2[state], repIndex - 2);
    }
  }
  return price;
}

static uint32_t GetRepPrice(CLzmaEnc *p, uint32_t repIndex, uint32_t len, State state, uint32_t posState)
{
  return p->repLenEnc.prices[posState][len - LZMA_MATCH_LEN_MIN] +
    GetPureRepPrice(p, repIndex, state, posState);
}

static uint32_t Backward(CLzmaEnc *p, uint32_t *backRes, uint32_t cur)
{
  uint32_t posMem = p->opt[cur].posPrev;
  uint32_t backMem = p->opt[cur].backPrev;
  p->optimumEndIndex = cur;
  do
  {
    if (p->opt[cur].prev1IsChar)
    {
      MakeAsChar(&p->opt[posMem])
      p->opt[posMem].posPrev = posMem - 1;
      if (p->opt[cur].prev2)
      {
        p->opt[posMem - 1].prev1IsChar = false;
        p->opt[posMem - 1].posPrev = p->opt[cur].posPrev2;
        p->opt[posMem - 1].backPrev = p->opt[cur].backPrev2;
      }
    }
    {
      uint32_t posPrev = posMem;
      uint32_t backCur = backMem;

      backMem = p->opt[posPrev].backPrev;
      posMem = p->opt[posPrev].posPrev;

      p->opt[posPrev].backPrev = backCur;
      p->opt[posPrev].posPrev = cur;
      cur = posPrev;
    }
  }
  while (cur != 0);
  *backRes = p->opt[0].backPrev;
  p->optimumCurrentIndex  = p->opt[0].posPrev;
  return p->optimumCurrentIndex;
}

#define LIT_PROBS(pos, prevByte) (p->litProbs + ((((pos) & p->lpMask) << p->lc) + ((prevByte) >> (8 - p->lc))) * 0x300)

static uint32_t GetOptimum(CLzmaEnc *p, uint32_t position, uint32_t *backRes)
{
  uint32_t numAvail, mainLen, numPairs, repMaxIndex, i, posState, lenEnd, len, cur;
  uint32_t matchPrice, repMatchPrice, normalMatchPrice;
  uint32_t reps[LZMA_NUM_REPS], repLens[LZMA_NUM_REPS];
  uint32_t *matches;
  const uint8_t *data;
  uint8_t curByte, matchByte;
  if (p->optimumEndIndex != p->optimumCurrentIndex)
  {
    const COptimal *opt = &p->opt[p->optimumCurrentIndex];
    uint32_t lenRes = opt->posPrev - p->optimumCurrentIndex;
    *backRes = opt->backPrev;
    p->optimumCurrentIndex = opt->posPrev;
    return lenRes;
  }
  p->optimumCurrentIndex = p->optimumEndIndex = 0;

  if (p->additionalOffset == 0)
    mainLen = ReadMatchDistances(p, &numPairs);
  else
  {
    mainLen = p->longestMatchLength;
    numPairs = p->numPairs;
  }

  numAvail = p->numAvail;
  if (numAvail < 2)
  {
    *backRes = (uint32_t)(-1);
    return 1;
  }
  if (numAvail > LZMA_MATCH_LEN_MAX)
    numAvail = LZMA_MATCH_LEN_MAX;

  data = Mf_GetPointerToCurrentPos(&p->matchFinderBase) - 1;
  repMaxIndex = 0;
  for (i = 0; i < LZMA_NUM_REPS; i++)
  {
    uint32_t lenTest;
    const uint8_t *data2;
    reps[i] = p->reps[i];
    data2 = data - (reps[i] + 1);
    if (data[0] != data2[0] || data[1] != data2[1])
    {
      repLens[i] = 0;
      continue;
    }
    for (lenTest = 2; lenTest < numAvail && data[lenTest] == data2[lenTest]; lenTest++) ;
    repLens[i] = lenTest;
    if (lenTest > repLens[repMaxIndex])
      repMaxIndex = i;
  }
  if (repLens[repMaxIndex] >= p->numFastBytes)
  {
    uint32_t lenRes;
    *backRes = repMaxIndex;
    lenRes = repLens[repMaxIndex];
    MovePos(p, lenRes - 1);
    return lenRes;
  }

  matches = p->matches;
  if (mainLen >= p->numFastBytes)
  {
    *backRes = matches[numPairs - 1] + LZMA_NUM_REPS;
    MovePos(p, mainLen - 1);
    return mainLen;
  }
  curByte = *data;
  matchByte = *(data - (reps[0] + 1));

  if (mainLen < 2 && curByte != matchByte && repLens[repMaxIndex] < 2)
  {
    *backRes = (uint32_t)-1;
    return 1;
  }

  p->opt[0].state = p->state;

  posState = (position & p->pbMask);

  {
    const int *probs = LIT_PROBS(position, *(data - 1));
    p->opt[1].price = GET_PRICE_0(p->isMatch[p->state][posState]) +
        (!IsCharState(p->state) ?
          LitEnc_GetPriceMatched(probs, curByte, matchByte, p->ProbPrices) :
          LitEnc_GetPrice(probs, curByte, p->ProbPrices));
  }

  MakeAsChar(&p->opt[1]);

  matchPrice = GET_PRICE_1(p->isMatch[p->state][posState]);
  repMatchPrice = matchPrice + GET_PRICE_1(p->isRep[p->state]);

  if (matchByte == curByte)
  {
    uint32_t shortRepPrice = repMatchPrice + GetRepLen1Price(p, p->state, posState);
    if (shortRepPrice < p->opt[1].price)
    {
      p->opt[1].price = shortRepPrice;
      MakeAsShortRep(&p->opt[1]);
    }
  }
  lenEnd = ((mainLen >= repLens[repMaxIndex]) ? mainLen : repLens[repMaxIndex]);

  if (lenEnd < 2)
  {
    *backRes = p->opt[1].backPrev;
    return 1;
  }

  p->opt[1].posPrev = 0;
  for (i = 0; i < LZMA_NUM_REPS; i++)
    p->opt[0].backs[i] = reps[i];

  len = lenEnd;
  do
    p->opt[len--].price = kInfinityPrice;
  while (len >= 2);

  for (i = 0; i < LZMA_NUM_REPS; i++)
  {
    uint32_t repLen = repLens[i];
    uint32_t price;
    if (repLen < 2)
      continue;
    price = repMatchPrice + GetPureRepPrice(p, i, p->state, posState);
    do
    {
      uint32_t curAndLenPrice = price + p->repLenEnc.prices[posState][repLen - 2];
      COptimal *opt = &p->opt[repLen];
      if (curAndLenPrice < opt->price)
      {
        opt->price = curAndLenPrice;
        opt->posPrev = 0;
        opt->backPrev = i;
        opt->prev1IsChar = false;
      }
    }
    while (--repLen >= 2);
  }

  normalMatchPrice = matchPrice + GET_PRICE_0(p->isRep[p->state]);

  len = ((repLens[0] >= 2) ? repLens[0] + 1 : 2);
  if (len <= mainLen)
  {
    uint32_t offs = 0;
    while (len > matches[offs])
      offs += 2;
    for (; ; len++)
    {
      COptimal *opt;
      uint32_t distance = matches[offs + 1];

      uint32_t curAndLenPrice = normalMatchPrice + p->lenEnc.prices[posState][len - LZMA_MATCH_LEN_MIN];
      uint32_t lenToPosState = GetLenToPosState(len);
      if (distance < kNumFullDistances)
        curAndLenPrice += p->distancesPrices[lenToPosState][distance];
      else
      {
        uint32_t slot;
        GetPosSlot2(distance, slot);
        curAndLenPrice += p->alignPrices[distance & kAlignMask] + p->posSlotPrices[lenToPosState][slot];
      }
      opt = &p->opt[len];
      if (curAndLenPrice < opt->price)
      {
        opt->price = curAndLenPrice;
        opt->posPrev = 0;
        opt->backPrev = distance + LZMA_NUM_REPS;
        opt->prev1IsChar = false;
      }
      if (len == matches[offs])
      {
        offs += 2;
        if (offs == numPairs)
          break;
      }
    }
  }

  cur = 0;

    #ifdef SHOW_STAT2
    if (position >= 0)
    {
      unsigned i;
      printf("\n pos = %4X", position);
      for (i = cur; i <= lenEnd; i++)
      printf("\nprice[%4X] = %d", position - cur + i, p->opt[i].price);
    }
    #endif

  for (;;)
  {
    uint32_t numAvailFull, newLen, numPairs, posPrev, state, posState, startLen;
    uint32_t curPrice, curAnd1Price, matchPrice, repMatchPrice;
    bool nextIsChar;
    uint8_t curByte, matchByte;
    const uint8_t *data;
    COptimal *curOpt;
    COptimal *nextOpt;

    cur++;
    if (cur == lenEnd)
      return Backward(p, backRes, cur);

    newLen = ReadMatchDistances(p, &numPairs);
    if (newLen >= p->numFastBytes)
    {
      p->numPairs = numPairs;
      p->longestMatchLength = newLen;
      return Backward(p, backRes, cur);
    }
    position++;
    curOpt = &p->opt[cur];
    posPrev = curOpt->posPrev;
    if (curOpt->prev1IsChar)
    {
      posPrev--;
      if (curOpt->prev2)
      {
        state = p->opt[curOpt->posPrev2].state;
        if (curOpt->backPrev2 < LZMA_NUM_REPS)
          state = kRepNextStates[state];
        else
          state = kMatchNextStates[state];
      }
      else
        state = p->opt[posPrev].state;
      state = kLiteralNextStates[state];
    }
    else
      state = p->opt[posPrev].state;
    if (posPrev == cur - 1)
    {
      if (IsShortRep(curOpt))
        state = kShortRepNextStates[state];
      else
        state = kLiteralNextStates[state];
    }
    else
    {
      uint32_t pos;
      const COptimal *prevOpt;
      if (curOpt->prev1IsChar && curOpt->prev2)
      {
        posPrev = curOpt->posPrev2;
        pos = curOpt->backPrev2;
        state = kRepNextStates[state];
      }
      else
      {
        pos = curOpt->backPrev;
        if (pos < LZMA_NUM_REPS)
          state = kRepNextStates[state];
        else
          state = kMatchNextStates[state];
      }
      prevOpt = &p->opt[posPrev];
      if (pos < LZMA_NUM_REPS)
      {
        uint32_t i;
        reps[0] = prevOpt->backs[pos];
        for (i = 1; i <= pos; i++)
          reps[i] = prevOpt->backs[i - 1];
        for (; i < LZMA_NUM_REPS; i++)
          reps[i] = prevOpt->backs[i];
      }
      else
      {
        uint32_t i;
        reps[0] = (pos - LZMA_NUM_REPS);
        for (i = 1; i < LZMA_NUM_REPS; i++)
          reps[i] = prevOpt->backs[i - 1];
      }
    }
    curOpt->state = state;

    curOpt->backs[0] = reps[0];
    curOpt->backs[1] = reps[1];
    curOpt->backs[2] = reps[2];
    curOpt->backs[3] = reps[3];

    curPrice = curOpt->price;
    nextIsChar = false;
    data = Mf_GetPointerToCurrentPos(&p->matchFinderBase) - 1;
    curByte = *data;
    matchByte = *(data - (reps[0] + 1));

    posState = (position & p->pbMask);

    curAnd1Price = curPrice + GET_PRICE_0(p->isMatch[state][posState]);
    {
      const int *probs = LIT_PROBS(position, *(data - 1));
      curAnd1Price +=
        (!IsCharState(state) ?
          LitEnc_GetPriceMatched(probs, curByte, matchByte, p->ProbPrices) :
          LitEnc_GetPrice(probs, curByte, p->ProbPrices));
    }

    nextOpt = &p->opt[cur + 1];

    if (curAnd1Price < nextOpt->price)
    {
      nextOpt->price = curAnd1Price;
      nextOpt->posPrev = cur;
      MakeAsChar(nextOpt);
      nextIsChar = true;
    }

    matchPrice = curPrice + GET_PRICE_1(p->isMatch[state][posState]);
    repMatchPrice = matchPrice + GET_PRICE_1(p->isRep[state]);

    if (matchByte == curByte && !(nextOpt->posPrev < cur && nextOpt->backPrev == 0))
    {
      uint32_t shortRepPrice = repMatchPrice + GetRepLen1Price(p, state, posState);
      if (shortRepPrice <= nextOpt->price)
      {
        nextOpt->price = shortRepPrice;
        nextOpt->posPrev = cur;
        MakeAsShortRep(nextOpt);
        nextIsChar = true;
      }
    }
    numAvailFull = p->numAvail;
    {
      uint32_t temp = kNumOpts - 1 - cur;
      if (temp < numAvailFull)
        numAvailFull = temp;
    }

    if (numAvailFull < 2)
      continue;
    numAvail = (numAvailFull <= p->numFastBytes ? numAvailFull : p->numFastBytes);

    if (!nextIsChar && matchByte != curByte) /* speed optimization */
    {
      /* try Literal + rep0 */
      uint32_t temp;
      uint32_t lenTest2;
      const uint8_t *data2 = data - (reps[0] + 1);
      uint32_t limit = p->numFastBytes + 1;
      if (limit > numAvailFull)
        limit = numAvailFull;

      for (temp = 1; temp < limit && data[temp] == data2[temp]; temp++) ;
      lenTest2 = temp - 1;
      if (lenTest2 >= 2)
      {
        State state2 = kLiteralNextStates[state];
        uint32_t posStateNext = (position + 1) & p->pbMask;
        uint32_t nextRepMatchPrice = curAnd1Price +
            GET_PRICE_1(p->isMatch[state2][posStateNext]) +
            GET_PRICE_1(p->isRep[state2]);
        /* for (; lenTest2 >= 2; lenTest2--) */
        {
          uint32_t curAndLenPrice;
          COptimal *opt;
          uint32_t offset = cur + 1 + lenTest2;
          while (lenEnd < offset)
            p->opt[++lenEnd].price = kInfinityPrice;
          curAndLenPrice = nextRepMatchPrice + GetRepPrice(p, 0, lenTest2, state2, posStateNext);
          opt = &p->opt[offset];
          if (curAndLenPrice < opt->price)
          {
            opt->price = curAndLenPrice;
            opt->posPrev = cur + 1;
            opt->backPrev = 0;
            opt->prev1IsChar = true;
            opt->prev2 = false;
          }
        }
      }
    }

    startLen = 2; /* speed optimization */
    {
    uint32_t repIndex;
    for (repIndex = 0; repIndex < LZMA_NUM_REPS; repIndex++)
    {
      uint32_t lenTest;
      uint32_t lenTestTemp;
      uint32_t price;
      const uint8_t *data2 = data - (reps[repIndex] + 1);
      if (data[0] != data2[0] || data[1] != data2[1])
        continue;
      for (lenTest = 2; lenTest < numAvail && data[lenTest] == data2[lenTest]; lenTest++) ;
      while (lenEnd < cur + lenTest)
        p->opt[++lenEnd].price = kInfinityPrice;
      lenTestTemp = lenTest;
      price = repMatchPrice + GetPureRepPrice(p, repIndex, state, posState);
      do
      {
        uint32_t curAndLenPrice = price + p->repLenEnc.prices[posState][lenTest - 2];
        COptimal *opt = &p->opt[cur + lenTest];
        if (curAndLenPrice < opt->price)
        {
          opt->price = curAndLenPrice;
          opt->posPrev = cur;
          opt->backPrev = repIndex;
          opt->prev1IsChar = false;
        }
      }
      while (--lenTest >= 2);
      lenTest = lenTestTemp;

      if (repIndex == 0)
        startLen = lenTest + 1;

      /* if (_maxMode) */
        {
          uint32_t lenTest2 = lenTest + 1;
          uint32_t limit = lenTest2 + p->numFastBytes;
          uint32_t nextRepMatchPrice;
          if (limit > numAvailFull)
            limit = numAvailFull;
          for (; lenTest2 < limit && data[lenTest2] == data2[lenTest2]; lenTest2++) ;
          lenTest2 -= lenTest + 1;
          if (lenTest2 >= 2)
          {
            State state2 = kRepNextStates[state];
            uint32_t posStateNext = (position + lenTest) & p->pbMask;
            uint32_t curAndLenCharPrice =
                price + p->repLenEnc.prices[posState][lenTest - 2] +
                GET_PRICE_0(p->isMatch[state2][posStateNext]) +
                LitEnc_GetPriceMatched(LIT_PROBS(position + lenTest, data[lenTest - 1]),
                    data[lenTest], data2[lenTest], p->ProbPrices);
            state2 = kLiteralNextStates[state2];
            posStateNext = (position + lenTest + 1) & p->pbMask;
            nextRepMatchPrice = curAndLenCharPrice +
                GET_PRICE_1(p->isMatch[state2][posStateNext]) +
                GET_PRICE_1(p->isRep[state2]);

            /* for (; lenTest2 >= 2; lenTest2--) */
            {
              uint32_t curAndLenPrice;
              COptimal *opt;
              uint32_t offset = cur + lenTest + 1 + lenTest2;
              while (lenEnd < offset)
                p->opt[++lenEnd].price = kInfinityPrice;
              curAndLenPrice = nextRepMatchPrice + GetRepPrice(p, 0, lenTest2, state2, posStateNext);
              opt = &p->opt[offset];
              if (curAndLenPrice < opt->price)
              {
                opt->price = curAndLenPrice;
                opt->posPrev = cur + lenTest + 1;
                opt->backPrev = 0;
                opt->prev1IsChar = true;
                opt->prev2 = true;
                opt->posPrev2 = cur;
                opt->backPrev2 = repIndex;
              }
            }
          }
        }
    }
    }
    /* for (uint32_t lenTest = 2; lenTest <= newLen; lenTest++) */
    if (newLen > numAvail)
    {
      newLen = numAvail;
      for (numPairs = 0; newLen > matches[numPairs]; numPairs += 2) ;
      matches[numPairs] = newLen;
      numPairs += 2;
    }
    if (newLen >= startLen)
    {
      uint32_t normalMatchPrice = matchPrice + GET_PRICE_0(p->isRep[state]);
      uint32_t offs, curBack, posSlot;
      uint32_t lenTest;
      while (lenEnd < cur + newLen)
        p->opt[++lenEnd].price = kInfinityPrice;

      offs = 0;
      while (startLen > matches[offs])
        offs += 2;
      curBack = matches[offs + 1];
      GetPosSlot2(curBack, posSlot);
      for (lenTest = /*2*/ startLen; ; lenTest++)
      {
        uint32_t curAndLenPrice = normalMatchPrice + p->lenEnc.prices[posState][lenTest - LZMA_MATCH_LEN_MIN];
        uint32_t lenToPosState = GetLenToPosState(lenTest);
        COptimal *opt;
        if (curBack < kNumFullDistances)
          curAndLenPrice += p->distancesPrices[lenToPosState][curBack];
        else
          curAndLenPrice += p->posSlotPrices[lenToPosState][posSlot] + p->alignPrices[curBack & kAlignMask];

        opt = &p->opt[cur + lenTest];
        if (curAndLenPrice < opt->price)
        {
          opt->price = curAndLenPrice;
          opt->posPrev = cur;
          opt->backPrev = curBack + LZMA_NUM_REPS;
          opt->prev1IsChar = false;
        }

        if (/*_maxMode && */lenTest == matches[offs])
        {
          /* Try Match + Literal + Rep0 */
          const uint8_t *data2 = data - (curBack + 1);
          uint32_t lenTest2 = lenTest + 1;
          uint32_t limit = lenTest2 + p->numFastBytes;
          uint32_t nextRepMatchPrice;
          if (limit > numAvailFull)
            limit = numAvailFull;
          for (; lenTest2 < limit && data[lenTest2] == data2[lenTest2]; lenTest2++) ;
          lenTest2 -= lenTest + 1;
          if (lenTest2 >= 2)
          {
            State state2 = kMatchNextStates[state];
            uint32_t posStateNext = (position + lenTest) & p->pbMask;
            uint32_t curAndLenCharPrice = curAndLenPrice +
                GET_PRICE_0(p->isMatch[state2][posStateNext]) +
                LitEnc_GetPriceMatched(LIT_PROBS(position + lenTest, data[lenTest - 1]),
                    data[lenTest], data2[lenTest], p->ProbPrices);
            state2 = kLiteralNextStates[state2];
            posStateNext = (posStateNext + 1) & p->pbMask;
            nextRepMatchPrice = curAndLenCharPrice +
                GET_PRICE_1(p->isMatch[state2][posStateNext]) +
                GET_PRICE_1(p->isRep[state2]);

            /* for (; lenTest2 >= 2; lenTest2--) */
            {
              uint32_t offset = cur + lenTest + 1 + lenTest2;
              uint32_t curAndLenPrice;
              COptimal *opt;
              while (lenEnd < offset)
                p->opt[++lenEnd].price = kInfinityPrice;
              curAndLenPrice = nextRepMatchPrice + GetRepPrice(p, 0, lenTest2, state2, posStateNext);
              opt = &p->opt[offset];
              if (curAndLenPrice < opt->price)
              {
                opt->price = curAndLenPrice;
                opt->posPrev = cur + lenTest + 1;
                opt->backPrev = 0;
                opt->prev1IsChar = true;
                opt->prev2 = true;
                opt->posPrev2 = cur;
                opt->backPrev2 = curBack + LZMA_NUM_REPS;
              }
            }
          }
          offs += 2;
          if (offs == numPairs)
            break;
          curBack = matches[offs + 1];
          if (curBack >= kNumFullDistances)
            GetPosSlot2(curBack, posSlot);
        }
      }
    }
  }
}

#define ChangePair(smallDist, bigDist) (((bigDist) >> 7) > (smallDist))

static uint32_t GetOptimumFast(CLzmaEnc *p, uint32_t *backRes)
{
  uint32_t numAvail, mainLen, mainDist, numPairs, repIndex, repLen, i;
  const uint8_t *data;
  const uint32_t *matches;

  if (p->additionalOffset == 0)
    mainLen = ReadMatchDistances(p, &numPairs);
  else
  {
    mainLen = p->longestMatchLength;
    numPairs = p->numPairs;
  }

  numAvail = p->numAvail;
  *backRes = (uint32_t)-1;
  if (numAvail < 2)
    return 1;
  if (numAvail > LZMA_MATCH_LEN_MAX)
    numAvail = LZMA_MATCH_LEN_MAX;
  data = Mf_GetPointerToCurrentPos(&p->matchFinderBase) - 1;

  repLen = repIndex = 0;
  for (i = 0; i < LZMA_NUM_REPS; i++)
  {
    uint32_t len;
    const uint8_t *data2 = data - (p->reps[i] + 1);
    if (data[0] != data2[0] || data[1] != data2[1])
      continue;
    for (len = 2; len < numAvail && data[len] == data2[len]; len++) ;
    if (len >= p->numFastBytes)
    {
      *backRes = i;
      MovePos(p, len - 1);
      return len;
    }
    if (len > repLen)
    {
      repIndex = i;
      repLen = len;
    }
  }

  matches = p->matches;
  if (mainLen >= p->numFastBytes)
  {
    *backRes = matches[numPairs - 1] + LZMA_NUM_REPS;
    MovePos(p, mainLen - 1);
    return mainLen;
  }

  mainDist = 0; /* for GCC */
  if (mainLen >= 2)
  {
    mainDist = matches[numPairs - 1];
    while (numPairs > 2 && mainLen == matches[numPairs - 4] + 1)
    {
      if (!ChangePair(matches[numPairs - 3], mainDist))
        break;
      numPairs -= 2;
      mainLen = matches[numPairs - 2];
      mainDist = matches[numPairs - 1];
    }
    if (mainLen == 2 && mainDist >= 0x80)
      mainLen = 1;
  }

  if (repLen >= 2 && (
        (repLen + 1 >= mainLen) ||
        (repLen + 2 >= mainLen && mainDist >= (1 << 9)) ||
        (repLen + 3 >= mainLen && mainDist >= (1 << 15))))
  {
    *backRes = repIndex;
    MovePos(p, repLen - 1);
    return repLen;
  }

  if (mainLen < 2 || numAvail <= 2)
    return 1;

  p->longestMatchLength = ReadMatchDistances(p, &p->numPairs);
  if (p->longestMatchLength >= 2)
  {
    uint32_t newDistance = matches[p->numPairs - 1];
    if ((p->longestMatchLength >= mainLen && newDistance < mainDist) ||
        (p->longestMatchLength == mainLen + 1 && !ChangePair(mainDist, newDistance)) ||
        (p->longestMatchLength > mainLen + 1) ||
        (p->longestMatchLength + 1 >= mainLen && mainLen >= 3 && ChangePair(newDistance, mainDist)))
      return 1;
  }

  data = Mf_GetPointerToCurrentPos(&p->matchFinderBase) - 1;
  for (i = 0; i < LZMA_NUM_REPS; i++)
  {
    uint32_t len, limit;
    const uint8_t *data2 = data - (p->reps[i] + 1);
    if (data[0] != data2[0] || data[1] != data2[1])
      continue;
    limit = mainLen - 1;
    for (len = 2; len < limit && data[len] == data2[len]; len++) ;
    if (len >= limit)
      return 1;
  }
  *backRes = mainDist + LZMA_NUM_REPS;
  MovePos(p, mainLen - 2);
  return mainLen;
}

static void LZe_full_flush(CLzmaEnc *p, uint32_t posState)
  {
  int emdros_errno = 0;
  const uint32_t len = LZMA_MATCH_LEN_MIN;
  File_trailer trailer;
  RangeEnc_EncodeBit(&p->rc, &p->isMatch[p->state][posState], 1);
  RangeEnc_EncodeBit(&p->rc, &p->isRep[p->state], 0);
  p->state = kMatchNextStates[p->state];
  LenEnc_Encode2(&p->lenEnc, &p->rc, len - LZMA_MATCH_LEN_MIN, posState, !p->fastMode, p->ProbPrices);
  RcTree_Encode(&p->rc, p->posSlotEncoder[GetLenToPosState(len)], kNumPosSlotBits, (1 << kNumPosSlotBits) - 1);
  RangeEnc_EncodeDirectBits(&p->rc, (((uint32_t)1 << 30) - 1) >> kNumAlignBits, 30 - kNumAlignBits);
  RcTree_ReverseEncode(&p->rc, p->posAlignEncoder, kNumAlignBits, kAlignMask);
  RangeEnc_FlushData(&p->rc);
  RangeEnc_FlushStream(&p->rc);
  Ft_set_data_crc( trailer, p->matchFinderBase.crc ^ 0xFFFFFFFFU );
  Ft_set_data_size( trailer, p->nowPos64 );
  Ft_set_member_size( trailer, p->rc.processed + EMDROS_LZMA_Fh_size + Ft_size );
  if( p->rc.lzma_writeblock( p->rc.pFOUT, trailer, Ft_size, &emdros_errno ) != Ft_size )
    p->rc.res = EMDROS_SZ_ERROR_WRITE;
  }

static int CheckErrors(CLzmaEnc *p)
{
  if (p->result != EMDROS_SZ_OK)
    return p->result;
  if (p->rc.res != EMDROS_SZ_OK)
    p->result = EMDROS_SZ_ERROR_WRITE;
  if (p->matchFinderBase.result != EMDROS_SZ_OK)
    p->result = EMDROS_SZ_ERROR_READ;
  if (p->result != EMDROS_SZ_OK)
    p->finished = true;
  return p->result;
}

static int Flush(CLzmaEnc *p, uint32_t nowPos)
{
  /* ReleaseMFStream(); */
  p->finished = true;
  LZe_full_flush(p, nowPos & p->pbMask);
  return CheckErrors(p);
}

static void FillAlignPrices(CLzmaEnc *p)
{
  uint32_t i;
  for (i = 0; i < kAlignTableSize; i++)
    p->alignPrices[i] = RcTree_ReverseGetPrice(p->posAlignEncoder, kNumAlignBits, i, p->ProbPrices);
  p->alignPriceCount = 0;
}

static void FillDistancesPrices(CLzmaEnc *p)
{
  uint32_t tempPrices[kNumFullDistances];
  uint32_t i, lenToPosState;
  for (i = kStartPosModelIndex; i < kNumFullDistances; i++)
  {
    uint32_t posSlot = GetPosSlot1(i);
    uint32_t footerBits = ((posSlot >> 1) - 1);
    uint32_t base = ((2 | (posSlot & 1)) << footerBits);
    tempPrices[i] = RcTree_ReverseGetPrice(p->posEncoders + base - posSlot - 1, footerBits, i - base, p->ProbPrices);
  }

  for (lenToPosState = 0; lenToPosState < kNumLenToPosStates; lenToPosState++)
  {
    uint32_t posSlot;
    const int *encoder = p->posSlotEncoder[lenToPosState];
    uint32_t *posSlotPrices = p->posSlotPrices[lenToPosState];
    for (posSlot = 0; posSlot < p->distTableSize; posSlot++)
      posSlotPrices[posSlot] = RcTree_GetPrice(encoder, kNumPosSlotBits, posSlot, p->ProbPrices);
    for (posSlot = kEndPosModelIndex; posSlot < p->distTableSize; posSlot++)
      posSlotPrices[posSlot] += ((((posSlot >> 1) - 1) - kNumAlignBits) << kNumBitPriceShiftBits);

    {
      uint32_t *distancesPrices = p->distancesPrices[lenToPosState];
      uint32_t i;
      for (i = 0; i < kStartPosModelIndex; i++)
        distancesPrices[i] = posSlotPrices[i];
      for (; i < kNumFullDistances; i++)
        distancesPrices[i] = posSlotPrices[GetPosSlot1(i)] + tempPrices[i];
    }
  }
  p->matchPriceCount = 0;
}


static int LzmaEnc_CodeOneBlock(CLzmaEnc *p)
{
  uint32_t nowPos32, startPos32;

  if (p->finished)
    return p->result;
  if( CheckErrors(p) != 0 ) return p->result;

  nowPos32 = (uint32_t)p->nowPos64;
  startPos32 = nowPos32;

  if (p->nowPos64 == 0)
  {
    uint32_t numPairs;
    uint8_t curByte;
    if (Mf_GetNumAvailableBytes(&p->matchFinderBase) == 0)
      return Flush(p, nowPos32);
    ReadMatchDistances(p, &numPairs);
    RangeEnc_EncodeBit(&p->rc, &p->isMatch[p->state][0], 0);
    p->state = kLiteralNextStates[p->state];
    curByte = Mf_GetIndexByte(&p->matchFinderBase, 0 - p->additionalOffset);
    LitEnc_Encode(&p->rc, p->litProbs, curByte);
    p->additionalOffset--;
    nowPos32++;
  }

  if (Mf_GetNumAvailableBytes(&p->matchFinderBase) != 0)
  for (;;)
  {
    uint32_t pos, len, posState;

    if (p->fastMode)
      len = GetOptimumFast(p, &pos);
    else
      len = GetOptimum(p, nowPos32, &pos);

    #ifdef SHOW_STAT2
    printf("\n pos = %4X,   len = %d   pos = %d", nowPos32, len, pos);
    #endif

    posState = nowPos32 & p->pbMask;
    if (len == 1 && pos == (uint32_t)-1)
    {
      uint8_t curByte;
      int *probs;
      const uint8_t *data;

      RangeEnc_EncodeBit(&p->rc, &p->isMatch[p->state][posState], 0);
      data = Mf_GetPointerToCurrentPos(&p->matchFinderBase) - p->additionalOffset;
      curByte = *data;
      probs = LIT_PROBS(nowPos32, *(data - 1));
      if (IsCharState(p->state))
        LitEnc_Encode(&p->rc, probs, curByte);
      else
        LitEnc_EncodeMatched(&p->rc, probs, curByte, *(data - p->reps[0] - 1));
      p->state = kLiteralNextStates[p->state];
    }
    else
    {
      RangeEnc_EncodeBit(&p->rc, &p->isMatch[p->state][posState], 1);
      if (pos < LZMA_NUM_REPS)
      {
        RangeEnc_EncodeBit(&p->rc, &p->isRep[p->state], 1);
        if (pos == 0)
        {
          RangeEnc_EncodeBit(&p->rc, &p->isRepG0[p->state], 0);
          RangeEnc_EncodeBit(&p->rc, &p->isRep0Long[p->state][posState], ((len == 1) ? 0 : 1));
        }
        else
        {
          uint32_t distance = p->reps[pos];
          RangeEnc_EncodeBit(&p->rc, &p->isRepG0[p->state], 1);
          if (pos == 1)
            RangeEnc_EncodeBit(&p->rc, &p->isRepG1[p->state], 0);
          else
          {
            RangeEnc_EncodeBit(&p->rc, &p->isRepG1[p->state], 1);
            RangeEnc_EncodeBit(&p->rc, &p->isRepG2[p->state], pos - 2);
            if (pos == 3)
              p->reps[3] = p->reps[2];
            p->reps[2] = p->reps[1];
          }
          p->reps[1] = p->reps[0];
          p->reps[0] = distance;
        }
        if (len == 1)
          p->state = kShortRepNextStates[p->state];
        else
        {
          LenEnc_Encode2(&p->repLenEnc, &p->rc, len - LZMA_MATCH_LEN_MIN, posState, !p->fastMode, p->ProbPrices);
          p->state = kRepNextStates[p->state];
        }
      }
      else
      {
        uint32_t posSlot;
        RangeEnc_EncodeBit(&p->rc, &p->isRep[p->state], 0);
        p->state = kMatchNextStates[p->state];
        LenEnc_Encode2(&p->lenEnc, &p->rc, len - LZMA_MATCH_LEN_MIN, posState, !p->fastMode, p->ProbPrices);
        pos -= LZMA_NUM_REPS;
        GetPosSlot(pos, posSlot);
        RcTree_Encode(&p->rc, p->posSlotEncoder[GetLenToPosState(len)], kNumPosSlotBits, posSlot);

        if (posSlot >= kStartPosModelIndex)
        {
          uint32_t footerBits = ((posSlot >> 1) - 1);
          uint32_t base = ((2 | (posSlot & 1)) << footerBits);
          uint32_t posReduced = pos - base;

          if (posSlot < kEndPosModelIndex)
            RcTree_ReverseEncode(&p->rc, p->posEncoders + base - posSlot - 1, footerBits, posReduced);
          else
          {
            RangeEnc_EncodeDirectBits(&p->rc, posReduced >> kNumAlignBits, footerBits - kNumAlignBits);
            RcTree_ReverseEncode(&p->rc, p->posAlignEncoder, kNumAlignBits, posReduced & kAlignMask);
            p->alignPriceCount++;
          }
        }
        p->reps[3] = p->reps[2];
        p->reps[2] = p->reps[1];
        p->reps[1] = p->reps[0];
        p->reps[0] = pos;
        p->matchPriceCount++;
      }
    }
    p->additionalOffset -= len;
    nowPos32 += len;
    if (p->additionalOffset == 0)
    {
      uint32_t processed;
      if (!p->fastMode)
      {
        if (p->matchPriceCount >= (1 << 7))
          FillDistancesPrices(p);
        if (p->alignPriceCount >= kAlignTableSize)
          FillAlignPrices(p);
      }
      if (Mf_GetNumAvailableBytes(&p->matchFinderBase) == 0)
        break;
      processed = nowPos32 - startPos32;
      if (processed >= (1 << 15))
      {
        p->nowPos64 += nowPos32 - startPos32;
        return CheckErrors(p);
      }
    }
  }
  p->nowPos64 += nowPos32 - startPos32;
  return Flush(p, nowPos32);
}


CLzmaEncHandle LzmaEnc_Init( const int dict_size, const int match_len_limit,
                             void *pFIN, void *pFOUT,
			     emdros_lzma_readblock_function_ptr emdros_lzma_readblock,
			     emdros_lzma_writeblock_function_ptr emdros_lzma_writeblock)
  {
  int i;
  const uint32_t beforeSize = kNumOpts;
  CLzmaEnc * const p = (CLzmaEnc *)malloc(sizeof(CLzmaEnc));
  if( !p ) return 0;

  p->nowPos64 = 0;
  p->dictSize = dict_size;
  p->numFastBytes = match_len_limit;
  p->lc = literal_context_bits;
  p->lp = 0;
  p->pb = pos_state_bits;
  p->optimumEndIndex = 0;
  p->optimumCurrentIndex = 0;
  p->additionalOffset = 0;
  p->state = 0;
  p->result = EMDROS_SZ_OK;
  p->fastMode = false;
  p->finished = false;

  if (!Mf_Init(&p->matchFinderBase, pFIN, 16 + ( match_len_limit / 2 ), p->dictSize, beforeSize, p->numFastBytes, LZMA_MATCH_LEN_MAX, emdros_lzma_readblock))
    { free( p ); return 0; }
  Mf_CreateVTable(&p->matchFinderBase, &p->matchFinder);

  LzmaEnc_FastPosInit(p->g_FastPos);
  LzmaEnc_InitPriceTables(p->ProbPrices);
  for (i = 0; i < kDicLogSizeMaxCompress; i++)
    if (p->dictSize <= ((uint32_t)1 << i))
      break;
  p->distTableSize = i * 2;
  if( !RangeEnc_Init( &p->rc, pFOUT, emdros_lzma_writeblock ) ) { free( p ); return 0; }
  p->litProbs = (int *)malloc((0x300 << (p->lc + p->lp)) * sizeof(int));
  if( !p->litProbs ) { free( p ); return 0; }

  for (i = 0 ; i < LZMA_NUM_REPS; i++)
    p->reps[i] = 0;
  for (i = 0; i < kNumStates; i++)
  {
    int j;
    for (j = 0; j < LZMA_NUM_PB_STATES_MAX; j++)
    {
      p->isMatch[i][j] = kProbInitValue;
      p->isRep0Long[i][j] = kProbInitValue;
    }
    p->isRep[i] = kProbInitValue;
    p->isRepG0[i] = kProbInitValue;
    p->isRepG1[i] = kProbInitValue;
    p->isRepG2[i] = kProbInitValue;
  }
  {
    const int num = 0x300 << (p->lp + p->lc);
    for (i = 0; i < num; i++)
      p->litProbs[i] = kProbInitValue;
  }
  for (i = 0; i < kNumLenToPosStates; i++)
  {
    int *probs = p->posSlotEncoder[i];
    uint32_t j;
    for (j = 0; j < (1 << kNumPosSlotBits); j++)
      probs[j] = kProbInitValue;
  }
  for (i = 0; i < kNumFullDistances - kEndPosModelIndex; i++)
    p->posEncoders[i] = kProbInitValue;
  LenEnc_Init(&p->lenEnc.p);
  LenEnc_Init(&p->repLenEnc.p);
  for (i = 0; i < (1 << kNumAlignBits); i++)
    p->posAlignEncoder[i] = kProbInitValue;
  p->pbMask = (1 << p->pb) - 1;
  p->lpMask = (1 << p->lp) - 1;

  if (!p->fastMode) { FillDistancesPrices(p); FillAlignPrices(p); }
  p->lenEnc.tableSize =
  p->repLenEnc.tableSize =
      p->numFastBytes + 1 - LZMA_MATCH_LEN_MIN;
  LenPriceEnc_UpdateTables(&p->lenEnc, 1 << p->pb, p->ProbPrices);
  LenPriceEnc_UpdateTables(&p->repLenEnc, 1 << p->pb, p->ProbPrices);
  return p;
  }


void LzmaEnc_Free(CLzmaEncHandle pp)
{
  CLzmaEnc *p = (CLzmaEnc *)pp;
  Mf_Free(&p->matchFinderBase);
  free(p->litProbs);
  p->litProbs = 0;
  RangeEnc_Free(&p->rc);
  free(p);
}


int LzmaEnc_Encode(CLzmaEncHandle pp)
{
  int res = EMDROS_SZ_OK;
  CLzmaEnc *p = (CLzmaEnc *)pp;

  for (;;)
  {
    res = LzmaEnc_CodeOneBlock(p);
    if( res != EMDROS_SZ_OK || p->finished )
      break;
  }
  return res;
}


bool EMDROS_LZMA_compress(const int dictionary_size,
			  const int match_len_limit,
			  size_t uncompressed_size,
			  void *pFIN, void *pFOUT,
			  emdros_lzma_readblock_function_ptr emdros_lzma_readblock,
			  emdros_lzma_writeblock_function_ptr emdros_lzma_writeblock, std::string& error_message)
{
	int rc = EMDROS_SZ_OK;
	
	bool bResult = true;
	CLzmaEncHandle pLzmaEnc = LzmaEnc_Init(dictionary_size,
					       match_len_limit,
					       pFIN, pFOUT,
					       emdros_lzma_readblock,
					       emdros_lzma_writeblock);
	if (pLzmaEnc == 0) {
		error_message += "LZMA: Could not initialize LZMA state.\n";
		bResult = false;
	} else {
		EMDROS_LZMA_File_header header;
		EMDROS_LZMA_Fh_set_magic(header);
		EMDROS_LZMA_Fh_set_dictionary_size(header, dictionary_size);
		int emdros_errno = 0;
		if (emdros_lzma_writeblock(pFOUT, header, EMDROS_LZMA_Fh_size, &emdros_errno) != EMDROS_LZMA_Fh_size) {
			rc = EMDROS_SZ_ERROR_WRITE;
			error_message += "LZMA: Could not write header.\n";
		} else {
			uint8_t uncompressed_size_buf[8];
			EMDROS_LZMA_set_uncompressed_size(uncompressed_size, uncompressed_size_buf);
			if (emdros_lzma_writeblock(pFOUT, uncompressed_size_buf, 8, &emdros_errno) != 8) {
				rc = EMDROS_SZ_ERROR_WRITE;
				error_message += "LZMA: Could not write uncompressed size header.\n";
			} else {
				rc = LzmaEnc_Encode(pLzmaEnc);
			}
		} 

		LzmaEnc_Free(pLzmaEnc);

		if (rc == EMDROS_SZ_OK) {
			bResult = true;
		} else {
			bResult = false;
			error_message += "LZMA: Could encode input.\n";
		}
	}
	
	return bResult;
}


/* LzmaDec.c -- LZMA Decoder
2009-09-20 : Igor Pavlov : Public domain */

#define _FILE_OFFSET_BITS 64



#define kNumTopBits 24
#define kTopValue ((uint32_t)1 << kNumTopBits)

#define kNumBitModelTotalBits 11
#define kBitModelTotal (1 << kNumBitModelTotalBits)
#define kNumMoveBits 5

#define RC_INIT_SIZE 5

#define NORMALIZE if (range < kTopValue) { range <<= 8; code = (code << 8) | (*buf++); }

#define IF_BIT_0(p) ttt = *(p); NORMALIZE; bound = (range >> kNumBitModelTotalBits) * ttt; if (code < bound)
#define UPDATE_0(p) range = bound; *(p) = (int)(ttt + ((kBitModelTotal - ttt) >> kNumMoveBits));
#define UPDATE_1(p) range -= bound; code -= bound; *(p) = (int)(ttt - (ttt >> kNumMoveBits));
#define GET_BIT2(p, i, A0, A1) IF_BIT_0(p) \
  { UPDATE_0(p); i = (i + i); A0; } else \
  { UPDATE_1(p); i = (i + i) + 1; A1; }
#define GET_BIT(p, i) GET_BIT2(p, i, ; , ;)

#define TREE_GET_BIT(probs, i) { GET_BIT((probs + i), i); }
#define TREE_DECODE(probs, limit, i) \
  { i = 1; do { TREE_GET_BIT(probs, i); } while (i < limit); i -= limit; }

/* #define _LZMA_SIZE_OPT */

#ifdef _LZMA_SIZE_OPT
#define TREE_6_DECODE(probs, i) TREE_DECODE(probs, (1 << 6), i)
#else
#define TREE_6_DECODE(probs, i) \
  { i = 1; \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  TREE_GET_BIT(probs, i); \
  i -= 0x40; }
#endif

#define NORMALIZE_CHECK if (range < kTopValue) { if (buf >= bufLimit) return DUMMY_ERROR; range <<= 8; code = (code << 8) | (*buf++); }

#define IF_BIT_0_CHECK(p) ttt = *(p); NORMALIZE_CHECK; bound = (range >> kNumBitModelTotalBits) * ttt; if (code < bound)
#define UPDATE_0_CHECK range = bound;
#define UPDATE_1_CHECK range -= bound; code -= bound;
#define GET_BIT2_CHECK(p, i, A0, A1) IF_BIT_0_CHECK(p) \
  { UPDATE_0_CHECK; i = (i + i); A0; } else \
  { UPDATE_1_CHECK; i = (i + i) + 1; A1; }
#define GET_BIT_CHECK(p, i) GET_BIT2_CHECK(p, i, ; , ;)
#define TREE_DECODE_CHECK(probs, limit, i) \
  { i = 1; do { GET_BIT_CHECK(probs + i, i) } while (i < limit); i -= limit; }


#define kNumPosBitsMax 4
#define kNumPosStatesMax (1 << kNumPosBitsMax)

#define kLenNumLowBits 3
#define kLenNumLowSymbols (1 << kLenNumLowBits)
#define kLenNumMidBits 3
#define kLenNumMidSymbols (1 << kLenNumMidBits)
#define kLenNumHighBits 8
#define kLenNumHighSymbols (1 << kLenNumHighBits)

#define LenChoice 0
#define LenChoice2 (LenChoice + 1)
#define LenLow (LenChoice2 + 1)
#define LenMid (LenLow + (kNumPosStatesMax << kLenNumLowBits))
#define LenHigh (LenMid + (kNumPosStatesMax << kLenNumMidBits))
#define kNumLenProbs (LenHigh + kLenNumHighSymbols)


#define kNumStates 12
#define kNumLitStates 7

#define kStartPosModelIndex 4
#define kEndPosModelIndex 14
#define kNumFullDistances (1 << (kEndPosModelIndex >> 1))

#define kNumPosSlotBits 6
#define kNumLenToPosStates 4

#define kNumAlignBits 4
#define kAlignTableSize (1 << kNumAlignBits)

#define kMatchMinLen 2
#define kMatchSpecLenStart (kMatchMinLen + kLenNumLowSymbols + kLenNumMidSymbols + kLenNumHighSymbols)

#define IsMatch 0
#define IsRep (IsMatch + (kNumStates << kNumPosBitsMax))
#define IsRepG0 (IsRep + kNumStates)
#define IsRepG1 (IsRepG0 + kNumStates)
#define IsRepG2 (IsRepG1 + kNumStates)
#define IsRep0Long (IsRepG2 + kNumStates)
#define PosSlot (IsRep0Long + (kNumStates << kNumPosBitsMax))
#define SpecPos (PosSlot + (kNumLenToPosStates << kNumPosSlotBits))
#define Align (SpecPos + kNumFullDistances - kEndPosModelIndex)
#define LenCoder (Align + kAlignTableSize)
#define RepLenCoder (LenCoder + kNumLenProbs)
#define Literal (RepLenCoder + kNumLenProbs)

#define LZMA_BASE_SIZE 1846
#define LZMA_LIT_SIZE 768

#define LzmaProps_GetNumProbs(p) ((uint32_t)LZMA_BASE_SIZE + (LZMA_LIT_SIZE << ((p)->lc + (p)->lp)))

#if Literal != LZMA_BASE_SIZE
StopCompilingDueBUG
#endif


/* First LZMA-symbol is always decoded.
And it decodes new LZMA-symbols while (buf < bufLimit), but "buf" is without last normalization
Out:
  Result:
    true - OK
    false - Error
  p->remainLen:
    < kMatchSpecLenStart : normal remain
    = kMatchSpecLenStart : finished
    = kMatchSpecLenStart + 1 : Flush marker
    = kMatchSpecLenStart + 2 : State Init Marker
*/

static bool LzmaDec_DecodeReal(CLzmaDec *p, uint32_t limit, const uint8_t *bufLimit)
{
  int *probs = p->probs;

  State state = p->state;
  uint32_t rep0 = p->reps[0], rep1 = p->reps[1], rep2 = p->reps[2], rep3 = p->reps[3];
  unsigned pbMask = ((unsigned)1 << (p->pb)) - 1;
  unsigned lpMask = ((unsigned)1 << (p->lp)) - 1;
  const unsigned lc = p->lc;

  uint8_t *dic = p->dic;
  const uint32_t dicBufSize = p->dicBufSize;
  uint32_t dicPos = p->dicPos;

  uint32_t processedPos = p->processedPos;
  uint32_t checkDicSize = p->checkDicSize;
  unsigned len = 0;

  const uint8_t *buf = p->buf;
  uint32_t range = p->range;
  uint32_t code = p->code;

  do
  {
    int *prob;
    uint32_t bound;
    unsigned ttt;
    unsigned posState = processedPos & pbMask;

    prob = probs + IsMatch + (state << kNumPosBitsMax) + posState;
    IF_BIT_0(prob)
    {
      unsigned symbol;
      UPDATE_0(prob);
      prob = probs + Literal;
      if (checkDicSize != 0 || processedPos != 0)
        prob += (LZMA_LIT_SIZE * (((processedPos & lpMask) << lc) +
        (dic[(dicPos == 0 ? dicBufSize : dicPos) - 1] >> (8 - lc))));

      if (state < kNumLitStates)
      {
        state -= (state < 4) ? state : 3;
        symbol = 1;
        do { GET_BIT(prob + symbol, symbol) } while (symbol < 0x100);
      }
      else
      {
        unsigned matchByte = p->dic[(dicPos - rep0) + ((dicPos < rep0) ? dicBufSize : 0)];
        unsigned offs = 0x100;
        state -= (state < 10) ? 3 : 6;
        symbol = 1;
        do
        {
          unsigned bit;
          int *probLit;
          matchByte <<= 1;
          bit = (matchByte & offs);
          probLit = prob + offs + bit + symbol;
          GET_BIT2(probLit, symbol, offs &= ~bit, offs &= bit)
        }
        while (symbol < 0x100);
      }
      dic[dicPos++] = (uint8_t)symbol;
      processedPos++;
      continue;
    }
    else
    {
      UPDATE_1(prob);
      prob = probs + IsRep + state;
      IF_BIT_0(prob)
      {
        UPDATE_0(prob);
        state += kNumStates;
        prob = probs + LenCoder;
      }
      else
      {
        UPDATE_1(prob);
        if (checkDicSize == 0 && processedPos == 0)
          return false;
        prob = probs + IsRepG0 + state;
        IF_BIT_0(prob)
        {
          UPDATE_0(prob);
          prob = probs + IsRep0Long + (state << kNumPosBitsMax) + posState;
          IF_BIT_0(prob)
          {
            UPDATE_0(prob);
            dic[dicPos] = dic[(dicPos - rep0) + ((dicPos < rep0) ? dicBufSize : 0)];
            dicPos++;
            processedPos++;
            state = state < kNumLitStates ? 9 : 11;
            continue;
          }
          UPDATE_1(prob);
        }
        else
        {
          uint32_t distance;
          UPDATE_1(prob);
          prob = probs + IsRepG1 + state;
          IF_BIT_0(prob)
          {
            UPDATE_0(prob);
            distance = rep1;
          }
          else
          {
            UPDATE_1(prob);
            prob = probs + IsRepG2 + state;
            IF_BIT_0(prob)
            {
              UPDATE_0(prob);
              distance = rep2;
            }
            else
            {
              UPDATE_1(prob);
              distance = rep3;
              rep3 = rep2;
            }
            rep2 = rep1;
          }
          rep1 = rep0;
          rep0 = distance;
        }
        state = state < kNumLitStates ? 8 : 11;
        prob = probs + RepLenCoder;
      }
      {
        unsigned limit, offset;
        int *probLen = prob + LenChoice;
        IF_BIT_0(probLen)
        {
          UPDATE_0(probLen);
          probLen = prob + LenLow + (posState << kLenNumLowBits);
          offset = 0;
          limit = (1 << kLenNumLowBits);
        }
        else
        {
          UPDATE_1(probLen);
          probLen = prob + LenChoice2;
          IF_BIT_0(probLen)
          {
            UPDATE_0(probLen);
            probLen = prob + LenMid + (posState << kLenNumMidBits);
            offset = kLenNumLowSymbols;
            limit = (1 << kLenNumMidBits);
          }
          else
          {
            UPDATE_1(probLen);
            probLen = prob + LenHigh;
            offset = kLenNumLowSymbols + kLenNumMidSymbols;
            limit = (1 << kLenNumHighBits);
          }
        }
        TREE_DECODE(probLen, limit, len);
        len += offset;
      }

      if (state >= kNumStates)
      {
        uint32_t distance;
        prob = probs + PosSlot +
            ((len < kNumLenToPosStates ? len : kNumLenToPosStates - 1) << kNumPosSlotBits);
        TREE_6_DECODE(prob, distance);
        if (distance >= kStartPosModelIndex)
        {
          unsigned posSlot = (unsigned)distance;
          int numDirectBits = (int)(((distance >> 1) - 1));
          distance = (2 | (distance & 1));
          if (posSlot < kEndPosModelIndex)
          {
            distance <<= numDirectBits;
            prob = probs + SpecPos + distance - posSlot - 1;
            {
              uint32_t mask = 1;
              unsigned i = 1;
              do
              {
                GET_BIT2(prob + i, i, ; , distance |= mask);
                mask <<= 1;
              }
              while (--numDirectBits != 0);
            }
          }
          else
          {
            numDirectBits -= kNumAlignBits;
            do
            {
              NORMALIZE
              range >>= 1;

              {
                uint32_t t;
                code -= range;
                t = (0 - ((uint32_t)code >> 31)); /* (uint32_t)((int)code >> 31) */
                distance = (distance << 1) + (t + 1);
                code += range & t;
              }
              /*
              distance <<= 1;
              if (code >= range)
              {
                code -= range;
                distance |= 1;
              }
              */
            }
            while (--numDirectBits != 0);
            prob = probs + Align;
            distance <<= kNumAlignBits;
            {
              unsigned i = 1;
              GET_BIT2(prob + i, i, ; , distance |= 1);
              GET_BIT2(prob + i, i, ; , distance |= 2);
              GET_BIT2(prob + i, i, ; , distance |= 4);
              GET_BIT2(prob + i, i, ; , distance |= 8);
            }
            if (distance == (uint32_t)0xFFFFFFFF)
            {
              len += kMatchSpecLenStart;
              state -= kNumStates;
              break;
            }
          }
        }
        rep3 = rep2;
        rep2 = rep1;
        rep1 = rep0;
        rep0 = distance + 1;
        if (checkDicSize == 0)
        {
          if (distance >= processedPos)
            return false;
        }
        else if (distance >= checkDicSize)
          return false;
        state = (state < kNumStates + kNumLitStates) ? kNumLitStates : kNumLitStates + 3;
      }

      len += kMatchMinLen;

      if (limit == dicPos)
        return false;
      {
        uint32_t rem = limit - dicPos;
        unsigned curLen = ((rem < len) ? (unsigned)rem : len);
        uint32_t pos = (dicPos - rep0) + ((dicPos < rep0) ? dicBufSize : 0);

        processedPos += curLen;

        len -= curLen;
        if (pos + curLen <= dicBufSize)
        {
          uint8_t *dest = dic + dicPos;
          ptrdiff_t src = (ptrdiff_t)pos - (ptrdiff_t)dicPos;
          const uint8_t *lim = dest + curLen;
          dicPos += curLen;
          do
            *(dest) = (uint8_t)*(dest + src);
          while (++dest != lim);
        }
        else
        {
          do
          {
            dic[dicPos++] = dic[pos];
            if (++pos == dicBufSize)
              pos = 0;
          }
          while (--curLen != 0);
        }
      }
    }
  }
  while (dicPos < limit && buf < bufLimit);
  NORMALIZE;
  p->buf = buf;
  p->range = range;
  p->code = code;
  p->remainLen = len;
  p->dicPos = dicPos;
  p->processedPos = processedPos;
  p->reps[0] = rep0;
  p->reps[1] = rep1;
  p->reps[2] = rep2;
  p->reps[3] = rep3;
  p->state = state;

  return true;
}

static void LzmaDec_WriteRem(CLzmaDec *p, uint32_t limit)
{
  if (p->remainLen != 0 && p->remainLen < kMatchSpecLenStart)
  {
    uint8_t *dic = p->dic;
    uint32_t dicPos = p->dicPos;
    const uint32_t dicBufSize = p->dicBufSize;
    unsigned len = p->remainLen;
    uint32_t rep0 = p->reps[0];
    if (limit - dicPos < len)
      len = (unsigned)(limit - dicPos);

    if (p->checkDicSize == 0 && dicBufSize - p->processedPos <= len)
      p->checkDicSize = dicBufSize;

    p->processedPos += len;
    p->remainLen -= len;
    while (len-- != 0)
    {
      dic[dicPos] = dic[(dicPos - rep0) + ((dicPos < rep0) ? dicBufSize : 0)];
      dicPos++;
    }
    p->dicPos = dicPos;
  }
}

static int LzmaDec_DecodeReal2(CLzmaDec *p, uint32_t limit, const uint8_t *bufLimit)
{
  const uint32_t dicBufSize = p->dicBufSize;
  do
  {
    uint32_t limit2 = limit;
    if (p->checkDicSize == 0)
    {
      uint32_t rem = dicBufSize - p->processedPos;
      if (limit - p->dicPos > rem)
        limit2 = p->dicPos + rem;
    }
    if( !LzmaDec_DecodeReal(p, limit2, bufLimit) ) return false;
    if (p->processedPos >= dicBufSize)
      p->checkDicSize = dicBufSize;
    LzmaDec_WriteRem(p, limit);
  }
  while (p->dicPos < limit && p->buf < bufLimit && p->remainLen < kMatchSpecLenStart);

  if (p->remainLen > kMatchSpecLenStart)
  {
    p->remainLen = kMatchSpecLenStart;
  }
  return true;
}

typedef enum
{
  DUMMY_ERROR, /* unexpected end of input stream */
  DUMMY_LIT,
  DUMMY_MATCH,
  DUMMY_REP
} ELzmaDummy;

static ELzmaDummy LzmaDec_TryDummy(const CLzmaDec *p, const uint8_t *buf, uint32_t inSize)
{
  uint32_t range = p->range;
  uint32_t code = p->code;
  const uint8_t *bufLimit = buf + inSize;
  int *probs = p->probs;
  State state = p->state;
  ELzmaDummy res;

  {
    int *prob;
    uint32_t bound;
    unsigned ttt;
    unsigned posState = (p->processedPos) & ((1 << p->pb) - 1);

    prob = probs + IsMatch + (state << kNumPosBitsMax) + posState;
    IF_BIT_0_CHECK(prob)
    {
      UPDATE_0_CHECK

      /* if (bufLimit - buf >= 7) return DUMMY_LIT; */

      prob = probs + Literal;
      if (p->checkDicSize != 0 || p->processedPos != 0)
        prob += (LZMA_LIT_SIZE *
          ((((p->processedPos) & ((1 << (p->lp)) - 1)) << p->lc) +
          (p->dic[(p->dicPos == 0 ? p->dicBufSize : p->dicPos) - 1] >> (8 - p->lc))));

      if (state < kNumLitStates)
      {
        unsigned symbol = 1;
        do { GET_BIT_CHECK(prob + symbol, symbol) } while (symbol < 0x100);
      }
      else
      {
        unsigned matchByte = p->dic[p->dicPos - p->reps[0] +
            ((p->dicPos < p->reps[0]) ? p->dicBufSize : 0)];
        unsigned offs = 0x100;
        unsigned symbol = 1;
        do
        {
          unsigned bit;
          int *probLit;
          matchByte <<= 1;
          bit = (matchByte & offs);
          probLit = prob + offs + bit + symbol;
          GET_BIT2_CHECK(probLit, symbol, offs &= ~bit, offs &= bit)
        }
        while (symbol < 0x100);
      }
      res = DUMMY_LIT;
    }
    else
    {
      unsigned len;
      UPDATE_1_CHECK;

      prob = probs + IsRep + state;
      IF_BIT_0_CHECK(prob)
      {
        UPDATE_0_CHECK;
        state = 0;
        prob = probs + LenCoder;
        res = DUMMY_MATCH;
      }
      else
      {
        UPDATE_1_CHECK;
        res = DUMMY_REP;
        prob = probs + IsRepG0 + state;
        IF_BIT_0_CHECK(prob)
        {
          UPDATE_0_CHECK;
          prob = probs + IsRep0Long + (state << kNumPosBitsMax) + posState;
          IF_BIT_0_CHECK(prob)
          {
            UPDATE_0_CHECK;
            NORMALIZE_CHECK;
            return DUMMY_REP;
          }
          else
          {
            UPDATE_1_CHECK;
          }
        }
        else
        {
          UPDATE_1_CHECK;
          prob = probs + IsRepG1 + state;
          IF_BIT_0_CHECK(prob)
          {
            UPDATE_0_CHECK;
          }
          else
          {
            UPDATE_1_CHECK;
            prob = probs + IsRepG2 + state;
            IF_BIT_0_CHECK(prob)
            {
              UPDATE_0_CHECK;
            }
            else
            {
              UPDATE_1_CHECK;
            }
          }
        }
        state = kNumStates;
        prob = probs + RepLenCoder;
      }
      {
        unsigned limit, offset;
        int *probLen = prob + LenChoice;
        IF_BIT_0_CHECK(probLen)
        {
          UPDATE_0_CHECK;
          probLen = prob + LenLow + (posState << kLenNumLowBits);
          offset = 0;
          limit = 1 << kLenNumLowBits;
        }
        else
        {
          UPDATE_1_CHECK;
          probLen = prob + LenChoice2;
          IF_BIT_0_CHECK(probLen)
          {
            UPDATE_0_CHECK;
            probLen = prob + LenMid + (posState << kLenNumMidBits);
            offset = kLenNumLowSymbols;
            limit = 1 << kLenNumMidBits;
          }
          else
          {
            UPDATE_1_CHECK;
            probLen = prob + LenHigh;
            offset = kLenNumLowSymbols + kLenNumMidSymbols;
            limit = 1 << kLenNumHighBits;
          }
        }
        TREE_DECODE_CHECK(probLen, limit, len);
        len += offset;
      }

      if (state < 4)
      {
        unsigned posSlot;
        prob = probs + PosSlot +
            ((len < kNumLenToPosStates ? len : kNumLenToPosStates - 1) <<
            kNumPosSlotBits);
        TREE_DECODE_CHECK(prob, 1 << kNumPosSlotBits, posSlot);
        if (posSlot >= kStartPosModelIndex)
        {
          int numDirectBits = ((posSlot >> 1) - 1);

          /* if (bufLimit - buf >= 8) return DUMMY_MATCH; */

          if (posSlot < kEndPosModelIndex)
          {
            prob = probs + SpecPos + ((2 | (posSlot & 1)) << numDirectBits) - posSlot - 1;
          }
          else
          {
            numDirectBits -= kNumAlignBits;
            do
            {
              NORMALIZE_CHECK
              range >>= 1;
              code -= range & (((code - range) >> 31) - 1);
              /* if (code >= range) code -= range; */
            }
            while (--numDirectBits != 0);
            prob = probs + Align;
            numDirectBits = kNumAlignBits;
          }
          {
            unsigned i = 1;
            do
            {
              GET_BIT_CHECK(prob + i, i);
            }
            while (--numDirectBits != 0);
          }
        }
      }
    }
  }
  NORMALIZE_CHECK;
  return res;
}


void EMDROS_LZMA_make_raw_props(unsigned int dictionarySize, uint8_t *raw_props)
{
	uint8_t lit_pos_bits = 0;
	
	uint8_t d = ((9*5) * pos_state_bits) + (9 * lit_pos_bits) + literal_context_bits;
	raw_props[0] = d;
	
	uint32_t myDS = dictionarySize;
	raw_props[1] = myDS & 0xFF;
	myDS >>= 8;
	raw_props[2] = myDS & 0xFF;
	myDS >>= 8;
	raw_props[3] = myDS & 0xFF;
	myDS >>= 8;
	raw_props[4] = myDS & 0xFF;
}
 
	  

static void LzmaDec_InitRc(CLzmaDec *p, const uint8_t *data)
{
  p->code = ((uint32_t)data[1] << 24) | ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 8) | ((uint32_t)data[4]);
  p->range = 0xFFFFFFFF;
  p->needFlush = false;
}


static bool LzmaDec_DecodeToDic(CLzmaDec *p, uint32_t dicLimit,
                                const uint8_t *src, uint32_t *srcLen,
                                ELzmaFinishMode finishMode, ELzmaStatus *status)
{
  uint32_t inSize = *srcLen;
  (*srcLen) = 0;
  LzmaDec_WriteRem(p, dicLimit);

  *status = LZMA_STATUS_NOT_SPECIFIED;

  while (p->remainLen != kMatchSpecLenStart)
  {
      int checkEndMarkNow;

      if( p->needFlush )
      {
        for (; inSize > 0 && p->tempBufSize < RC_INIT_SIZE; (*srcLen)++, inSize--)
          p->tempBuf[p->tempBufSize++] = *src++;
        if (p->tempBufSize < RC_INIT_SIZE)
        {
          *status = LZMA_STATUS_NEEDS_MORE_INPUT;
          return true;
        }
        if (p->tempBuf[0] != 0)
          return false;

        LzmaDec_InitRc(p, p->tempBuf);
        p->tempBufSize = 0;
      }

      checkEndMarkNow = 0;
      if (p->dicPos >= dicLimit)
      {
        if (p->remainLen == 0 && p->code == 0)
        {
          *status = LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK;
          return true;
        }
        if (finishMode == LZMA_FINISH_ANY)
        {
          *status = LZMA_STATUS_NOT_FINISHED;
          return true;
        }
        if (p->remainLen != 0)
        {
          *status = LZMA_STATUS_NOT_FINISHED;
          return false;
        }
        checkEndMarkNow = 1;
      }

      if (p->tempBufSize == 0)
      {
        uint32_t processed;
        const uint8_t *bufLimit;
        if (inSize < LZMA_REQUIRED_INPUT_MAX || checkEndMarkNow)
        {
          int dummyRes = LzmaDec_TryDummy(p, src, inSize);
          if (dummyRes == DUMMY_ERROR)
          {
            memcpy(p->tempBuf, src, inSize);
            p->tempBufSize = (unsigned)inSize;
            (*srcLen) += inSize;
            *status = LZMA_STATUS_NEEDS_MORE_INPUT;
            return true;
          }
          if (checkEndMarkNow && dummyRes != DUMMY_MATCH)
          {
            *status = LZMA_STATUS_NOT_FINISHED;
            return false;
          }
          bufLimit = src;
        }
        else
          bufLimit = src + inSize - LZMA_REQUIRED_INPUT_MAX;
        p->buf = src;
        if( !LzmaDec_DecodeReal2(p, dicLimit, bufLimit) )
          return false;
        processed = (uint32_t)(p->buf - src);
        (*srcLen) += processed;
        src += processed;
        inSize -= processed;
      }
      else
      {
        unsigned rem = p->tempBufSize, lookAhead = 0;
        while (rem < LZMA_REQUIRED_INPUT_MAX && lookAhead < inSize)
          p->tempBuf[rem++] = src[lookAhead++];
        p->tempBufSize = rem;
        if (rem < LZMA_REQUIRED_INPUT_MAX || checkEndMarkNow)
        {
          int dummyRes = LzmaDec_TryDummy(p, p->tempBuf, rem);
          if (dummyRes == DUMMY_ERROR)
          {
            (*srcLen) += lookAhead;
            *status = LZMA_STATUS_NEEDS_MORE_INPUT;
            return true;
          }
          if (checkEndMarkNow && dummyRes != DUMMY_MATCH)
          {
            *status = LZMA_STATUS_NOT_FINISHED;
            return false;
          }
        }
        p->buf = p->tempBuf;
        if( !LzmaDec_DecodeReal2(p, dicLimit, p->buf) )
          return false;
        lookAhead -= (rem - (unsigned)(p->buf - p->tempBuf));
        (*srcLen) += lookAhead;
        src += lookAhead;
        inSize -= lookAhead;
        p->tempBufSize = 0;
      }
  }
  if (p->code == 0)
    *status = LZMA_STATUS_FINISHED_WITH_MARK;
  return (p->code == 0);
}

bool LzmaDec_DecodeToBuf( CLzmaDec *p, uint8_t *dest, uint32_t *destLen,
                          const uint8_t *src, uint32_t *srcLen,
                          ELzmaFinishMode finishMode, ELzmaStatus *status )
{
  uint32_t outSize = *destLen;
  uint32_t inSize = *srcLen;
  *srcLen = *destLen = 0;
  for (;;)
  {
    uint32_t inSizeCur = inSize, outSizeCur, dicPos;
    ELzmaFinishMode curFinishMode;
    bool res;
    if (p->dicPos == p->dicBufSize)
      p->dicPos = 0;
    dicPos = p->dicPos;
    if (outSize > p->dicBufSize - dicPos)
    {
      outSizeCur = p->dicBufSize;
      curFinishMode = LZMA_FINISH_ANY;
    }
    else
    {
      outSizeCur = dicPos + outSize;
      curFinishMode = finishMode;
    }

    res = LzmaDec_DecodeToDic(p, outSizeCur, src, &inSizeCur, curFinishMode, status);
    src += inSizeCur;
    inSize -= inSizeCur;
    *srcLen += inSizeCur;
    outSizeCur = p->dicPos - dicPos;
    memcpy(dest, p->dic + dicPos, outSizeCur);
    dest += outSizeCur;
    outSize -= outSizeCur;
    *destLen += outSizeCur;
    if( !res )
      return false;
    if (outSizeCur == 0 || outSize == 0)
      return true;
    else
      return false;
  }
}


void LzmaDec_Free(CLzmaDec *p)
{
  free( p->dic );
  free( p->probs );
}


bool LzmaDec_Init(CLzmaDec *p, const uint8_t *raw_props)
{
  uint32_t i;
  uint8_t d = raw_props[0];

  p->lc = d % 9;
  d /= 9;
  p->pb = d / 5;
  p->lp = d % 5;

  p->dicBufSize = raw_props[1] | ((uint32_t)raw_props[2] << 8) |
                  ((uint32_t)raw_props[3] << 16) | ((uint32_t)raw_props[4] << 24);
  if (p->dicBufSize < min_dictionary_size) p->dicBufSize = min_dictionary_size;

  p->numProbs = LzmaProps_GetNumProbs(p);
  p->probs = (int *)malloc(p->numProbs * sizeof(int));
  if( !p->probs ) return false;
  p->dic = (uint8_t *)malloc(p->dicBufSize);
  if (p->dic == 0)
    {
    free( p->probs );
    return false;
    }
  p->dicPos = 0;
  p->needFlush = true;
  p->remainLen = 0;
  p->tempBufSize = 0;
  p->processedPos = 0;
  p->checkDicSize = 0;
  for( i = 0; i < p->numProbs; ++i ) p->probs[i] = kBitModelTotal >> 1;
  p->reps[0] = p->reps[1] = p->reps[2] = p->reps[3] = 1;
  p->state = 0;
  return true;
}

#endif // #if defined(USE_BPT) && USE_BPT
