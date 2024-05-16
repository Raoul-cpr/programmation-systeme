#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

// Structure pour passer des données aux threads
typedef struct {
    int fd;  // Descripteur de fichier
    int max; // Nombre maximum à écrire
} ThreadData;

// Fonction exécutée par le thread des nombres pairs
void* write_even_numbers(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int fd = data->fd;

    for (int i = 2; i <= data->max; i += 2) {
        dprintf(fd, "%d ", i);
    }

    pthread_exit(NULL);
}

// Fonction exécutée par le thread des nombres impairs
void* write_odd_numbers(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int fd = data->fd;

    for (int i = 1; i <= data->max; i += 2) {
        dprintf(fd, "%d ", i);
    }

    pthread_exit(NULL);
}

int main() {
    const char* filename = "exercice-1-1.txt";
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    const int max_number = 20; // Exemple : écrire des nombres jusqu'à 20

    pthread_t thread_even, thread_odd;

    // Structure de données pour les threads
    ThreadData data = {
        .fd = fd,
        .max = max_number
    };

    // Créer les threads
    if (pthread_create(&thread_even, NULL, write_even_numbers, (void*)&data) != 0 ||
        pthread_create(&thread_odd, NULL, write_odd_numbers, (void*)&data) != 0) {
        perror("Erreur lors de la création des threads");
        exit(EXIT_FAILURE);
    }

    // Attendre que les threads terminent
    pthread_join(thread_even, NULL);
    pthread_join(thread_odd, NULL);

    close(fd);

    printf("Écriture des nombres pairs et impairs terminée dans le fichier %s.\n", filename);

    return 0;
}
