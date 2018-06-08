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

char negra_schema[] = 
"CREATE OBJECT TYPE\n"
"WITH SINGLE MONAD OBJECTS\n"
"HAVING UNIQUE FIRST MONADS\n"
"[Word\n"
"    surface : STRING WITH INDEX;\n"
"    postag : STRING FROM SET WITH INDEX;\n"
"    morphtag : STRING FROM SET WITH INDEX;\n"
"    edge : STRING WITH INDEX;\n"
"    parent : ID_D;\n"
"    parents : LIST OF ID_D;\n"
"    secedge1 : STRING FROM SET WITH INDEX;\n"
"    secparent1 : ID_D;\n"
"    secedge2 : STRING FROM SET WITH INDEX;\n"
"    secparent2 : ID_D;\n"
"    secedge3 : STRING FROM SET WITH INDEX;\n"
"    secparent3 : ID_D;\n"
"    secedge4 : STRING FROM SET WITH INDEX;\n"
"    secparent4 : ID_D;\n"
"    secedge5 : STRING FROM SET WITH INDEX;\n"
"    secparent5 : ID_D;\n"
"]\n"
"GO\n"
"\n"
"\n"
"CREATE OBJECT TYPE\n"
"[Phrase\n"
"    tag : STRING FROM SET WITH INDEX;\n"
"    morphtag : STRING FROM SET WITH INDEX;\n"
"    edge : STRING FROM SET WITH INDEX;\n"
"    parent : ID_D;\n"
"    parents : LIST OF ID_D;\n"
"    secedge1 : STRING FROM SET WITH INDEX;\n"
"    secparent1 : ID_D;\n"
"    secedge2 : STRING FROM SET WITH INDEX;\n"
"    secparent2 : ID_D;\n"
"    secedge3 : STRING FROM SET WITH INDEX;\n"
"    secparent3 : ID_D;\n"
"    secedge4 : STRING FROM SET WITH INDEX;\n"
"    secparent4 : ID_D;\n"
"    secedge5 : STRING FROM SET WITH INDEX;\n"
"    secparent5 : ID_D;\n"
"]\n"
"GO\n"
"\n"
"CREATE OBJECT TYPE\n"
"WITH SINGLE RANGE OBJECTS\n"
"HAVING UNIQUE FIRST MONADS\n"
"[Sentence\n"
"    sentence_id : INTEGER;\n"
"    editor_id : STRING FROM SET WITH INDEX;\n"
"    date : INTEGER;\n"
"    origin_id : STRING WITH INDEX;\n"
"]\n"
"GO\n"
;
