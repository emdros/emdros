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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2003-2008  Ulrik Petersen
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation, license version 2.  
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *   02111-1307 USA
 *
 *
 *   Special exception
 *   =================
 * 
 *   In addition, as a special exception, Ulrik Petersen, the
 *   copyright holder of Emdros, gives permission to link Emdros, in
 *   whole or in part, with the libraries which are normally
 *   distributed with:
 *   
 *   a) Sun's Java platform,
 *   b) Python, 
 *   c) Jython,
 *   d) Ruby, and/or 
 *   e) Perl 
 *   f) PostgreSQL
 *   g) OpenSSL
 *
 *   (or with modified versions of these), and to distribute linked
 *   combinations including both Emdros, in whole or in part, and one
 *   or more of the libraries normally distributed with (a)-(g) above.
 *
 *   Please note: This gives you special rights concerning the
 *   libraries which normally accompany the above pieces of software.
 *   It gives you no special rights concerning software that you write
 *   yourself.  You must obey the GNU General Public License in all
 *   respects for all of the code used other than the libraries
 *   normally distributed with (a)-(g) above.
 *
 *   If you modify this file, you may extend this exception to your
 *   version of the file, but you are not obligated to do so. If you
 *   do not wish to do so, delete this exception statement from your
 *   version.
 *
 *
 *   Other licensing forms
 *   =====================
 *
 *   If you wish to negotiate commercial licensing, please contact
 *   Ulrik Petersen at ulrikp[at]users.sourceforge.net.
 *
 *   Licensing can also be negotiated if your organization is an
 *   educational, non-profit, charity, missionary or similar
 *   organization.
 *
 *
 *   Website
 *   =======
 *
 *   Emdros has a website here:
 *
 *   http://emdros.org
 *
 *
 *
 **************************************************************************/


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


