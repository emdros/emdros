REM
REM Use the signtool.exe from Microsoft Windows 7 SDK 7.1.
REM
signtool sign /tr http://timestamp.digicert.com/ /td sha256 /fd sha256 /a %1 %2 %3 %4 %5 %6 %7
