#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define NUM_DIGITS 1000
#define ADJACENT_DIGITS 4

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char serverResponse[NUM_DIGITS];
    int requestedDigits = NUM_DIGITS;
    char clientResult[ADJACENT_DIGITS + 1];

    // Création du socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connexion au serveur
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Erreur lors de la connexion au serveur");
        exit(EXIT_FAILURE);
    }

    // Envoie de la demande au serveur
    send(clientSocket, &requestedDigits, sizeof(int), 0);

    // Réception des chiffres du serveur
    recv(clientSocket, serverResponse, requestedDigits, 0);

    // Recherche des chiffres adjacents ayant le plus grand produit
    long long maxProduct = 0;
    for (int i = 0; i <= NUM_DIGITS - ADJACENT_DIGITS; i++) {
        long long product = 1;
        for (int j = 0; j < ADJACENT_DIGITS; j++) {
            product *= serverResponse[i + j] - '0';
        }

        if (product > maxProduct) {
            maxProduct = product;
            strncpy(clientResult, serverResponse + i, ADJACENT_DIGITS);
            clientResult[ADJACENT_DIGITS] = '\0';
        }
    }

    // Envoie du résultat au serveur
    send(clientSocket, clientResult, ADJACENT_DIGITS, 0);

    // Fermeture du socket
    close(clientSocket);

    return 0;
}
