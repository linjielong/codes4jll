@REM  copyright (c) 2013.   lin_jie_long@126.com, 2013-09-11. All rights reserved.

@ECHO OFF


:looped
    @REM SET  pathvar=%CD%
    @ECHO %pathvar%
    @ECHO %time%
    @CALL :do_func
    @PAUSE
    @REM ECHO %pathvar%
    @REMCD   %pathvar%
    @REMCD
    @REM GOTO looped

goto :eof



:do_func

@SET RootPath=C:\cygwin\home\linjielong
@SET fn=%RootPath%\shelltest.sh
@SET CompileContent=cd temp/;make;./hk
@SET dest=%RootPath%\temp\






@RD /S /Q  %dest%              2>nul
@DEL /Q Makefile            2>nul

@REM  Generating the general Makefile

@ECHO #copyright (c) 2013.   lin_jie_long@126.com, 2010-09-01. All rights reserved. > Makefile
@ECHO.
@ECHO SOURCE = $(wildcard *.c)  >> Makefile
@ECHO OBJECT = $(patsubst %%.c, %%.o, $(SOURCE)) >> Makefile
@ECHO CC = gcc >> Makefile
@ECHO CFLAGS = -Wall >> Makefile
@ECHO hk : $(OBJECT) >> Makefile
@ECHO 	$(CC) $(CFLAGS) -o $@ $^^ >> Makefile
@ECHO depends : $(SOURCE) >> Makefile
@ECHO 	$(CC) -MM $(SOURCE) ^> depends >> Makefile
@ECHO -include depends >> Makefile
@ECHO .PHONY : clean >> Makefile
@ECHO clean : >> Makefile
@ECHO 	-rm hk.exe *.o depends >> Makefile
 

@REM  Generating the general shell file

@ECHO %CompileContent% > %fn%  2>nul
@C:\cygwin\bin\dos2unix.exe %fn%

@MD                  %dest%
@XCOPY  /Y src       %dest%
@MOVE   /Y Makefile  %dest%


C:
chdir C:\cygwin\bin

bash --login "./shelltest.sh"

goto :eof
