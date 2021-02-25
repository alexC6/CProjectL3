#include "../../inc/serveur/requetes_serveur.h"

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

int traiterAutentification(requete req, compte *cpt) {
    char* mail;
    char* password;
    requete reponse;
    compte cptBuffer;
    int erreur;
    mail = recupererArgument(req,"mail");
    password = recupererArgument(req,"password");
    //on verifie si on a les deux argument mail et password
    if(mail == NULL || password == NULL) {
        reponse = initRequete(REPONSE,"BAD_REQUEST");
        erreur = envoyerRequete(reponse);
        if(erreur) return erreur;
        return BAD_REQUEST;
    } 
    //on recupère le compte et verifions si le mail est correct (compte != NULL)

    cptBuffer = getAccount(mail);

    if(cptBuffer == NULL) {
        reponse = initRequete(REPONSE,"CONNECT_DENIED");
        ajouterArgument(reponse,"cause","Compte introuvable");
        erreur = envoyerRequete(reponse);
        if(erreur) return erreur;
        return BAD_ARGUMENT;
    }
    //On verifie si le password match
    if(strcmp(cptBuffer->mdp,password)) {
        reponse = initRequete(REPONSE,"CONNECT_DENIED");
        ajouterArgument(reponse,"cause","Mot de passe incorrect");
        erreur = envoyerRequete(reponse);
        if(erreur) return erreur;
        return BAD_ARGUMENT;
    }
    //dans le cas ou tout est bon on a juste a envoyer la reponse OK
    reponse = initRequete(REPONSE,"CONNECT_OK");
    erreur = envoyerRequete(reponse);
    if(erreur) return erreur;
    // Ici on sait que l'utilisateur est bien connecté, on peut donc mettre lier le compte
    *cpt = cptBuffer;
    printf(GRN ">>> utilisateur %d connecte : %s\n" RESET,(*cpt)->id,(*cpt)->mail);
    return OK;
}

int traiterDeconnexion(requete req, compte compte) {
    requete reponse;
    int erreur;
    //On enleve le compte
    printf(GRN ">>> utilisateur %d deconnecté : %s\n" RESET ,compte->id,compte->mail);
    compte = NULL;
    //On répond à l'utilisateur
    reponse = initRequete(REPONSE,"DISCONNECT_OK");
    erreur = envoyerRequete(reponse);
    if(erreur) return erreur;
    return DISCONECTION;

}

int traiterConsultationAnnuaire(requete req,compte compte) {
    requete reponse;
    char* mail;
    int erreur;
    annuaire annuaire;
    size_t tailleAnnuaire = 0;
    mail = recupererArgument(req,"mail");
    //on verifie si il y a un mail en argument
    if(mail == NULL) {
        reponse = initRequete(REPONSE,"BAD_REQUEST");
        erreur = envoyerRequete(reponse);
        if(erreur) return erreur;
        return BAD_REQUEST;
    }

    annuaire = getAnnuaire(mail,&tailleAnnuaire);
    //verifie si l'annuaire en question existe
    if(annuaire == NULL) {
        reponse = initRequete(REPONSE,"ACCESS_DIRECTORY_KO");
        ajouterArgument(reponse,"cause","Annuaire introuvable");
        erreur = envoyerRequete(reponse);
        if(erreur) return erreur;
        return BAD_ARGUMENT;
    }
    //verifie si le client à la permission
    if(!gotPermission(compte,mail)) {
        reponse = initRequete(REPONSE,"ACCESS_DIRECTORY_KO");
        ajouterArgument(reponse,"cause","Accès annuaire refusé");
        erreur = envoyerRequete(reponse);
        if(erreur) return erreur;
        return BAD_ARGUMENT;
    }
    //Cas normal
    reponse = initRequete(REPONSE,"ACCESS_DIRECTORY_OK");
    reponse->tDonnees = (tailleAnnuaire*sizeof(tContact))/sizeof(char);
    if(reponse->tDonnees > 0) reponse->donnees = dataToChar(annuaire,sizeof(tContact),tailleAnnuaire);
    erreur = envoyerRequete(reponse);
    if(erreur) return erreur;
    return OK;
}

int traiterListeAnnuaires(requete req, compte compte) {
    requete reponse;
    int erreur;
    char** listeMail;
    int* listeID;
    size_t tailleListe = 0;
    listeID = getView(compte->mail,&tailleListe);
    //verie si on arrive a accéder a cette liste
    if(tailleListe != 0 && listeID == NULL) {
        reponse = initRequete(REPONSE,"CONSULT_SHARED_DIRECTORIES_KO");
        ajouterArgument(reponse,"cause","Impossible d'accéder a la liste de mails");
        erreur = envoyerRequete(reponse);
        if(erreur) return erreur;
        return BAD_ARGUMENT;
    }
    listeMail = listIDToListMail(listeID,tailleListe);
    //Cas normal, on init la requete et remplis les données
    reponse = initRequete(REPONSE,"CONSULT_SHARED_DIRECTORIES_OK");
    reponse->tDonnees = SIZE_MAIL*tailleListe;
    if(reponse->tDonnees > 0) reponse->donnees = listeToChaine(listeMail,tailleListe,SIZE_MAIL);

    if(listeMail != NULL) free(listeMail);
    erreur = envoyerRequete(reponse);
    if(erreur) return erreur;
    return OK;
}

int traiterAccesAnnuaire(requete req, compte compte) {
    requete reponse;
    int erreur;
    char** listeMail;
    int* listeID;
    size_t tailleListe = 0;
    listeID = getShared(compte->mail,&tailleListe);
    //verie si on arrive a accéder a cette liste
    if(tailleListe != 0 && listeID == NULL) {
        reponse = initRequete(REPONSE,"CONSULT_USERS_ACCESS_KO");
        ajouterArgument(reponse,"cause","Impossible d'accéder a la liste de mails");
        erreur = envoyerRequete(reponse);
        if(erreur) return erreur;
        return BAD_ARGUMENT;
    }
    listeMail = listIDToListMail(listeID,tailleListe);
    //Cas normal, on init la requete et remplis les données
    reponse = initRequete(REPONSE,"CONSULT_USERS_ACCESS_OK");
    reponse->tDonnees = SIZE_MAIL*tailleListe;
    if(reponse->tDonnees > 0) reponse->donnees = listeToChaine(listeMail,tailleListe,SIZE_MAIL);

    if(listeMail != NULL) free(listeMail);
    erreur = envoyerRequete(reponse);
    if(erreur) return erreur;
    return OK;
}

int traiterAjouterContact(requete req, compte compte) {
    requete reponse;
    int erreur;
    contact ctt;
    //on verifie qu'il y a des données comparable a un contact
    if((req->tDonnees)*sizeof(char) != sizeof(tContact)) {
        reponse = initRequete(REPONSE,"ADD_CONTACT_KO");
        ajouterArgument(reponse,"cause","Les données envoyés sont de la mauvaise taille");
        erreur = envoyerRequete(reponse);
        if(erreur) return erreur;
        return BAD_SEND_DATA;
    }
    //on recupère le contact et on tente de le placer dans l'annuaire
    ctt = (contact) charToData(req->donnees,req->tDonnees);

    erreur = addContact(ctt,compte->mail);
    //si on y arrive pas on renvoie une erreur
    if(erreur) {
        reponse = initRequete(REPONSE,"ADD_CONTACT_KO");
        ajouterArgument(reponse,"cause","Impossible d'ajouter le contact sur le serveur");
        erreur = envoyerRequete(reponse);
        if(erreur) return erreur;
        return erreur;
    }
    //Cas normal
    reponse = initRequete(REPONSE,"ADD_CONTACT_OK");
    erreur = envoyerRequete(reponse);
    if(erreur) return erreur;
    return OK;
}

int traiterSupprimerContact(requete req, compte compte) {
    requete reponse;
    int erreur;
    char* mail;

    //récupéeation requète et mail pour cibler la supression
    mail = recupererArgument(req,"mail");
    if(mail == NULL) {
        reponse = initRequete(REPONSE,"BAD_REQUEST");
        erreur = envoyerRequete(reponse);
        if(erreur) return erreur;
        return BAD_REQUEST;
    }

    erreur = deleteContact(mail, compte->mail);

    //si on y arrive pas on renvoie une erreur
    if(erreur) {
        reponse = initRequete(REPONSE,"DELETE_CONTACT_KO");
        ajouterArgument(reponse,"cause","Impossible de supprimer le contact sur le serveur");
        erreur = envoyerRequete(reponse);
        if(erreur) return erreur;
        return erreur;
    }
    //Cas normal
    reponse = initRequete(REPONSE,"DELETE_CONTACT_OK");
    erreur = envoyerRequete(reponse);
    if(erreur) return erreur;
    
    return OK;
    }

int traiterAjouterPartage(requete req, compte compte) {
    requete reponse;
    int erreur;
    char* mail;
    
    //récupéeation requète et mail à ajouter à la liste des partages
    mail = recupererArgument(req,"mail");
    if(mail == NULL) {
        reponse = initRequete(REPONSE,"BAD_REQUEST");
        erreur = envoyerRequete(reponse);
        if(erreur) return erreur;
        return BAD_REQUEST;
    }

    erreur = addShared(compte->mail,mail);

    //si on y arrive pas on renvoie une erreur
    if(erreur) {
        reponse = initRequete(REPONSE,"SHARE_DIRECTORY_KO");
        ajouterArgument(reponse,"cause","Impossible d'ajouter le mail de partage");
        erreur = envoyerRequete(reponse);
        if(erreur) return erreur;
        return erreur;
    }
    //Cas normal
    reponse = initRequete(REPONSE,"SHARE_DIRECTORY_OK");
    erreur = envoyerRequete(reponse);
    if(erreur) return erreur;
    return OK;}

int traiterSupprimerPartage(requete req, compte compte) {
    requete reponse;
    int erreur;
    char* mail;
    
    //récupéeation requète et mail à retirer de la liste des partages
    mail = recupererArgument(req,"mail");
    if(mail == NULL) {
        reponse = initRequete(REPONSE,"BAD_REQUEST");
        erreur = envoyerRequete(reponse);
        if(erreur) return erreur;
        return BAD_REQUEST;
    }

    erreur = deleteShared(compte->mail,mail);

    //si on y arrive pas on renvoie une erreur
    if(erreur) {
        reponse = initRequete(REPONSE,"CANCEL_SHAREDIRECTORY_KO");
        ajouterArgument(reponse,"cause","Impossible de supprimer le mail de partage");
        erreur = envoyerRequete(reponse);
        if(erreur) return erreur;
        return erreur;
    }
    //Cas normal
    reponse = initRequete(REPONSE,"CANCEL_SHAREDIRECTORY_OK");
    erreur = envoyerRequete(reponse);
    if(erreur) return erreur;
    return OK;
    }