#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Structure contenant les données partagées entre les threads
typedef struct {
    FILE* file;
    pthread_mutex_t lock;
} SharedData;

// Fonction pour écrire des nombres pairs dans le fichier
void* write_even_numbers(void* arg) {
    SharedData* data = (SharedData*)arg;

    for (int i = 2; i <= 100; i += 2) {
        pthread_mutex_lock(&data->lock);
        fprintf(data->file, "%d\n", i);
        pthread_mutex_unlock(&data->lock);
    }

    pthread_exit(NULL);
}

// Fonction pour écrire des nombres impairs dans le fichier
void* write_odd_numbers(void* arg) {
    SharedData* data = (SharedData*)arg;

    for (int i = 1; i <= 100; i += 2) {
        pthread_mutex_lock(&data->lock);
        fprintf(data->file, "%d\n", i);
        pthread_mutex_unlock(&data->lock);
    }

    pthread_exit(NULL);
}

int main() {
    FILE* file = fopen("exercice-1-2.txt", "w");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Initialiser les données partagées
    SharedData data;
    data.file = file;
    pthread_mutex_init(&data.lock, NULL);

    // Créer deux threads
    pthread_t even_thread, odd_thread;
    pthread_create(&even_thread, NULL, write_even_numbers, (void*)&data);
    pthread_create(&odd_thread, NULL, write_odd_numbers, (void*)&data);

    // Attendre que les threads se terminent
    pthread_join(even_thread, NULL);
    pthread_join(odd_thread, NULL);

    // Fermer le fichier et détruire le verrou
    fclose(file);
    pthread_mutex_destroy(&data.lock);

    printf("Le programme s'est exécuté correctement. Le résultat a été écrit dans le fichier 'exercice-1-2.txt'.\n");

    return 0;
}
