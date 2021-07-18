@echo off

REM Build file for littlewizdata game tool

REM Compiler options
set BuildStyle=-Od -W4 -WX -Oi -Zo -Z7

set Includes=-IC:\DEV\lib\stb -IC:\DEV\lib\

set AlwaysIgnore=-wd4201 -wd4244 -wd4100
set WarningSurpression=-wd4189 -wd4505 -wd4127 -wd4267 -wd4313 -wd4996 -wd4700 -wd4701 -wd4703 -wd4800 -wd4456 %AlwaysIgnore%

set CompilerFlags=%BuildStyle% -nologo -Gm- -GR- -EHa- -fp:except- -fp:fast -FC -WL %WarningSurpression% %Includes% -EHsc

REM Linker options
set LinkerLibraries=kernel32.lib user32.lib
set LinkerFlags=-incremental:no -opt:ref %LinkerLibraries%

IF NOT EXIST %ProjectDirectory%\build mkdir %ProjectDirectory%\build
pushd %ProjectDirectory%\build

cl %CompilerFlags% %ProjectDirectory%\code\littlewizdata.cpp /link %LinkerFlags%

set EXITCODE=%ERRORLEVEL%

popd
EXIT /B %EXITCODE%
