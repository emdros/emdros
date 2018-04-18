const emdros = require('./build/Release/emdrosnode');

// Local variables for convenience.
var SetOfMonads = emdros.SetOfMonads;

// Construct a new object
var som1 = new SetOfMonads(1,3);

// Call a method
som1.add(7,9);

// Will print " { 1-3, 7-9 } "
console.log(som1.toString());



