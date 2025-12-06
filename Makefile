# Makefile pour le TP Producteur/Consommateur
# Compilation des programmes utilisant les IPC System V

CC = gcc
CFLAGS = -Wall -Wextra -g
TARGETS = principal Pgme_Producteur Pgme_Consommateur

# Règle par défaut : compiler tous les programmes
all: $(TARGETS)
	@echo "Compilation terminée avec succès!"
	@echo "Pour exécuter: ./principal"

# Compilation du programme principal
principal: principal.c common.h
	$(CC) $(CFLAGS) -o principal principal.c

# Compilation du programme producteur
Pgme_Producteur: Pgme_Producteur.c common.h
	$(CC) $(CFLAGS) -o Pgme_Producteur Pgme_Producteur.c

# Compilation du programme consommateur
Pgme_Consommateur: Pgme_Consommateur.c common.h
	$(CC) $(CFLAGS) -o Pgme_Consommateur Pgme_Consommateur.c

# Nettoyer les fichiers compilés
clean:
	rm -f $(TARGETS)
	rm -f /tmp/ipc_key_file
	@echo "Nettoyage terminé"

# Nettoyer et recompiler
rebuild: clean all

# Exécuter le programme
run: all
	./principal

# Afficher les ressources IPC utilisées
ipc:
	@echo "=== Segments de mémoire partagée ==="
	ipcs -m
	@echo ""
	@echo "=== Ensembles de sémaphores ==="
	ipcs -s

# Nettoyer les ressources IPC orphelines
clean-ipc:
	@echo "Nettoyage des ressources IPC..."
	@for id in $$(ipcs -m | grep $(USER) | awk '{print $$2}'); do \
		ipcrm -m $$id 2>/dev/null || true; \
	done
	@for id in $$(ipcs -s | grep $(USER) | awk '{print $$2}'); do \
		ipcrm -s $$id 2>/dev/null || true; \
	done
	@echo "Ressources IPC nettoyées"

.PHONY: all clean rebuild run ipc clean-ipc
