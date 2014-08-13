#!/bin/sh
echo "logto slave1"
ssh -l kuangren 192.168.5.11 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/kuangren/bishe/project/my_work_process_pic/my_work/test/'

