#include "../../inc/client/requetes_client.h"

int recupererRequete(requete requete) {
    char** messages;
    char* Ctaille;
    char* buffer[2];
    int nbMessages;
    messages = malloc(sizeof(char*) * NB_ARG_MAX);
    nbMessages = 0;
    //on recupère les message jusqu'a la ligne vide
    do {
        messages[nbMessages++] = Reception();
        //cette condition est la pour les messages non decoupés
        while(messages[nbMessages-1] != NULL && correctifChaine(messages[nbMessages-1],buffer) && nbMessages < NB_ARG_MAX - 1) {
            messages[nbMessages-1] = strdup(buffer[1]);
            messages[nbMessages] = strdup(buffer[2]);
            free(buffer[1]);
            free(buffer[2]);
            nbMessages++;
        }
    } while (messages[nbMessages-1] != NULL && strcmp(messages[nbMessages-1],"-\n") && nbMessages < NB_ARG_MAX - 1);
    //on libère le dernier message "-\n" si il existe
    if(messages[nbMessages-1] != NULL && !strcmp(messages[nbMessages-1],"-\n")) free(messages[nbMessages-1]);
    //on enleve ce message de fin de chaine
    nbMessages--;
    //on verifie qu'on a bien un premier message
    if(messages[0] == NULL) {
        return NO_MESSAGES;
    }
    
    //on formate en requete
    if(formaterEnRequete(requete,messages,nbMessages) == BAD_REQUEST) return BAD_REQUEST;
    //on recupère la taille
    Ctaille = recupererArgument(requete,"taille");
    //si on a pas de taille, on a pas de données, sinon on recupère les données
    if (Ctaille != NULL) {
        requete->tDonnees = atol(Ctaille);
        //printf("RECEPTION DONNEES : %ld bits\n",requete->tDonnees);
        if(requete->tDonnees > 0) {
            requete->donnees = malloc(sizeof(char)*requete->tDonnees);
            if(ReceptionBinaire(requete->donnees,requete->tDonnees) != requete->tDonnees) return DATA_MISSING;
        }
    }
    //printf("Recu : %s\n",requete->nom);
    return OK;
}


int envoyerRequete(requete requete) {
    char tailleChar[12];
    int nbMessages;
    int i;
    char** messages = malloc(sizeof(char *) * (NB_ARG_MAX +2));
    if(requete->tDonnees > 0) {
        sprintf(tailleChar,"%ld",requete->tDonnees);
        ajouterArgument(requete,"taille",tailleChar);
    }
    if(formaterEnChaine(requete,messages,&nbMessages)) return BAD_REQUEST;

    for(i = 0; i < nbMessages; i++) {
        if(!Emission(messages[i])) {
            while(nbMessages--) free(messages[nbMessages]);
            free(messages);
            libererRequete(requete);
            return BAD_SEND;
        }
    }
    while(nbMessages--) free(messages[nbMessages]);
    free(messages);
    
    if(requete->tDonnees > 0) {
        //printf("ENVOIE DONNEES : %ld bits\n",requete->tDonnees);
        if(EmissionBinaire(requete->donnees,requete->tDonnees) != requete->tDonnees) {
            libererRequete(requete);
            return BAD_SEND_DATA;
        }
    }
    //printf("Envoi : %s\n",requete->nom);
    libererRequete(requete); 
    return OK;
}

int connectionCompte(char* mail, char* password) {
    requete req;
    requete reponse;
    int erreur;
    char* cause;
    //on creer la requete en question
    req = initRequete(REQUEST,"CONNECT");
    ajouterArgument(req,"mail",mail);
    ajouterArgument(req,"password",password);
    //on envoie la requete en verifiant les reponses
    erreur = envoyerRequete(req);
    if(erreur) return erreur;
    //on recupère la requete en verifiant les reponses
    reponse = allouerRequete();
    erreur = recupererRequete(reponse);
    if(erreur) return erreur;
    //Recuperation des reponses
    //Cas de connexion
    if (!strcmp(reponse->nom,"CONNECT_OK")) {
        libererRequete(reponse);
        return OK;
    }
    //Cas de denied du serveur (mauvais mdp/mail)
    if(!strcmp(reponse->nom,"CONNECT_DENIED")) {
        cause = recupererArgument(reponse,"cause");
        if(cause != NULL) printf(MAG "\n>>> %s\n" RESET,cause);
        libererRequete(reponse);
        return BAD_ARGUMENT;
    }
    //Cas ou le server renvoie une BAD_REQUEST car on a mal fait notre requete
    if (!strcmp(reponse->nom,"BAD_REQUEST")) {
        libererRequete(reponse);
        return BAD_REQUEST;
    }
    libererRequete(reponse);
    
    // Cas ou on a recu une requete mal formée (on renvoie un BAD request)
    req = initRequete(REPONSE,"BAD_REQUEST");
    erreur = envoyerRequete(reponse);
    if(erreur) return erreur;
    return BAD_REQUEST;
}

int demanderDeconnexion() {
    requete req;
    requete reponse;
    int erreur;

    //On envoie d'abord la requete de deconnexion
    req = initRequete(REQUEST,"DISCONNECT");
    erreur = envoyerRequete(req);
    if(erreur) return erreur;

    //on recupère la requete en verifiant les reponses
    reponse = allouerRequete();
    erreur = recupererRequete(reponse);
    if(erreur) return erreur;

    //Cas normal de deconnexion
    if (!strcmp(reponse->nom,"DISCONNECT_OK")) {
        libererRequete(reponse);
        return OK;
    }

    //Cas ou le server renvoie une BAD_REQUEST car on a mal fait notre requete
    if (!strcmp(reponse->nom,"BAD_REQUEST")) {
        libererRequete(reponse);
        return BAD_REQUEST;
    }
    libererRequete(reponse);

    // Cas ou on a recu une requete mal formée (on renvoie un BAD request)
    req = initRequete(REPONSE,"BAD_REQUEST");
    erreur = envoyerRequete(req);
    if(erreur) return erreur;
    return BAD_REQUEST;
}

annuaire recupererAnnuaire(char* mail, size_t* tailleAnnuaire) {
    annuaire annu = NULL;
    requete req;
    requete reponse;
    *tailleAnnuaire = -1;
    int erreur;
    char* cause;
    //on envoie d'abord la requete de consultation d'annuaire
    req = initRequete(REQUEST,"CONSULT_DIRECTORY");
    ajouterArgument(req,"mail",mail);
    erreur = envoyerRequete(req);
    if(erreur) return NULL;

    //on recupère la requete en verifiant les reponses
    reponse = allouerRequete();
    erreur = recupererRequete(reponse);

    //Cas normal de recuperation d'annuaire
    if (!strcmp(reponse->nom,"ACCESS_DIRECTORY_OK")) {
        *tailleAnnuaire = 0;
        if(reponse->tDonnees > 0) {
                annu = (annuaire) charToData(reponse->donnees,reponse->tDonnees);
            *tailleAnnuaire = (reponse->tDonnees)/sizeof(tContact);
        }
        libererRequete(reponse);
        return annu;
    }

    //Cas de denied du serveur (n'existe pas ou pas la permission)
    if(!strcmp(reponse->nom,"ACCESS_DIRECTORY_KO")) {
        cause = recupererArgument(reponse,"cause");
        if(cause != NULL) printf(MAG "\n>>> %s\n" RESET,cause);
        libererRequete(reponse);
        return NULL;
    }

    //Cas ou le server renvoie une BAD_REQUEST car on a mal fait notre requete
    if (!strcmp(reponse->nom,"BAD_REQUEST")) {
        libererRequete(reponse);
        return NULL;
    }

    // Cas ou on a recu une requete mal formée (on renvoie un BAD request)
    req = initRequete(REPONSE,"BAD_REQUEST");
    envoyerRequete(req);
    return NULL;
}

char** recupererSharedAnnuaires(size_t* tailleListe) {
    char **listeMails = NULL;
    requete req;
    requete reponse;
    *tailleListe = -1;
    int erreur;
    char* cause;
    //on envoie d'abord la requete de consultation de la liste d'annuaire
    req = initRequete(REQUEST,"CONSULT_SHARED_DIRECTORIES");
    erreur = envoyerRequete(req);
    if(erreur) return NULL;

    //on recupère la requete en verifiant les reponses
    reponse = allouerRequete();
    erreur = recupererRequete(reponse);

    //Cas normal de recuperation de la liste
    if (!strcmp(reponse->nom,"CONSULT_SHARED_DIRECTORIES_OK")) {
        *tailleListe = 0;
        if(reponse->tDonnees > 0) {
            *tailleListe = (reponse->tDonnees)/SIZE_MAIL;
            listeMails = chaineToListe(reponse->donnees, tailleListe,SIZE_MAIL);
        }
        libererRequete(reponse);
        return listeMails;
    }
    
    //Cas de denied du serveur
    if(!strcmp(reponse->nom,"CONSULT_SHARED_DIRECTORIES_KO")) {
        cause = recupererArgument(reponse,"cause");
        if(cause != NULL) printf(MAG "\n>>> %s\n" RESET,cause);
        libererRequete(reponse);
        return NULL;
    }

    //Cas ou le server renvoie une BAD_REQUEST car on a mal fait notre requete
    if (!strcmp(reponse->nom,"BAD_REQUEST")) {
        libererRequete(reponse);
        return NULL;
    }

    // Cas ou on a recu une requete mal formée (on renvoie un BAD request)
    req = initRequete(REPONSE,"BAD_REQUEST");
    envoyerRequete(req);
    return NULL;
}

char** recupererAccesAnnuaires(size_t* tailleListe) {
    char **listeMails = NULL;
    requete req;
    requete reponse;
    *tailleListe = -1;
    int erreur;
    char* cause;
    //on envoie d'abord la requete de consultation de la liste d'annuaire
    req = initRequete(REQUEST,"CONSULT_USERS_ACCESS");
    erreur = envoyerRequete(req);
    if(erreur) return NULL;

    //on recupère la requete en verifiant les reponses
    reponse = allouerRequete();
    erreur = recupererRequete(reponse);

    //Cas normal de recuperation de la liste
    if (!strcmp(reponse->nom,"CONSULT_USERS_ACCESS_OK")) {
        *tailleListe = 0;
        if(reponse->tDonnees > 0) {
            *tailleListe = (reponse->tDonnees)/SIZE_MAIL;
            listeMails = chaineToListe(reponse->donnees, tailleListe,SIZE_MAIL);
        }
        libererRequete(reponse);
        return listeMails;
    }
    
    //Cas de denied du serveur
    if(!strcmp(reponse->nom,"CONSULT_USERS_ACCESS_KO")) {
        cause = recupererArgument(reponse,"cause");
        if(cause != NULL) printf(MAG "\n>>> %s\n" RESET,cause);
        libererRequete(reponse);
        return NULL;
    }

    //Cas ou le server renvoie une BAD_REQUEST car on a mal fait notre requete
    if (!strcmp(reponse->nom,"BAD_REQUEST")) {
        libererRequete(reponse);
        return NULL;
    }

    // Cas ou on a recu une requete mal formée (on renvoie un BAD request)
    req = initRequete(REPONSE,"BAD_REQUEST");
    envoyerRequete(req);
    return NULL;
}

int demandeAjoutContact(contact c) {
    requete req;
    requete reponse;
    int erreur;
    char* cause;
    //on envoie d'abord la requete de consultation d'annuaire
    req = initRequete(REQUEST,"ADD_CONTACT");
    req->tDonnees = sizeof(tContact)/sizeof(char);
    req->donnees = dataToChar(c, sizeof(tContact), 1);
    free(c);
    erreur = envoyerRequete(req);
    if(erreur) return erreur;

    //on recupère la requete en verifiant les reponses
    reponse = allouerRequete();
    erreur = recupererRequete(reponse);

    //Cas normal de recuperation d'annuaire
    if (!strcmp(reponse->nom,"ADD_CONTACT_OK")) {
        libererRequete(reponse);
        return OK;
    }

    //Cas de denied du serveur (n'existe pas ou pas la permission)
    if(!strcmp(reponse->nom,"ADD_CONTACT_KO")) {
        cause = recupererArgument(reponse,"cause");
        if(cause != NULL) printf(MAG "\n>>> %s\n" RESET,cause);
        libererRequete(reponse);
        return ACCESS_DENIED;
    }

    //Cas ou le server renvoie une BAD_REQUEST car on a mal fait notre requete
    if (!strcmp(reponse->nom,"BAD_REQUEST")) {
        libererRequete(reponse);
        return BAD_REQUEST;
    }

    // Cas ou on a recu une requete mal formée (on renvoie un BAD request)
    req = initRequete(REPONSE,"BAD_REQUEST");
    erreur = envoyerRequete(req);
    if(erreur) return erreur;
    return BAD_REQUEST;
}
 
int demandeSuppressionContact(contact c) {
    requete req, reponse;
    int erreur;
    char* cause;

    //formation et envoi de la requète de suppression
    req = initRequete(REQUEST,"DELETE_CONTACT");
    ajouterArgument(req,"mail",c->mail);
    erreur = envoyerRequete(req);

    // erreurs possibles : BAD_REQUEST, BAD_SEND, BAD_SEND_DATA, OK(0)
    if(erreur) return erreur;

    reponse = allouerRequete();
    erreur = recupererRequete(reponse);

    //Cas normal
    if (!strcmp(reponse->nom,"DELETE_CONTACT_OK")) {
        libererRequete(reponse);
        return OK;
    }

    //Cas de denied du serveur. La cause est récupérée et affichée
    if(!strcmp(reponse->nom,"DELETE_CONTACT_KO")) {
        cause = recupererArgument(req,"cause");
        if(cause != NULL) printf(MAG "\n>>> %s\n" RESET,cause);
        libererRequete(reponse);
        return ACCESS_DENIED;
    }

    //Cas ou le server renvoie une BAD_REQUEST car on a mal formé notre requete
    if (!strcmp(reponse->nom,"BAD_REQUEST")) {
        libererRequete(reponse);
        return BAD_REQUEST;
    }

    // Cas ou on a recu une requete mal formée de la part du serveur 
    req = initRequete(REPONSE,"BAD_REQUEST");
    erreur = envoyerRequete(req);
    if(erreur) return erreur;
    return BAD_REQUEST;
}

int demandeAjoutPartage(char* mail) {
    requete req, reponse;
    int erreur;
    char* cause;

    //formation et envoi de requète de partage
    req = initRequete(REQUEST,"SHARE_DIRECTORY");
    ajouterArgument(req,"mail",mail);
    erreur = envoyerRequete(req);

    // erreurs possibles : BAD_REQUEST, BAD_SEND, BAD_SEND_DATA, OK(0)
    if(erreur) return erreur;

    reponse = allouerRequete();
    erreur = recupererRequete(reponse);

    //Cas normal
    if (!strcmp(reponse->nom,"SHARE_DIRECTORY_OK")) {
        libererRequete(reponse);
        return OK;
    }

    //Cas de denied du serveur. La cause est récupérée et affichée (ex mail utilisateur non trouvé)
    if(!strcmp(reponse->nom,"SHARE_DIRECTORY_KO")) {
        cause = recupererArgument(reponse,"cause");
        if(cause != NULL) printf(MAG "\n>>> %s\n" RESET,cause);
        libererRequete(reponse);
        return ACCESS_DENIED;
    }

    //Cas ou le server renvoie une BAD_REQUEST car on a mal formé notre requete
    if (!strcmp(reponse->nom,"BAD_REQUEST")) {
        libererRequete(reponse);
        return BAD_REQUEST;
    }

    // Cas ou on a recu une requete mal formée de la part du serveur 
    req = initRequete(REPONSE,"BAD_REQUEST");
    erreur = envoyerRequete(req);
    if(erreur) return erreur;
    return BAD_REQUEST;
}

int demandeSuppressionPartage(char* mail) {
    requete req, reponse;
    int erreur;
    char* cause;

    //formation et envoi de requète de partage
    req = initRequete(REQUEST,"CANCEL_SHAREDIRECTORY");
    ajouterArgument(req,"mail",mail);
    erreur = envoyerRequete(req);

    // erreurs possibles : BAD_REQUEST, BAD_SEND, BAD_SEND_DATA, OK(0)
    if(erreur) return erreur;

    reponse = allouerRequete();
    erreur = recupererRequete(reponse);
    
    //Cas normal
    if (!strcmp(reponse->nom,"CANCEL_SHAREDIRECTORY_OK")) {
        libererRequete(reponse);
        return OK;
    }

    //Cas de denied du serveur. La cause est récupérée et affichée
    if(!strcmp(reponse->nom,"SHARE_DIRECTORY_KO")) {
        cause = recupererArgument(req,"cause");
        if(cause != NULL) printf(MAG "\n>>> %s\n" RESET,cause);
        libererRequete(reponse);
        return ACCESS_DENIED;
    }

    //Cas ou le server renvoie une BAD_REQUEST car on a mal formé notre requete
    if (!strcmp(reponse->nom,"BAD_REQUEST")) {
        libererRequete(reponse);
        return BAD_REQUEST;
    }

    // Cas ou on a recu une requete mal formée de la part du serveur 
    req = initRequete(REPONSE,"BAD_REQUEST");
    erreur = envoyerRequete(req);
    if(erreur) return erreur;
    return BAD_REQUEST;
}