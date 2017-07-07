:: Copyrights (c) 2016-2100    jielong.lin       All rights reserved.

@ECHO off
cls

::type NUL > MergedLogcat.txt

ECHO  ###################################################################

ECHO  # MERGING CAN TAKE SEVERAL MINUTES, PLEASE WAIT TILL IT COMPLETES #

ECHO  ###################################################################

ECHO     MERGING IN PROGRESS......


ECHO  # Current Path is :  %CD%
::启用变量延时扩展，才能在for语句中使用!完成赋值给变量
SETLOCAL ENABLEDELAYEDEXPANSION 
for /F "delims=" %%F in ('dir /B /S /O:D /AD "%CD%"') do (
    ::判断当前路径名有效
    if not "%%~nxF"=="" (
	    set MyVariant=%%~nxF
		set MyVariant=!MyVariant:~0,5!
		if "!MyVariant!"=="Boot_" (
		    ::进入当前Boot_*文件夹所在目录，合并Boot_*中的logcat.txt.*
		    cd %%F
			::echo "!CD!"
			ECHO  ####################################################################################  >  ..\MergedLogcat.%%~nxF.txt
            ECHO  # Merged Logcat Content For %%~nxF @ Copyrights jielong.lin  All Rights Reserved.     >> ..\MergedLogcat.%%~nxF.txt
			ECHO  ####################################################################################  >> ..\MergedLogcat.%%~nxF.txt
			ECHO  ####################################################################################  >  ..\MergeOrder.%%~nxF.txt
			ECHO  # Merged Files Order @ Copyrights jielong.lin  All Rights Reserved.                   >> ..\MergeOrder.%%~nxF.txt
			ECHO  ####################################################################################  >> ..\MergeOrder.%%~nxF.txt
			::按文件时期进行排序收集并合并
			FOR /F "delims=" %%J IN ('DIR /S /O:D /B logcat.txt.*') DO (
			    ECHO;                                               >> ..\MergedLogcat.%%~nxF.txt
			    ECHO  ## JLL Logcat Merged Segment File: %%~nxJ     >> ..\MergedLogcat.%%~nxF.txt
				ECHO  ## PATH= %%J                                  >> ..\MergedLogcat.%%~nxF.txt
				ECHO;                                               >> ..\MergedLogcat.%%~nxF.txt
			    TYPE "%%J" >> ..\MergedLogcat.%%~nxF.txt
				ECHO;                                               >> ..\MergeOrder.%%~nxF.txt
				ECHO  %%J  >> ..\MergeOrder.%%~nxF.txt
			)
		    cd ..
		)
	)
)
exit 0