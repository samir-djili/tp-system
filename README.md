# TP2 - Producteur/Consommateur avec IPC System V

## Description
Implémentation du problème classique du producteur/consommateur en C utilisant les mécanismes IPC (Inter-Process Communication) System V d'UNIX/Linux :
- **Sémaphores** pour la synchronisation
- **Mémoire partagée** pour le tampon de communication

## Structure du projet

```
├── common.h              # Fichier d'en-tête commun avec les définitions partagées
├── principal.c           # Programme principal qui crée les ressources IPC et les processus
├── Pgme_Producteur.c     # Programme du processus producteur
├── Pgme_Consommateur.c   # Programme du processus consommateur
├── Makefile              # Fichier de compilation
└── README.md             # Ce fichier
```

## Fonctionnement

### Synchronisation
- **Sémaphore `svide`** : Initialisé à 1, indique si le tampon est vide
- **Sémaphore `splein`** : Initialisé à 0, indique si le tampon contient un objet

### Processus Producteur
1. Attend que le tampon soit vide (`P(svide)`)
2. Dépose un objet dans le tampon
3. Signale que le tampon est plein (`V(splein)`)
4. Répète à l'infini

### Processus Consommateur
1. Attend que le tampon soit plein (`P(splein)`)
2. Retire l'objet du tampon
3. Signale que le tampon est vide (`V(svide)`)
4. Répète à l'infini

## Compilation

### Compilation simple
```bash
make
```

### Compilation avec nettoyage préalable
```bash
make rebuild
```

## Exécution

### Lancer le programme
```bash
./principal
```
ou
```bash
make run
```

### Arrêter le programme
Appuyez sur `Ctrl+C` pour arrêter proprement le programme. Le nettoyage des ressources IPC sera effectué automatiquement.

## Commandes utiles

### Voir les ressources IPC utilisées
```bash
make ipc
```
ou manuellement :
```bash
ipcs -m    # Voir la mémoire partagée
ipcs -s    # Voir les sémaphores
```

### Nettoyer les ressources IPC orphelines
```bash
make clean-ipc
```
ou manuellement :
```bash
ipcrm -m <shm_id>    # Supprimer un segment de mémoire partagée
ipcrm -s <sem_id>    # Supprimer un ensemble de sémaphores
```

### Nettoyer les fichiers compilés
```bash
make clean
```

## Exemple de sortie

```
=== Programme Principal - Producteur/Consommateur ===

Clé IPC générée: 1234567
Ensemble de sémaphores créé (ID: 0)
Sémaphore svide initialisé à 1
Sémaphore splein initialisé à 0
Mémoire partagée créée (ID: 1)
Mémoire partagée attachée

=== Création des processus ===
Processus Producteur créé (PID: 1234)
Processus Consommateur créé (PID: 1235)

=== Les processus sont en cours d'exécution ===
Appuyez sur Ctrl+C pour arrêter le programme

[PRODUCTEUR] Démarrage du processus producteur (PID: 1234)
[PRODUCTEUR] Connexion aux ressources IPC réussie
[PRODUCTEUR] Début de la production...

[CONSOMMATEUR] Démarrage du processus consommateur (PID: 1235)
[CONSOMMATEUR] Connexion aux ressources IPC réussie
[CONSOMMATEUR] Début de la consommation...

[PRODUCTEUR] Le producteur vient de déposer un objet: 1
[CONSOMMATEUR] Le consommateur vient de retirer un objet: 1
[PRODUCTEUR] Le producteur vient de déposer un objet: 2
[CONSOMMATEUR] Le consommateur vient de retirer un objet: 2
...
```

## Détails techniques

### Ressources IPC créées
- **1 ensemble de sémaphores** contenant 2 sémaphores (svide et splein)
- **1 segment de mémoire partagée** pour le tampon T (une seule case de type entier)

### Clé IPC
La clé IPC est générée avec `ftok()` à partir de :
- Fichier : `/tmp/ipc_key_file`
- Projet : `'P'`

### Nettoyage automatique
Le programme principal gère le nettoyage automatique des ressources IPC :
- À la fin normale du programme
- En cas d'interruption (Ctrl+C)
- En cas d'erreur lors de la création des processus

## Notes importantes

⚠️ **Ce programme est conçu pour Linux/UNIX** et utilise les IPC System V qui ne sont pas disponibles sur Windows natif. Pour l'exécuter sous Windows, utilisez :
- WSL (Windows Subsystem for Linux)
- Une machine virtuelle Linux
- Docker avec une image Linux

