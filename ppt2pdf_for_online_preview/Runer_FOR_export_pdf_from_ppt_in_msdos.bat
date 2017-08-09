@ECHO OFF
CLS
ECHO.
ECHO.

REM SETLOCAL DISABLEDELAYEDEXPANSION

FOR /f "tokens=1 delims=" %%i IN ( ' dir *.ppt /B 2^> NUL ' ) DO (
    ECHO Checking if "%CD%wwwroot\%%~ni.pdf" exist...
    IF exist "%CD%wwwroot\%%~ni.pdf" (
        ECHO Removing "%CD%wwwroot\%%~ni.pdf" ...
        DEL /F /Q "%CD%wwwroot\%%~ni.pdf"
        ECHO.
    )
    IF not exist "%CD%wwwroot" (
        ECHO Creating "%CD%wwwroot" ...
        MKDIR "%CD%wwwroot"
        ECHO.
    )
    ECHO Start to Export "%%~ni.pdf" 
    ECHO       from "%%i"
    ECHO.
    CSCRIPT export_pdf_from_ppt_by_msoffice.vbs "%CD%%%i" "%CD%wwwroot\%%~ni.pdf"
    ECHO End to Export "%%~ni.pdf" 
    ECHO     from "%%i"
    ECHO.
)
ECHO.

FOR /f "tokens=1 delims=" %%i IN ( ' dir *.pptx /B 2^> NUL ' ) DO (
    ECHO Checking if "%CD%wwwroot\%%~ni.pdf" exist...
    IF exist "%CD%wwwroot\%%~ni.pdf" (
        ECHO Removing "%CD%wwwroot\%%~ni.pdf" ...
        DEL /F /Q "%CD%wwwroot\%%~ni.pdf"
        ECHO.
    )
    IF not exist "%CD%wwwroot" (
        ECHO Creating "%CD%wwwroot" ...
        MKDIR "%CD%wwwroot"
        ECHO.
    )
    ECHO Start to Export "%%~ni.pdf" 
    ECHO       from "%%i"
    ECHO.
    CSCRIPT export_pdf_from_ppt_by_msoffice.vbs "%CD%%%i" "%CD%wwwroot\%%~ni.pdf"
    ECHO End to Export "%%~ni.pdf" 
    ECHO       from "%%i"
    ECHO.
)
ECHO.

rem PAUSE
timeout 10