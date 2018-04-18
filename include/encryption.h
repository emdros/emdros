/*
 * encryption.h
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
 *   Copyright (C) 2003-2008  Ulrik Sandborg-Petersen
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
  for (int f = eor_string.length() - 1; f >= 0; f--) {
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

