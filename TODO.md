# TODO for Emdros release 4.0

## Documentation

- Fix README-Emdros-on-Android.txt
- Fix README-Emdros-on-iOS.txt
- Fix README.txt
- Fix INSTALL.Win32.txt
- Fix INSTALL.Mac.txt
- Read through all man pages, amend where necessary.
- Upgrade LyX docs from CreativeCommons 2.5 to latest CC license.
- Port MQL Programmer's Reference Guide to Restructured text.


## Build development
- Rename win32.mak -> windows.mak.
- macOS: HomeBrew
- Python: pip install emdros-py (2 & 3)
- Amalgamation: Reimplement mkamalgamation.py in C++, for Windows
  buildability without Python.
- Allow building from source repo without jar(1) installed (amend
  SWIG/java/Makefile.am)

## Continuous Integration

Set up Travis etc.

## Examples

Release separate emdros-examples package.

## Web

New website based on Jekyll or similar.

