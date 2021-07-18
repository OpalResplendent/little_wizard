@echo off

REM Build file for littlewizdata game tool

set UNICODE=1

REM Compiler options
set BuildStyle=-Od -W4 -WX -Oi -Zo -Z7
IF DEFINED UNICODE set BuildStyle=%BuildStyle% -DUNICODE -D_UNICODE

set Includes=-IC:\DEV\lib\stb

set AlwaysIgnore=-wd4201 -wd4244 -wd4100
set WarningSurpression=-wd4189 -wd4505 -wd4127 -wd4267 -wd4313 -wd4996 -wd4700 -wd4701 -wd4703 -wd4800 -wd4456 %AlwaysIgnore%

set CompilerFlags=%BuildStyle% -nologo -Gm- -GR- -EHa- -fp:except- -fp:fast -FC -WL %WarningSurpression% %Includes% -EHsc

REM Linker options
set LinkerLibraries=kernel32.lib user32.lib
set LinkerFlags=-incremental:no -opt:ref %LinkerLibraries%

IF NOT EXIST %ProjectDirectory%\exec mkdir %ProjectDirectory%\exec
pushd %ProjectDirectory%\exec

cl %CompilerFlags% %ProjectDirectory%\code\littlewizdata.cpp /link %LinkerFlags%

set EXITCODE=%ERRORLEVEL%

popd
EXIT /B %EXITCODE%
