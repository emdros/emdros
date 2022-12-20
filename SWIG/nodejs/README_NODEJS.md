README for Emdros SWIG Node.js
Created: 8/17-2013
Last update: 12/20-2022

# Introduction

This document explains how to use the SWIG PHP8 bindings for Emdros.

This document is still sketchy.


# How to use

There is an example in /usr/share/doc/emdros/test_Emdros_Node.js


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


