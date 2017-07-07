@echo off

title DOS 

cls

color 1f

echo.

echo.

echo                   win7搭建无线AP

echo.

echo.

echo                                 即将进行安装，选择你所需的服务......

echo.

echo.     

echo -------------------------------------------------------------------------------

:cl

echo.

set /p no= Y(一键完成) E(启用承载) D(停止承载) N(退出):

echo.

if /I "%no%"=="Y" goto ks

if /I "%no%"=="E" goto ss

if /I "%no%"=="D" goto dd

if /I "%no%"=="N" goto exit

echo         输入错误，请重新输入...

goto cl

:ks

color 2

netsh wlan set hostednetwork mode=allow ssid="Win7 AP WOW!" key="11111111"

netsh wlan start hostednetwork

echo.

echo.

color 6

echo                      创建成功丶密码是8个1

echo.

echo.

echo.

echo              任意键返回主菜单

echo.

pause >nul

goto menu

:ss

cls

color 5d

netsh wlan start hostednetwork

echo.

echo.

echo 启用承载成功

echo.

echo.

echo.

echo                               任意键返回主菜单

echo.

pause >nul

goto menu

:dd

color 3e

cls

echo.

echo.

echo.

echo.

netsh wlan stop hostednetwork

echo.

echo.

echo.

echo                               任意键返回主菜单

echo.

pause >nul

goto menu

:menu

cls

color 5

echo -------------------------------------------------------------------------------

echo.

echo.

echo.

echo                                 请选择：

echo.

echo.

:mm

echo.

set /p no= Y(一键完成) E(启用承载) D(停止承载) N(退出):

echo.

if /I "%no%"=="Y" goto ks

if /I "%no%"=="E" goto ss

if /I "%no%"=="D" goto dd

if /I "%no%"=="N" goto exit

echo         输入错误，请重新输入...

goto mm
