#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <cjson/cJSON.h>

// Initialize Global Variables
char nninit[] = { 0x00, 0x53, 0x50, 0x00, 0x00, 0x21, 0x00, 0x00 };
char nnires[] = { 0x00, 0x53, 0x50, 0x00, 0x00, 0x20, 0x00, 0x00 };
int iDemodPort = 6001;
int iDecodePort = 6002;
char* sIP = "10.42.0.135";

void *recvDemod()
{
    int sock = 0;
    int valread;
    struct sockaddr_in serv_addr;
    char buffer[8] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("recvDemod: Socket creation error.\n");
        // Finish thread TODO
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(iDemodPort);

    if (inet_pton(AF_INET, sIP, &serv_addr.sin_addr) <= 0)
    {
        printf("recvDemod: Invalid address/Address not supported.\n");
        // Finish thread TODO
    }

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("recvDemod: Connection failed.\n");
        // Finish thread TODO
    }

    printf("recvDemod: Sending nninit byte array.\n");
    send(sock, nninit, sizeof(nninit), 0);
    printf("recvDemod: Receiving byte array.\n");
    valread = read(sock, buffer, 8);
    printf("recvDemod: Comparing nnires and received byte array (memcmp).\n");
    
    int iCheck;
    iCheck = memcmp(nnires, buffer, sizeof(nnires));
    if (iCheck == 0)
    {
        printf("recvDemod: nnires and received byte array match.\n");
    } else
    {
        printf("recvDemod: nnires and received byte array don't match.\n");
    }

    char res[256] = {0};
    int num;

    printf("recvDemod: Receiving data.\n");

    while (1)
    {
        num = read(sock, res, 8);
        if (num == 0)
        {
            printf("recvDemod: Connection lost.\n");
        } 
        
        int msglen = res[7];
        num = read(sock, res, msglen);

        // Parse JSON
        cJSON *oJSON = cJSON_Parse(res);
        cJSON *oTimestamp = cJSON_GetObjectItem(oJSON, "timestamp");
        cJSON *oGain = cJSON_GetObjectItem(oJSON, "gain");
        cJSON *oFrequency = cJSON_GetObjectItem(oJSON, "frequency");
        cJSON *oOmega = cJSON_GetObjectItem(oJSON, "omega");

        // Convert JSON Objects to Strings
        char *sTimestamp = cJSON_Print(oTimestamp);
        char *sGain = cJSON_Print(oGain);
        char *sFrequency = cJSON_Print(oFrequency);
        char *sOmega = cJSON_Print(oOmega);

        // Remove quotes from timestamp
        memmove(sTimestamp, sTimestamp + 1, strlen(sTimestamp));
        sTimestamp[strlen(sTimestamp) - 1] = '\0';

        // Print results to console
        printf("recvDemod: Timestamp: %s\n", sTimestamp);
        printf("recvDemod: Gain: %s\n", sGain);
        printf("recvDemod: Frequency: %s\n", sFrequency);
        printf("recvDemod: Omega: %s\n", sOmega);
    }
}

void *recvDecode()
{
    int sock = 0;
    int valread;
    struct sockaddr_in serv_addr;
    char buffer[8] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("recvDecode: Socket creation error.\n");
        // Finish thread TODO
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(iDecodePort);

    if (inet_pton(AF_INET, sIP, &serv_addr.sin_addr) <= 0)
    {
        printf("recvDecode: Invalid address/Address not supported.\n");
        // Finish thread TODO
    }

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("recvDecode: Connection failed.\n");
        // Finish thread TODO
    }

    printf("recvDecode: Sending nninit byte array.\n");
    send(sock, nninit, sizeof(nninit), 0);
    printf("recvDecode: Receiving byte array.\n");
    valread = read(sock, buffer, 8);
    printf("recvDecode: Comparing nnires and received byte array (memcmp).\n");

    int iCheck;
    iCheck = memcmp(nnires, buffer, sizeof(nnires));
    if (iCheck == 0)
    {
        printf("recvDecode: nnires and received byte array match.\n");
    } else
    {
        printf("recvDecode: nnires and received byte array don't match.\n");
    }

    char res[256] = {0};
    int num;

    printf("recvDecode: Receiving data.\n");

    while (1)
    {
        num = read(sock, res, 8);
        if (num == 0)
        {
            printf("recvDecode: Connection lost.\n");
        }

        int msglen = res[7];
        num = read(sock, res, msglen);

        // Parse JSON
        cJSON *oJSON = cJSON_Parse(res);
        cJSON *oTimestamp = cJSON_GetObjectItem(oJSON, "timestamp");
        cJSON *oSkippedSymbols = cJSON_GetObjectItem(oJSON, "skipped_symbols");
        cJSON *oViterbiErrors = cJSON_GetObjectItem(oJSON, "viterbi_errors");
        cJSON *oReedSolomonErrors = cJSON_GetObjectItem(oJSON, "reed_solomon_errors");
        cJSON *oOk = cJSON_GetObjectItem(oJSON, "ok");

        // Convert JSON Objects to Strings
        char *sTimestamp = cJSON_Print(oTimestamp);
        char *sSkippedSymbols = cJSON_Print(oSkippedSymbols);
        char *sViterbiErrors = cJSON_Print(oViterbiErrors);
        char *sReedSolomonErrors = cJSON_Print(oReedSolomonErrors);
        char *sOk = cJSON_Print(oOk);

        // Remove quotes from timestamp
        memmove(sTimestamp, sTimestamp + 1, strlen(sTimestamp));
        sTimestamp[strlen(sTimestamp) - 1] = '\0';

        // Print results to console
        printf("recvDecode: Timestamp: %s\n", sTimestamp);
        printf("recvDecode: SkippedSymbols: %s\n", sSkippedSymbols);
        printf("recvDecode: ViterbiErrors: %s\n", sViterbiErrors);
        printf("recvDecode: ReedSolomonErrors: %s\n", sReedSolomonErrors);
        printf("recvDecode: Ok: %s\n", sOk);
    }
}

int main()
{
    // Create thread objects
    pthread_t tRecvDemod;
    pthread_t tRecvDecode;

    // Run recvDemod Thread
    pthread_create(&tRecvDemod, NULL, recvDemod, NULL);

    // Run recvDecode Thread
    pthread_create(&tRecvDecode, NULL, recvDecode, NULL);

    // Join threads
    pthread_join(tRecvDemod, NULL);
    pthread_join(tRecvDecode, NULL);

    // End
    printf("This message will be displayed if i ever implement a quit function\n");
    return 0;
}
