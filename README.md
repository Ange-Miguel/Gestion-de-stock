# Gestion de stock en C

Application console en langage C permettant de gérer le stock d'un magasin.

## Fonctionnalités
- Ajouter, afficher, rechercher et supprimer un produit
- Entrées de stock (réapprovisionnement) et sorties (ventes)
- Modification du prix
- Alertes de stock faible (seuil configurable)
- Calcul de la valeur totale du stock
- Sauvegarde automatique dans `stock.txt`

## Compilation et exécution
```bash
gcc gestion_stock.c -o gestion_stock -Wall
./gestion_stock
```

## Concepts utilisés
Structures (`struct`), tableaux, fichiers, saisies sécurisées (`fgets`, `strtol`), manipulation de chaînes, menu avec `switch`.

## Format du fichier de données
Une ligne par produit : `id;nom;quantite;prix`

## Auteur
Ange, étudiant en Licence Pro Systèmes Embarqués et IoT, IFRI-UAC (Bénin)
