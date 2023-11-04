// Use the correct path to the 'Emdros.node' module.
// For example:

//var Emdros = require("/usr/lib/emdros/Emdros")
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

//
// Test harvest library
//
json_string = `
{
   "fetchinfo" : {
      "base" : {
         "object_types" : {
            "token" : {
               "get" : ["surface"],
               "start" : "{{ feature 0 }}",
               "end" : " "
            }
         },
         "priority_list" : [
            "token"
         ],
         "postprocess" : [
            { "prefix" : "<sentence>" },
            { "suffix" : "</sentence>" }
         ],
         "prepend_XML_declaration" : true
      }
   }
}
`;

// Use Emdros' JSON implementation to parse the json_string into a
// Emdros.JSONValue value.
var error_message = "";
var pFetchInfo;
parse_output = Emdros.readAndParseJSONFromString(json_string);
[pFetchInfo, error_message] = parse_output;

console.log("UP200: parse_output should be a list, because the error_message parameter is an output parameter.");
console.log("UP201: parse_output = ", parse_output);

console.log("error_message = '%s'\n", error_message);

console.log("UP210");

// Pretty-print the JSON value
var bEscapeAsUnicode = false;
var json_string_pretty_printed = pFetchInfo.castToString(bEscapeAsUnicode);
console.log("pFetchInfo's castToString() method returns:", json_string_pretty_printed);

//
// Now render the document
//

//
// The db_name parameter to render_objects does not need to be the
// same name as the database name. This value is used inside the
// rendering mechanism, e.g., witht he {{ dbname }} template, which
// emits this value.
//
var db_name = "node_emdros_test";

// We use the stylesheet named 'base'
var stylesheet = "base";

// The monad range to query
var first_monad = 1;
var last_monad = 10;

// Make placeholder variables for output
var rendered_doc = "";
var bResult = false;

// The function returns an array
var render_result_array = Emdros.render_objects(env, db_name, pFetchInfo, stylesheet, first_monad, last_monad, bResult);

// Unpack the array.
[rendered_doc, bResult] = render_result_array


console.log("render_objects result: ", bResult);
console.log("render_objects output:");
console.log(rendered_doc);
console.log("\n");


