@echo off

setlocal
set RUNINSTANCES=%1
IF NOT [%1]==[] (
	IF [%1] GTR [4] (
		echo Too many! Max 4, setting to 1.
		set RUNINSTANCES=1
	)
) else (
	set RUNINSTANCES=1
)

pushd %ProjectDirectory%\build

for /l %%x in (1, 1, %RUNINSTANCES%) do (
	start %ProjectName%.exe
)
popd

:END
endlocal