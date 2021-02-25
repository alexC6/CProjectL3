/**
 * @file fichier.c
 * @author Alexandre Coulais - Guillaume Fourcroy
 * @brief Code source des fonctions de gestion des fichiers
 * @version 2021-02-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "../../inc/serveur/fichier.h"

int filesError = NO_ERROR;
size_t nbComptes;
compte accountList = NULL;
size_t nbContact;
annuaire contactList = NULL;

char *fileAccounts = "files/accounts";
char *fileAnnuaire = "files/annuaires/Annuaire";
char *direcFiles = "files";
char *direcID = "files/id";
char *direcAnnuaire = "files/annuaires";

int getID(char *mail) {
    size_t i = 0;
    int id = -1;

    // Parcourt de tous les comptes tant que le mail ne correspond pas
    while ((i < nbComptes) && (id == -1)) {
        if (strcmp(accountList[i].mail, mail) == 0) {
            id = accountList[i].id;
        }

        i++;
    }

    return id;
}

int checkFileAccess(char *fileName) {
    if (access(fileName, F_OK | R_OK) != 0) {
        FILE *file = NULL;

        fprintf(stdout, GRN ">>> Fichier %s absent\n" RESET, fileName);
        fprintf(stdout, GRN ">>> Création du fichier ...\n" RESET);
        
        if ((file = fopen(fileName, "w")) == NULL) {
            fprintf(stdout,RED ">>> Erreur lors de la création du fichier %s\n" RESET, fileName);
            filesError = FILE_ACCESS;
            return 1;
        }

        fprintf(stdout, GRN ">>> Fichier %s créé\n" RESET, fileName);
        fclose(file);

        return 2;
    }

    return 0;
}

int getNewID(char *type) {
    int id = 0;
    char fileName[FILENAME_MAX];
    FILE *file = NULL;

    /**
     * Pour differencier les ID de contact et les ID de compte utilisateur
     * deux fichiers sont utilises : idAccount et idContact
     * Les parametres a passer sont donc Account ou Contact
     */
    sprintf(fileName, "%s%s", direcID, type);

    if (checkFileAccess(fileName) == 0) {
        // Tentative d'ouverture du fichier en lecture
        if ((file = fopen(fileName, "rb")) == NULL) {
            fprintf(stdout, RED ">>> Erreur lors de l'ouverture en lecture du fichier des ID\n" RESET);
            filesError = FILE_ACCESS;
            return -1;
        }

        // Recuperation du dernier ID utilise puis incrementation
        if (fscanf(file, "%d", &id) != 1) {
            fprintf(stdout, RED ">>> Erreur lors de la lecture du fichier des ID\n" RESET);
            filesError = FILE_READ;
            return -1;
        }

        fclose(file);

        id++;
    }

    // Tentative d'ouverture du fichier en ecriture
    if ((file = fopen(fileName, "wb")) == NULL) {
        fprintf(stdout,RED ">>> Erreur lors de l'ouverture en écriture du fichier des ID\n" RESET);
        filesError = FILE_ACCESS;
        return -1;
    }

    // On ecrit le nouvel ID dernierement utilise dans le fichier
    if (fprintf(file, "%d", id) != 1) {
        fprintf(stdout,RED ">>> Erreur lors de l'écriture du fichier des ID\n" RESET);
        filesError = FILE_WRITE;
        return -1;
    }

    fclose(file);

    return id;
}

ssize_t calculTailleFichier(char *fileName) {
    FILE *file = NULL;
    ssize_t size = 0;

    // On test l'ouverture du fichier, et on retourne -1 si probleme
    if ((file = fopen(fileName, "rb")) == NULL) {
        fprintf(stdout,RED ">>> Erreur lors de la tentative d'ouverture du fichier %s\n" RESET, fileName);
        filesError = FILE_ACCESS;
        return -1;
    }

    // On place le curseur a la fin du buffer et on demande sa position
    fseek(file, 0, SEEK_END);
    size = ftell(file);

    fclose(file);

    return size;
}

int checkDirectoriesAccess() {
    DIR *aux = NULL;
    char *direc[3] = {direcFiles, direcAnnuaire, direcID};

    for (int i = 0; i < 3; i++) {
        if ((aux = opendir(direc[i])) == NULL) {
            if (mkdir(direc[i], 0775)) {
                fprintf(stdout,RED ">>> Erreur lors de la creation du dossier %s\n" RESET, direc[i]);
                filesError = FILE_ACCESS;
                return 1;
            }
        } else {
            closedir(aux);
        }
    }

    return 0;
}

compte loadAccount() {
    ssize_t fileSize = 0;
    FILE *file = NULL;

    // On verifie que les dossiers de stockage des donnees existent
    if (checkDirectoriesAccess()) {
        fprintf(stdout,RED ">>> Erreur pendant la verification de l'acces aux dossiers\n" RESET);
        return NULL;
    }

    // Dans le cas ou le fichier n'existait pas ou qu'il y a eu une erreur dans sa creation
    if (checkFileAccess(fileAccounts)) {
        return NULL;
    }

    // Recuperation de la taille du fichier
    if ((fileSize = calculTailleFichier(fileAccounts)) < 0) {
        fprintf(stdout,RED ">>> Erreur lors du calcul de la taille du fichier des comptes\n" RESET);
        filesError = FILE_ACCESS;
        return NULL;
    }

    // Tentative d'ouverture du fichier en lecture
    if ((file = fopen(fileAccounts, "rb")) == NULL) {
        fprintf(stdout,RED ">>> Erreur à l'ouverture du fichier des comptes\n" RESET);
        filesError = FILE_ACCESS;
        return NULL;
    }

    /** On calcule le nombre de comptes presents a l'aide de la taille du fichier
     * et de la taille de chaque structure tCompte
     * puis allocation de la memoire necessaire pour les stocker
     */
    if ((nbComptes = (size_t) fileSize / sizeof(tCompte)) != 0) {
        if ((accountList = malloc(nbComptes * sizeof(tCompte))) == NULL) {
            fprintf(stdout,RED ">>> Erreur d'allocation memoire\n" RESET);
            filesError = MEM_ALLOC;
            return NULL;
        }

        // On recupere la liste des comptes depuis le fichier
        if (fread(accountList, sizeof(tCompte), nbComptes, file) != nbComptes) {
            fprintf(stdout,RED ">>> Erreur lors de la lecture du fichier des comptes\n" RESET);
            filesError = FILE_READ;
            return NULL;
        }
    }

    printf(GRN ">>> Chargement de %ld comptes\n" RESET, nbComptes);
    
    fclose(file);

    return accountList;
}

int addAccount(compte pCompte) {
    // Si le compte n'existe pas
    if (pCompte->id == -1) {
        if (getAccount(pCompte->mail) != NULL) {
            fprintf(stdout,MAG ">>> Un compte avec le mail %s existe déjà\n" RESET, pCompte->mail);
            filesError = EXIST_ACCOUNT;
            free(pCompte);
            return 1;
        }

        int id = 0;
        char fileName[FILENAME_MAX];
        compte tmp = NULL;
        FILE *file = NULL;

        // On realloue la memoire pour avoir la place pour stocker un nouveau compte a la suite du dernier
        if ((tmp = realloc(accountList, ++nbComptes * sizeof(tCompte))) == NULL) {
            fprintf(stdout,RED ">>> Problème de mémoire\n" RESET);
            filesError = MEM_ALLOC;
            return 1;
        }

        accountList = tmp;

        // Obtention de l'ID du nouvelle utilisateur
        if ((id = getNewID("Account")) < 0) {
            fprintf(stdout,RED ">>> Erreur lors de l'obtention d'un nouvel ID\n" RESET);
            filesError = GET_ID;
            return 1;
        }

        pCompte->id = id;
        sprintf(fileName, "%s%d", fileAnnuaire, id);

        // Creation annuaire du nouvel utilisateur
        if ((file = fopen(fileName, "wb")) == NULL) {
            fprintf(stdout,RED ">>> Erreur lors de la creation d'un nouvel annuaire\n" RESET);
            filesError = FILE_ACCESS;
            return 1;
        }

        fclose(file);

        // On copie le nouveau compte a la suite de la liste globale
        memcpy(&accountList[nbComptes-1], pCompte, sizeof(tCompte));

        free(pCompte);
    }

    saveAccount();

    return 0;
}

compte getAccount(char *mail) {
    size_t i = 0;
    compte aux = NULL;

    // On parcourt tous les comptes a la recherche de celui dont le mail concorde avec celui recherche
    if (accountList != NULL) { // On verifie qu'il existe des comptes en memoire
        while ((i < nbComptes) && (aux == NULL)) {
            if (strcmp(accountList[i].mail, mail) == 0) {
                aux = &accountList[i];
            }
            i++;
        }
    }

    if (aux == NULL) {
        filesError = NO_ACCOUNT;
    }

    // On retourne l'adresse de l'emplacement memoire du compte dans la liste
    return aux;
}

int saveAccount(void) {
    FILE *file = NULL;
    size_t taille;

    // Tentative d'ouverture du fichier des comptes en ecriture
    if ((file = fopen(fileAccounts, "wb")) == NULL) {
        fprintf(stdout,RED ">>> Erreur lors de l'ouverture du fichier des comptes\n" RESET);
        filesError = FILE_ACCESS;
        return 1;
    }

    // On ecrit toutes les structures comptes connues dans le fichier
    if ((taille = fwrite(accountList, sizeof(tCompte), nbComptes, file)) != nbComptes) {
        fprintf(stdout,RED ">>> Erreur lors de l'écriture des comptes dans le fichier\n" RESET);
        filesError = FILE_READ;
        return 1;
    }

    fclose(file);

    return 0;
}

int deleteAccount(char *mail) {
    size_t i = 0;
    compte aux = NULL;

    /**
     * Faire une recherche manuelle plutot que recuperer un pointeur sur le compte
     * a l'aide de getAccount() permet d'obtenir l'indice du compte dans le tableau
     * afin de decaler tous les suivants pour ecraser le compte que l'on souhaite supprimer
     */
    if (accountList != NULL) { // On verifie qu'il existe des comptes en memoire
        while ((i < nbComptes) && (aux == NULL)) {
            if (strcmp(accountList[i].mail, mail) == 0) {
                aux = &(accountList[i]);
            }
            i++;
        }
    }

    if (aux == NULL) {
        fprintf(stdout,MAG ">>> Compte introuvable\n" RESET);
        filesError = NO_ACCOUNT;
        return 1;
    }

    // On verifie s'il reste un seul compte
    if (nbComptes != 1) {
        for (i = i; i < nbComptes; i++) {
            memcpy(&accountList[i-1], &accountList[i], sizeof(tCompte));
        }

        if ((aux = realloc(accountList, --nbComptes * sizeof(tCompte))) == NULL) {
            fprintf(stdout,RED ">>> Erreur de reallocation memoire\n" RESET);
            filesError = MEM_ALLOC;
            return 1;
        }

        accountList = aux;

        saveAccount();
    } else { // Sinon on libere les comptes en memoire et on recree un fichier de compte
        free(accountList);
        accountList = NULL;
        remove(fileAccounts);
        loadAccount();
        nbComptes = 0;
    }

    return 0;
}

char **getMailList(size_t *nbMails) {
    char **buffer = NULL;

    /**
     * On alloue la memoire necessaire pour stocker autant de mail qu'il n'y de comptes
     * On parcourt ensuite tous les comptes en recopiant le mail de chacun
     * dans la matrice de caracters dont on retournera le pointeur a la fin
     */
    if ((buffer = malloc(nbComptes * sizeof(char *))) != NULL) {
        for (size_t i = 0; i < nbComptes; i++) {
            if ((buffer[i] = malloc(SIZE_MAIL * sizeof(char))) == NULL) {
                fprintf(stdout,RED ">>> Problème de mémoire\n" RESET);
                filesError = MEM_ALLOC;
                return NULL;
            }

            strcpy(buffer[i], accountList[i].mail);
        }

        *nbMails = nbComptes;
    } else {
        fprintf(stdout,RED ">>> Problème de mémoire\n" RESET);
        filesError = MEM_ALLOC;
        return NULL;
    }

    return buffer;
}

char **getInfoComptes() {
    char **buffer = NULL;

    /**
     * On alloue la memoire necessaire pour stocker autant de mail qu'il n'y de comptes
     * On parcourt ensuite tous les comptes en recopiant le nom et le prenom de chacun
     * dans la matrice de caracters dont on retournera le pointeur a la fin
     */
    if ((buffer = malloc(nbComptes * sizeof(char *))) != NULL) {
        for (size_t i = 0; i < nbComptes; i++) {
            if ((buffer[i] = malloc((SIZE_NOM + SIZE_PRENOM + 3) * sizeof(char))) == NULL) {
                fprintf(stdout,RED ">>> Problème de mémoire\n" RESET);
                filesError = MEM_ALLOC;
                return NULL;
            }

            sprintf(buffer[i],"(%s %s)",accountList[i].nom, accountList[i].prenom);
        }
    } else {
        fprintf(stdout,RED ">>> Problème de mémoire\n" RESET);
        filesError = MEM_ALLOC;
        return NULL;
    }

    return buffer;
}

annuaire getAnnuaire(char *mail, size_t *number) {
    int id = 0;
    char fileName[FILENAME_MAX];
    FILE *file = NULL;
    compte aux = NULL;

    // On recupere l'ID du compte correspondant au mail passe en parametre
    if ((id = getID(mail)) < 0) {
        fprintf(stdout,RED ">>> Erreur lors de la récupération de l'ID du compte %s\n" RESET, mail);
        filesError = GET_ID;
        return NULL;
    }

    // On constitue le nom du fichier stockant l'annuaire du compte
    sprintf(fileName, "%s%d", fileAnnuaire, id);

    // Tentative d'ouverture du fichier en lecture
    if ((file = fopen(fileName, "rb")) == NULL) {
        fprintf(stdout,RED ">>> Erreur lors de l'ouverture de l'annuaire du compte %s\n" RESET, mail);
        filesError = FILE_ACCESS;
        return NULL;
    }

    // On recupere le pointeur sur le compte correspondant au mail
    if ((aux = getAccount(mail)) == NULL) {
        fprintf(stdout,RED ">>> Erreur lors de la récupération du compte %s\n" RESET, mail);
        filesError = NO_ACCOUNT;
        return NULL;
    }

    nbContact = aux->nbContact;
    *number = nbContact;

    // Pour eviter les fuites memoires, on verifie que l'annuaire global est vide
    if (contactList != NULL) {
        free(contactList);
    }

    // On alloue la memoire necessaire pour stocker l'annuaire demande
    if ((contactList = malloc(sizeof(tContact) * nbContact)) == NULL) {
        fprintf(stdout,RED ">>> Problème de mémoire\n" RESET);
        filesError = MEM_ALLOC;
        return NULL;
    }

    // On recupere tous les contacts que l'on stock dans l'annuaire global
    if (fread(contactList, sizeof(tContact), nbContact, file) < nbContact) {
        fprintf(stdout,RED ">>> Erreur lors de la lecture des contacts du compte %s\n" RESET, mail);
        filesError = FILE_READ;
        return NULL;
    }

    fclose(file);

    return contactList;
}

int saveAnnuaire(annuaire pAnnuaire, char *mail) {
    int id = 0;
    size_t nbContactAux = 0;
    char fileName[FILENAME_MAX];
    FILE *file = NULL;
    compte aux = NULL;

    // On recupere l'ID du compte correspondant au mail passe en parametre
    if ((id = getID(mail)) < 0) {
        fprintf(stdout,RED ">>> Erreur lors de la récupération de l'ID du compte %s\n" RESET, mail);
        filesError = GET_ID;
        return 1;
    }

    // On constitue le nom du fichier stockant l'annuaire du compte
    sprintf(fileName, "%s%d", fileAnnuaire, id);

    // On recupere le pointeur sur le compte correspondant au mail
    if ((aux = getAccount(mail)) == NULL) {
        fprintf(stdout,RED ">>> Erreur lors de la récupération du compte %s\n" RESET, mail);
        filesError = NO_ACCOUNT;
        return 1;
    }

    nbContactAux = aux->nbContact;

    // Tentative d'ouverture de l'annuaire du compte en ecriture
    if ((file = fopen(fileName, "wb")) == NULL) {
        fprintf(stdout,RED ">>> Erreur lors de l'ouverture de l'annuaire du compte %s\n" RESET, mail);
        filesError = FILE_ACCESS;
        return 1;
    }

    // On ecrit tous les contacts de l'annuaire passe en parametre dans le fichier du compte
    if (fwrite(pAnnuaire, sizeof(tContact), nbContactAux, file) != nbContactAux) {
        fprintf(stdout,RED ">>> Erreur lors de l'écriture des contacts dans l'annuaire du compte %s\n" RESET, mail);
        filesError = FILE_READ;
        return 1;
    }

    fclose(file);

    return 0;
}

int addContact(contact pContact, char *mail) {
    int id = 0;
    int newContact = (pContact->id == -1)?TRUE:FALSE;
    size_t emplacementContact;
    size_t nbContactAux = 0;
    char fileName[FILENAME_MAX];
    FILE *file = NULL;
    compte aux = NULL;
    annuaire buffer = NULL;

    // On recupere l'ID du compte correspondant au mail passe en parametre
    if ((id = getID(mail)) < 0) {
        fprintf(stdout,RED ">>> Erreur lors de la récupération de l'ID du compte %s\n" RESET, mail);
        filesError = GET_ID;
        free(pContact);
        return 1;
    }

    // On constitue le nom du fichier stockant l'annuaire du compte
    sprintf(fileName, "%s%d", fileAnnuaire, id);

    if ((aux = getAccount(mail)) == NULL) {
        fprintf(stdout,RED ">>> Erreur lors de la récupération du compte %s\n" RESET, mail);
        filesError = NO_ACCOUNT;
        free(pContact);
        return 1;
    }

    // On incremente le nombre de contacts de l'annuaire si il n'existe pas
    if(newContact)  aux->nbContact++;
    nbContactAux = aux->nbContact;

    // On alloue la memoire necessaire pour stocker l'annuaire du compte
    if ((buffer = malloc(nbContactAux * sizeof(tContact))) == NULL) {
        fprintf(stdout,RED ">>> Problème de mémoire\n" RESET);
        filesError = MEM_ALLOC;
        free(pContact);
        return 1;
    }

    // Tentative d'ouverture de l'annuaire du compte en lecture
    if ((file = fopen(fileName, "rb")) == NULL) {
        fprintf(stdout,RED ">>> Erreur lors de l'ouverture de l'annuaire du compte %s\n" RESET, mail);
        filesError = FILE_ACCESS;
        free(buffer);
        free(pContact);
        return 1;
    }

    // On recupere tous les contacts de l'annuaire du compte (nbContactAux si il existe deja, nbContactAux-1 sinon)
    if (fread(buffer, sizeof(tContact), (newContact?(nbContactAux-1):nbContactAux), file) < (newContact?(nbContactAux-1):nbContactAux)) {
        fprintf(stdout,RED ">>> Erreur lors de la lecture des contacts du compte %s\n" RESET, mail);
        filesError = FILE_READ;
        free(buffer);
        free(pContact);
        return 1;
    }

    fclose(file);

    //On récupère l'emplacement du conact si c'est un contact à modifier
    if(!newContact)  {
        emplacementContact = getIndiceContactAnnuaire(pContact,buffer,nbContactAux);
        if(emplacementContact == -1) {
            fprintf(stdout,RED ">>> L'id du contact %s n'existe pas\n" RESET, mail);
            free(buffer);
            free(pContact);
            return 1;
        }
    } else {
        // On affecte un ID au contact cree
        if ((pContact->id = getNewID("Contact")) < 0) {
            fprintf(stdout,RED ">>> Erreur lors de l'obtention d'un nouvel ID de contact\n" RESET);
            filesError = GET_ID;
            free(buffer);
            free(pContact);
            return 1;
        }
    }

    // On copie le nouveau contact a la suite des precedents à l'emplacement (emplacementContact si il existe deja, nbContactAux-1 sinon)
    memcpy(&buffer[newContact?(nbContactAux-1):emplacementContact], pContact, sizeof(tContact));
    free(pContact);
    // On sauvegarde le buffer qui constitue le nouvel annuaire
    if (saveAnnuaire(buffer, mail) != 0) {
        fprintf(stdout,RED ">>> Erreur lors de la sauvegarde de l'annuaire du compte %s\n" RESET, mail);
        free(buffer);
        return 1;
    }
    free(buffer);
    //On oublie pas de sauvegarder la modification apporté au compte (l'incrémentation du nombre de compte)
    if (saveAccount()) {
        fprintf(stdout,RED ">>> Erreur lors de la sauvegarde des comptes\n" RESET);
        return 1;
    }

    return 0;
}

int deleteContact(char *contactMail, char *userMail) {
    int id = 0;
    size_t nbContactAux = 0, i = 0;
    char fileName[FILENAME_MAX];
    FILE *file = NULL;
    compte aux = NULL;
    annuaire buffer = NULL;

    // On recupere l'ID du compte correspondant au mail passe en parametre
    if ((id = getID(userMail)) < 0) {
        fprintf(stdout,RED ">>> Erreur lors de la récupération de l'ID du compte %s\n" RESET, userMail);
        filesError = GET_ID;
        return 1;
    }

    // On constitue le nom du fichier stockant l'annuaire du compte
    sprintf(fileName, "%s%d", fileAnnuaire, id);

    // On recupere le pointeur sur le compte correspondant au mail
    if ((aux = getAccount(userMail)) == NULL) {
        fprintf(stdout,RED ">>> Erreur lors de la récupération du compte %s\n" RESET, userMail);
        filesError = NO_ACCOUNT;
        return 1;
    }

    // On decremente le nombre de contact de l'annuaire apres avoir recupere le nombre de contact
    nbContactAux = aux->nbContact;
    aux->nbContact--;

    // On alloue la memoire necessaire pour stocker tous les contacts
    if ((buffer = malloc(nbContactAux * sizeof(tContact))) == NULL) {
        fprintf(stdout,RED ">>> Problème de mémoire\n" RESET);
        filesError = MEM_ALLOC;
        return 1;
    }

    // Tentative d'ouverture de l'annuaire en lecture
    if ((file = fopen(fileName, "rb")) == NULL) {
        fprintf(stdout,RED ">>> Erreur lors de l'ouverture de l'annuaire du compte %s\n" RESET, userMail);
        filesError = FILE_ACCESS;
        free(buffer);
        return 1;
    }

    // On recupere tous les contacts dans le buffer
    if (fread(buffer, sizeof(tContact), nbContactAux, file) < nbContactAux) {
        fprintf(stdout,RED ">>> Erreur lors de la lecture des contacts du compte %s\n" RESET, userMail);
        filesError = FILE_READ;
        free(buffer);
        return 1;
    }

    fclose(file);

    // On parcourt les contacts a la recherche de celui a supprimer
    while ((i < nbContactAux) && (strcmp(contactMail, buffer[i].mail) != 0)) {
        i++;
    }

    // Lorsque le contact est trouve, on decale tous les suivants pour l'ecraser
    if (i != nbContactAux) {
        for (i++; i < nbContactAux; i++) {
            buffer[i-1] = buffer[i];
        }
    } else { // Sinon on informe que le contact souhaite n'existe pas
        fprintf(stdout,MAG ">>> Le contact à supprimer est inexistant\n" RESET);
        filesError = NO_CONTACT;
        free(buffer);
        return 1;
    }

    // On termine en sauvegardant l'annuaire de l'utilisateur
    if (saveAnnuaire(buffer, userMail) != 0) {
        fprintf(stdout,RED ">>> Erreur lors de la sauvegarde de l'annuaire du compte %s\n" RESET, userMail);
        free(buffer);
        return 1;
    }
    free(buffer);
    //On oublie pas de sauvegarder la modification apporté au compte (la décrémentation du nombre de compte)
    if (saveAccount()) {
        fprintf(stdout,RED ">>> Erreur lors de la sauvegarde des comptes\n" RESET);
        return 1;
    }

    return 0;
}

int deleteShared(char *userMail, char *otherMail) {
    int userID = 0, otherID = 0;
    size_t i = 0;
    compte user = NULL, other = NULL;

    // On recupere l'ID du compte de l'utilisateur connecte
    if ((userID = getID(userMail)) < 0) {
        fprintf(stdout,RED ">>> Erreur lors de la récupération de l'ID du compte %s\n" RESET, userMail);
        filesError = GET_ID;
        return 1;
    }

    // On recupere l'ID du compte de l'utilisateur dont on souhaite supprimer l'acces
    if ((otherID = getID(otherMail)) < 0) {
        fprintf(stdout,RED ">>> Erreur lors de la récupération de l'ID du compte %s\n" RESET, otherMail);
        filesError = GET_ID;
        return 1;
    }

    // On recupere le pointeur sur le compte de l'utilisateur connecte
    if ((user = getAccount(userMail)) == NULL) {
        fprintf(stdout,RED ">>> Erreur lors de la récupération du compte %s\n" RESET, userMail);
        filesError = NO_ACCOUNT;
        return 1;
    }

    // On recupere le pointeur sur le compte de l'utilisateur dont on souhaite supprimer l'acces
    if ((other = getAccount(otherMail)) == NULL) {
        fprintf(stdout,RED ">>> Erreur lors de la récupération du compte %s\n" RESET, otherMail);
        filesError = NO_ACCOUNT;
        return 1;
    }

    // On cherche l'index de l'ID a supprimer
    while ((i < user->nbShared) && (user->idShared[i] != otherID)) {
        i++;
    }

    // On decale tous les ID qui suivent pour ecraser celui a supprimer
    for (i++; i < user->nbShared; i++) {
        user->idShared[i-1] = user->idShared[i];
    }

    i = 0;

    // On cherche l'index de l'ID a supprimer
    while ((i < other->nbView) && (other->idView[i] != otherID)) {
        i++;
    }
    
    // On decale tous les ID qui suivent pour ecraser celui a supprimer
    for (i++; i < other->nbView; i++) {
        other->idView[i-1] = other->idView[i];
    }

    user->nbShared--;
    other->nbView--;

    if (saveAccount()) {
        fprintf(stdout,RED ">>> Erreur lors de la sauvegarde des comptes\n" RESET);
        return 1;
    }

    return 0;
}

int addShared(char *userMail, char *otherMail) {
    int userID = 0, otherID = 0;
    compte user = NULL, other = NULL;

    // On recupere l'ID du compte de l'utilisateur connecte
    if ((userID = getID(userMail)) < 0) {
        fprintf(stdout,RED ">>> Erreur lors de la récupération de l'ID du compte %s\n" RESET, userMail);
        filesError = GET_ID;
        return 1;
    }

    // On recupere l'ID du compte de l'utilisateur dont on souhaite autoriser l'acces
    if ((otherID = getID(otherMail)) < 0) {
        fprintf(stdout,RED ">>> Erreur lors de la récupération de l'ID du compte %s\n" RESET, otherMail);
        filesError = GET_ID;
        return 1;
    }

    // On recupere le pointeur sur le compte de l'utilisateur connecte
    if ((user = getAccount(userMail)) == NULL) {
        fprintf(stdout,RED ">>> Erreur lors de la récupération du compte %s\n" RESET, userMail);
        filesError = NO_ACCOUNT;
        return 1;
    }

    // On recupere le pointeur sur le compte de l'utilisateur dont on souhaite autoriser l'acces
    if ((other = getAccount(otherMail)) == NULL) {
        fprintf(stdout,RED ">>> Erreur lors de la récupération du compte %s\n" RESET, otherMail);
        filesError = NO_ACCOUNT;
        return 1;
    }

    // On incremente le nombre d'utilisateur autorises et on ajout l'ID a la fin
    user->nbShared++;
    user->idShared[user->nbShared-1] = otherID;

    // On incremente le nombre d'annauires accessibles et on ajout l'ID a la fin
    other->nbView++;
    other->idView[other->nbView-1] = userID;

    if (saveAccount()) {
        fprintf(stdout,RED ">>> Erreur lors de la sauvegarde des comptes\n" RESET);
        return 1;
    }

    return 0;
}

int *getView(char *mail, size_t *number) {
    compte aux = NULL;

    // On recupere le pointeur sur le tableau de comptes autorises
    if ((aux = getAccount(mail)) == NULL) {
        fprintf(stdout,RED ">>> Erreur lors de la récupération du compte %s\n" RESET, mail);
        filesError = NO_ACCOUNT;
        return NULL;
    }

    *number = aux->nbView;

    return aux->idView;
}

int *getShared(char *mail, size_t *number) {
    compte aux = NULL;

    // On recupere le pointeur sur le tableau de comptes autorises
    if ((aux = getAccount(mail)) == NULL) {
        fprintf(stdout,RED ">>> Erreur lors de la récupération du compte %s\n" RESET, mail);
        filesError = NO_ACCOUNT;
        return NULL;
    }

    *number = aux->nbShared;

    return aux->idShared;
}

void unload() {
    if (contactList != NULL) {
        free(contactList);
    }

    saveAccount();

    if (accountList != NULL) {
        free(accountList);
    }
}

int gotPermission(compte pCompte, char* mail) {
    int id;
    int i;
    id = getID(mail);
    if(id == pCompte->id) return 2;
    for(i = 0; i < pCompte->nbView; i++) {
        if(id == pCompte->idView[i]) return 1;
    }
    return 0;
}

int getfileError() {
    int error = filesError;
    filesError = NO_ERROR;
    return error;
}

char** listIDToListMail(int* listeID, int tailleListe) {
    char **listeMail;
    if(tailleListe == 0) return NULL;
    listeMail = malloc(sizeof(char*) * tailleListe);
    for(int i = 0; i < tailleListe; i++) listeMail[i] = accountList[listeID[i]].mail;
    return listeMail;
}

size_t getIndiceContactAnnuaire(contact contact, annuaire annuaire, size_t tailleAnnuaire) {
    for(size_t i = 0; i < tailleAnnuaire; i++) {
        if((&annuaire[i])->id == contact->id) return i;
    }
    return -1;
}