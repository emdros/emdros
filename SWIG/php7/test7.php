<?php
require('/usr/lib/emdros/EmdrosPHP.php');
/* require('/home/ulrikp/Ongoing/emdros/trunk/emdros/SWIG/php/EmdrosPHP.php'); */

echo "<pre>\n";

function execString(&$env, $query) {
	 $bPrintResult = true;
	 $bReportError = true;
	 $arrResult = $env->executeString($query, $bPrintResult, $bReportError);
	 $bDBOK = $arrResult[0];
	 $bCompilerOK = $arrResult[1];
	 return $bDBOK && $bCompilerOK;
}

$env = new EmdrosEnv(kOKConsole,
     kCSUTF8,
     "", // hostname, e.g., localhost
     "", // database user, e.g, emdf or ulrikp
     "", // database password, e.g., secrit
     "emdf", // initial database
     kSQLite3); // database kind

echo "connectionOk() = " . $env->connectionOk() . "\n";
echo "getBackendName() = " . $env->getBackendName() . "\n";

if (!execString($env, "DROP DATABASE 'phpemdrostest'GO")) {
   ; // Do nothing. If it doesn't exist, execString will return false, but that doesn't matter.
}


$query = "CREATE DATABASE 'phpemdrostest' GO\n" .
       "USE DATABASE 'phpemdrostest' GO\n" .
       "CREATE OBJECT TYPE WITH SINGLE MONAD OBJECTS\n" .
       "[token surface : STRING;]GO\n" .
       "CREATE OBJECTS WITH OBJECT TYPE [token]\n" .
       "CREATE OBJECT FROM MONADS = { 1 } WITH ID_D = 1 [surface := 'The';]" .
       "CREATE OBJECT FROM MONADS = { 2 } WITH ID_D = 2 [surface := 'door';]" .
       "CREATE OBJECT FROM MONADS = { 3 } WITH ID_D = 3 [surface := 'was';]" .
       "CREATE OBJECT FROM MONADS = { 4 } WITH ID_D = 4 [surface := 'blue.';]";

if (!execString($env, $query)) {
   die("Emdros error");
}


if (!execString($env, "SELECT ALL OBJECTS IN {1-10} WHERE [Token GET surface]")) {
   die("Emdros error");
}

$mySheaf = $env->takeOverSheaf();

$mySheafConstIterator = $mySheaf->const_iterator();

while ($mySheafConstIterator->hasNext()) {
      $myStraw = $mySheafConstIterator->next();
      $myStrawConstIterator = $myStraw->const_iterator();
      while ($myStrawConstIterator->hasNext()) {
      	    $myMatchedObject = $myStrawConstIterator->next();
	    
	    $monads = $myMatchedObject->getMonads();
	    
	    $feature = $myMatchedObject->getFeatureAsString(0); // First feature

	    echo "first_monad = " . $monads->first() . " feature = '" . $feature . "'\n";
      }
}

if (!execString($env, "GET OBJECTS HAVING MONADS IN {1-10} [Token GET surface]")) {
   die("Emdros error");
}

$myFlatSheaf = $env->takeOverFlatSheaf();

$myFlatSheafConstIterator = $myFlatSheaf->const_iterator();

while ($myFlatSheafConstIterator->hasNext()) {
      $myFlatStraw = $myFlatSheafConstIterator->next();
      $myFlatStrawConstIterator = $myFlatStraw->const_iterator();
      while ($myFlatStrawConstIterator->hasNext()) {
      	    $myMatchedObject = $myFlatStrawConstIterator->next();
	    
	    $monads = $myMatchedObject->getMonads();
	    
	    $feature = $myMatchedObject->getFeatureAsString(0); // First feature

	    echo "first_monad = " . $monads->first() . " feature = '" . $feature . "'\n";
      }
}

echo "</pre>\n";

?>