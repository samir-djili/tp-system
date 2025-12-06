#include "common.h"
#include <sys/wait.h>
#include <signal.h>

/* Variables globales pour le nettoyage */
int sem_id = -1;
int shm_id = -1;

/* Fonction de nettoyage appelée avant la sortie */
void cleanup() {
    printf("\n=== Nettoyage des ressources IPC ===\n");
    
    /* Destruction des sémaphores */
    if (sem_id != -1) {
        if (semctl(sem_id, 0, IPC_RMID, 0) == -1) {
            perror("Erreur lors de la destruction des sémaphores");
        } else {
            printf("Sémaphores détruits avec succès\n");
        }
    }
    
    /* Destruction de la mémoire partagée */
    if (shm_id != -1) {
        if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
            perror("Erreur lors de la destruction de la mémoire partagée");
        } else {
            printf("Mémoire partagée détruite avec succès\n");
        }
    }
    
    printf("Nettoyage terminé\n");
}

/* Gestionnaire de signal pour interruption (Ctrl+C) */
void signal_handler(int signum) {
    printf("\n\nSignal d'interruption reçu...\n");
    cleanup();
    exit(0);
}

int main() {
    key_t cle;
    TamponPartage *tampon;
    pid_t pid_producteur, pid_consommateur;
    union semun arg;
    
    printf("=== Programme Principal - Producteur/Consommateur ===\n\n");
    
    /* Installer le gestionnaire de signal */
    signal(SIGINT, signal_handler);
    
    /* 1. Créer le fichier de clé s'il n'existe pas */
    FILE *f = fopen(FICHIER_CLE, "w");
    if (f == NULL) {
        perror("Erreur lors de la création du fichier de clé");
        exit(EXIT_FAILURE);
    }
    fclose(f);
    
    /* 2. Générer une clé IPC unique */
    cle = ftok(FICHIER_CLE, PROJET_ID);
    if (cle == -1) {
        perror("Erreur lors de la génération de la clé");
        exit(EXIT_FAILURE);
    }
    printf("Clé IPC générée: %d\n", cle);
    
    /* 3. Créer l'ensemble de sémaphores (2 sémaphores: svide et splein) */
    sem_id = semget(cle, 2, IPC_CREAT | IPC_EXCL | 0666);
    if (sem_id == -1) {
        perror("Erreur lors de la création des sémaphores");
        exit(EXIT_FAILURE);
    }
    printf("Ensemble de sémaphores créé (ID: %d)\n", sem_id);
    
    /* 4. Initialiser les sémaphores */
    /* svide (SEM_VIDE) = 1 (tampon initialement vide) */
    arg.val = 1;
    if (semctl(sem_id, SEM_VIDE, SETVAL, arg) == -1) {
        perror("Erreur lors de l'initialisation du sémaphore svide");
        cleanup();
        exit(EXIT_FAILURE);
    }
    printf("Sémaphore svide initialisé à 1\n");
    
    /* splein (SEM_PLEIN) = 0 (aucun objet au début) */
    arg.val = 0;
    if (semctl(sem_id, SEM_PLEIN, SETVAL, arg) == -1) {
        perror("Erreur lors de l'initialisation du sémaphore splein");
        cleanup();
        exit(EXIT_FAILURE);
    }
    printf("Sémaphore splein initialisé à 0\n");
    
    /* 5. Créer le segment de mémoire partagée pour le tampon */
    shm_id = shmget(cle, sizeof(TamponPartage), IPC_CREAT | IPC_EXCL | 0666);
    if (shm_id == -1) {
        perror("Erreur lors de la création de la mémoire partagée");
        cleanup();
        exit(EXIT_FAILURE);
    }
    printf("Mémoire partagée créée (ID: %d)\n", shm_id);
    
    /* 6. Attacher le segment de mémoire partagée */
    tampon = (TamponPartage *) shmat(shm_id, NULL, 0);
    if (tampon == (TamponPartage *) -1) {
        perror("Erreur lors de l'attachement de la mémoire partagée");
        cleanup();
        exit(EXIT_FAILURE);
    }
    printf("Mémoire partagée attachée\n");
    
    /* Initialiser le tampon */
    tampon->T = 0;
    
    /* Détacher la mémoire partagée du processus principal */
    if (shmdt(tampon) == -1) {
        perror("Erreur lors du détachement de la mémoire partagée");
    }
    
    printf("\n=== Création des processus ===\n");
    
    /* 7. Créer le processus Producteur */
    pid_producteur = fork();
    if (pid_producteur == -1) {
        perror("Erreur lors de la création du processus producteur");
        cleanup();
        exit(EXIT_FAILURE);
    }
    
    if (pid_producteur == 0) {
        /* Processus fils - Producteur */
        execl("./Pgme_Producteur", "Pgme_Producteur", NULL);
        /* Si execl échoue */
        perror("Erreur lors de l'exécution du programme producteur");
        exit(EXIT_FAILURE);
    }
    
    printf("Processus Producteur créé (PID: %d)\n", pid_producteur);
    
    /* 8. Créer le processus Consommateur */
    pid_consommateur = fork();
    if (pid_consommateur == -1) {
        perror("Erreur lors de la création du processus consommateur");
        kill(pid_producteur, SIGTERM);  /* Arrêter le producteur */
        cleanup();
        exit(EXIT_FAILURE);
    }
    
    if (pid_consommateur == 0) {
        /* Processus fils - Consommateur */
        execl("./Pgme_Consommateur", "Pgme_Consommateur", NULL);
        /* Si execl échoue */
        perror("Erreur lors de l'exécution du programme consommateur");
        exit(EXIT_FAILURE);
    }
    
    printf("Processus Consommateur créé (PID: %d)\n", pid_consommateur);
    
    printf("\n=== Les processus sont en cours d'exécution ===\n");
    printf("Appuyez sur Ctrl+C pour arrêter le programme\n\n");
    
    /* 9. Attendre que les processus se terminent (ils tournent en boucle infinie) */
    int status;
    pid_t pid_termine;
    
    /* Attendre qu'un des processus se termine */
    pid_termine = wait(&status);
    
    if (pid_termine == pid_producteur) {
        printf("\nLe processus Producteur s'est terminé\n");
        kill(pid_consommateur, SIGTERM);
    } else {
        printf("\nLe processus Consommateur s'est terminé\n");
        kill(pid_producteur, SIGTERM);
    }
    
    /* Attendre le second processus */
    wait(&status);
    
    /* 10. Nettoyer les ressources IPC */
    cleanup();
    
    return 0;
}
