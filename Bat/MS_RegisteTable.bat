@echo  OFF
@echo.
@echo  Copyright (c) 2016-2100.   jielong.lin.    All rights reserved.
@echo.
@echo. Set Local Area Network (LAN) Settings For TPV Corporation in XM.

CLS

MODE con: COLS=100 lines=40

color 9d


echo.
echo.
echo.

GOTO MENU

:MENU

ECHO.
ECHO. ===== Local Network Area Settings =====
ECHO.
ECHO. ID : Description
ECHO. ---------------------
ECHO. 1  : TPV - Wan Proxy: (USA VPN)  Address is 172.20.30.1:3128
ECHO.
ECHO. 2  : TPV - Lan
ECHO.
ECHO. 3  : Test
ECHO.
ECHO. =======================================
ECHO.
set /p ID=Please Input Selected Item ID:  

if "%id%"=="1" goto cmd1

if "%id%"=="2" goto cmd2

if "%id%"=="3" goto cmd3

exit

:cmd3:
setlocal ENABLEEXTENSIONS
set KEY_NAME="HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings\Connections"
set ItemName=test
FOR /F "usebackq skip=1 tokens=1-3" %%A IN (`REG QUERY %KEY_NAME% /v %ItemName% 2^>nul`) DO (
    set ItemName=%%A
    set ItemType=%%B
    set ItemValue=%%C
)
set ValuePrev=%ItemValue:~0,2%
set ValueSettings=%ItemValue:~2,2%
set ValuePost=%ItemValue:~4%

echo ValuePrev=%ValuePrev%
echo ValueSettings=%ValueSettings%
echo ValuePost=%ValuePost%

setlocal EnableDelayedExpansion
set ValueSettings=89

@reg add %KEY_NAME% /v %ItemName% /t %ItemType% /d "%ValuePrev%!ValueSettings!%ValuePost%" /f

pause

exit

:cmd2:
ECHO.  Flushing DNS
ipconfig /flushdns


ECHO.  Showwing New Settings
ipconfig /all


ECHO.  Closing Internet Explorer
taskkill  /f /t /im  IEXPLORE.exe

ECHO.
ECHO.
ECHO.
ECHO. Starting...TPV - Lan
ECHO.

setlocal ENABLEEXTENSIONS
set KEY_NAME="HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings"
set ItemName=AutoConfigURL
FOR /F "usebackq skip=1 tokens=1-3" %%A IN (`REG QUERY %KEY_NAME% /v %ItemName% 2^>nul`) DO (
    set ItemName=%%A
    set ItemType=%%B
    set ItemValue=%%C
)
@ECHO.
@ECHO.RegistedTable
@ECHO.^[%KEY_NAME%^]
@ECHO.  ^.ItemName: %ItemName%
@ECHO.  ^.ItemType: %ItemType%
@ECHO.  ^.ItemValue: ^(as follows^)
@ECHO.%ItemValue%
@ECHO.
IF NOT "%ItemValue%"=="http://paccn.tpvaoc.com:8080/proxy.pac" (
    setlocal EnableDelayedExpansion
    IF "%ItemType%"=="" (set ItemType=REG_SZ)
    reg add %KEY_NAME% /v %ItemName% /t !ItemType! /d "http://paccn.tpvaoc.com:8080/proxy.pac" /f
    @ECHO.^>^>^>The value of "%ItemName%" is Changed to "http://paccn.tpvaoc.com:8080/proxy.pac"
) else (
    @ECHO.^>^>^>The value of "%ItemName%" has already been "http://paccn.tpvaoc.com:8080/proxy.pac"
)
@ECHO.
pause. Continue if press any.

setlocal ENABLEEXTENSIONS
set KEY_NAME="HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings\Connections"
set ItemName=DefaultConnectionSettings
FOR /F "usebackq skip=1 tokens=1-3" %%A IN (`REG QUERY %KEY_NAME% /v %ItemName% 2^>nul`) DO (
    set ItemName=%%A
    set ItemType=%%B
    set ItemValue=%%C
)

set ValuePrev=%ItemValue:~0,16%
set ValueSettings=%ItemValue:~16,2%
set ValuePost=%ItemValue:~18%
@ECHO.
@ECHO.RegistedTable
@ECHO.^[%KEY_NAME%^]
@ECHO.  ^.ItemName: %ItemName%
@ECHO.  ^.ItemType: %ItemType%
@ECHO.  ^.ItemValue: ^(as follows^)
@ECHO.%ValuePrev%'%ValueSettings%'%ValuePost%
@ECHO.
@ECHO.^>^>^>Debugging 9th Hex Byte is %ValueSettings%
@ECHO.
IF NOT "%ValueSettings%"=="0D" (
    @ECHO. :: Changed %ValueSettings% to 0D - it indicates that just the Use automatic configuration script is checked
    @ECHO.
	setlocal EnableDelayedExpansion
    IF "%ItemType%"=="" (set ItemType=REG_BINARY)
	set ValueSettings=0D
    @ECHO.  ^.ItemValue: ^(LATEST as follows^)
    @ECHO.%ValuePrev%'!ValueSettings!'%ValuePost%
    @ECHO.
    @ECHO. :: The Changed will be imported into windows registed table
	@rem ECHO. :: reg add %KEY_NAME% /v %ItemName% /t !ItemType! /d "%ValuePrev%!ValueSettings!%ValuePost%" /f
    reg add %KEY_NAME% /v %ItemName% /t %ItemType% /d "%ValuePrev%!ValueSettings!%ValuePost%" /f
	@ECHO.
) ELSE (
    @ECHO. :: Keeping %ValueSettings% - it indicates that just the Use automatic configuration script is checked
    @ECHO.
)

ECHO.
ECHO.  Flushing DNS
ipconfig /flushdns


ECHO.  Showwing New Settings
ipconfig /all


ECHO.  Closing Internet Explorer
taskkill  /f /t /im  IEXPLORE.exe

pause
exit

:cmd1
ECHO.  Flushing DNS
ipconfig /flushdns


ECHO.  Showwing New Settings
ipconfig /all


ECHO.  Closing Internet Explorer
taskkill  /f /t /im  IEXPLORE.exe

ECHO.
ECHO.
ECHO.
ECHO. Starting...TPV - Wan Proxy: (USA VPN) 172.20.30.1:3128...
ECHO.

setlocal ENABLEEXTENSIONS
set KEY_NAME="HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings"
set ItemName=ProxyServer
FOR /F "usebackq skip=1 tokens=1-3" %%A IN (`REG QUERY %KEY_NAME% /v %ItemName% 2^>nul`) DO (
    set ItemName=%%A
    set ItemType=%%B
    set ItemValue=%%C
)
@ECHO.
@ECHO.RegistedTable
@ECHO.^[%KEY_NAME%^]
@ECHO.  ^.ItemName: %ItemName%
@ECHO.  ^.ItemType: %ItemType%
@ECHO.  ^.ItemValue: ^(as follows^)
@ECHO.%ItemValue%
@ECHO.
IF NOT "%ItemValue%"=="172.20.30.1:3128" (
    setlocal EnableDelayedExpansion
    IF "%ItemType%"=="" (set ItemType=REG_SZ)
    reg add %KEY_NAME% /v %ItemName% /t !ItemType! /d "172.20.30.1:3128" /f
    @ECHO.^>^>^>The value of "%ItemName%" is Changed to "172.20.30.1:3128"
) else (
    @ECHO.^>^>^>The value of "%ItemName%" has already been "172.20.30.1:3128"
)
@ECHO.
pause. Continue if press any.


setlocal ENABLEEXTENSIONS
setlocal EnableDelayedExpansion
set KEY_NAME="HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings\Connections"
set ItemName=DefaultConnectionSettings
FOR /F "usebackq skip=1 tokens=1-3" %%A IN (`REG QUERY %KEY_NAME% /v !ItemName! 2^>nul`) DO (
    set ItemName=%%A
    set ItemType=%%B
    set ItemValue=%%C
)

set ValuePrev=%ItemValue:~0,16%
set ValueSettings=%ItemValue:~16,2%
set ValuePost=%ItemValue:~18%
@ECHO.
@ECHO.RegistedTable
@ECHO.^[%KEY_NAME%^]
@ECHO.  ^.ItemName: !ItemName!
@ECHO.  ^.ItemType: !ItemType!
@ECHO.  ^.ItemValue: ^(as follows^)
@ECHO.!ValuePrev!'!ValueSettings!'!ValuePost!
@ECHO.
@ECHO.^>^>^>Debugging 9th Hex Byte is !ValueSettings!
@ECHO.
IF NOT "!ValueSettings!"=="01" (
    @ECHO. :: Changed !ValueSettings! to 01 - it indicates that neither of those 2 auto config boxes are checked
    @ECHO.
	setlocal EnableDelayedExpansion
    IF "%ItemType%"=="" (set ItemType=REG_BINARY)
	set ValueSettings=01
    @ECHO.  ^.ItemValue: ^(LATEST as follows^)
    @ECHO.!ValuePrev!'!ValueSettings!'!ValuePost!
    @ECHO.
    @ECHO. :: The Changed will be imported into windows registed table
	@rem ECHO. :: reg add %KEY_NAME% /v !ItemName! /t !ItemType! /d "!ValuePrev!!ValueSettings!!ValuePost!" /f
    reg add %KEY_NAME% /v !ItemName! /t !ItemType! /d "!ValuePrev!!ValueSettings!!ValuePost!" /f
	@ECHO.
) ELSE (
    @ECHO. :: Keeping !ValueSettings! - it indicates that neither of those 2 auto config boxes are checked
    @ECHO.
)


setlocal ENABLEEXTENSIONS
setlocal EnableDelayedExpansion
set KEY_NAME="HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings"
set ItemName=ProxyEnable
FOR /F "usebackq skip=1 tokens=1-3" %%D IN (`REG QUERY !KEY_NAME! /v !ItemName! 2^>nul`) DO (
    set ItemName=%%D
    set ItemType=%%E
    set ItemValue=%%F
)
@ECHO.
@ECHO.RegistedTable
@ECHO.^[!KEY_NAME!^]
@ECHO.  ^.ItemName: !ItemName!
@ECHO.  ^.ItemType: !ItemType!
@ECHO.  ^.ItemValue: ^(as follows^)
@ECHO.!ItemValue!
@ECHO.
IF NOT "!ItemValue!"=="1" (
    IF "%ItemType%"=="" (set ItemType=REG_SZ)
    reg add !KEY_NAME! /v !ItemName! /t !ItemType! /d "1" /f
    @ECHO.^>^>^>The value of "!ItemName!" is Changed to "1"
) else (
    @ECHO.^>^>^>The value of "!ItemName!" has already been "1"
)
@ECHO.
pause. Continue if press any.

ECHO.

pause
exit

echo.
echo.
echo  I know this is an old question, but it was not so easy for me to find a solution so I want to share what I found with you just in case you land in this page.
echo.
echo  In the RegKey HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Internet Settings\Connections the 9th value of DefaultConnectionSettings store the flag of:
echo  Automatically detect settings
echo  Use Automatic Configuration script
echo  Available values:
echo  01 - neither of those 2 auto config boxes are checked
echo  05 - just the Use automatic configuration script is checked
echo  09 - just the Automatically detect settings is checked
echo  0d - both of them are checked
echo.
echo  Personally I exported the current key, I created a copy and modified it with the text editor.
echo.
echo  Then on needs I load through Command Line the initial RegKey or the modified one through command reg import KeyIWantToUse.reg
echo.

