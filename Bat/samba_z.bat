
@echo off
rem  lin_jie_long@126.com, 2013-05-15.  All rights reserved.

net use * /d /y

echo 如果想建立 与 \\192.168.1.200\samba 的连接,请继续,否则直接关闭窗口结束...

pause
net use z: \\192.168.1.200\samba  123456  /USER:samba


echo 如果想打开source insight的内核源码树,请继续,否则直接关闭窗口结束...

pause

start  D:\LinuxDeveloper\si\VMwareLinuxKernel\VMwareLinuxKernel.PR

