
@echo ------------- auto close the specify the vmware   --------------
@echo -------------   Created by  lin_jie_long@126.com  --------------
@echo -------------       All rights reserved.          --------------                             
@echo starting to close the some windows services of the vmware 

@sc stop ufad-ws60
@sc stop VMAuthdService
@sc stop VMnetDHCP
@sc stop "VMware NAT Service"
@sc stop VMUSBArbService

@echo finished to close the some windows services of the vmware

@pause