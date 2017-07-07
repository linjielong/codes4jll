@ECHO OFF
ECHO.
ECHO. Copyright(c) 2016-2100.   jielong_lin.   All rights reserved.
ECHO. Multise Network Routes Selector For PhilipsTV work 
ECHO.

CLS

MODE con: COLS=30 lines=15

color 9d

GOTO MENU

:MENU

ECHO.
ECHO. =====多网路控制器=====
ECHO.
ECHO. ID : Description
ECHO. ---------------------
ECHO. 1  : 外网: (美国专线)
ECHO.
ECHO. 2  : 内网
ECHO.
ECHO. 3  : 退出
ECHO.
ECHO. ======================
ECHO.
set /p ID=请输入选择项的序号：

if "%id%"=="1" goto cmd1

if "%id%"=="2" goto cmd2

if "%id%"=="3" goto cmd3

exit

PAUSE

:cmd1

ECHO.
ECHO.
ECHO.
ECHO. 正在启用...外网...
ECHO.

ROUTE DELETE 0.0.0.0
REM  ROUTE ADD 0.0.0.0 MASK 0.0.0.0 <gateway>
ROUTE ADD    0.0.0.0 MASK 0.0.0.0 192.168.1.1
ECHO.
ECHO.
ECHO. 完成启用...外网...
ECHO.

PAUSE
exit



:cmd2

ECHO.
ECHO.
ECHO.
ECHO. 正在启用...内网...
ECHO.

ROUTE DELETE 0.0.0.0
REM  ROUTE ADD 0.0.0.0 MASK 0.0.0.0 <gateway>
ROUTE ADD    0.0.0.0 MASK 0.0.0.0 172.20.27.254 
ECHO.
ECHO.
ECHO. 完成启用...内网...
ECHO.

PAUSE
exit



:cmd3

ECHO.
exit

