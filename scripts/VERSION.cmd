@echo off
SETLOCAL ENABLEDELAYEDEXPANSION
REM  Script for generation of version info suitable for windows VERSION_INFO and application use
REM  Some of the ideas/implementaiton is based on GIT-VERSION-GEN.bat
REM  see https://github.com/Thell/git-vs-versioninfo-gen/blob/master/GIT-VS-VERSION-GEN.bat
REM
REM  Using git and a tracked file %DEFAULTS_FILE% and using user tags of the form [appid-]Major.Minor
REM  the following values are generated.  appid comes from the file %DEFAULTS_FILE% if it exists
REM  it may be null
REM
REM  GIT_VERSION: Major.Minor.Commits [qualifier]       where Major and Minor come from a user tag of the format
REM                                                     where qualifier is [.P | .X] [-branch] 
REM                                                       .P - using uncommited files
REM                                                       .X - externally tracked i.e. no git
REM                                                       -branch is present if branch is not master
REM  GIT_BUILDTYPE n                                    where n is
REM                                                     0 for normal build on master branch
REM                                                     1 for normal build not on master branch
REM                                                     2 for build using uncommited files
REM                                                     3 for build using untracked files 
REM  Note a windows resource version is also generated as Major,Minor,Commits,buildtype
REM  The tag used is chosen as highest parent version tag applicable to the current directory
REM  ** For some scenarios this may not always be the desired outcome, but for my usage it is fine **
REM  Commits is the count of commit applicable to this directory since the tag or when git was first used if no tag
REM  GIT_APPID:   Application name from file %DEFAULTS_FILE%  - omitted if no prefix
REM  GIT_SHA1:    SHA1 value of current commit or 'untracked' if outside git
REM  GIT_CTIME:   DATE & TIME of current commit (in UTC)
REM  GIT_CYEAR:   Year of current commit (useful for Copyright)
REM  GIT_APPDIR:  current directory, potentially useful for default app name
REM
REM  All generated values are strings, except for the windows resource value GIT_VERSION_RC
REM
REM  if git is not installed or applicable and %DEFAULTS_FILE% file exists, some of the information is extracted
REM  from this file. This is the only  instance where the qualifier '.X' is used

set DEFAULTS_FILE=version.in

:: --------------------
:CHECK_ARGS
:: --------------------

:: Console output only.
IF [%1] == [] GOTO START

IF "%~1" == "--help" GOTO USAGE
:optloop
IF "%~1" == "--quiet" SET fQUIET=1& SHIFT & goto :optloop
IF "%~1" == "--force" SET fFORCE=1& SHIFT & goto :optloop

IF EXIST %~1\NUL (
  :: %1 is a path
  SET CACHE_FILE=%~s1\GIT_VERSION_INFO
  SHIFT
)

IF [%~nx1] NEQ [] (
  :: %1 is a file
  SET HEADER_OUT_FILE=%~fs1
  SHIFT
)
:: This should always be the last
:: Some basic sanity checks.
IF DEFINED fQUIET (
  IF NOT DEFINED HEADER_OUT_FILE GOTO USAGE
)

IF DEFINED CACHE_FILE (
  SET CACHE_FILE=%CACHE_FILE:\\=\%
  IF NOT DEFINED HEADER_OUT_FILE GOTO USAGE
)
GOTO START

:: --------------------
:USAGE
:: --------------------
ECHO usage: [--help] ^| [--quiet] [--force] [CACHE_PATH OUT_FILE]
ECHO.
ECHO  When called without arguments version information writes to console.
ECHO.
ECHO  --help      - displays this output.
ECHO.
ECHO  --quiet     - Suppress console output.
ECHO  --force     - Ignore cached version information.
ECHO  CACHE_PATH  - Path for non-tracked file to store git version info used.
ECHO  OUT_FILE    - Path to writable file where the generated information is saved
ECHO.
ECHO  Example pre-build event:
ECHO  CALL $(SolutionDir)..\tools\VERSION.cmd "$(IntDir)" "$(ProjectDir)version.h"
ECHO.
GOTO :EOF


REM ===================
REM Entry Point
REM ===================
:START
CALL :LOAD_DEFAULTS
CALL :GET_VERSION_STRING
if [%GIT_SHA1%] == [] (
    if [%defGIT_SHA1%] == [] (
        echo No Git information and no "%DEFAULTS_FILE%" file
        exit /b 1
    ) else (
        if defined HEADER_OUT_FILE (
            echo copying %DEFAULTS_FILE% to %HEADER_OUT_FILE%
            copy /y/b "%DEFAULTS_FILE%" "%HEADER_OUT_FILE%" & exit /b 0
        )
        set GIT_SHA1=%defGIT_SHA1%
        set GIT_VERSION=%defGIT_VERSION%
        set GIT_BUILDTYPE=%defGIT_BUILDTYPE%
        set GIT_BRANCH=%defGIT_BRANCH%
        set curGMT=%defGIT_CTIME%
    )
)
if DEFINED CACHE_FILE CALL :CHECK_CACHE
if DEFINED fDONE exit /b 0
call :WRITE_OUT
exit /b 0



REM ====================
REM FUNCTIONS
REM ====================
:: --------------------
:LOAD_DEFAULTS
:: --------------------
:: load the defaults from the %DEFAULTS_FILE% file
:: line #define VAR VALUE gets a new variable defVAR with the VALUE (quotes are removed)
if exist %DEFAULTS_FILE% (
    for /f "tokens=1,2,*" %%A in (%DEFAULTS_FILE%) do if [%%A] == [#define] (call :setVar %%B %%C)
)
:: Application directory
for %%I in (%CD%) do set GIT_APPDIR=%%~nxI
GOTO :EOF

:: --------------------
:GET_VERSION_STRING
:: --------------------
set GIT_BUILDTYPE=0
:: Get which branch we are on and whether any outstanding commits in current tree
for /f "tokens=1,2 delims=. " %%A in ('"git status -s -b -uno -- . 2>NUL"') do (
    if [%%A] == [##] (
        set GIT_BRANCH=%%B
        if [%%B] neq [master] set GIT_BUILDTYPE=1
    ) else (
        if [%%A%%B] neq [] (
            set GIT_QUALIFIER=.P
            set GIT_BUILDTYPE=2
            goto :gotQualifier
        )
    )
)
:gotQualifier
:: error then no git or not in a repo (ERRORLEVEL not reliable)
IF not defined GIT_BRANCH goto :EOF

if [%GIT_BRANCH%] neq [master] set GIT_QUALIFIER=%GIT_QUALIFIER%-%GIT_BRANCH%
::
:: get the current SHA1 and commit time for items in this directory
for /f "tokens=1,2" %%A in ('git log -1 "--format=%%h %%ct" -- .') do (
    set GIT_SHA1=%%A
    call :gmTime GIT_CTIME %%B
)
if defined defGIT_APPID set strPREFIX=%defGIT_APPID%-
:: Use git tag to get the lastest tag applicable to the contents of this directory
for /f "tokens=1"  %%A in ('"git tag -l %strPREFIX%[0-9]*.*[0-9] --sort=-v:refname --merged %GIT_SHA1% 2>NUL"') do (
    set strTAG=%%A
    goto :haveTag
)
:haveTag

if [%strTAG%] neq [] set strFROM=%strTAG%..
:: get the commits in play
for /f "tokens=1" %%A in ('git rev-list --count %strFROM%HEAD -- .') do set GIT_COMMITS=%%A

:: remove any appid prefix
if defined defGIT_APPID set strTAG=%strTAG:*-=%
:: use 0.0 if no tag found
if [%strTAG%] == [] set strTAG=0.0
set GIT_VERSION_RC=%strTAG:.=,%,%GIT_COMMITS%,%GIT_BUILDTYPE%
set GIT_VERSION=%strTAG%.%GIT_COMMITS%%GIT_QUALIFIER%
goto :EOF


:: --------------------
:CHECK_CACHE
:: --------------------
:: Exit early if a cached git built version matches the current version.
IF EXIST "%HEADER_OUT_FILE%" (
    IF EXIST "%CACHE_FILE%" (
      if [%fFORCE%] == [1] goto :overwrite
      FOR /F "tokens=*" %%A IN (%CACHE_FILE%) DO (
        IF "%%A" == "%GIT_VERSION%-%GIT_SHA1%" (
          IF NOT DEFINED fQUIET (
            ECHO Build version is assumed unchanged from: %GIT_VERSION%.
          )
          SET fDONE=1
        )
      )
    )
)
:overwrite
ECHO %GIT_VERSION%-%GIT_SHA1%> "%CACHE_FILE%"

GOTO :EOF

:WRITE_OUT
:: -------
IF NOT DEFINED HEADER_OUT_FILE goto :CON_OUT
set GUARD=v%GIT_VERSION_RC:,=_%
:: create header file
ECHO // Autogenerated version file >"%HEADER_OUT_FILE%"
ECHO #ifndef %GUARD% >>"%HEADER_OUT_FILE%"
ECHO #define %GUARD% >>"%HEADER_OUT_FILE%"
if [%defGIT_APPID%] neq [] (
ECHO #define GIT_APPID       "%defGIT_APPID%" >>"%HEADER_OUT_FILE%"
)
ECHO #define GIT_VERSION     "%GIT_VERSION%" >>"%HEADER_OUT_FILE%"
ECHO #define GIT_VERSION_RC  %GIT_VERSION_RC% >>"%HEADER_OUT_FILE%"
ECHO #define GIT_SHA1        "%GIT_SHA1%" >>"%HEADER_OUT_FILE%"
echo #define GIT_BUILDTYPE   %GIT_BUILDTYPE% >>"%HEADER_OUT_FILE%"
ECHO #define GIT_APPDIR      "%GIT_APPDIR%" >>"%HEADER_OUT_FILE%"
ECHO #define GIT_CTIME       "%GIT_CTIME%" >>"%HEADER_OUT_FILE%"
ECHO #define GIT_YEAR        "%GIT_CTIME:~,4%" >>"%HEADER_OUT_FILE%"
ECHO #endif >>"%HEADER_OUT_FILE%"
:: --------------------
:CON_OUT
:: --------------------
IF DEFINED fQUIET GOTO :EOF
ECHO Git App Id:           %defGIT_APPID%
ECHO Git version:          %GIT_VERSION%
ECHO Build type:           %GIT_BUILDTYPE%
ECHO SHA1:                 %GIT_SHA1%
ECHO App dir:              %GIT_APPDIR%
ECHO Committed:            %GIT_CTIME%
GOTO :EOF


:setVar
    set _var=%1
    set _value=%2
    set def!_var!=!_value:"=!
goto :eof

:: convert unix time to gmt yyyy-mm-dd hh:mm:ss
:gmtime gmtstr utime
setlocal
set /a z=%2/86400+719468,d=z%%146097,y=^(d-d/1460+d/36525-d/146096^)/365,d-=365*y+y/4-y/100,m=^(5*d+2^)/153
set /a d-=^(153*m+2^)/5-1,y+=z/146097*400+m/11,m=^(m+2^)%%12+1
set /a h=%2/3600%%24,mi=%2%%3600/60,s=%2%%60
if %m% lss 10 set m=0%m%
if %d% lss 10 set d=0%d%
if %h% lss 10 set h=0%h%
if %mi% lss 10 set mi=0%mi%
if %s% lss 10 set s=0%s%
endlocal & set %1=%y%-%m%-%d% %h%:%mi%:%s%
goto :EOF
