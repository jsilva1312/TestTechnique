#include "udp_sender.h"

int send_msg(char * msg, char * ip_addr, char * server_port)
{
    int socketDescriptor, status;
    unsigned int msgLength;
    struct addrinfo hints, *servinfo;
    struct timeval timeVal;
    fd_set readSet;

    // Contrôle des arguments
    if(NULL == msg)
    {
        perror("Msg NULL");
        exit(EXIT_FAILURE);
    }
    if(NULL == ip_addr || strlen(ip_addr)> IP_ARRAY_SIZE)
    {
        perror("Bad ip_addr ");
        exit(EXIT_FAILURE);
    }
    if(NULL == server_port || strlen(server_port)> PORT_ARRAY_SIZE)
    {
        perror("Bad server_port ");
        exit(EXIT_FAILURE);
    }


    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_INET;
    // Socket UDP = SOCK_DGRAM
    hints.ai_socktype = SOCK_DGRAM;

    if ((status = getaddrinfo(ip_addr, server_port, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    if ((socketDescriptor = socket(servinfo->ai_family, servinfo->ai_socktype,
                                   servinfo->ai_protocol)) == -1)
    {
        perror("socket:");
        exit(EXIT_FAILURE);
    }

    int nb_of_try =0;
    do
    {

        if ((msgLength = strlen(msg)) > 0)
        {
            // Envoi de la ligne au serveur
            if (sendto(socketDescriptor, msg, msgLength, 0,
                       servinfo->ai_addr, servinfo->ai_addrlen) == -1)
            {
                perror("sendto:");
                close(socketDescriptor);
                exit(EXIT_FAILURE);
            }
            // Attente de la réponse pendant une seconde.
            FD_ZERO(&readSet);
            FD_SET(socketDescriptor, &readSet);
            timeVal.tv_sec = 1;
            timeVal.tv_usec = 0;
            if (select(socketDescriptor+1, &readSet, NULL, NULL, &timeVal))
            {

                printf("Message sent : %s\n", msg);
                break;
            }

        }
        nb_of_try++;
    }
    while(nb_of_try < 10);

    close(socketDescriptor);
    return EXIT_SUCCESS;
}
