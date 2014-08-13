#!/bin/sh
#mpirun -f machine -np 3 ./test_wordcount test_wordcount.cpp test_wordcount.cpp
#scp testdata1.txt 192.168.5.132:/home/kuangren/bishe/project/my_work/test
#scp testdata2.txt 192.168.5.132:/home/kuangren/bishe/project/my_work/test
#scp testdata3.txt 192.168.5.132:/home/kuangren/bishe/project/my_work/test
#scp test_wordcount 192.168.5.132:/home/kuangren/bishe/project/my_work/test
#scp test_wordcount 192.168.5.199:/home/kuangren/bishe/project/my_work/test
mpirun -f machine -np 2 ./test_wordcount files.txt 
#mpirun -f machine -np 4 ./test_wordcount  /mnt/pic_dir
