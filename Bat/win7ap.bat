@echo off

title DOS 

cls

color 1f

echo.

echo.

echo                   win7�����AP

echo.

echo.

echo                                 �������а�װ��ѡ��������ķ���......

echo.

echo.     

echo -------------------------------------------------------------------------------

:cl

echo.

set /p no= Y(һ�����) E(���ó���) D(ֹͣ����) N(�˳�):

echo.

if /I "%no%"=="Y" goto ks

if /I "%no%"=="E" goto ss

if /I "%no%"=="D" goto dd

if /I "%no%"=="N" goto exit

echo         �����������������...

goto cl

:ks

color 2

netsh wlan set hostednetwork mode=allow ssid="Win7 AP WOW!" key="11111111"

netsh wlan start hostednetwork

echo.

echo.

color 6

echo                      �����ɹ�ؼ������8��1

echo.

echo.

echo.

echo              ������������˵�

echo.

pause >nul

goto menu

:ss

cls

color 5d

netsh wlan start hostednetwork

echo.

echo.

echo ���ó��سɹ�

echo.

echo.

echo.

echo                               ������������˵�

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

echo                               ������������˵�

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

echo                                 ��ѡ��

echo.

echo.

:mm

echo.

set /p no= Y(һ�����) E(���ó���) D(ֹͣ����) N(�˳�):

echo.

if /I "%no%"=="Y" goto ks

if /I "%no%"=="E" goto ss

if /I "%no%"=="D" goto dd

if /I "%no%"=="N" goto exit

echo         �����������������...

goto mm
