/*
 * LZMA encoding and decoding for Emdros.
 *
 * Created: 3/29-2018.
 * Last update: 4/3-2018.
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
#ifndef EMDROS_LZMA_H_
#define EMDROS_LZMA_H_

#include <string>

#include <cstddef>
#include <cstdint>
	
typedef int State;

#define EMDROS_SZ_OK (0)
#define EMDROS_SZ_ERROR_READ (8)
#define EMDROS_SZ_ERROR_WRITE (9)
#define EMDROS_SZ_END (10)

typedef int (*emdros_lzma_readblock_function_ptr)(void *pFIN, uint8_t * const buf, const int size, int *emdros_errno);
typedef int (*emdros_lzma_writeblock_function_ptr)(void *pFOUT, const uint8_t * const buf, const int size, int *emdros_errno);

/* LzFind.h -- Match finder for LZ algorithms
2009-04-22 : Igor Pavlov : Public domain */

typedef uint32_t CLzRef;

typedef struct
{
  uint8_t *bufferBase;
  uint8_t *buffer;
  CLzRef *hash;
  CLzRef *son;
  uint32_t pos;
  uint32_t posLimit;
  uint32_t streamPos;
  uint32_t lenLimit;

  uint32_t cyclicBufferPos;
  uint32_t cyclicBufferSize; /* it must be = (historySize + 1) */

  uint32_t matchMaxLen;
  uint32_t hashMask;
  uint32_t cutValue;

  uint32_t blockSize;
  uint32_t keepSizeBefore;
  uint32_t keepSizeAfter;

  uint32_t numHashBytes;
  uint32_t historySize;
  uint32_t hashSizeSum;
  uint32_t numSons;
  void *pFIN;
  int result;
  uint32_t crc;
  bool btMode;
  bool streamEndWasReached;
  emdros_lzma_readblock_function_ptr lzma_readblock;
} CMatchFinder;


/* Conditions:
     historySize <= 3 GB
     keepAddBufferBefore + matchMaxLen + keepAddBufferAfter < 511MB
*/
int Mf_Init(CMatchFinder *p, void *pFIN, const int mc, uint32_t historySize,
	    uint32_t keepAddBufferBefore, uint32_t matchMaxLen, uint32_t keepAddBufferAfter, emdros_lzma_readblock_function_ptr rdblk_func);

void Mf_Free(CMatchFinder *p);


/*
Conditions:
  Mf_GetNumAvailableBytes_Func must be called before each Mf_GetMatchLen_Func.
  Mf_GetPointerToCurrentPos_Func's result must be used only before any other function
*/

typedef uint32_t (*Mf_GetMatches_Func)(void *object, uint32_t *distances);
typedef void (*Mf_Skip_Func)(void *object, uint32_t);

typedef struct _IMatchFinder
{
  Mf_GetMatches_Func GetMatches;
  Mf_Skip_Func Skip;
} IMatchFinder;

void Mf_CreateVTable(CMatchFinder *p, IMatchFinder *vTable);





	
/* LzmaEnc.h -- LZMA Encoder
2009-02-07 : Igor Pavlov : Public domain */


/* ---------- CLzmaEncHandle Interface ---------- */

/* LzmaEnc_* functions can return the following exit codes:
Returns:
  EMDROS_SZ_OK           - OK
  EMDROS_SZ_ERROR_WRITE  - Write callback error.
*/


// Emdros-provided compress() function.
extern bool EMDROS_LZMA_compress(const int dictionary_size,
				 const int match_len_limit,
				 size_t uncompressed_size,
				 void *pFIN, void *pFOUT,
				 emdros_lzma_readblock_function_ptr emdros_lzma_readblock,
				 emdros_lzma_writeblock_function_ptr emdros_lzma_writeblock,
				 std::string& error_message);




/* LzmaDec.h -- LZMA Decoder
2009-02-07 : Igor Pavlov : Public domain */

/* ---------- LZMA Properties ---------- */

#define LZMA_PROPS_SIZE 5


/* ---------- LZMA Decoder state ---------- */

/* LZMA_REQUIRED_INPUT_MAX = number of required input bytes for worst case.
   Num bits = log2((2^11 / 31) ^ 22) + 26 < 134 + 26 = 160; */

#define LZMA_REQUIRED_INPUT_MAX 20

typedef struct
{
  int *probs;
  uint8_t *dic;
  const uint8_t *buf;
  uint32_t range, code;
  uint32_t dicPos;
  uint32_t dicBufSize;
  uint32_t processedPos;
  uint32_t checkDicSize;
  unsigned lc, lp, pb;
  State state;
  uint32_t reps[4];
  unsigned remainLen;
  uint32_t numProbs;
  unsigned tempBufSize;
  bool needFlush;
  uint8_t tempBuf[LZMA_REQUIRED_INPUT_MAX];
} CLzmaDec;


/* There are two types of LZMA streams:
     0) Stream with end mark. That end mark adds about 6 bytes to compressed size.
     1) Stream without end mark. You must know exact uncompressed size to decompress such stream. */

typedef enum
{
  LZMA_FINISH_ANY,   /* finish at any point */
  LZMA_FINISH_END    /* block must be finished at the end */
} ELzmaFinishMode;

/* ELzmaFinishMode has meaning only if the decoding reaches output limit !!!

   You must use LZMA_FINISH_END, when you know that current output buffer
   covers last bytes of block. In other cases you must use LZMA_FINISH_ANY.

   If LZMA decoder sees end marker before reaching output limit, it returns EMDROS_SZ_OK,
   and output value of destLen will be less than output buffer size limit.
   You can check status result also.

   You can use multiple checks to test data integrity after full decompression:
     1) Check Result and "status" variable.
     2) Check that output(destLen) = uncompressedSize, if you know real uncompressedSize.
     3) Check that output(srcLen) = compressedSize, if you know real compressedSize.
        You must use correct finish mode in that case. */

typedef enum
{
  LZMA_STATUS_NOT_SPECIFIED,               /* use main error code instead */
  LZMA_STATUS_FINISHED_WITH_MARK,          /* stream was finished with end mark. */
  LZMA_STATUS_NOT_FINISHED,                /* stream was not finished */
  LZMA_STATUS_NEEDS_MORE_INPUT,            /* you must provide more input bytes */
  LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK  /* there is probability that stream was finished without end mark */
} ELzmaStatus;

/* ELzmaStatus is used only as output value for function call */


bool LzmaDec_Init(CLzmaDec *p, const uint8_t *raw_props);
void LzmaDec_Free(CLzmaDec *p);


/* ---------- Buffer Interface ---------- */

/* It's zlib-like interface.

finishMode:
  It has meaning only if the decoding reaches output limit (*destLen).
  LZMA_FINISH_ANY - Decode just destLen bytes.
  LZMA_FINISH_END - Stream must be finished after (*destLen).
*/

bool LzmaDec_DecodeToBuf( CLzmaDec *p, uint8_t *dest, uint32_t *destLen,
                          const uint8_t *src, uint32_t *srcLen,
                          ELzmaFinishMode finishMode, ELzmaStatus *status );

/* 0-3 magic bytes */
/*   4 version */
/*   5 coded_dict_size */
#define EMDROS_LZMA_Fh_size (6)
typedef uint8_t EMDROS_LZMA_File_header[EMDROS_LZMA_Fh_size];

extern bool EMDROS_LZMA_Fh_verify_magic(const EMDROS_LZMA_File_header data);

extern uint32_t EMDROS_LZMA_Fh_get_dictionary_size(const EMDROS_LZMA_File_header data);

extern void EMDROS_LZMA_set_uncompressed_size(size_t uncompressed_size, uint8_t *uncompressed_size_buf);
extern size_t EMDROS_LZMA_get_uncompressed_size(uint8_t *uncompressed_size_buf);


extern void EMDROS_LZMA_make_raw_props(unsigned int dictionarySize, uint8_t raw_props[]);

#endif /* EMDROS_LZMA_H_ */
