/* See md5.c for explanation and copyright information.  */

/*
 * $FreeBSD: src/contrib/cvs/lib/md5.h,v 1.2 1999/12/11 15:10:02 peter Exp $
 */

#ifndef MD5_EMDROS_H
#define MD5_EMDROS_H

#include <string>

/* Unlike previous versions of this code, uint32 need not be exactly
   32 bits, merely 32 bits or more.  Choosing a data type which is 32
   bits instead of 64 is not important; speed is considerably more
   important.  ANSI guarantees that "unsigned long" will be big enough,
   and always using it seems to have few disadvantages.  */
typedef unsigned long emdros_uint32;

struct emdros_MD5Context {
	emdros_uint32 buf[4];
	emdros_uint32 bits[2];
	unsigned char in[64];
};

void emdros_MD5Init(struct emdros_MD5Context *context);
void emdros_MD5Update(struct emdros_MD5Context *context,
		    unsigned char const *buf, unsigned len);
void emdros_MD5Final(unsigned char digest[16],
		   struct emdros_MD5Context *context);
void emdros_MD5Transform(emdros_uint32 buf[4], const unsigned char in[64]);

std::string emdros_MD5Digest(const std::string& instring);

#endif /* !MD5_EMDROS_H */

