@echo off



set vardate=%date:~0,4%-%date:~5,2%-%date:~8,2%

set vartime=%time:~0,2%-%time:~3,2%-%time:~6,2%



set varproductdir=AlphaRobot1s_QT
set varproductshortname=%varproductdir%_setup
set varproductname=%varproductshortname%.exe
set rarpath=..\trd\rar\rar.exe


if "%vartime:~0,1%"==" " set vartime=0%time:~1,1%-%time:~3,2%-%time:~6,2%



setlocal enabledelayedexpansion

set PWD=%~p0

set PWD=%PWD:~1,-1%


for /l %%a in (1,1,100) do (
   if "!PWD:~%%a,1!"=="\" set i=%%a
)


set /a i+=1

set PWD=!PWD:~%i%!


set varfile=%varproductshortname%_%vardate%_%vartime%.zip
set varrar=%rarpath%
set varfilelist=%varproductname%


echo create back file %varfile%


if not exist %varproductname% goto EXIT_BAT



if exist %varfile% goto BACK_FILE_EXIST




:FILE_BACKUP


%varrar% A "%varfile%" %varfilelist%



echo %cd%\%varfile%

echo back file end!


goto EXIT_BAT



:BACK_FILE_EXIST


set /P OK= dest file %varfile% is exist£¬override£¿(Y/N): 
IF /I "%OK:~0,1%"=="Y" (

	del /f /s /q %varfile%
	goto FILE_BACKUP

) ELSE (

	goto EXIT_BAT

)



:EXIT_BAT


@echo on
