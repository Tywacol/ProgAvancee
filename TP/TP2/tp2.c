#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 10
#define MAX_PERSONNES 100

typedef struct {
    int annee, mois, jour;
} Date;

typedef struct {
    char nom[SIZE], prenom[SIZE], tel[SIZE];
    Date Naissance;
} Personne;

typedef Personne Annuaire[MAX_PERSONNES];


/* On devrait utiliser malloc pour passer les structs et eviter les copies mais
 * nous ne sommes pas (?) encore au cours 3.
 * */
Date lire_date()
{
    Date d;
    scanf("%d%d%d",
          &d.jour,
          &d.mois,
          &d.annee);
    return d;
}

int cmpDate(Date d1, Date d2)
{
    if (d1.annee < d2.annee) {
        return -1;
    }
    if (d2.annee < d1.annee) {
        return 1;
    }
    if (d1.mois < d2.mois) {
        return -1;
    }
    if (d2.mois < d1.mois) {
        return 1;
    }
    if (d1.jour < d2.jour) {
        return -1;
    }
    if (d2.jour < d1.jour) {
        return 1;
    }
    return 0;
}

void afficher_date(Date date)
{
    printf("%02d/%02d/%d\n",
           date.jour,
           date.mois,
           date.annee);
}


int lire_personne(Personne *ptr_pers, FILE *fd)
{
    if (fd != NULL) {
        fscanf(fd, "%s%s%d%d%d%s",
                  ptr_pers->nom,
                  ptr_pers->prenom,
                  &ptr_pers->Naissance.jour,
                  &ptr_pers->Naissance.mois,
                  &ptr_pers->Naissance.annee,
                  ptr_pers->tel);
        if (!feof(fd)){
            return 0;
        }
    }
    return 1;
}

int construire_annuaire(Annuaire annuaire, int current_pos, char* f_name)
{
    FILE* fd;
    fd = fopen(f_name, "r");
    while (lire_personne(&annuaire[current_pos],fd) == 0) {
        current_pos++;
    }
    fclose(fd);
    return current_pos;
}

void affiche_personne(Personne p)
{
    printf("%s %s. %02d/%02d/%d. Tel. %s\n",
           p.nom,
           p.prenom,
           p.Naissance.jour,
           p.Naissance.mois,
           p.Naissance.annee,
           p.tel);
}


void affiche_annaire(Annuaire annuaire, int last_pos)
{
    for (int i = 0; i < last_pos; ++i) {
        printf("Personne %d : ", i);
        affiche_personne(annuaire[i]);
    }

}


void permuter_pers(Personne *p1, Personne *p2)
{
    Personne tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}

void bubble_sort(Annuaire annuaire, int nb_personnes)
{
    for (int i = 0; i < nb_personnes; ++i) {
        for (int j = 0; j < nb_personnes - i - 1; ++j) {
            if (cmpDate(annuaire[j].Naissance, annuaire[j + 1].Naissance) > 0) {
                permuter_pers(&annuaire[j], &annuaire[j + 1]);
            }
        }
    }
}

void bubble_sort_nom(Annuaire annuaire, int nb_personnes)
{
    for (int i = 0; i < nb_personnes; ++i) {
        for (int j = 0; j < nb_personnes - i - 1; ++j) {
            if (strcmp(annuaire[j].nom, annuaire[j + 1].nom) > 0) {
                permuter_pers(&annuaire[j], &annuaire[j + 1]);
            }
        }
    }
}

int rech_dico(char * nom_pers, int  size_nom_pers, Annuaire annuaire, int size_annuaire) {
    int debut = 0;
    int fin = size_annuaire;
    int mil;
    while (debut <= fin) {
        mil = (fin + debut) / 2;
        if (strcmp(annuaire[mil].nom,nom_pers) == 0) {
            return mil;
        } else if (strcmp(annuaire[mil].nom,nom_pers) > 0) {
            fin = mil - 1;
        } else {
            debut = mil + 1; /* + 1 pour terminaison */
        }
    }
    return -1;
}

void modif_tel(Personne *p, char *new_num) {
    strcpy(p->tel,new_num);
}

void sauvegarde_annuaire(Annuaire annuaire, char * f_name, int size_annuaire) {
    FILE * fd;
    int i = 0;
    /* open the file for writing*/
    fd = fopen (f_name,"w");
    while (i < size_annuaire) {
        fprintf(fd, "%s\n%s\n%d\n%d\n%d\n%s\n",
                  annuaire[i].nom,
                  annuaire[i].prenom,
                  annuaire[i].Naissance.jour,
                  annuaire[i].Naissance.mois,
                  annuaire[i].Naissance.annee,
                  annuaire[i].tel);
        i++;
    }
 
    /* close the file*/  
    fclose (fd);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Utilisation : %s fichier.txt ! \n",argv[0]);
        printf("Nombre d'arguments fournis : %d\n",argc);

    }
    Annuaire annuaire;

    FILE *fp;
    fp = fopen("annu.txt", "r");
    int size = construire_annuaire(annuaire, 0,argv[1]);
    printf("Avant tri : \n");
    
    size++; // A DEBUGGER : si feof(fd) la personne a ete importe mais la fonction retourne 1 et donc le compteur ne prend pas en compte le dernier.. 
    affiche_annaire(annuaire, size);
    printf("Apres tri : \n");
    bubble_sort_nom(annuaire, size);
    //bubble_sort_nom(annuaire, size);
    affiche_annaire(annuaire, size);
    char nom_cherche[SIZE];
    printf("Entrez un nom : ");
    scanf("%s",nom_cherche);
    int pos = rech_dico(nom_cherche, SIZE, annuaire, size);
    if (pos != -1) {
        printf("Personne trouvee à l'indice %d :\n",pos);
        affiche_personne(annuaire[pos]);
        printf("Modifier son numéro ?\n");
        int rep;
        scanf("%d", &rep);
        if (rep) {
            printf("Nouveau numéro (1=oui, 0=non) ? : ");
            char new_num[size];
            scanf("%s", new_num);
            modif_tel(&annuaire[pos], new_num);
            printf("Modification effectuee. Nouveau profil :\n");
            affiche_personne(annuaire[pos]);
            sauvegarde_annuaire(annuaire, "nouveau_annu.txt", size);
            printf("Fichier sauvegarde.\n");
        }   
    } else {
        printf("inconnu\n");
    }
    return 0;
}
