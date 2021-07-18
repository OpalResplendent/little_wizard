@echo off

REM call devenv "%ProjectDirectory%\exec\%ProjectName%.exe" "%ProjectDirectory%\code\%ProjectName%.cpp" %*

call C:\DEV\CDbg\cdbg64.exe %ProjectDirectory%\build\%ProjectName%.exe
REM call C:\DEV\CDbg_0.7\cdbg64.exe %ProjectDirectory%\exec\%ProjectName%.exe
