#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_CHAIRS 5

int numWaiting = 0;        // Nombre de clients en attente
int barberSleeping = 0;    // Indique si le barbier dort
int clientReady = 0;       // Indique si le client est prêt
int barberReady = 0;       // Indique si le barbier est prêt
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t clientCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t barberCond = PTHREAD_COND_INITIALIZER;

void *client_function(void *arg) {
    printf("Client : Je suis un nouveau client.\n");

    pthread_mutex_lock(&mutex);
    if (numWaiting < MAX_CHAIRS) {
        numWaiting++;
        printf("Client : Je m'installe dans la salle d'attente. Clients en attente : %d\n", numWaiting);
    } else {
        printf("Client : Pas de place dans la salle d'attente, je rentre chez moi.\n");
        pthread_mutex_unlock(&mutex);
        return NULL;
    }

    if (!barberSleeping) {
        printf("Client : Réveille le barbier.\n");
        barberSleeping = 1;
    }

    clientReady = 1;
    pthread_cond_signal(&clientCond);
    printf("Client : Attends d'être rasé.\n");

    while (!barberReady) {
        pthread_cond_wait(&barberCond, &mutex);
    }

    pthread_mutex_unlock(&mutex);
    printf("Client : Je suis rasé, je m'en vais. Clients en attente : %d\n", numWaiting);

    return NULL;
}

void *barber_function(void *arg) {
    printf("Barbier : Je suis le barbier.\n");

    while (1) {
        pthread_mutex_lock(&mutex);
        while (numWaiting == 0) {
            barberSleeping = 1;
            printf("Barbier : Dort.\n");
            pthread_cond_wait(&clientCond, &mutex);
        }

        barberSleeping = 0;
        numWaiting--;

        printf("Barbier : Invite un client dans le salon. Clients en attente : %d\n", numWaiting);

        barberReady = 1;
        pthread_cond_signal(&barberCond);
        pthread_mutex_unlock(&mutex);

        printf("Barbier : Rase le client.\n");

        barberReady = 0;
        printf("Barbier : Le client est rasé, retourne dormir.\n");
    }
}

int main() {
    pthread_t barberThread, clientThread1, clientThread2;

    pthread_create(&barberThread, NULL, barber_function, NULL);
    pthread_create(&clientThread1, NULL, client_function, NULL);
    pthread_create(&clientThread2, NULL, client_function, NULL);

    sleep(5);

    pthread_cancel(barberThread);
    pthread_cancel(clientThread1);
    pthread_cancel(clientThread2);

    pthread_join(barberThread, NULL);
    pthread_join(clientThread1, NULL);
    pthread_join(clientThread2, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&clientCond);
    pthread_cond_destroy(&barberCond);

    return 0;
}
