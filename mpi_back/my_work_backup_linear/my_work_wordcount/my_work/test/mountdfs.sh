#!/bin/sh
echo "logto slave1"
ssh -l root 192.168.5.11 'cd;./mount.sh'

echo "logto slave2"
ssh -l root 192.168.5.12 'cd;./mount.sh'

echo "logto slave3"
ssh -l root 192.168.5.13 'cd;./mount.sh'

echo "logto slave4"
ssh -l root 192.168.5.14 'cd;./mount.sh'

echo "logto slave5"
ssh -l root 192.168.5.15 'cd;./mount.sh'

echo "logto slave6"
ssh -l root 192.168.5.16 'cd;./mount.sh'

echo "logto slave7"
ssh -l root 192.168.5.17 'cd;./mount.sh'

echo "logto slave8"
ssh -l root 192.168.5.18 'cd;./mount.sh'
