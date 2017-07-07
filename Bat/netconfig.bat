
@echo Copyright (c) 2013.  lin_jie_long@126.com 2013.

@echo  off
@echo  无线网络连接
@echo  IPv4地址: 192.168.1.15
@echo  网络掩码: 255.255.255.0
@echo  网关地址: 192.168.1.1
@echo  域名地址: 218.85.157.99

@netsh interface ipv4 add address "无线网络连接" 192.168.1.15 255.255.255.0

@netsh interface ipv4 add address "无线网络连接" gateway=192.168.1.1 gwmetric=2

@netsh interface ipv4 add dnsservers "无线网络连接"  218.85.157.99

@echo 配置已完成!
@pause

@echo 显示ipconfig配置信息

@ipconfig /all

@pause