/*
 * bptdumper.cpp
 *
 * Classes and methods to dump another Emdros database into a Bit
 * Packed Table
 *
 * Ulrik Petersen
 * Created: 3/2-2010
 * Last update: 4/3-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2010-2018  Ulrik Sandborg-Petersen
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

#include <bptdumper.h>
#include <environment_emdros.h>


bool BPTdumpAsBPT(EmdrosEnv *pSourceEE, const std::string& bptfilename, const std::string& bptkey, std::string& /* out */ error_message, long schema_version, const std::string& payload_filename, const std::string& payload_inkey, const std::string& payload_codec)
{
	UNUSED(pSourceEE);
	UNUSED(bptfilename);
	UNUSED(bptkey);
	UNUSED(error_message);
	UNUSED(schema_version);
	UNUSED(payload_filename);
	UNUSED(payload_inkey);
	UNUSED(payload_codec);
	
	error_message = "Sorry, the BPT engine is not available as Open Source.\n"
		"Please contact Ulrik Sandborg-Petersen, <ulrikp{at}emergence[dot]dk>,\n"
		"for inquiries concerning licensing.\nThank you.\n\n";
	return false;
}


