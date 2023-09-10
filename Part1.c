#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>

void processPacket(unsigned char* buffer, int size);
int main() {
    int sock_raw;
    int saddr_size;
    struct sockaddr saddr;
    unsigned char *buffer = (unsigned char *)malloc(65536);
    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); //capture packet
    if (sock_raw < 0) {
        perror("Socket Error :( "); return 1;
    }
    while (1) {
        saddr_size = sizeof saddr;
        int dataSize = recvfrom(sock_raw, buffer, 65536, 0, &saddr, (socklen_t*)&saddr_size); // receiving packet
        if (dataSize < 0) {
            perror("Recvfrom Error :( "); return 1;
        }
        processPacket(buffer, dataSize); // to process packet
    }
    close(sock_raw);
    free(buffer);
    return 0;
}
void processPacket(unsigned char* buffer, int size) {
    struct ip *iph = (struct ip *)(buffer + 14);
    struct tcphdr *tcph = (struct tcphdr *)(buffer + 14 + iph->ip_hl * 4); // TCP header location
    if (iph->ip_p == IPPROTO_TCP) {
        printf("Source IP: %s\n", inet_ntoa(iph->ip_src));
        printf("Destination IP: %s\n", inet_ntoa(iph->ip_dst));
        printf("Source Port: %d\n", ntohs(tcph->th_sport));
        printf("Destination Port: %d\n", ntohs(tcph->th_dport));
        printf("--------------------\n");
    }
}
