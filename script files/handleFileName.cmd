@echo on
rem setlocal EnableDelayedExpansion

goto start
    set var=%%a
		set subname=%var:*qp30_=%
		set subname=%subname:.mp4=%
		echo %subname%
		
for /f "tokens=1,2,3 delims=." %%a in (filelist.txt) do (
    echo %%a %%b %%c
    rename %%aqp30_%%b.%%c %%b_qp30.mp4
)	
	
:start

for /f "tokens=1,2 delims=." %%a in (filelist2.txt) do (
    ffmpeg -i %%a.%%b -vcodec copy %%a_atom.h264
)

