
@echo -------------  auto open the specify the vmware   --------------
@echo -------------   Created by  lin_jie_long@126.com  --------------
@echo -------------       All rights reserved.          --------------                
@echo starting to open the some windows services of the vmware 

@sc start ufad-ws60
@sc start VMAuthdService
@sc start VMnetDHCP
@sc start "VMware NAT Service"
@sc start VMUSBArbService

@echo finished to open the some windows services of the vmware

@echo running the specify ubuntu10 of the vmware.
@cd "c:\Program Files\VMware\VMware Workstation"
@start vmware.exe -B -x  E:\ubuntu10\ubuntu10_vm_passwd=123456\Ubuntu10.vmx
@echo Thank you

@pause

