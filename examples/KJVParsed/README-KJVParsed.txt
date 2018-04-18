What is this?
=============

This is the King James Version of the Bible, the New Testament, with
parse trees and part of speech tagging.

The parsing and part of speech tagging was done with the Charniak
parser by Mark Johnson and Eugene Charniak.  The parser file
downloaded was:

reranking-parserAug06.tar.gz

No attemp has been made to clean up the data, except for one thing: In
Luke chapter 3, a very long sentence occurs (548 words), which the
parser couldn't parse, even after raising the maximum sentence length
to 600.  This sentence has been removed from the base file for Luke
chapter 3.


The parse trees (and the base text) are in the Public Domain, and may
be used for any purpose.


Usage
=====

Creating the database anew
--------------------------

The database has already been created with an SQLite3 backend, as
"KJV.sqlite3".

To create the database anew: Run these two commands from the
command-line:

   mql -b 3 KJV_schema.mql
   mql -b 3 -d KJV KJV_data.mql

This will create an SQLite 3 database called 'KJV'.  If you wish to
use another backend, please consult the manual page for mql(1), to see
what options you have.

Running the Emdros Query Tool
-----------------------------

The Emdros Query Tool is a graphical tool for querying Emdros
databases.

On Linux and Unix, it is called "eqtu".  On Mac OS X, it is called
"EmdrosQueryTool".  On Windows, it is available in the "Applications"
folder.

To use the KJV.sqlite3 database with the Emdros Query Tool, follow
these steps:

1) Start the Emdros Query Tool.

2) Click on the first button on the toolbar from the left. It looks
somewhat like a white cylinder with a cheesy star on it.  This will
start a new connection.

3) In the "Connection settings" dialog box, choose "SQLite3" as the
backend.

4) Still in the "Connection settings" dialog box, click the "Browse"
button next to the "Configuration" text box.  Navigate to the KJVParse
directory (where the database is located), and open the
"EmdrosQueryTool-KJVParse.cfg" file.

5) Still in the "Connection settings" dialog box, click the "Browse"
button next to the "Database" text box.  Navigate to the KJVParse
directory and open the "KJV.sqlite3" database.

6) In the "Connection settings" dialog box, click "OK".  It should now
say "Successfully connected." in the lower left of the query tool
window.

You're now ready to run a query.  You can run the query, for example:

SELECT ALL OBJECTS
WHERE
[NP as np1 FOCUS
   [NP AS np2 parent=np1.self
     [Token surface="the" and parent=np2.self]
     [Token surface="grace" and parent=np2.self]
   ]
   [PP as pp1 parent=np1.self
     [Token surface="of" and parent=pp1.self]
     [NP as np3 parent=pp1.self
       [Token parent=np3.self]*{3-}
     ]
   ]
]
GO

Copy/paste this into the upper left part of the Query Tool, and click
the lightning button on the toolbar.  The results should appear in the
lower part of the left hand side of the query tool window.

Before running another query, you might want to wipe the results with
the white square button on the toolbar.

Sample queries
--------------

In the "queries" subdirectory, you will find a number of sample
queries.  In the Emdros Query Tool, you can open these by clicking
"File -> Open", or by clicking the "orange opening folder" button on
the toolbar.

Ulrik Sandborg-Petersen
Last update: May 12, 2014


