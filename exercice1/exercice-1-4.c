#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

// Déclaration d'un sémaphore
sem_t *sem;

void writeNumbers(int start, int end, int fd, int increment) {
    for (int i = start; i <= end; i += increment) {
        // Attente du sémaphore
        sem_wait(sem);
        dprintf(fd, "%d\n", i);
        // Libération du sémaphore
        sem_post(sem);
    }
}

int main() {
    // Initialisation du sémaphore avec une valeur initiale de 1
    sem = sem_open("/my_semaphore", O_CREAT, 0666, 1);

    if (sem == SEM_FAILED) {
        perror("Erreur lors de l'initialisation du sémaphore");
        exit(EXIT_FAILURE);
    }

    // Ouvrir un fichier en écriture (créer s'il n'existe pas)
    int fd = open("exercice-1-4.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    
    if (fd == -1) {
        perror("Erreur lors de l'ouverture du fichier");
        sem_close(sem);
        sem_unlink("/my_semaphore");
        exit(EXIT_FAILURE);
    }

    // Créer un processus fils
    pid_t childPid = fork();

    if (childPid == -1) {
        perror("Erreur lors de la création du processus fils");
        close(fd);
        sem_close(sem);
        sem_unlink("/my_semaphore");
        exit(EXIT_FAILURE);
    }

    if (childPid > 0) {
        // Code du processus père
        writeNumbers(2, 20, fd, 2);
    } else {
        // Code du processus fils
        writeNumbers(1, 19, fd, 2);
    }

    // Fermer le fichier
    close(fd);

    // Fermer et détruire le sémaphore
    sem_close(sem);
    sem_unlink("/my_semaphore");

    return 0;
}
