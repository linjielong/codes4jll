@ECHO off

cls

type NUL > MergedKernelBuffer.txt

ECHO  ###################################################################

ECHO  # MERGING CAN TAKE SEVERAL MINUTES, PLEASE WAIT TILL IT COMPLETES #
ECHO  # Modification By JIELONG.LIN IN XMIC                             #

ECHO  ###################################################################

ECHO     MERGING IN PROGRESS......

@ECHO on

(FOR /F "delims=" %%F IN ('DIR /S /O:D /B KernelBuffer.txt.*') DO TYPE "%%F" >> MergedKernelBuffer.txt) > MergeOrder.KernelBuffer.txt

type MergeOrder.KernelBuffer.txt

@ECHO off

ECHO MERGING COMPLETED

