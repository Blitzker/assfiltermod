@ECHO OFF
SETLOCAL

PUSHD "%~dp0"

SET nbHASH_PART=0000000
SET OLDVER=

:: check for git presence
CALL git log -1 HEAD >NUL 2>&1
IF ERRORLEVEL 1 (
    GOTO NOGIT
)

:: Get git short log
FOR /F "tokens=*" %%A IN ('"git log -1 HEAD --format=%%h"') DO (
  SET nbHASH_PART=%%A
)

:WRITE_VER

:: check if info changed, and write if needed
IF EXIST version_rev.h (
    SET /P OLDVER=<version_rev.h
)
SET NEWVER=#define ASSF_VERSION_HASH %nbHASH_PART%
IF NOT "%NEWVER%" == "%OLDVER%" (
    :: swapped order to avoid trailing newlines
    > version_rev.h ECHO %NEWVER%
)
GOTO :END

:NOGIT
echo Git not found
goto WRITE_VER

:END
POPD
ENDLOCAL
