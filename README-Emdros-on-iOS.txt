Instructions for compiling Emdros for iOS
=========================================

Emdros works great on iOS.  In fact, it has been successfully deployed
to thousands of users in at least one app on the App store.

The following caveats apply:

- You cannot use SQLite2, MySQL, or PostgreSQL as backends.

- You should use the SQLite3 provided by the system.  See below for
  how this works.

- You will get the best performance, not to mention smaller
  on-the-device database footprints, if you license the BPT engine and
  use it as your backend.  Contact the author of Emdros, Ulrik
  Sandborg-Petersen, for more details if you are interested in hearing
  more about the BPT engine.


General procedure
-----------------

The general steps to compiling Emdros for iOS are:

a) In Xcode, create a new Framework/static library project, called
   something like Emdros.  You can also just elect to add the sources
   to your App's source code, the same as detailed below.

b) On you Mac, in a command line prompt, unpack, configure, and make
   Emdros (see below for the detailed instructions, which you should
   follow now *before* proceeding with step (c)).

c) In Xcode, add the following files to your Xcode project:

   - amalgamation/emdros_amalgamation.cpp
   - amalgamation/emdros_c_amalgamation.c
   - amalgamation/emdros.h
   - amalgamation/emdros-config.h
   - amalgamation/emdros-lconfig.h

d) Make sure that the Emdros framework depends on the libz library.

e) Still in Xcode, make sure the Emdros framework compiles.

f) If you created a Framework in step (a), add the "Emdros" Framework
   to your main project. #include "emdros.h" from the Framework in
   order to use Emdros's headers.

g) Write an Objective C++ wrapper which wraps the parts of Emdros
   which you need to access for use from Objective-C or Swift.  See
   below under "Strategies..." for hints on how to do this.


Making Emdros on the Mac (prerequisite)
---------------------------------------

On your Mac, do the following:

0) Install Xcode command line tools, if not installed already.

1) Somewhere on your file system (it doesn't matter much where), untar
   the Emdros sources:

   tar xfzv emdros-<version>.tar.gz

2) cd emdros-<version>

3) If you have the BPT engine sources, unpack them now:

   unzip bpt-X.Y.zip

   ("A" to overwrite all)

4) ./configure --without-mysql --without-postgresql --with-sqlite3=local --without-sqlite --with-pcre=local --without-wx --disable-shared

5) run the make command  (Still on the Mac, in the command line!)

   make

   Let it run until it finishes.

You are now ready to proceed with step (c) above.


Strategies for using Emdros from Objective-C and Swift
======================================================

Regardless of how you use Emdros on iOS, you'll need to wrap it in an
Objective-C++ (*.mm) wrapper in order to access it from Objective-C or
Swift.

Three strategies suggest themselves:

1) Write an Objective-C++ wrapper which wraps the parts of Emdros that
   you need.

   You'll most likely need to wrap:

   - EmdrosEnv
   - SetOfMonads, SOMConstIterator, and possibly FastSetOfMonads.
   - Sheaf, Straw, MatchedObject, SheafConstIterator, StrawConstIterator.
   - FlatSheaf, FlatStraw, FlatSheafConstIterator, FlatStrawConstIterator.
   - Plus any others you will need for your purposes.

2) Write a "business logic" layer in C++, which talks directly to the
   domain-oblivious Emdros API, exposing its own, domain-dependent and
   domain-aware concepts that fit your Application domain.  Then wrap
   this C++ "business logic" layer in an Objective-C++ implementation
   for use from Objective-C or Swift.

3) Do the "business logic" implementation from (2), but instead of
   wrapping it in an Objective-C++ wrapper, wrap it for use with
   Node.JS (Cordova/PhoneGap), or React.

Ulrik has customers which, between them, do some form of all three
strategies.
