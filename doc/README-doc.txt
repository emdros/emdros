README for Emdros documentation
Created: 7/21-2002
Last update: 3/11-2006

Introduction
============

This README file explains what all the different files in the Emdros
documentation are.


More documentation
==================

There is a Programmer's Reference Guide:

http://emdros.org/progref/

(see below for more on this progref).


The query tool has a documentation here:

http://emdros.org/qrytool/




Text file documentation
=======================

bootstrapping.txt           : Information on how to set up your database
                              back-end for use with Emdros.
ChangeLog-doc.txt           : Changes made to the documentation
emdftry.1                   : Manual page for the tests/emdftry program.
emdftry.html                : Same in html format.
emdros.1                    : Overall manual page for Emdros
emdros.html                 : Overall manual page for Emdros
eqt.1                       : Manual page for the Emdros Query Tool.
eqt.html                    : Manual page for the Emdros Query Tool.
eqtc.1                      : Manual page for the Emdros Query Tool.
eqtc.html                   : Manual page for the Emdros Query Tool.
eqtu.1                      : Manual page for the Emdros Query Tool.
eqtu.html                   : Manual page for the Emdros Query Tool.
HOW-TO-USE.txt              : Hints on getting started with Emdros.
manage_indices.1            : Manual page for the src/manage_indices program.
manage_indices.html         : Same in html format.
mql.1                       : Manual page for the src/mql program.
mql.html                    : Same in html format.
mqldump.1                   : Manual page for the src/mqldump program.
mqldump.html                : Same in html format.
mqlhal.1                    : Manual page for the src/mqlhal program.
mqlhal.html                 : Same in html format.
MQL-README.txt              : Minor note for developers on the function 
                              signatures of the MQL engine.
mqltry.1                    : Manual page for the tests/mqltry program.
mqltry.html                 : Same in html format.
mqllingtry.1                : Manual page for the tests/mqllingtry program.
mqllingtry.html             : Same in html format.
negraimport.1               : Manual page for src/pennimport program.
negraimport.html            : Same in html format.
pennimport.1                : Manual page for src/pennimport program.
pennimport.html             : Same in html format.
NEWS-doc.txt                : Release notes for the documentation
README-doc.txt              : This file.
upgrade_db.1                : Manual page for the src/upgrade_db program.
upgrade_db.html             : Same in html format.
xinetd.mql                  : Sample xinetd configuration file for the
                              mql program to be run as a daemon.



PostScript/PDF-documentation
============================

The printable documentation is offered in PDF format.  It is split up
into two kinds, namely user documentation and implementation
documentation.

The first kind is documentation that you as a developer using Emdros
may find interesting, as well as documentation for your end-users.

The second kind, implementation documentation, is only useful if you
want to know how Emdros functions internally.  It is mainly written as
working-documents for the author of Emdros.


User documentation
------------------

Information which the user of Emdros (either the developer or their
end-users) may find interesting.

- MQL-User.pdf: "MQL User's Guide". Gives a detailed description and
explanation of MQL for users.

- Standard-MdF.pdf: "The standard MdF model". Gives an introduction to
the standard MdF model, on which the EMdF model is based.

- New-EMdF.pdf: "The Extended MdF model". Gives a definition of the
EMdF model.

- Monad-Sets.pdf: "Monad Sets -- Implementation and Mathematical
Foundations".  Gives an introduction to an efficient way of
implementing monad sets.  You may wish to use this if you wish to
implement monad sets in a language other than C++.


Implementation documentation
----------------------------

Documenting the internals of Emdros.

- MQL-Query-subset.pdf: "MQL Query subset". Gives detailed information
on the semantics and implementation of the MQL query language.

- Rel-EMdF.pdf: "Relational implementation of EMdF and MQL". A working
paper on how to store and retrieve EMdF data in and from a relational
database.


Programmer's Reference Guide
============================

The Emdros Programmer's Reference Guide aims to get you up to speed
quickly on how to program for Emdros.  Available online at:

http://emdros.org/progref/                   -- for stable version

This is for the stable version of Emdros.  If there is a preview
version, and it merits a different progref, it will have a
Programmer's Reference Guide here:

http://emdros.org/preview/progref/           -- for preview version
