#include "../inc/serveur.h"

int main(int argc, char *argv[]) {
    
    compte comptes;
    char choix = 0;
    clrscr();//On commence par clear le screan, pour en avoir un tout propre

    //On initialise le serveur
    if(initialisationServeur(argc, argv)) return BAD_INITIALISATION;
    sleep(1);//Attente d'une seconde pour les messages

    //On charge les comptes
    comptes = loadAccount();
    comptes = comptes;// ALED
    if(getfileError()) return BAD_ACCESS_FILES;
    sleep(1);//Attente d'une seconde pour les messages

    //Menu principal
    while (choix != '3') {
        
        clrscr();
        printf(BWHT "\n\tMENU PRINCIPAL\n" RESET);
        printf("\n\t[1] Lancer le serveur\n\t[2] Acceder a l'interface d'administration\n\t[3] Quitter\n");

        choix = recupererChar("123");

        switch(choix) {

        case '1':
            clrscr();
            lancementServeur();
            break;

        case '2': 
            administration();
            break;
        }
    }
    //On decharge le serveur et on clear le screen
    unload();
    clrscr();
    return OK;
}

void signalHandler(int num) {
    fprintf(stdout, GRN "\n>>> SIGINT reçu\n" RESET);
    sleep(1);
    fprintf(stdout, GRN ">>> Interruption du serveur\n" RESET);
}

int lancementServeur() {
    requete req;
    int error;
    compte compte = NULL;

    //On initialise tout pour sortir du serveur avec un Ctrl-C
    struct sigaction action;
    action.sa_handler = signalHandler;
    sigemptyset(&(action.sa_mask));
    action.sa_flags = SA_RESETHAND;
    if (sigaction(SIGINT, &action, NULL) != 0) {
        perror("sigaction(SIGINT)");
        return 1;
    }

    //On boucle infinie sur l'attente du client
    while(1) {
        error = -1;

        printf(GRN ">>> Attente du client\n" RESET);
        sleep(1);
        printf(GRN ">>> Ctrl-C pour quitter\n" RESET);

        if (!AttenteClient()) {
            return 1;
        }

        //NO_MESSAGES => le client s'est deconnecté brutalement
        while(error != NO_MESSAGES && error != DISCONECTION) {

            //on recupère la première requete du client 
            req = allouerRequete();
            error = recupererRequete(req);

            //si il y a pas de soucis on la traite en tant qu'authentification
            if(!error) error = traiterAutentification(req,&compte);
            libererRequete(req);

            if(!error) {

                //on boucle tant qu'il y a pas de messages
                while(error != NO_MESSAGES && error != DISCONECTION) {
                    req = allouerRequete();
                    error = recupererRequete(req);

                    if(error && error != DISCONECTION) fprintf(stderr,RED ">>> Erreur %d\n" RESET,error);
                    if(traiterRequete(req,compte) == DISCONECTION) error = DISCONECTION;

                    libererRequete(req);
                }
            }
        }
        TerminaisonClient();
        sleep(1);
        clrscr();
    }
    Terminaison();
}

int initialisationServeur(int argc, char *argv[]) {
        switch(argc) {

            //Cas sans arguments
            case 1 : if(!Initialisation()) {
                sleep(1);               
                return BAD_INITIALISATION;
            }
            break;

            //Cas avec un argument (le service)
            case 2 : if(!InitialisationAvecService(argv[1])) {
                fprintf(stderr,RED "Arguments incorrects\n" RESET);
                fprintf(stderr,"Usage : %s [service]\n",argv[0]);
                sleep(1);
                return BAD_INITIALISATION;
            }
            break;

            //Cas avec trop d'arguments
            default :
                fprintf(stderr,RED "Nombre Arguments incorrects\n" RESET);
                fprintf(stderr,"Usage : %s [service]\n",argv[0]);
                sleep(1);
                return BAD_INITIALISATION;
    }
    return 0;
}

int traiterRequete(requete req, compte compte) {
    requete reponse;
    int erreur;

    if (req->type == REQUEST) {

        //On répond pour chaque requete du client grâce a la fonction adéquate
        if(!strcmp(req->nom,"CONSULT_DIRECTORY")) return traiterConsultationAnnuaire(req,compte);
        if(!strcmp(req->nom,"CONSULT_SHARED_DIRECTORIES")) return traiterListeAnnuaires(req,compte);
        if(!strcmp(req->nom,"CONSULT_USERS_ACCESS")) return traiterAccesAnnuaire(req,compte);
        if(!strcmp(req->nom,"ADD_CONTACT")) return traiterAjouterContact(req,compte);
        if(!strcmp(req->nom,"DELETE_CONTACT")) return traiterSupprimerContact(req,compte);
        if(!strcmp(req->nom,"SHARE_DIRECTORY")) return traiterAjouterPartage(req,compte);
        if(!strcmp(req->nom,"CANCEL_SHAREDIRECTORY")) return traiterSupprimerPartage(req,compte);
        if(!strcmp(req->nom,"DISCONNECT")) return traiterDeconnexion(req, compte);
    }
    //Dans le cas ou a une requête inconnue
    reponse = initRequete(REPONSE,"BAD_REQUEST");
    erreur = envoyerRequete(reponse);
    if(erreur) return erreur;
    return BAD_REQUEST;
}

int administration() {
    char** mails;
    char** informations;
    size_t nbMails;
    int i;
    
    char choix = '0';
    while(choix != '4') {

        //On récupère la liste de mail
        mails = getMailList(&nbMails);
        informations = getInfoComptes();

        //Affichage
        clrscr();
        printf(BWHT "\n\tCOMPTES :\n\n" RESET);
        for(i = 0; i < nbMails; i++) printf("\t%s %s\n",mails[i],informations[i]);

        libererPointeurChaine(mails,nbMails);
        libererPointeurChaine(informations,nbMails);

        //On effectue un choix et on l'execute
        choix = menuAdministration();
        executerChoixMenuAdministration(choix);
    }
    return OK;
}

char menuAdministration() {
    char choix = 0;

    printf("\n\t[1] Ajouter un compte\n\t[2] Modifier un Compte\n\t[3] Supprimer un compte\n\t[4] Retour\n");
    choix = recupererChar("1234");
    return choix;
}

int executerChoixMenuAdministration(char choix) {
    clrscr();
    //On effectue une fonction par rapport au choix
    switch(choix) {
        case '1': 
            return ajouterCompte();
        case '2': 
            return modifierCompte();
        case '3': 
            return supprimerCompte();
    }
    return OK; //Case 4
}

int ajouterCompte() {
    int erreur;
    //On alloue et initialise le compte à 0
    compte compte = malloc(sizeof(tCompte));
    memset(compte, 0, sizeof(tCompte));

    printf(UWHT "\tNouveau compte\n\n" RESET);
    compte->id = -1;

    //On récupère chaque champs
    printf("MAIL : ");
    recupererChaine(compte->mail,SIZE_MAIL-1,3,TRUE,"mail");
    if(compte->mail[0] == '\0') {
        free(compte);
        return ANNULATION;
    }

    printf("\nNOM : ");
    recupererChaine(compte->nom,SIZE_NOM-1,3,FALSE,"nom");

    printf("\nPRENOM : ");
    recupererChaine(compte->prenom,SIZE_PRENOM-1,3,FALSE,"prenom");

    printf("\nMOT DE PASSE : ");
    recupererChaine(compte->mdp,SIZE_PASSWORD-1,SIZE_MIN_PASSWORD,FALSE,"mot de passe");

    //On initialise les valeur du compte
    compte->nbContact = 0;
    compte->nbShared = 0;
    compte->nbView = 0;

    //Le addAccount gère et affiche les erreurs;
    clrscr();
    erreur = addAccount(compte);
    if(!erreur) printf(GRN ">>> Compte ajouté\n" RESET);
    sleep(1);
    return erreur;
}

int modifierCompte() {
    int erreur;
    char mail[SIZE_MAIL];
    
    //On créer un second compte tampon pour prendre les modifications (dans le cas ou l'utilisateur passe certaines modifications)
    compte modifications = malloc(sizeof(struct sCompte));
    compte compte = NULL;

    //pour reset le file error
    getfileError();
    //On récupère le mail du compte que si il existe
    do {  
        if (getfileError() == NO_ACCOUNT) {
            clrscr();
            printf(RED ">>> Ce compte n'existe pas\n"RESET);
            sleep(1);
            clrscr();
        }
        printf(UWHT "\tModifier compte\n\n" RESET);
        printf("\nMAIL DU COMPTE A MODIFIER : ");
        recupererChaine(mail,SIZE_MAIL-1,SIZE_MIN,TRUE,"mail");
        if(mail[0] == '\0') {
            free(modifications);
            return ANNULATION;
        }
    } while ((compte = getAccount(mail)) == NULL);

    //On modifie les champs un a un (l'utilisateur peu passer des champs)
    printf("MAIL : ");
    recupererChaine(modifications->mail,SIZE_MAIL-1,SIZE_MIN,TRUE,"mail");
    if(modifications->mail[0] != '\0') strcpy(compte->mail,modifications->mail);
   
    printf("\nNOM : ");
    recupererChaine(modifications->nom,SIZE_NOM-1,SIZE_MIN,TRUE,"nom");
    if(modifications->nom[0] != '\0') strcpy(compte->nom,modifications->nom);

    printf("\nPRENOM : ");
    recupererChaine(modifications->prenom,SIZE_PRENOM-1,SIZE_MIN,TRUE,"prenom");
    if(modifications->prenom[0] != '\0') strcpy(compte->prenom,modifications->prenom);

    printf("\nMOT DE PASSE : ");
    recupererChaine(modifications->mdp,SIZE_PASSWORD-1,SIZE_MIN_PASSWORD,TRUE,"mot de passe");
     if(modifications->mdp[0] != '\0') strcpy(compte->mdp,modifications->mdp);

    free(modifications);
    clrscr();

    //Le addAccount gère et affiche les erreurs;
    erreur = addAccount(compte);
    if(!erreur) printf(GRN ">>> Compte modifié\n" RESET);
    sleep(1);
    return erreur;
}

int supprimerCompte() {    
    char mail[SIZE_MAIL];
    int erreur = OK;
    //On récupère le mail en vérifiant que l'on peut supprimer celui ci 
    do {
        printf(UWHT "\tSuppression compte\n\n" RESET);
        printf("\nMAIL DU COMPTE A SUPPRIMER : ");
        recupererChaine(mail,SIZE_MAIL-1,SIZE_MIN,TRUE,"mail"); 
        if(mail[0] == '\0') return ANNULATION;
        clrscr();
        erreur = deleteAccount(mail);
        if(erreur) sleep(1);
        clrscr();
    } while (erreur);

    printf(GRN ">>> Compte supprimé\n" RESET);
    sleep(1);
    return OK;
}
