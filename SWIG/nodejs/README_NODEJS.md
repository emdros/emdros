README for Emdros SWIG Node.js
Created: 8/17-2013
Last update: 12/20-2022

# Introduction

This document explains how to use the SWIG PHP8 bindings for Emdros.

This document is still sketchy.


# How to use

Here is an example:

```
var Emdros = require("./build/Release/Emdros")

function execString(env, query) {
    var bCompilerOK = true;
    var bPrintResult = true;
    var bReportError = true;
    var bCompilerOK = true;
    console.log("query = '%s'", query);
    var arrOutput = env.executeString(query, bCompilerOK, bPrintResult, bReportError);
    [bDBOK, bCompilerOK] = arrOutput;
    
    console.log("bDBOK = %s  bCompilerOK = %s", bDBOK, bCompilerOK);
    return bDBOK && bCompilerOK;
}


var env = new Emdros.EmdrosEnv(Emdros.kOKConsole,
     Emdros.kCSUTF8,
     "", // hostname, e.g., localhost
     "", // database user, e.g, emdf or ulrikp
     "", // database password, e.g., secrit
     "emdf", // initial database
     Emdros.kSQLite3); // database kind

console.log("connectionOk() = %s\n", env.connectionOk());
console.log("getBackendName() = %s\n", env.getBackendName());

if (!execString(env, "DROP DATABASE 'node_emdros_test.sqlite3' GO")) {
   ; // Do nothing. If it doesn't exist, execString will return false, but that doesn't matter.
}


query = "CREATE DATABASE 'node_emdros_test.sqlite3' GO\n" +
       "USE DATABASE 'node_emdros_test.sqlite3' GO\n" +
       "CREATE OBJECT TYPE WITH SINGLE MONAD OBJECTS\n" +
       "[token surface : STRING;]GO\n" +
       "CREATE OBJECTS WITH OBJECT TYPE [token]\n" +
       "CREATE OBJECT FROM MONADS = { 1 } WITH ID_D = 1 [surface := 'The';]" +
       "CREATE OBJECT FROM MONADS = { 2 } WITH ID_D = 2 [surface := 'door';]" +
       "CREATE OBJECT FROM MONADS = { 3 } WITH ID_D = 3 [surface := 'was';]" +
       "CREATE OBJECT FROM MONADS = { 4 } WITH ID_D = 4 [surface := 'blue.';]";

if (!execString(env, query)) {
   throw new Error("Emdros error");
}


if (!execString(env, "SELECT ALL OBJECTS IN {1-10} WHERE [Token GET surface]")) {
   throw new Error("Emdros error");
}

mySheaf = env.takeOverSheaf();

mySheafConstIterator = mySheaf.const_iterator();

while (mySheafConstIterator.hasNext()) {
      myStraw = mySheafConstIterator.next();
      myStrawConstIterator = myStraw.const_iterator();
      while (myStrawConstIterator.hasNext()) {
      	    myMatchedObject = myStrawConstIterator.next();
	    
	    monads = myMatchedObject.getMonads();
	    
	    feature = myMatchedObject.getFeatureAsString(0); // First feature

	  console.log("first_monad = %d feature = '%s'\n", monads.first(), feature);
      }
}

if (!execString(env, "GET OBJECTS HAVING MONADS IN {1-10} [Token GET surface]")) {
   throw new Error("Emdros error");
}

myFlatSheaf = env.takeOverFlatSheaf();

myFlatSheafConstIterator = myFlatSheaf.const_iterator();

while (myFlatSheafConstIterator.hasNext()) {
      myFlatStraw = myFlatSheafConstIterator.next();
      myFlatStrawConstIterator = myFlatStraw.const_iterator();
      while (myFlatStrawConstIterator.hasNext()) {
      	    myMatchedObject = myFlatStrawConstIterator.next();
	    
	    monads = myMatchedObject.getMonads();
	    
	    feature = myMatchedObject.getFeatureAsString(0); // First feature

	    console.log("first_monad = %d feature = '%s'\n", monads.first(), feature);
      }
}


```


# Things to be aware of

## bool& and long& parameters

Many functions in the C++ API take a bool& or a long& parameter. These
parameters are used to return a value.

In the JavaScript bindings, the return type is converted to a list of
values. The first entry will be the return value from the
function. The rest will be the values of the bool& and long&
parameters upon exit.

For example, this C++ function:

```
  bool executeString(const std::string& input, bool& bResult, 
                      bool bPrintResult, bool bReportError);
```

must be called like this:

```
    var bCompilerOK = true;
    var bPrintResult = true;
    var bReportError = true;
    var bCompilerOK = true;

    var arrOutput = env.executeString(query, bCompilerOK, bPrintResult, bReportError);
    [bDBOK, bCompilerOK] = arrOutput;
```


# Communication welcome

Please write to me if you have any further insights, or if something
doesn't work.


Ulrik Sandborg-Petersen

