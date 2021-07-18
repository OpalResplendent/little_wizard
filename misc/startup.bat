@echo off

set ProjectName=littlewiz
set ProjectDirectory=C:\DEV\PROJ\littlewiz

REM 4coder sublime
set Editor=4coder
set StartupFiles="%ProjectDirectory%\code\%ProjectName%.cpp" "%ProjectDirectory%\code\win32_%ProjectName%.cpp"

cd %ProjectDirectory%

REM subst /d v:
REM subst v: %ProjectDirectory%

call %ProjectDirectory%\misc\shell.bat x64
call %ProjectDirectory%\misc\editor.bat %StartupFiles%
REM call %ProjectDirectory%\misc\4ed.bat %StartupFileA% %StartupFileB%
