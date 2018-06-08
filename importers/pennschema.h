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

char penn_treebank_schema[] = 
"//\n"
"// Penn Treebank Schema for Emdros 1.2.0.pre199 or later\n"
"// Author: Ulrik Sandborg-Petersen\n"
"// Last update: 4/27-2013\n"
"//\n"
"\n"
"CREATE OBJECT TYPE\n"
"WITH SINGLE RANGE OBJECTS\n"
"HAVING UNIQUE FIRST MONADS\n"
"[Document\n"
"   docid : INTEGER;\n"
"]\n"
"GO\n"
"\n"
"\n"
"CREATE OBJECT TYPE\n"
"WITH SINGLE RANGE OBJECTS\n"
"HAVING UNIQUE FIRST MONADS\n"
"[DocumentRoot\n"
"   docid : INTEGER;\n"
"   parent : INTEGER; // Points to the document of which it is a part\n"
"]\n"
"GO\n"
"\n"
"\n"
"CREATE OBJECT TYPE\n"
"WITH SINGLE RANGE OBJECTS\n"
"[Nonterminal\n"
"   docid : INTEGER;\n"
"   parent : INTEGER; \n"
"   coref : LIST OF INTEGER; // Coreference links\n"
"   nttype : STRING FROM SET WITH INDEX; // First part of nonterminal name\n"
"   function : STRING FROM SET WITH INDEX; // The rest of the nonterminal name (except for coreference pointer)\n"
"]\n"
"GO\n"
"\n"
"CREATE OBJECT TYPE\n"
"WITH SINGLE MONAD OBJECTS\n"
"HAVING UNIQUE FIRST MONADS\n"
"[Token\n"
"   docid : INTEGER;\n"
"   parent : INTEGER;\n"
"   coref : LIST OF INTEGER;\n"
"   surface : STRING WITH INDEX; // Do not use FROM SET here, as it will likely slow things down, due to the high cardinality of \"surface\".\n"
"   mytype : STRING FROM SET WITH INDEX; // First part of terminal name\n"
"   function : STRING FROM SET WITH INDEX; // The rest of the terminal name (except for coreference pointer)\n"
"]\n"
"GO\n"
"\n"
"// Drop all indexes, so that importing takes a shorter amount of\n"
"// time.  \n"
"DROP INDEXES ON OBJECT TYPES [All] GO\n"
"\n"
;

char penn_treebank_id_d_schema[] = 
"//\n"
"// Penn Treebank Schema for Emdros 1.2.0.pre199 or later\n"
"// Author: Ulrik Sandborg-Petersen\n"
"// Last update: 7/11-2013\n"
"//\n"
"\n"
"CREATE OBJECT TYPE\n"
"WITH SINGLE RANGE OBJECTS\n"
"HAVING UNIQUE FIRST MONADS\n"
"[Document\n"
"]\n"
"GO\n"
"\n"
"\n"
"CREATE OBJECT TYPE\n"
"WITH SINGLE RANGE OBJECTS\n"
"HAVING UNIQUE FIRST MONADS\n"
"[DocumentRoot\n"
"   parent : ID_D; // Points to the document of which it is a part\n"
"]\n"
"GO\n"
"\n"
"\n"
"CREATE OBJECT TYPE\n"
"WITH SINGLE RANGE OBJECTS\n"
"[Nonterminal\n"
"   parent : ID_D;\n"
"   coref : LIST OF ID_D; // Coreference links\n"
"   nttype : STRING FROM SET WITH INDEX; // First part of nonterminal name\n"
"   function : STRING FROM SET WITH INDEX; // The rest of the nonterminal name (except for coreference pointer)\n"
"]\n"
"GO\n"
"\n"
"CREATE OBJECT TYPE\n"
"WITH SINGLE MONAD OBJECTS\n"
"HAVING UNIQUE FIRST MONADS\n"
"[Token\n"
"   parent : ID_D;\n"
"   coref : LIST OF ID_D;\n"
"   surface : STRING WITH INDEX; // Do not use FROM SET here, as it will likely slow things down, due to the high cardinality of \"surface\".\n"
"   mytype : STRING FROM SET WITH INDEX; // First part of terminal name\n"
"   function : STRING FROM SET WITH INDEX; // The rest of the terminal name (except for coreference pointer)\n"
"]\n"
"GO\n"
"\n"
"// Drop all indexes, so that importing takes a shorter amount of\n"
"// time.  \n"
"DROP INDEXES ON OBJECT TYPES [All] GO\n"
"\n"
;
