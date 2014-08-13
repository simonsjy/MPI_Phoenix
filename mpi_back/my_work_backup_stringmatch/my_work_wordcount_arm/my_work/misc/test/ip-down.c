#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

int main(int argc, char *argv[])
{
  /*
    interface-name       tty-device      speed      local-IP-address
    remote-IP-address ipparam
  */
  char *ifname = argv[1];
  char *tty = argv[2];
  char *speed =argv[3];
  char *local_ip = argv[4];
  char *remote_ip = argv[5];
  char *ipparam = argv[6];

	system("rm -f /var/log/pppfile");
	sylog(LOG_INFO,"IP-DOWN remove /var/log/pppfile");
	
	syslog(LOG_INFO,"IP-DOWN Interface-name:%s",ifname);
	syslog(LOG_INFO,"IP-DOWN tty-device:%s",tty);
	syslog(LOG_INFO,"IP-DOWN Speed:%s",speed);
	syslog(LOG_INFO,"IP-DOWN Local-IP-address:%s",local_ip);
	syslog(LOG_INFO,"IP-DOWN Remote-IP-address:%s",ipparam);	
		

  return 0;
}
