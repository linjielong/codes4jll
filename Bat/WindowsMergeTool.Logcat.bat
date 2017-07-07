@ECHO off
cls
type NUL > MergedLogcat.txt
ECHO  ###################################################################
ECHO  # MERGING CAN TAKE SEVERAL MINUTES, PLEASE WAIT TILL IT COMPLETES #
ECHO  ###################################################################
ECHO     MERGING IN PROGRESS......
@ECHO on
(FOR /F "delims=" %%F IN ('DIR /S /O:D /B logcat.txt.*') DO TYPE "%%F" >> MergedLogcat.txt) > MergeOrder.txt
type MergeOrder.txt
@ECHO off
ECHO MERGING COMPLETED
