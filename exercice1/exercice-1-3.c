#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void writeNumbers(int start, int end, int fd) {
    for (int i = start; i <= end; i += 2) {
        dprintf(fd, "%d\n", i);
    }
}

int main() {
    // Ouvrir un fichier en écriture (créer s'il n'existe pas)
    int fd = open("exercice-1-3.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    
    if (fd == -1) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Créer un processus fils
    pid_t childPid = fork();

    if (childPid == -1) {
        perror("Erreur lors de la création du processus fils");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (childPid > 0) {
        // Code du processus père
        writeNumbers(2, 20, fd);
    } else {
        // Code du processus fils
        writeNumbers(1, 19, fd);
    }

    // Fermer le fichier
    close(fd);

    return 0;
}
