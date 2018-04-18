README for Emdros on Win32 using MySQL
Ulrik Petersen
Created: 4/16-2005
Last update: 7/27-2006


You need libmysql.dll
=====================

In order for the Emdros MySQL Win32 binaries to work, you must have
libmysql.dll in your PATH.  libmysql.dll is usually located in:

%MYSQLPREFIX%\lib\opt\libmysql.dll

where MYSQLPREFIX is something like

C:\Program Files\MySQL\MySQL Server 5.0

You can copy this file either to your C:\WINDOWS\System32 directory,
or to the directory where the Emdros binaries reside.

This is normally distributed with Emdros.  The sources to MySQL are
available from the download section:

http://emdros.org/download.html


Ulrik Petersen

