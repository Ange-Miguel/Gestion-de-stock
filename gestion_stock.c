/*
 * Gestion de stock d'un magasin - Langage C
 * Compilation : gcc gestion_stock.c -o gestion_stock -Wall
 * Execution   : ./gestion_stock
 *
 * Fonctionnalites :
 *  1. Ajouter un produit
 *  2. Afficher le stock
 *  3. Rechercher un produit (par nom)
 *  4. Entree de stock (reapprovisionnement)
 *  5. Sortie de stock (vente)
 *  6. Modifier le prix
 *  7. Supprimer un produit
 *  8. Produits en rupture / stock faible
 *  9. Valeur totale du stock
 *  Les donnees sont sauvegardees dans le fichier stock.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PRODUITS 500
#define TAILLE_NOM   50
#define FICHIER      "stock.txt"
#define SEUIL_ALERTE 5

typedef struct {
    int    id;
    char   nom[TAILLE_NOM];
    int    quantite;
    double prix;
} Produit;

static Produit stock[MAX_PRODUITS];
static int nb_produits = 0;
static int prochain_id = 1;

/* ------------------------- Saisies securisees ------------------------- */

static void lire_texte(const char *message, char *dest, int taille)
{
    do {
        printf("%s", message);
        if (fgets(dest, taille, stdin) == NULL) {
            dest[0] = '\0';
            return;
        }
        dest[strcspn(dest, "\n")] = '\0';
    } while (dest[0] == '\0');

    /* Le point-virgule sert de separateur dans le fichier */
    for (int i = 0; dest[i] != '\0'; i++) {
        if (dest[i] == ';') dest[i] = ',';
    }
}

static int lire_entier(const char *message, int min, int max)
{
    char buffer[64];
    char *fin;
    long valeur;

    while (1) {
        printf("%s", message);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) return min;
        valeur = strtol(buffer, &fin, 10);
        if (fin != buffer && (*fin == '\n' || *fin == '\0')
            && valeur >= min && valeur <= max) {
            return (int)valeur;
        }
        printf("  Entree invalide (valeur entre %d et %d attendue).\n", min, max);
    }
}

static double lire_reel(const char *message, double min)
{
    char buffer[64];
    char *fin;
    double valeur;

    while (1) {
        printf("%s", message);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) return min;
        valeur = strtod(buffer, &fin);
        if (fin != buffer && (*fin == '\n' || *fin == '\0') && valeur >= min) {
            return valeur;
        }
        printf("  Entree invalide (nombre >= %.2f attendu).\n", min);
    }
}

/* ------------------------- Outils internes ------------------------- */

static int trouver_index(int id)
{
    for (int i = 0; i < nb_produits; i++) {
        if (stock[i].id == id) return i;
    }
    return -1;
}

static void mettre_en_minuscules(const char *src, char *dest, int taille)
{
    int i;
    for (i = 0; src[i] != '\0' && i < taille - 1; i++) {
        dest[i] = (char)tolower((unsigned char)src[i]);
    }
    dest[i] = '\0';
}

static void afficher_entete(void)
{
    printf("\n%-5s %-30s %10s %12s\n", "ID", "Nom", "Quantite", "Prix");
    printf("---------------------------------------------------------------\n");
}

static void afficher_ligne(const Produit *p)
{
    printf("%-5d %-30s %10d %12.2f\n", p->id, p->nom, p->quantite, p->prix);
}

/* ------------------------- Sauvegarde / Chargement ------------------------- */

static void sauvegarder(void)
{
    FILE *f = fopen(FICHIER, "w");
    if (f == NULL) {
        printf("Erreur : impossible de sauvegarder dans %s\n", FICHIER);
        return;
    }
    for (int i = 0; i < nb_produits; i++) {
        fprintf(f, "%d;%s;%d;%.2f\n", stock[i].id, stock[i].nom,
                stock[i].quantite, stock[i].prix);
    }
    fclose(f);
}

static void charger(void)
{
    FILE *f = fopen(FICHIER, "r");
    if (f == NULL) return; /* premier lancement : pas encore de fichier */

    Produit p;
    while (nb_produits < MAX_PRODUITS &&
           fscanf(f, "%d;%49[^;];%d;%lf\n", &p.id, p.nom,
                  &p.quantite, &p.prix) == 4) {
        stock[nb_produits++] = p;
        if (p.id >= prochain_id) prochain_id = p.id + 1;
    }
    fclose(f);
}

/* ------------------------- Fonctionnalites ------------------------- */

static void ajouter_produit(void)
{
    if (nb_produits >= MAX_PRODUITS) {
        printf("Stock plein : impossible d'ajouter un produit.\n");
        return;
    }

    Produit p;
    p.id = prochain_id++;
    lire_texte("Nom du produit : ", p.nom, TAILLE_NOM);
    p.quantite = lire_entier("Quantite initiale : ", 0, 1000000);
    p.prix = lire_reel("Prix unitaire : ", 0.0);

    stock[nb_produits++] = p;
    sauvegarder();
    printf("Produit ajoute avec l'ID %d.\n", p.id);
}

static void afficher_stock(void)
{
    if (nb_produits == 0) {
        printf("Le stock est vide.\n");
        return;
    }
    afficher_entete();
    for (int i = 0; i < nb_produits; i++) {
        afficher_ligne(&stock[i]);
    }
}

static void rechercher_produit(void)
{
    char terme[TAILLE_NOM], terme_min[TAILLE_NOM], nom_min[TAILLE_NOM];
    int trouve = 0;

    lire_texte("Nom (ou partie du nom) a rechercher : ", terme, TAILLE_NOM);
    mettre_en_minuscules(terme, terme_min, TAILLE_NOM);

    for (int i = 0; i < nb_produits; i++) {
        mettre_en_minuscules(stock[i].nom, nom_min, TAILLE_NOM);
        if (strstr(nom_min, terme_min) != NULL) {
            if (!trouve) afficher_entete();
            afficher_ligne(&stock[i]);
            trouve = 1;
        }
    }
    if (!trouve) printf("Aucun produit trouve.\n");
}

static void entree_stock(void)
{
    int id = lire_entier("ID du produit : ", 1, 1000000);
    int idx = trouver_index(id);
    if (idx < 0) {
        printf("Produit introuvable.\n");
        return;
    }
    int qte = lire_entier("Quantite recue : ", 1, 1000000);
    stock[idx].quantite += qte;
    sauvegarder();
    printf("Nouveau stock de \"%s\" : %d\n", stock[idx].nom, stock[idx].quantite);
}

static void sortie_stock(void)
{
    int id = lire_entier("ID du produit : ", 1, 1000000);
    int idx = trouver_index(id);
    if (idx < 0) {
        printf("Produit introuvable.\n");
        return;
    }
    int qte = lire_entier("Quantite vendue : ", 1, 1000000);
    if (qte > stock[idx].quantite) {
        printf("Stock insuffisant (disponible : %d).\n", stock[idx].quantite);
        return;
    }
    stock[idx].quantite -= qte;
    sauvegarder();
    printf("Vente enregistree : %.2f\n", qte * stock[idx].prix);
    printf("Nouveau stock de \"%s\" : %d\n", stock[idx].nom, stock[idx].quantite);
    if (stock[idx].quantite <= SEUIL_ALERTE) {
        printf("ATTENTION : stock faible pour ce produit !\n");
    }
}

static void modifier_prix(void)
{
    int id = lire_entier("ID du produit : ", 1, 1000000);
    int idx = trouver_index(id);
    if (idx < 0) {
        printf("Produit introuvable.\n");
        return;
    }
    stock[idx].prix = lire_reel("Nouveau prix : ", 0.0);
    sauvegarder();
    printf("Prix mis a jour.\n");
}

static void supprimer_produit(void)
{
    int id = lire_entier("ID du produit a supprimer : ", 1, 1000000);
    int idx = trouver_index(id);
    if (idx < 0) {
        printf("Produit introuvable.\n");
        return;
    }
    char reponse[8];
    printf("Supprimer \"%s\" ? (o/n) : ", stock[idx].nom);
    if (fgets(reponse, sizeof(reponse), stdin) == NULL) return;
    if (tolower((unsigned char)reponse[0]) != 'o') {
        printf("Suppression annulee.\n");
        return;
    }
    for (int i = idx; i < nb_produits - 1; i++) {
        stock[i] = stock[i + 1];
    }
    nb_produits--;
    sauvegarder();
    printf("Produit supprime.\n");
}

static void afficher_alertes(void)
{
    int trouve = 0;
    for (int i = 0; i < nb_produits; i++) {
        if (stock[i].quantite <= SEUIL_ALERTE) {
            if (!trouve) {
                printf("\nProduits en rupture ou stock faible (<= %d) :", SEUIL_ALERTE);
                afficher_entete();
            }
            afficher_ligne(&stock[i]);
            trouve = 1;
        }
    }
    if (!trouve) printf("Aucun produit en stock faible.\n");
}

static void valeur_totale(void)
{
    double total = 0.0;
    for (int i = 0; i < nb_produits; i++) {
        total += stock[i].quantite * stock[i].prix;
    }
    printf("Valeur totale du stock : %.2f\n", total);
}

/* ------------------------- Menu principal ------------------------- */

static void afficher_menu(void)
{
    printf("\n======== GESTION DE STOCK ========\n");
    printf(" 1. Ajouter un produit\n");
    printf(" 2. Afficher le stock\n");
    printf(" 3. Rechercher un produit\n");
    printf(" 4. Entree de stock\n");
    printf(" 5. Sortie de stock (vente)\n");
    printf(" 6. Modifier le prix\n");
    printf(" 7. Supprimer un produit\n");
    printf(" 8. Alertes stock faible\n");
    printf(" 9. Valeur totale du stock\n");
    printf(" 0. Quitter\n");
    printf("==================================\n");
}

int main(void)
{
    int choix;

    charger();

    do {
        afficher_menu();
        choix = lire_entier("Votre choix : ", 0, 9);

        switch (choix) {
            case 1: ajouter_produit();    break;
            case 2: afficher_stock();     break;
            case 3: rechercher_produit(); break;
            case 4: entree_stock();       break;
            case 5: sortie_stock();       break;
            case 6: modifier_prix();      break;
            case 7: supprimer_produit();  break;
            case 8: afficher_alertes();   break;
            case 9: valeur_totale();      break;
            case 0: printf("Au revoir !\n"); break;
        }
    } while (choix != 0);

    return 0;
}
