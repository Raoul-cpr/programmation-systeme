#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define NUM_DIGITS 1000

void findMaxProduct(char *number, int length, int y, char *result) {
    long long maxProduct = 0;
    
    for (int i = 0; i <= length - y; i++) {
        long long product = 1;
        for (int j = 0; j < y; j++) {
            product *= number[i + j] - '0';
        }

        if (product > maxProduct) {
            maxProduct = product;
            strncpy(result, number + i, y);
            result[y] = '\0';
        }
    }
}

int main() {
    char number[NUM_DIGITS + 1] = "7316717653133062491922511967442657474235534919493496983520312774506326239578318016984801869478851843858615607891129494954595017379583319528532088055111254069874715852386305071569329096329522744304355766896648950445244523161731856403098711121722383113622298934233803081353362766142828064444866452387493035890729629049156044077239071381051585930796086670172427121883998797908792274921901699720888093776657273330010533678812202354218097512545405947522435258490771167055601360483958644670632441572215539753697817977846174064955149290862569321978468622482839722413756570560574902614079729686524145351004748216637048440319989000889524345065854122758866688116427171479924442928230863465674813919123162824586178664583591245665294765456828489128831426076900422421902267105562632111110937054421750694165896040807198403850962455444362981230987879927244284909188845801561660979191338754992005240636899125607176060588611646710940507754100225698315520005593572972571636269561882670428252483600823257530420752963450"; 


    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    // Création du socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Liaison du socket à l'adresse du serveur
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Erreur lors de la liaison du socket à l'adresse");
        exit(EXIT_FAILURE);
    }

    // Écoute des connexions entrantes
    if (listen(serverSocket, 1) == -1) {
        perror("Erreur lors de l'écoute des connexions entrantes");
        exit(EXIT_FAILURE);
    }

    printf("Serveur en attente de connexions...\n");

    while (1) {
        // Accepte une connexion entrante
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen);
        if (clientSocket == -1) {
            perror("Erreur lors de l'acceptation de la connexion");
            exit(EXIT_FAILURE);
        }

        // Réception de la demande du client
        int requestedDigits;
        recv(clientSocket, &requestedDigits, sizeof(int), 0);

        // Envoie des chiffres au client
        send(clientSocket, number, requestedDigits, 0);

        // Réception du résultat du client
        char clientResult[requestedDigits + 1];
        recv(clientSocket, clientResult, requestedDigits, 0);
        clientResult[requestedDigits] = '\0';

        printf("Résultat reçu du client : %s\n", clientResult);

        // Fermeture du socket du client
        close(clientSocket);
    }

    // Fermeture du socket du serveur
    close(serverSocket);

    return 0;
}
