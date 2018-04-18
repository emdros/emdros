..\win32\Release\emdftry.exe -b 3
IF /I "%ERRORLEVEL%" NEQ "0" GOTO fail

..\win32\Release\mqltry.exe -b 3
IF /I "%ERRORLEVEL%" NEQ "0" GOTO fail

..\win32\Release\mqllingtry.exe -b 3
IF /I "%ERRORLEVEL%" NEQ "0" GOTO fail

..\win32\Release\bpttry.exe -b 3
IF /I "%ERRORLEVEL%" NEQ "0" GOTO fail

GOTO success

:fail
ECHO FAILURE
EXIT /B 1

:success
ECHO SUCCESS
EXIT /B 0

