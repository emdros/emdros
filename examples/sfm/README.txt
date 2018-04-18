README for Emdros SFM example
Ulrik Petersen

What is this?
=============

This is an example of how to use the SFM importer, sfmimport, in the
Emdros distribution.

How do I use it?
================

1) Run the following command at the command-line:

sfmimport -i schema.sfm test.sfm > objects.mql

This will produce an MQL file containing the objects in test.sfm.


2) Now run the following command:

sfmimport -i schema.sfm --schema --dbname gen > schema.mql

This will produce an MQL file containing the MQL schema described in
the file schema.sfm.


3) Import both into Emdros by running the following two commands:

   mql -p <db-password -u <db-user> --backend <your-backend> schema.mql
   mql -p <db-password -u <db-user> --backend <your-backend> objects.mql

This will give you a database called 'gen'.

However, your best course of action would be to study the relationship
between the schema.sfm and schema.mql files, and the relationship
between test.sfm and objects.mql files.  The relationships are
explained on the manual page 'sfmimport(1)'.  

On Unix/Linux, this should be availabe by typing 'man sfmimport' on
the command line.  

On Windows, it is availabl in the Documentation folder of the Emdros
folder in the Start-menu's "All Programs" folder.
