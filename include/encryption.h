/*
 * encryption.h
 *
 * Functions to encrypt and decrypt strings
 *
 * Ulrik Petersen
 * Created: 1/4-2003 (January 4, 2003)
 * Last update: 11/30-2018
 *
 */

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


#include <ctime>
#include <cstdlib>
#include <string>


#define NO_OF_EOR_STRING_CHARS (10)



#define ROL_BITS (get_rol_bits())
  
#define EOR_STRING (get_eor_string())
  
#define FIRST_EOR (get_first_eor())

extern void seed_random_generator(void);
extern unsigned int get_rol_bits();
extern unsigned int get_first_eor();
extern std::string get_eor_string();

inline unsigned int ror(unsigned int i, int rol_bits)
{
  // std::cerr << "Ror in: '" << (unsigned char) i << "'" << std::endl;
  unsigned int a = (i >> rol_bits) & 0xFF;
  unsigned int mask = (1 << rol_bits) - 1;
  int b = (a | ((i & mask) << (8-rol_bits)));
  // std::cerr << "Ror out: '" << (unsigned char) b << "'" << std::endl;
  return b;
}

inline unsigned char encryptChar(unsigned char c, unsigned char prev_char)
{
  unsigned int i = c;
  int rol_bits = ROL_BITS;
  i = (i ^ (unsigned int) prev_char) & 0xFF;
  std::string eor_string = EOR_STRING;
  for (int f = (int) eor_string.length() - 1; f >= 0; f--) {
    i = (i ^ (unsigned int) eor_string[f]) & 0xFF;
    if ((f % (eor_string.length()/2)) == 1)
      i = ror(i, rol_bits);
  }
  return (unsigned char) i;
}


inline unsigned int rol(unsigned int c, unsigned int rol_bits)
{
  unsigned int a = c << rol_bits;
  return (unsigned int) ((a & 0xFF) | (a >> 8));
}

inline unsigned char unencryptChar(unsigned char c, unsigned char prev_char)
{
  unsigned int i = c;
  std::string eor_string = std::string(EOR_STRING);
  for (unsigned int f = 0; f < eor_string.length(); f++) {
    if ((f % (eor_string.length()/2)) == 1)
      i = rol(i, ROL_BITS);
    i = (i ^ (unsigned int) eor_string[f]) & 0xFF;
  }

  i = (i ^ (unsigned int) prev_char) & 0xFF;
  return (unsigned char) i;
}

inline void encryptString(std::string& str)
{
  unsigned char prev_char = (unsigned char) FIRST_EOR;
  unsigned char temp;
  for (unsigned int f = 0; f < str.length(); f++) {
    temp = str[f];
    str[f] = encryptChar(str[f], prev_char);
    prev_char = temp;
  }
}

inline void unencryptString(std::string& str)
{
  unsigned char prev_char = (unsigned char) FIRST_EOR;
  for (unsigned int f = 0; f < str.length(); f++) {
    str[f] = unencryptChar(str[f], prev_char);
    prev_char = str[f];
  }
}

inline void zeroFillString(std::string& str)
{
  for (unsigned int f = 0; f < str.length(); f++) {
    // Delete char
    str[f] = '\0';

    // This is necessary because some compilers optimize the above
    // away if we don't read the byte again.
    char c = str[f]; 
    char p = c + 1;
    c = p;
  }
}

