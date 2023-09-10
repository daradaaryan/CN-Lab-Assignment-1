#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

void handleIncomingPacket(unsigned char *, int);
void lookupAssociatedProcess(int);

int main() {
    int socketRaw;
    struct sockaddr_in serverAddress;
    socklen_t serverLength = sizeof(serverAddress);
    unsigned char packetData[65536];
    socketRaw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (socketRaw == -1) {
        perror("Error creating socket"); exit(1);
    }
    while (1) {
        int packetSize = recvfrom(socketRaw, packetData, sizeof(packetData), 0, (struct sockaddr *)&serverAddress, &serverLength);
        if (packetSize == -1) {
            perror("Error receiving packet :( "); close(socketRaw); exit(1);
        }
        handleIncomingPacket(packetData, packetSize);
        printf("Enter a unique number :) : ");
        int portNumber;
        scanf("%d", &portNumber);
        lookupAssociatedProcess(portNumber);
    }
    close(socketRaw);
    return 0;
}

void handleIncomingPacket(unsigned char *data, int size) {
    struct ip *ipHeader = (struct ip *)(data);
    struct tcphdr *tcpHeader = (struct tcphdr *)(data + ipHeader->ip_hl * 4);
    char sourceIP[INET_ADDRSTRLEN];
    char destinationIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ipHeader->ip_src), sourceIP, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ipHeader->ip_dst), destinationIP, INET_ADDRSTRLEN);
    printf("Source IP: %s\n", sourceIP);
    printf("Source Port: %d\n", ntohs(tcpHeader->th_sport));
    printf("Destination IP: %s\n", destinationIP);
    printf("Destination Port: %d\n", ntohs(tcpHeader->th_dport));
    printf("-----------------------\n");
}

void lookupAssociatedProcess(int number) {
    char command[128];
    snprintf(command, sizeof(command), "lsof -i :%d | awk '{print $2}' | tail -n +2", number);
    FILE *filePtr = popen(command, "r");
    if (filePtr == NULL) {
        printf("Failed to execute the command !!! \n"); return;
    }
    char result[128];
    if (fgets(result, sizeof(result) - 1, filePtr) != NULL) printf("Identification: %s", result);
    else printf("No identification associated with the number !!! %d\n", number);
    pclose(filePtr);
}
