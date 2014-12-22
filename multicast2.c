/* Send Multicast Datagram code example. */
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>      /* for fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for sleep() */
#include<errno.h>
#include<netdb.h>
#include<netinet/ip_icmp.h>   //Provides declarations for icmp header
#include<netinet/udp.h>   //Provides declarations for udp header
#include<netinet/tcp.h>   //Provides declarations for tcp header
#include<netinet/ip.h>    //Provides declarations for ip header
#include<netinet/if_ether.h>  //For ETH_P_ALL
#include<net/ethernet.h>  //For ether_header
#include<sys/ioctl.h>
#include<sys/time.h>

#include<time.h>
#define SPORT 520
#define RPORT 520

struct in_addr localInterface;
struct sockaddr_in groupSock;
int sd;
char databuf[1024] = "Multicast test message lol!";



struct rip {
	u_int8_t rip_cmd;		/* request/response */
	u_int8_t rip_vers;		/* protocol version # */
	u_int8_t unused[2];		/* unused */
};

struct rip_netinfo {
	u_int16_t rip_family;
	u_int16_t rip_tag;
	u_int32_t rip_dest;
	u_int32_t rip_dest_mask;
	u_int32_t rip_router;
	u_int32_t rip_metric;		/* cost of route */
};

int datalen;
int main (int argc, char *argv[ ])
{
	struct sockaddr_in cliaddr;

	/*RIP Variables*/
	struct sockaddr_in entry , mask , nexthop;
	int temp;

	int saddr_size , data_size;
        struct sockaddr saddr;
	char *buffer = NULL;

	buffer = (char *) malloc (sizeof(struct rip_netinfo) + sizeof(struct rip));

	struct rip *rph = NULL;
	struct rip_netinfo *ni = NULL;

	rph = (struct rip *) malloc(sizeof(struct rip));
	ni  = (struct rip_netinfo *)malloc(sizeof(struct rip_netinfo));
	/*----------------------------------------------------------------*/


	cliaddr.sin_family = AF_INET;
	cliaddr.sin_addr.s_addr= htonl(INADDR_ANY);
	cliaddr.sin_port=htons(RPORT); //source port for outgoing packets

	/* Create a datagram socket on which to send. */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
	{
  		perror("Opening datagram socket error");
  		exit(1);
	}
	else
  		printf("Opening the datagram socket...OK.\n");

	memset((char *) &groupSock, 0, sizeof(groupSock));
	groupSock.sin_family = AF_INET;
	groupSock.sin_addr.s_addr = inet_addr("224.0.0.9");
	groupSock.sin_port = htons(SPORT);
	fprintf(stdout , "\n ServerPort:   %u \n",groupSock.sin_port);


	/* Set local interface for outbound multicast datagrams. */
	/* The IP address specified must be associated with a local, */
	/* multicast capable interface. */

	bind(sd,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
	localInterface.s_addr = inet_addr("10.1.2.2");
	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
	{
  		perror("Setting local interface error");
  		exit(1);
	}
	else
  		printf("Setting the local interface...OK\n");
	/* Send a message to the multicast group specified by the*/
	/* groupSock sockaddr structure. */

	/*_________Making the RIP structure____________*/
	rph->rip_cmd          = 2;
	rph->rip_vers         = 2;
	ni->rip_family        = htons(AF_INET) ;
	ni->rip_tag           = 0;
	temp                  = inet_aton("10.1.1.0", &entry.sin_addr);
	ni->rip_dest          = (u_int32_t) entry.sin_addr.s_addr;
	temp                  = inet_aton("255.255.255.0",&mask.sin_addr);
	ni->rip_dest_mask     = (u_int32_t) mask.sin_addr.s_addr;
	temp                  = inet_aton("0.0.0.0",&nexthop.sin_addr);
	ni->rip_router        = (u_int32_t) nexthop.sin_addr.s_addr;
	ni->rip_metric        = htonl(1);


	memcpy(buffer , rph , sizeof(struct rip));
	memcpy((buffer + sizeof(struct rip)) , ni , sizeof(struct rip_netinfo));

	datalen = sizeof(struct rip) + sizeof(struct rip_netinfo) ;

	*( buffer + datalen) = '\0';
	int i = 0;
	while( ++i <3)
	{

		if(sendto(sd, buffer, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0)
		{
			perror("Sending datagram message error");
		}
		else
  			printf("Sending datagram message...OK\n");

		sleep(10);

 	}
	return 0;
}


