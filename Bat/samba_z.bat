
@echo off
rem  lin_jie_long@126.com, 2013-05-15.  All rights reserved.

net use * /d /y

echo ����뽨�� �� \\192.168.1.200\samba ������,�����,����ֱ�ӹرմ��ڽ���...

pause
net use z: \\192.168.1.200\samba  123456  /USER:samba


echo ������source insight���ں�Դ����,�����,����ֱ�ӹرմ��ڽ���...

pause

start  D:\LinuxDeveloper\si\VMwareLinuxKernel\VMwareLinuxKernel.PR

