#! /bin/sh

rm -f /var/log/messages

killall syslogd

syslogd
