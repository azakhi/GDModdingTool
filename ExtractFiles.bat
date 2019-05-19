@ECHO OFF
SET /p GamePath=Please enter game path (enter nothing if run from game folder):
CD %GamePath%
SET /p OutputPath=Please enter output path (enter nothing for default):
IF "%OutputPath%" EQU "" SET OutputPath=%cd%\database

IF EXIST database\templates.arc (
ECHO Extracting templates ..
ArchiveTool.exe database\templates.arc -extract "%cd%\database" > nul
) ELSE (
ECHO Error: Couldn't find templates.arc!
PAUSE
)

IF EXIST database\database.arz (
ECHO Extracting dbr files .. ^(This may take some time^)
ArchiveTool.exe database\database.arz -database "%OutputPath%" > nul
) ELSE (
ECHO Error: Couldn't find database.arz!
PAUSE
)

IF EXIST gdx1\database\GDX1.arz (
ECHO Extracting Expansion dbr files .. ^(This may take some time^)
ArchiveTool.exe gdx1\database\GDX1.arz -database "%OutputPath%" > nul
)

IF EXIST gdx2\database\GDX2.arz (
ECHO Extracting Expansion 2 dbr files .. ^(This may take some time^)
ArchiveTool.exe gdx2\database\GDX2.arz -database "%OutputPath%" > nul
)

ECHO Completed
PAUSE