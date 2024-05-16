#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_CHAIRS 5

int numWaiting = 0;  // Nombre de clients en attente
int barberSleeping = 0;  // Indique si le barbier dort
int clientSeated = 0;  // Indique si le client est assis sur le siège

// Mutex pour protéger les variables partagées
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Condition pour signaler que le client est prêt
pthread_cond_t clientReady = PTHREAD_COND_INITIALIZER;

// Condition pour signaler que le barbier est prêt
pthread_cond_t barberReady = PTHREAD_COND_INITIALIZER;

// Fonction exécutée par le client
void *client(void *arg) {
    printf("Client : Je suis un nouveau client.\n");

    pthread_mutex_lock(&mutex);

    if (numWaiting < MAX_CHAIRS) {
        // Il y a de la place dans la salle d'attente
        numWaiting++;
        printf("Client : Je m'installe dans la salle d'attente. Clients en attente : %d\n", numWaiting);
    } else {
        // Pas de place dans la salle d'attente, le client rentre chez lui
        printf("Client : Pas de place dans la salle d'attente, je rentre chez moi.\n");
        pthread_mutex_unlock(&mutex);
        return NULL;
    }

    if (!barberSleeping) {
        // Le barbier est réveillé, réveille-le
        printf("Client : Réveille le barbier.\n");
        barberSleeping = 1;
    }

    pthread_cond_signal(&clientReady);  // Signal au barbier que le client est prêt
    printf("Client : Attends d'être rasé.\n");

    while (!clientSeated) {
        // Attente active
        pthread_cond_wait(&barberReady, &mutex);  // Attend que le barbier soit prêt
    }

    pthread_mutex_unlock(&mutex);

    // Le client est rasé
    printf("Client : Je suis rasé, je m'en vais. Clients en attente : %d\n", numWaiting);

    return NULL;
}

// Fonction exécutée par le barbier
void *barber(void *arg) {
    printf("Barbier : Je suis le barbier.\n");

    while (1) {
        pthread_mutex_lock(&mutex);
        while (numWaiting == 0) {
            // La salle d'attente est vide, le barbier dort
            barberSleeping = 1;
            printf("Barbier : Dort.\n");
            pthread_cond_wait(&clientReady, &mutex);  // Attend qu'un client soit prêt
        }

        // Il y a des clients en attente
        barberSleeping = 0;
        numWaiting--;

        printf("Barbier : Invite un client dans le salon. Clients en attente : %d\n", numWaiting);

        // Rase le client
        printf("Barbier : Rase le client.\n");

        clientSeated = 1;  // Indique au client qu'il peut s'asseoir
        pthread_cond_signal(&barberReady);  // Signal au client que le barbier est prêt
        pthread_mutex_unlock(&mutex);

        // Le client est rasé
        printf("Barbier : Le client est rasé, retourne dormir.\n");

        // Réinitialise la variable pour le client suivant
        clientSeated = 0;
    }

    return NULL;
}

int main() {
    // Créez des threads pour le barbier et quelques clients
    pthread_t barberThread, clientThread1, clientThread2;

    pthread_create(&barberThread, NULL, barber, NULL);
    pthread_create(&clientThread1, NULL, client, NULL);
    pthread_create(&clientThread2, NULL, client, NULL);

    // Attendez que les threads se terminent
    sleep(5);  // Attendez pendant un certain temps pour la démonstration
    pthread_cancel(barberThread);
    pthread_cancel(clientThread1);
    pthread_cancel(clientThread2);

    // Joignez-vous aux threads pour vous assurer qu'ils se terminent correctement
    pthread_join(barberThread, NULL);
    pthread_join(clientThread1, NULL);
    pthread_join(clientThread2, NULL);

    // Détruisez le mutex et les conditions
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&clientReady);
    pthread_cond_destroy(&barberReady);

    return 0;
}
