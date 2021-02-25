#include "../../inc/global/utilitaire.h"


int formaterEnRequete(requete requete, char** messages, int nbMessages) {
    char *elem1,*elem2;
    int i;
    //on prepare le nombre d'argument au cas ou on a un echec
    requete->nbArgs = 0;
    //on alloue les deux element en prenant le pire cas (chaine[0] == elem1 ou chaine[0] == elem2)
    elem1 = malloc(sizeof(char) * (strlen(messages[0]) + 1));
    elem2 = malloc(sizeof(char) * (strlen(messages[0]) + 1));
    //On divise la première ligne et renvoie 0 si un des deux elements est de longueur 0
    if(diviserChaine(messages[0],elem1,elem2,' ')) {
        free(elem1);
        free(elem2);
        libererPointeurChaine(messages,nbMessages);
        return BAD_REQUEST;
    }
    //On change le type en fonction du premier element, ou en renvoie une erreur si la requête n'a aucun type
    if(!strcmp(elem1,"REQUEST")) requete->type = REQUEST;
    else if (!strcmp(elem1,"REPONSE")) requete->type = REPONSE;
    else {
        free(elem1);
        free(elem2);
        libererPointeurChaine(messages,nbMessages);
        return BAD_REQUEST;
    }
    //on rajoute le nom de la requete
    requete->nom = malloc(sizeof(char) * (strlen(elem2) + 1));
    strcpy(requete->nom,elem2);

    free(elem1);
    free(elem2);
    for(i = 1; i < nbMessages; i++) {
        elem1 = malloc(sizeof(char) * (strlen(messages[i]) + 1));
        elem2 = malloc(sizeof(char) * (strlen(messages[i]) + 1));
        //on coupe la ligne en deux
        //on sort si l'un des deux arguments est de taille 0
        if(diviserChaine(messages[i],elem1,elem2,':')) {
            free(elem1);
            free(elem2);
            libererPointeurChaine(messages,nbMessages);
            return BAD_REQUEST;
        }
        //on recupère le nom de l'argument
        requete->arguments[requete->nbArgs].nom = malloc(sizeof(char) * (strlen(elem1)+ 1));
        strcpy(requete->arguments[requete->nbArgs].nom,elem1);
        //on récupère la valeur de l'argument;
        requete->arguments[requete->nbArgs].valeur = malloc(sizeof(char) * (strlen(elem2)+ 1));
        strcpy(requete->arguments[requete->nbArgs].valeur,elem2);
        //on incremente le nombre d'argument
        (requete->nbArgs)++;
        free(elem1);
        free(elem2);
    }
    libererPointeurChaine(messages,nbMessages);
    return OK;
}

int formaterEnChaine(requete requete, char** messages, int* nbMessages) {
    int i;
    messages[0] = malloc(sizeof(char) * (strlen("REPONSE ") + strlen(requete->nom) + 2));
    //on recupère le type
    if(requete->type == REPONSE) strcpy(messages[0],"REPONSE ");
    else if(requete->type == REQUEST) strcpy(messages[0],"REQUEST ");
    else {
        free(messages[0]);
        return BAD_REQUEST;
    }
    //on recpuère le nom
    strcat(messages[0],requete->nom);
    strcat(messages[0],"\n");
    //on rajoute les arguments
    for(i = 1; i < requete->nbArgs + 1; i++) {
        messages[i] = malloc(sizeof(char) * (strlen(requete->arguments[i-1].nom) + 1 + strlen(requete->arguments[i-1].valeur) + 2));
        sprintf(messages[i],"%s:%s\n",requete->arguments[i-1].nom,requete->arguments[i-1].valeur);
    }
    messages[i] = malloc(sizeof(char) * 3);
    strcpy(messages[i],"-\n");
    *nbMessages = requete->nbArgs + 2;
    return OK;
}

int diviserChaine(char* source, char* destination1,char* destination2, char separateur) {
    long int tailleMax;
    int i,j;
    tailleMax = strlen(source);
    if(source[tailleMax - 1] != '\n') return 1;
    i = 0;
    while(i < tailleMax && source[i] != separateur) {
        destination1[i] = source[i];
        i++;
    }
    destination1[i] = '\0';
    i++;
    j = 0;
    while((i + j) < tailleMax && source[i+j] != '\n') {
        destination2[j] = source[i+j];
        j++;
    }
    destination2[j] = '\0';
    if (i == 0 || j == 0) return 1;
    return 0;
}

requete initRequete(int type, char* nom) {
    requete requete = allouerRequete();
    requete->type = type;
    requete->nom = malloc(sizeof(char) * (strlen(nom) + 1));
    strcpy(requete->nom,nom);
    return requete;
}

int ajouterArgument(requete requete, char *nom, char *valeur) {
    if(requete->nbArgs >= NB_ARG_MAX) return 1;
    requete->arguments[requete->nbArgs].nom = malloc(sizeof(char) * (strlen(nom) + 1));
    strcpy(requete->arguments[requete->nbArgs].nom,nom);
    requete->arguments[requete->nbArgs].valeur = malloc(sizeof(char) * (strlen(valeur) + 1));
    strcpy(requete->arguments[requete->nbArgs].valeur,valeur);
    requete->nbArgs++;
    return 0;
}


char* recupererArgument(requete requete, char* nomArgument) {
    int i;
    for(i = 0;i<requete->nbArgs;i++) {
        if(!strcmp(nomArgument,requete->arguments[i].nom)) return requete->arguments[i].valeur;
    }
    return NULL;
}


void libererRequete(requete requete) {
    int i;
    if (requete != NULL) {
        if (requete->nom != NULL) free(requete->nom);
            for (i = 0; i < requete->nbArgs;i++) {
                if(requete->arguments[i].nom != NULL) free(requete->arguments[i].nom);
                if(requete->arguments[i].valeur != NULL) free(requete->arguments[i].valeur);
            }
        if(requete->donnees != NULL) free(requete->donnees);
        free(requete);
    }
}

requete allouerRequete() {
    int i;
    requete requete = malloc(sizeof(struct sRequete));
    requete->nom = NULL;
    for(i = 0; i< NB_ARG_MAX; i++) {
        requete->arguments[i].nom = NULL;
        requete->arguments[i].valeur = NULL;
    }
    
    requete->type = -1;
    requete->nbArgs = 0;
    requete->tDonnees = 0;
    requete->donnees = NULL;
    return requete;
}

void libererPointeurChaine(char** chaines, int nbChaines) {
    int i;
    for (i = 0; i < nbChaines; i++) free(chaines[i]);
    free(chaines);
}

void* charToData(char* donnees,size_t nbDonnees) {
    void* structures;
    structures = malloc(nbDonnees*sizeof(char));
    memcpy(structures,donnees,nbDonnees*sizeof(char));
    return structures;
}


char* dataToChar(void* structures, size_t tailleStructure, size_t nbStructures) {
    char* donnees;
    donnees = (char*) malloc(tailleStructure * nbStructures);
    memcpy(donnees,structures,tailleStructure*nbStructures);
    return donnees;
}

void afficherListe(char** liste, size_t taille) {
    int i;
    for(i = 0; i < taille; i++) {
        printf("\t%s\n",liste[i]);
    }
}

void freeBuffer()
{
    //tant qu'on atteind pas un retour a la ligne ou End of File on boucle pour vide le buffer
    int c = 0; 
    while (c != '\n' && c != EOF) c = getchar();
}
 
int lire(char *chaine, int longueur)
{
    char *positionEntree = NULL;

    if (fgets(chaine, longueur, stdin) != NULL) {
        positionEntree = strchr(chaine, '\n');
        if (positionEntree != NULL) *positionEntree = '\0';
        //Si on ne trouve pas \n alors il y a un overflow
        else freeBuffer();
        return 1;
    }
    else {
        //cas ou il y a eut une erreur
        freeBuffer();
        return 0;
    }
}

char recupererChar(char* liste) {
    char chaine[2];
    int i;
    //on boucle tant qu'on a pas ce que l'on veut
    while (1) {
        i = 0;
        lire(chaine,2);
        //on verifie chaque charactères entrés
        while(liste[i] != '\0') {
            if (chaine[0] == liste[i++]) return chaine[0];
        }
    }
}

void recupererChaine(char* chaine,int longueurMax,int longueurMin, int skip, char* nomVariable) {
    int fin =0;
    int i;
    while(!fin) {
        fin = lire(chaine,longueurMax+1);
        //Si on ne peux pas skip la chaine on verifie la taille
        if(fin && (!skip || (chaine[0] != '\0'))) {
            i = 0;
            while(chaine[i++] != '\0');
            if (i<=longueurMin) {
                fin = 0;
                printf("\nTaille de %s trop petit\n",nomVariable);
            }
        }
    }

}

int correctifChaine(char* message,char** destination) {
    int j,jbis,k;
    j = 0;
    //on calcul la taille de la première chaine
    while(message[j++] != '\n');
    //si on est pas a la fin c'est qu'il y a deux chaines
    if(message[j] != '\0') {
        jbis = j;
        //on calcul la taille de la chaine qui suit
        while(message[jbis++] != '\0');
        jbis= jbis - (j + 1);
        //on aloue en fonction de la taille de la chaine
        destination[1] = malloc(sizeof(char) * (j + 1));
        destination[2] = malloc(sizeof(char) * (jbis + 1));
        //on copie la première decoupe
        for (k = 0; k < j; k++) {
            destination[1][k] = message[k];
        }
        destination[1][j] = '\0';
        //on copie la seconde chaine
        for (k = 0; k < jbis; k++) {
            destination[2][k] = message[j+k];
        }
        destination[2][jbis] = '\0';
        //on a plus besoin de l'ancien message
        free(message);
        return 1;
    }
    return 0;
}

void afficherIndicationAnnuaire() {
    int i,j;
    int tabSize[7] = {SIZE_NOM,SIZE_PRENOM,SIZE_MAIL/2,SIZE_TEL + SIZE_TEL/2 - 2,SIZE_CITY,SIZE_CP,SIZE_ADRESSE/2};
    char tabName[7][11] = {"Nom","Prenom","Mail","Telephone","Ville","CP","Adresse"};
    printf("\n| ");
    //Affichage indications
    for(i = 0; i <7; i++) {
        afficherChaineEspaces(tabName[i],tabSize[i]);
        printf(" | ");
    }
    printf("\n|-");
    //Affichage séparateur
    for(i = 0; i <7; i++) {
        j = 0;
        while(j++ < tabSize[i]) putchar('-');
        printf("-|");
        if(i < 6) putchar('-');
    }
    putchar('\n');
}

void afficherAnnuaire(annuaire annuaire, size_t nbContacts) {
    int i;
    //Affichage en tête Annuaire 
    afficherIndicationAnnuaire();
    //boucle de parcour de chaque contact 
    for(i = 0; i < nbContacts; i++) {
        //affichage information annuaire 
        afficherContact(&annuaire[i],i);
    }
}

void afficherContact(contact contact ,int nba) {
    int i=0;
    int y=0;
    int k,j;
    char *ch=NULL;
    int tabSize[7] = {SIZE_NOM,SIZE_PRENOM,SIZE_MAIL/2,SIZE_TEL + SIZE_TEL/2 - 2,SIZE_CITY,SIZE_CP,SIZE_ADRESSE/2};
    ch = malloc(15*sizeof(char));
    char* tabName[7] = {contact->nom,contact->prenom,contact->mail,ch,contact->ville,contact->cp,contact->adresse};
    
    //On ajoute des points separateur tout les deux carateres du numero de telephone.
    for( i=0 ; i< (SIZE_TEL-1) ; i++ ){
        if( i%2 != 0 && i != 9 ){
            ch[y] = (char) contact->tel[i];
            y++;
            ch[y] =  '.';
            y++;
        }else{
            ch[y] = (char) contact->tel[i];
            y++;
        }     
        
    } 
    ch[SIZE_TEL- 1 + SIZE_TEL/2-1]='\0';

    i = 0;
    //affichage du contact avec une couleur en fonction de son rang
    //Affichage de la première ligne
    printf("| ");
    for(i = 0; i <7; i++) {
        if (nba%2 != 0 ) printf(BLC);
        afficherChaineEspaces(tabName[i],tabSize[i]);
        printf(RESET " | ");
    }
    putchar('\n');
    //Affichage de la seconde ligne si pas assez d'espace
    if((strlen(contact->adresse) > (SIZE_ADRESSE/2)) || strlen(contact->mail) > (SIZE_MAIL/2)) {
        printf("| ");
        for(i = 0; i <7; i++) {
            if (nba%2 != 0 ) printf(BLC);
            if(i == 2 && strlen(contact->mail) > (SIZE_MAIL/2)) afficherChaineEspaces(contact->mail + SIZE_MAIL/2,tabSize[2]);
            else if(i == 6 && strlen(contact->adresse) > (SIZE_ADRESSE/2)) afficherChaineEspaces(contact->adresse + SIZE_ADRESSE/2,tabSize[6]);
            else afficherChaineEspaces(" ",tabSize[i]);
            printf(RESET " | ");
        }
        putchar('\n');
    }

    printf("|-");
    for(k = 0; k <7; k++) {
        j = 0;
        while(j++ < tabSize[k]) putchar('-');
        printf("-|");
        if(k < 6) putchar('-');
    }
    putchar('\n');
    //Liberation du numero de tel en pointé 
    free(ch);
}

void afficherChaineEspaces(char* chaine, int taille) {
    int i = 0;
    //On commence par afficher la chaine
    while(i < taille && chaine[i] != '\0') {
        putchar(chaine[i]);
        i++;
    }
    //On rempli le reste par des espaces
    while(i < taille) {
        putchar(' ');
        i++;
    }
}

int existeChaine(char** listeChaines, size_t tailleChaine, char* chaine) {
    int i = 0;
    int presente = 0;
   
    while (i < tailleChaine) {
         //on incremante tant qu'on trouve pas la chaine de caractère
        if(strcmp(chaine, listeChaines[i]) != 0) i++;
        //si on la trouve on invalide la condition de boucle et on met présente en 1
        else {
            presente = 1;
            i = tailleChaine;
        }
    }
    return presente;
}

contact getContact(annuaire annuaire, size_t tailleAnnuaire, char* mail) {
    int i = 0;
    contact contact = NULL;
    //on cherche dans l'annuaire le contact en question
    while ((i < tailleAnnuaire) && (strcmp(mail, annuaire[i].mail) != 0)) i++;
    //si on l'a trouvé on returne le contact
    if(i != tailleAnnuaire) contact = &(annuaire[i]);
    return contact;
}

char* listeToChaine(char** liste, size_t tailleListe, size_t tailleChaine) {
    char *chaine,*pchaine;
    size_t i;
    chaine = malloc(tailleChaine * tailleListe * sizeof(char));
    pchaine = chaine;
    for (i = 0; i < tailleListe; i++) {
        pchaine = memcpy(pchaine, liste[i], tailleChaine);
        pchaine += tailleChaine;
    }
    return chaine;
}

char** chaineToListe(char*chaine, size_t* tailleListe, size_t tailleChaine) {
    char** liste;
    size_t i;
    liste = malloc(sizeof(char*) * (*tailleListe));
    for(i = 0; i < *tailleListe; i++) {
        liste[i] = malloc(tailleChaine * sizeof(char));
        liste[i] = memcpy(liste[i],chaine + i*tailleChaine,tailleChaine);
    }
    return liste;
}

int occurenceContact(char* chaine, contact contact) {
    if(verifierOccurence(contact->mail,chaine))
        return TRUE;
    if(verifierOccurence(contact->nom,chaine))
        return TRUE;
    if (verifierOccurence(contact->prenom,chaine))
        return TRUE;
    if(verifierOccurence(contact->tel,chaine))
        return TRUE;
    if(verifierOccurence(contact->adresse,chaine))
        return TRUE;
    if(verifierOccurence(contact->ville,chaine))
        return TRUE;
    if(verifierOccurence(contact->cp,chaine)) 
        return TRUE;  
    return FALSE;
}

void toLowerCase(char* chaine) {
    int i = 0;
    while(chaine[i] != '\0') {
        if(65 <= chaine[i] && chaine[i] <= 90) chaine[i] += 32;
        i++;
    }
}

int verifierOccurence(char*chaine1, char*chaine2) {
    int occurence = FALSE;
    char* copie1 = strdup(chaine1);
    char* copie2 = strdup(chaine2);
    toLowerCase(copie1);
    toLowerCase(copie2);
    if(strstr(copie1,copie2) != NULL) occurence = TRUE;
    free(copie1);
    free(copie2);
    return occurence;
}


