/*
* host_send.c
*
*/
#include "host_send.h"




int main (int argc, char** argv) {	


	if(argc<1){
		printf("Error: Format required: send:1 rcv:0");
		exit(1);
	}

	if(atoi(argv[1])==1)
	{
		if (argc < 4) {
			printf("Error: Format required: msg msg_size dest_addr\n");
			exit(1);
		}

		int size=atoi(argv[3]);

		uint8_t *payload = NULL;
		//uint8_t payloadLen;
		payload = (uint8_t*) calloc (1, size);
		memcpy(payload, argv[2], size);
			
		ctrlSend(payload, size, ether_aton(argv[4]));
	}
	else if(atoi(argv[1])==0)
	{
		host_rcv();
	}
	else
	{ 
		printf("Error: Format required: send:1 rcv:0");
		exit(1); 
	}

return 0;
}






int ctrlSend(uint8_t *inPayload, int payloadLen, struct ether_addr *dest) {
int frame_Size = -1;
int sockfd;
struct ifreq if_idx;
struct ifreq if_mac;
char ifName[IFNAMSIZ];
strcpy(ifName, "eth1");
frame_Size = HEADER_SIZE + payloadLen;
// creating frame
uint8_t frame[frame_Size];
struct ether_header *eh = (struct ether_header*)calloc(1, sizeof(struct ether_header));
struct sockaddr_ll socket_address;
// Open RAW socket to send on
if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
perror("Socket Error");
}
memset(&if_idx, 0, sizeof(struct ifreq));
strncpy(if_idx.ifr_name, ifName, IFNAMSIZ - 1);
if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
perror("SIOCGIFINDEX - Misprint Compatibility");
memset(&if_mac, 0, sizeof(struct ifreq));
strncpy(if_mac.ifr_name, ifName, IFNAMSIZ - 1);
if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
perror("SIOCGIFHWADDR - Either interface is not correct or disconnected");
/*
* Ethernet Header - 14 bytes
*
* 6 bytes - Source MAC Address
* 6 bytes - Destination MAC Address
* 2 bytes - EtherType
*
*/
eh->ether_shost[0] = ((uint8_t *) &if_mac.ifr_hwaddr.sa_data)[0];
eh->ether_shost[1] = ((uint8_t *) &if_mac.ifr_hwaddr.sa_data)[1];
eh->ether_shost[2] = ((uint8_t *) &if_mac.ifr_hwaddr.sa_data)[2];
eh->ether_shost[3] = ((uint8_t *) &if_mac.ifr_hwaddr.sa_data)[3];
eh->ether_shost[4] = ((uint8_t *) &if_mac.ifr_hwaddr.sa_data)[4];
eh->ether_shost[5] = ((uint8_t *) &if_mac.ifr_hwaddr.sa_data)[5];

memcpy(&eh->ether_dhost, dest, sizeof(struct ether_addr));

eh->ether_type = htons(0x806);

/*eh->ether_dhost[0] = MY_DEST_MAC0;
eh->ether_dhost[1] = MY_DEST_MAC1;
eh->ether_dhost[2] = MY_DEST_MAC2;
eh->ether_dhost[3] = MY_DEST_MAC3;
eh->ether_dhost[4] = MY_DEST_MAC4;
eh->ether_dhost[5] = MY_DEST_MAC5;
eh->ether_type = htons(0x8850);
*/

// Copying header to frame
memcpy(frame, eh, sizeof(struct ether_header));
// Copying Payload (No. of tier addr + x times (tier addr length + tier addr) )
// Copying payLoad to frame
memcpy(frame + sizeof(struct ether_header), inPayload, payloadLen);
// Index of the network device
socket_address.sll_ifindex = if_idx.ifr_ifindex;
// Address length - 6 bytes
socket_address.sll_halen = ETH_ALEN;
// Destination MAC Address

memcpy(&socket_address.sll_addr, dest, sizeof(struct ether_addr));

/*
socket_address.sll_addr[0] = MY_DEST_MAC0;
socket_address.sll_addr[1] = MY_DEST_MAC1;
socket_address.sll_addr[2] = MY_DEST_MAC2;
socket_address.sll_addr[3] = MY_DEST_MAC3;
socket_address.sll_addr[4] = MY_DEST_MAC4;
socket_address.sll_addr[5] = MY_DEST_MAC5;
*/
// Send packet
if (sendto(sockfd, frame, frame_Size, 0, (struct sockaddr*) &socket_address, sizeof(struct sockaddr_ll)) < 0) {
printf("ERROR: Send failed\n");
}
free(eh);
close(sockfd);
return 0;
}


int dataSend(uint8_t *inPayload, int payloadLen) {

char *etherPort;
strcpy(etherPort, "eth1");

int sockfd;
struct ifreq if_idx;
struct sockaddr_ll socket_address;
// Open RAW socket to send on
if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
perror("Socket Error");
}
memset(&if_idx, 0, sizeof(struct ifreq));
strncpy(if_idx.ifr_name, etherPort, strlen(etherPort));
if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0) {
perror("SIOCGIFINDEX - Misprint Compatibility");
}
// Index of the network device
socket_address.sll_ifindex = if_idx.ifr_ifindex;
// Address length - 6 bytes
socket_address.sll_halen = ETH_ALEN;
// Send packet
if (sendto(sockfd, inPayload, payloadLen, 0, (struct sockaddr*) &socket_address, sizeof(struct sockaddr_ll)) < 0) {
printf("ERROR: Send failed\n");
}
close(sockfd);
return 0;
}


void host_rcv()
{
	int sock, n, cnt;
	char buffer[2048];
	unsigned char *iphead, *ethhead;
	struct ether_addr ether;
	
	printf("aaaaaaaaaaaaaaaaaaaaaa");

	//if ((sock = socket(AF_PACKET, SOCK_RAW, htons(0x8850))) < 0) { //AF_INET/AF_PACKET, SOCK_RAW ,IPPROTO_RAW/htons(ETH_P_ALL)
    if ((sock = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) < 0) {	
	perror("ERROR: Socket");
		exit(1);
	}

	//char *opt;
	//opt = "eth0";
	//setsockopt(sd, SOL_SOCKET, SO_BINDTODEVICE, opt, 4);

//int nnn=1;
	while (1) {
		int flag=0; //nnn++;
		n=recvfrom(sock, buffer, 2048, 0, NULL,NULL);
		//n=recvfrom(sock, buffer, 2048, MSG_DONTWAIT, NULL,NULL);
		if (n == -1) {
			perror("ERROR: Recvfrom");
			//close(sock);
			flag=1;

			//exit(1);
		}
		//printf("n value %d\n",n);
	//	if (n > 0) {
		if (flag==0) {

			ethhead = (unsigned char *) buffer;
			//printf("1\n");
			if (ethhead != NULL) {
				//printf("2\n");
				iphead = (unsigned char *) (buffer + 14); // Skip Ethernet header
				printf("\n--------------------------------------"
						"\nMAC Destination : "
						"%02x:%02x:%02x:%02x:%02x:%02x\n", ethhead[0], ethhead[1], ethhead[2],
						ethhead[3], ethhead[4], ethhead[5]);
				//printf("3\n");
				printf("MAC Origin : "
						"%02x:%02x:%02x:%02x:%02x:%02x\n", ethhead[6], ethhead[7], ethhead[8],
						ethhead[9], ethhead[10], ethhead[11]);
				printf("Type : %02x:%02x \n", ethhead[12], ethhead[13]);
				printf("Data : %s\n", &ethhead[14]);

			}
			break;
		}
	}

close(sock);
}
