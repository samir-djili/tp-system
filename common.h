#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>

/* Fichier et projet pour générer la clé IPC */
#define FICHIER_CLE "/tmp/ipc_key_file"
#define PROJET_ID 'P'

/* Indices des sémaphores dans l'ensemble */
#define SEM_VIDE 0
#define SEM_PLEIN 1

/* Structure pour le tampon partagé */
typedef struct {
    int T;  /* Le buffer d'une seule case */
} TamponPartage;

/* Union pour semctl (requise sur certains systèmes) */
union semun {
    int val;                    /* valeur pour SETVAL */
    struct semid_ds *buf;       /* buffer pour IPC_STAT, IPC_SET */
    unsigned short *array;      /* tableau pour GETALL, SETALL */
};

/* Fonction pour opération P (wait) sur un sémaphore */
void P(int sem_id, int sem_num) {
    struct sembuf operation;
    operation.sem_num = sem_num;
    operation.sem_op = -1;  /* Décrémenter */
    operation.sem_flg = 0;
    
    if (semop(sem_id, &operation, 1) == -1) {
        perror("Erreur lors de l'opération P");
        exit(EXIT_FAILURE);
    }
}

/* Fonction pour opération V (signal) sur un sémaphore */
void V(int sem_id, int sem_num) {
    struct sembuf operation;
    operation.sem_num = sem_num;
    operation.sem_op = 1;   /* Incrémenter */
    operation.sem_flg = 0;
    
    if (semop(sem_id, &operation, 1) == -1) {
        perror("Erreur lors de l'opération V");
        exit(EXIT_FAILURE);
    }
}

#endif /* COMMON_H */
