@echo off

pushd %ProjectDirectory%\code

REM f # font size, W fullscreenwindowd, F fullscreenborderless
if %Editor% == 4coder (start C:\DEV\4coder\4ed.exe -W %*)

if %Editor% == sublime (SET PYTHONPATH=)
if %Editor% == sublime (start C:\DEV\SublimeText3\sublime_text.exe %*)

popd
