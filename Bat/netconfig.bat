
@echo Copyright (c) 2013.  lin_jie_long@126.com 2013.

@echo  off
@echo  ������������
@echo  IPv4��ַ: 192.168.1.15
@echo  ��������: 255.255.255.0
@echo  ���ص�ַ: 192.168.1.1
@echo  ������ַ: 218.85.157.99

@netsh interface ipv4 add address "������������" 192.168.1.15 255.255.255.0

@netsh interface ipv4 add address "������������" gateway=192.168.1.1 gwmetric=2

@netsh interface ipv4 add dnsservers "������������"  218.85.157.99

@echo ���������!
@pause

@echo ��ʾipconfig������Ϣ

@ipconfig /all

@pause