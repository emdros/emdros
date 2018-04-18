README for MQL
Ulrik Petersen
Created: 3/1-2001 (1st of March, 2001)
Last update: 8/6-2004

Introduction
============

This README documents one aspect of the MQL code, namely return values
and the bResult parameter.


Return values and bResult for Weed, Symbol, etc.
================================================

The functions Weed, Symbol, Type, Monads, Exec(ute), and their helper
functions all return bool (except weed) and have a parameter, bResult.

The interpretation of the return value is this: If it is true, it
means that the method succeeded with no database errors.  If it is
false, it means that some database error occurred.  Examine pDB to
find out what went wrong.

The interpretation of the bResult parameter is this: If it is true, it
means that the method's purpose (weed, symbol, etc.) succeeded.  For
example, if bResult is true after a weed, it means that there were no
weeding errors.  If bResult is false, however, then there was an
error.

The weed functions don't need to return a value, since they don't
manipulate the database.
