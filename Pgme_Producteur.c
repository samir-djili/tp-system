#include "common.h"

int main() {
    key_t cle;
    int sem_id, shm_id;
    TamponPartage *tampon;
    int Mp;  /* Message à produire */
    
    printf("[PRODUCTEUR] Démarrage du processus producteur (PID: %d)\n", getpid());
    
    /* 1. Récupérer la clé IPC */
    cle = ftok(FICHIER_CLE, PROJET_ID);
    if (cle == -1) {
        perror("[PRODUCTEUR] Erreur lors de la génération de la clé");
        exit(EXIT_FAILURE);
    }
    
    /* 2. Récupérer l'ID de l'ensemble de sémaphores */
    sem_id = semget(cle, 2, 0666);
    if (sem_id == -1) {
        perror("[PRODUCTEUR] Erreur lors de la récupération des sémaphores");
        exit(EXIT_FAILURE);
    }
    
    /* 3. Récupérer l'ID du segment de mémoire partagée */
    shm_id = shmget(cle, sizeof(TamponPartage), 0666);
    if (shm_id == -1) {
        perror("[PRODUCTEUR] Erreur lors de la récupération de la mémoire partagée");
        exit(EXIT_FAILURE);
    }
    
    /* 4. Attacher le segment de mémoire partagée */
    tampon = (TamponPartage *) shmat(shm_id, NULL, 0);
    if (tampon == (TamponPartage *) -1) {
        perror("[PRODUCTEUR] Erreur lors de l'attachement de la mémoire partagée");
        exit(EXIT_FAILURE);
    }
    
    printf("[PRODUCTEUR] Connexion aux ressources IPC réussie\n");
    printf("[PRODUCTEUR] Début de la production...\n\n");
    
    /* 5. Boucle de production */
    Mp = 1;  /* Commencer la production à partir de 1 */
    
    while (1) {
        /* Produire un objet */
        /* (Simulation : Mp est simplement incrémenté) */
        
        /* P(svide) - Attendre que le tampon soit vide */
        P(sem_id, SEM_VIDE);
        
        /* Section critique : Déposer l'objet dans le tampon */
        tampon->T = Mp;
        printf("[PRODUCTEUR] Le producteur vient de déposer un objet: %d\n", Mp);
        
        /* V(splein) - Signaler que le tampon est plein */
        V(sem_id, SEM_PLEIN);
        
        /* Préparer le prochain objet à produire */
        Mp++;
        
        /* Pause pour visualiser le comportement (optionnel) */
        sleep(1);
    }
    
    /* Détacher la mémoire partagée (code jamais atteint car boucle infinie) */
    shmdt(tampon);
    
    return 0;
}
