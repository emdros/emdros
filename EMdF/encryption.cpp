/*
 * encryption.cpp
 *
 * Functions to encrypt and decrypt strings
 *
 * Ulrik Petersen
 * Created: 1/4-2003 (January 4, 2003)
 * Last update: 1/4-2008
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



#include <encryption.h>

#define NO_ROL_BITS  (0xff)
unsigned int g_rol_bits = NO_ROL_BITS;

#define NO_EOR_STRING ("")
std::string g_eor_string = NO_EOR_STRING;

#define NO_FIRST_EOR (0xcafebabe)
unsigned int g_first_eor = NO_FIRST_EOR;

static bool g_bRandomGeneratorSeeded = false;

void seed_random_generator(void)
{
  // Get current time
  time_t curtime = time(NULL);

  // Eor with a number so we don't get -1
  unsigned int seed = ((unsigned int) curtime) ^ 0xed739516;

  // Seed random number generator
  srand(seed);

  // We now don't need to do it again.
  g_bRandomGeneratorSeeded = true;
}

unsigned int my_rand(void)
{
  if (!g_bRandomGeneratorSeeded) {
    seed_random_generator();
  }
  return rand();
}

  
  
unsigned int get_rol_bits()
{
  while (g_rol_bits == NO_ROL_BITS) {
    g_rol_bits = my_rand() & 0x03;
  }

  return g_rol_bits;
}


unsigned int get_first_eor()
{
  while (g_first_eor == NO_FIRST_EOR) {
    g_first_eor = my_rand() & 0xFF;
  }

  return g_first_eor;
}


void generate_eor_string(std::string& eor_string)
{
  for (int f = 0; f < NO_OF_EOR_STRING_CHARS; ++f) {
    // Generate something printable
    unsigned char c = 0;
    while (c == 0 
	   || (c < 0x20)
	   || (c >= 0x80 && c <= 0xb0)) {
      c = my_rand() & 0xFF;
    }

    // Add it to eor_string
    eor_string += c;
  }
}


std::string get_eor_string()
{
  while (g_eor_string == NO_EOR_STRING) {
    generate_eor_string(g_eor_string);
  }

  return g_eor_string;
}


