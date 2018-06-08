/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2006-2017  Ulrik Sandborg-Petersen
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
 *   d) PostgreSQL
 *   e) OpenSSL
 *
 *   (or with modified versions of these), and to distribute linked
 *   combinations including both Emdros, in whole or in part, and one
 *   or more of the libraries normally distributed with (a)-(e) above.
 *
 *   Please note: This gives you special rights concerning the
 *   libraries which normally accompany the above pieces of software.
 *   It gives you no special rights concerning software that you write
 *   yourself.  You must obey the GNU General Public License in all
 *   respects for all of the code used other than the libraries
 *   normally distributed with (a)-(e) above.
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

char simple_bible_schema[] = 
"//\n"
"// simplebibleschema.mql - Simple bible schema\n"
"// Last update: March 11, 2006\n"
"//\n"
"CREATE ENUM \n"
"book_name_t = {\n"
"  Genesis,\n"
"  Exodus,\n"
"  Leviticus,\n"
"  Numbers,\n"
"  Deuteronomy,\n"
"  Joshua,\n"
"  Judges,\n"
"  Ruth,\n"
"  I_Samuel,\n"
"  II_Samuel,\n"
"  I_Kings,\n"
"  II_Kings,\n"
"  I_Chronicles,\n"
"  II_Chronicles,\n"
"  Ezra,\n"
"  Nehemiah,\n"
"  Esther,\n"
"  Job,\n"
"  Psalms,\n"
"  Proverbs,\n"
"  Ecclesiastes,\n"
"  Canticles,\n"
"  Isaiah,\n"
"  Jeremiah,\n"
"  Lamentations,\n"
"  Ezekiel,\n"
"  Daniel,\n"
"  Hosea,\n"
"  Joel,\n"
"  Amos,\n"
"  Obadiah,\n"
"  Jonah,\n"
"  Micah,\n"
"  Nahum,\n"
"  Habakkuk,\n"
"  Zephaniah,\n"
"  Haggai,\n"
"  Zechariah,\n"
"  Malachi,\n"
"  Matthew,\n"
"  Mark,\n"
"  Luke,\n"
"  John,\n"
"  Acts,\n"
"  Romans,\n"
"  I_Corinthians,\n"
"  II_Corinthians,\n"
"  Galatians,\n"
"  Ephesians,\n"
"  Philippians,\n"
"  Colossians,\n"
"  I_Thessalonians,\n"
"  II_Thessalonians,\n"
"  I_Timothy,\n"
"  II_Timothy,\n"
"  Titus,\n"
"  Philemon,\n"
"  Hebrews,\n"
"  James,\n"
"  I_Peter,\n"
"  II_Peter,\n"
"  I_John,\n"
"  II_John,\n"
"  III_John,\n"
"  Jude,\n"
"  Revelation,\n"
"  Baruch,\n"
"  Sirach,\n"
"  I_Esdras,\n"
"  II_Esdras,\n"
"  Judith,\n"
"  Epistle_to_the_Laodicaeans,\n"
"  I_Maccabees,\n"
"  II_Maccabees,\n"
"  III_Maccabees,\n"
"  IV_Maccabees,\n"
"  Prayer_of_Manasseh,\n"
"  Psalm_151,\n"
"  Tobit,\n"
"  Wisdom_of_Solomon,\n"
"  Susanna,\n"
"  Bel_and_the_Dragon,\n"
"  Prayer_of_Azariah, // Also Song of the three young men\n"
"  Psalms_of_Solomon,\n"
"  Odes,\n"
"  Additions_to_Esther,\n"
"  Psalms_Vulgata,\n"
"  Letter_of_Jeremiah\n"
"}\n"
"GO\n"
"\n"
"\n"
"\n"
"\n"
"CREATE OBJECT TYPE\n"
"WITH SINGLE RANGE OBJECTS\n"
"HAVING UNIQUE FIRST MONADS\n"
"[Book\n"
"  book : book_name_t;\n"
"]\n"
"GO\n"
"\n"
"\n"
"\n"
"CREATE OBJECT TYPE\n"
"WITH SINGLE RANGE OBJECTS\n"
"HAVING UNIQUE FIRST MONADS\n"
"[Chapter\n"
"  chapter : integer;\n"
"  book : book_name_t;\n"
"]\n"
"GO\n"
"\n"
"\n"
"\n"
"CREATE OBJECT TYPE\n"
"WITH SINGLE RANGE OBJECTS\n"
"HAVING UNIQUE FIRST MONADS\n"
"[Verse\n"
"  chapter : integer;\n"
"  verse : integer;\n"
"  book : book_name_t;\n"
"]\n"
"GO\n"
"\n"
"\n"
"CREATE OBJECT TYPE\n"
"WITH SINGLE MONAD OBJECTS\n"
"HAVING UNIQUE FIRST MONADS\n"
"[Word\n"
"  surface : STRING FROM SET WITH INDEX;\n"
"  stripped_surface : STRING FROM SET WITH INDEX;\n"
"]\n"
"GO\n"
"\n"
"\n"
;
