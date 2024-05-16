#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_CHAIRS 5

int numWaiting = 0;  // Nombre de clients en attente
int barberSleeping = 0;  // Indique si le barbier dort

// Mutex pour protéger les variables partagées
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Fonction exécutée par le client
void *client(void *arg) {
    printf("Client : Je suis un nouveau client.\n");

    pthread_mutex_lock(&mutex);

    if (numWaiting < MAX_CHAIRS) {
        // Il y a de la place dans la salle d'attente
        numWaiting++;
        printf("Client : Je m'installe dans la salle d'attente.\n");
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

    pthread_mutex_unlock(&mutex);

    // Attends d'être rasé
    while (1) {
        pthread_mutex_lock(&mutex);
        if (!barberSleeping) {
            // Le client est rasé
            printf("Client : Je suis rasé, je m'en vais.\n");
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
        // Attente active
    }

    return NULL;
}

// Fonction exécutée par le barbier
void *barber(void *arg) {
    printf("Barbier : Je suis le barbier.\n");

    while (1) {
        pthread_mutex_lock(&mutex);
        if (numWaiting > 0) {
            // Il y a des clients en attente
            barberSleeping = 0;
            numWaiting--;

            printf("Barbier : Invite un client dans le salon.\n");

            // Rase le client
            printf("Barbier : Rase le client.\n");

            // Le client est rasé
            printf("Barbier : Le client est rasé, retourne dormir.\n");
        } else {
            // La salle d'attente est vide, le barbier dort
            barberSleeping = 1;
            printf("Barbier : Dort.\n");
        }
        pthread_mutex_unlock(&mutex);
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
    sleep(2);  // Attendez pendant un certain temps pour la démonstration
    pthread_cancel(barberThread);
    pthread_cancel(clientThread1);
    pthread_cancel(clientThread2);

    // Joignez-vous aux threads pour vous assurer qu'ils se terminent correctement
    pthread_join(barberThread, NULL);
    pthread_join(clientThread1, NULL);
    pthread_join(clientThread2, NULL);

    // Détruisez le mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}

