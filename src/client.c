#include "../inc/client.h"

int main(int argc, char *argv[]) {
    char mail[SIZE_MAIL];
    clrscr();// On clear le screen
    
    struct sigaction action;

    //initialisation et connexion
    if(connexionServeur(argc,argv)) return BAD_INITIALISATION;
    usleep(500000);
    while(menuConnexion(mail) != OK);

    //On initialise tout pour sortir du client avec un Ctrl-C
    action.sa_handler = signalHandler;
    sigemptyset(&(action.sa_mask));
    action.sa_flags = SA_RESETHAND;
    if (sigaction(SIGINT, &action, NULL) != 0) {
        perror("sigaction(SIGINT)");
        return 1;
    }

    //Coeur du programme
    menuPrincipal(mail);

    //Terinaison du programme
    demanderDeconnexion();
    Terminaison();
    clrscr();

    return 0;
}

void signalHandler(int num) {
    fprintf(stdout, GRN "\n>>> SIGINT reçu\n" RESET);
    sleep(1);
    fprintf(stdout, GRN ">>> Deconnection du client\n" RESET);
    demanderDeconnexion();
    Terminaison();
    exit(0);
}

int menuConnexion(char* mail){
    int error = 0;
    char* password;
    clrscr();
    //Selection du mail
    printf(BWHT "\n\tIDENTIFICATION\n" RESET);
    printf("\nMAIL : ");
    recupererChaine(mail,SIZE_MAIL-1,SIZE_MIN,FALSE,"mail");
    //Selection du mot de passe
    do {
        password = getpass("\nMOT DE PASSE : ");
    } while (strlen(password) > SIZE_PASSWORD -1);
    
    
    //Authentification
    clrscr();
    error = connectionCompte(mail,password);
    if(!error) free(password);// On free que quand on se co getpass réutilise le meme buffer
    //Affichage
    if(error == OK) printf(GRN ">>> Authentification réussie\n" RESET);
    sleep(1);
    return error;
}

int connexionServeur(int argc, char *argv[]) {
        switch(argc) {

                //Cas sans argument = lancement par default de la connexion
            case 1 : if(!Initialisation("localhost")) {
                return BAD_INITIALISATION;
            } break;

                //Cas avec un argument = lancement en local + port donné
            case 2 : if(!Initialisation(argv[1])) {
                fprintf(stderr,RED "Arguments incorrects\n" RESET);
                fprintf(stderr,"Usage : %s [adresse] [service]\n",argv[0]);
                return BAD_INITIALISATION;
            } break;

                //Cas avec 2 argument = lacement sur un serveur distant + port donné
            case 3 :  if(!InitialisationAvecService(argv[1],argv[2])) {
                fprintf(stderr,RED "Arguments incorrects\n" RESET);
                fprintf(stderr,"Usage : %s [adresse] [service]\n",argv[0]);
                return BAD_INITIALISATION;
            } break;
            
                //Cas mauvais nombre d'arguments
            default :
                fprintf(stderr,RED "Nombre Arguments incorrects\n" RESET);
                fprintf(stderr,"Usage : %s [adresse] [service]\n",argv[0]);
                return BAD_INITIALISATION;
    }
    return OK;
}

int menuPrincipal(char* mail) {
    char choix = '0';
    
    while(choix != '4') {

        //Affichage et selection du choix
        clrscr();
        printf(BWHT "\n\tMENU PRINCIPAL :\n" RESET);
        printf("\n\t[1] Consulter Annuaire Perso\n\t[2] Consulter Autre annuaire\n\t[3] Rechercher un contact \n\t[4] Deconnexion\n");
        choix = recupererChar("1234");

        //Selection du choix
        switch (choix) {
            case '1': menuPerso(mail);
            break;
            case '2': menuAutreAnnuaire(mail);
            break;
            case '3' : rechercheGlobale(mail);
            break;
            case '4': return OK;
        }
    }
    return OK;
}

int menuPerso(char* mail) {
    annuaire annuaire;
    size_t tailleAnnuaire;
    char choix = '0';
    
    while(choix != '5') {
        
        //affichage et reception de l'annuaire
        clrscr();
        annuaire = recupererAnnuaire(mail,&tailleAnnuaire);
        if(annuaire == NULL && tailleAnnuaire == -1) return NO_MESSAGES;
        afficherAnnuaire(annuaire,tailleAnnuaire);

        //affichage des options
        printf(BWHT "\n\tMENU ANNUAIRE PERSONNEL :\n" RESET);
        printf("\n\t[1] Ajouter un contact\n\t[2] Modifier un contact\n\t[3] Supprimer un contact\n\t[4] Consulter partages\n\t[5] Retour\n");
        choix = recupererChar("12345");

        //selection du choix
        switch (choix) {

            case '1': 
                clrscr();
                afficherAnnuaire(annuaire,tailleAnnuaire);
                ajouterContact(annuaire, tailleAnnuaire);
            break;

            case '2': 
                clrscr();
                afficherAnnuaire(annuaire,tailleAnnuaire);
                modifierContact(annuaire, tailleAnnuaire);
            break;

            case '3': 
                clrscr();
                afficherAnnuaire(annuaire,tailleAnnuaire);
                supprimerContact(annuaire, tailleAnnuaire);
            break;

            case '4': menuPartage(mail);
        }

        free(annuaire);
    }
    return OK;
}

int menuPartage(char* mail) {
    char** accesAnnuaire;
    size_t tailleAcces;
    char choix = '0';

    while(choix != '3') {

        clrscr();
        //Affichage des utilisateur ayant le droit de lire l'annuaire de l'utilisateur
        accesAnnuaire = recupererAccesAnnuaires(&tailleAcces);
        if(tailleAcces != 0 && accesAnnuaire == NULL) return NO_MESSAGES;

        //Affichage du menu
        printf(BWHT "\n\tMENU GESTION PARTAGES :\n\n" RESET);
        afficherListe(accesAnnuaire,tailleAcces);
        printf("\n\t[1] Ajouter un partage\n\t[2] Supprimer un partage\n\t[3] Retour\n");
        choix = recupererChar("123");

        //selection du choix
        switch (choix) {
            case '1': ajouterPartage(accesAnnuaire,tailleAcces);
            break;
            case '2': supprimerPartage(accesAnnuaire,tailleAcces);
            break;
        }
        libererPointeurChaine(accesAnnuaire,tailleAcces);
    }
    return OK;
}

int menuAutreAnnuaire(char* mail) {
    annuaire annuaire;
    size_t tailleAnnuaire = -1;
    char** mailsAnnuaires;
    size_t tailleMailsAnnuaires;

    //on recupère la liste des annuaires et on l'affiche
    mailsAnnuaires =  recupererSharedAnnuaires(&tailleMailsAnnuaires);
    if(tailleMailsAnnuaires == -1) {
        sleep(1);
        return NO_MESSAGES;
    }

    //on vérifie qu il y a bien quelqu'un qui a partagé son annuaire avec le client
    if(mailsAnnuaires == NULL) {
        clrscr();
        printf(MAG "\n>>> Personne à partagé d'annuaire avec vous :'(\n" RESET);
        sleep(1);
        return OK;
    }

    //Affichage de la liste
    clrscr();
    printf(BWHT "\n\tANNUAIRES DISPONIBLES :\n\n" RESET);
    afficherListe(mailsAnnuaires,tailleMailsAnnuaires);

    //on demande a l'utilisateur l'annuaire en question et si le serveur le donne, l'affiche
    annuaire = selectionAutreAnnuaire(mailsAnnuaires,tailleMailsAnnuaires,&tailleAnnuaire);
    libererPointeurChaine(mailsAnnuaires,tailleMailsAnnuaires);
    if(annuaire == NULL && tailleAnnuaire == -1) {
        sleep(1);
        return DATA_MISSING;
    }
    afficherAnnuaire(annuaire,tailleAnnuaire);
    
    tailleAnnuaire = getchar(); //Pour effectuer un point d'arret (la variable est pour prendre en compte le getchar, sinon il le skip)
    free(annuaire);
    return OK;
}

int ajouterContact(annuaire annuaire, size_t tailleAnnuaire) {
    int error = OK;

    //on initialise et met tout les champs de contact à 0
    contact contact = malloc(sizeof(tContact));
    memset(contact, 0, sizeof(tContact));

    printf(UWHT "\n\tNouveau contact\n" RESET);
    contact->id = -1;

    //on demande un a un les champs
    printf("\nNOM : ");
    recupererChaine(contact->nom,SIZE_NOM-1,SIZE_MIN,FALSE,"nom");

    printf("\nPRENOM : ");
    recupererChaine(contact->prenom,SIZE_PRENOM-1,SIZE_MIN,FALSE,"prenom");

    //pour le mail on verifie qu'il n'existe pas dans le mail local
    do {
        if(error != OK) printf(MAG "\n>>> Ce mail existe deja dans l'annuaire, veillez en selectionner un autre\n" RESET);
        printf("\nMAIL : ");
        recupererChaine(contact->mail,SIZE_MAIL-1,SIZE_MIN,FALSE,"mail");
        error = (getContact(annuaire, tailleAnnuaire, contact->mail) != NULL);
    } while(error);

    printf("\nTELEPHONE : ");
    recupererChaine(contact->tel,SIZE_TEL-1,SIZE_TEL-1,FALSE,"telephone");

    printf("\nADRESSE : ");
    recupererChaine(contact->adresse,SIZE_ADRESSE-1,SIZE_MIN,FALSE,"adresse");

    printf("\nVILLE : ");
    recupererChaine(contact->ville,SIZE_CITY-1,SIZE_MIN,FALSE,"adresse");

    printf("\nCODE POSTAL : ");
    recupererChaine(contact->cp,SIZE_CP-1,SIZE_CP-1,FALSE,"code postal");

    //On demande l'ajout de contact et on affiche (demande Ajout Contact affiche une erreur dans le cas ou il y a un soucis)
    clrscr();
    error = demandeAjoutContact(contact);
    if(!error) printf(GRN "\n>>> Contact ajouté\n" RESET);
    sleep(1);

    return error;
}

int modifierContact(annuaire annuaire, size_t tailleAnnuaire) {
    //On initialise également un deuxieme champ contact nommée modification, pour comparer les résultats (dans le cas ou l'on a pas de mondifications)
    char mail[SIZE_MAIL];
    contact modifications = malloc(sizeof(struct sContact));
    contact contact = NULL;
    int error;

    printf(UWHT "\n\tModifier contact\n" RESET);
    
    //On vérifie que le contact existe, c'est a dire qu'il est dans l'annuaire passé en argument
    while (contact == NULL) {
        printf("\nMAIL DU CONTACT : ");
        recupererChaine(mail,SIZE_MAIL-1,SIZE_MIN,TRUE,"mail");
        if(mail[0] == '\0') return ANNULATION;
        contact = getContact(annuaire,tailleAnnuaire,mail);
        if(contact == NULL) printf(MAG "\n>>> Ce contact n'existe pas !" RESET);
    }

    //Pour toutes les modification, on écrit d'abord sur modifications, et ensuite on le copie sur le vrai contact si l'utilisateur n'a pas skip le message (\0)
    printf("NOM : ");
    recupererChaine(modifications->nom,SIZE_NOM-1,SIZE_MIN,TRUE,"nom");
    if(modifications->nom[0] != '\0') strcpy(contact->nom,modifications->nom);

    printf("PRENOM : ");
    recupererChaine(modifications->prenom,SIZE_PRENOM-1,SIZE_MIN,TRUE,"prenom");
    if(modifications->prenom[0] != '\0') strcpy(contact->prenom,modifications->prenom);

    printf("MAIL : ");
    recupererChaine(modifications->mail,SIZE_MAIL-1,SIZE_MIN,TRUE,"mail");
    if(modifications->mail[0] != '\0') strcpy(contact->mail,modifications->mail);

    printf("TELEPHONE : ");
    recupererChaine(modifications->tel,SIZE_TEL-1,SIZE_TEL-1,TRUE,"telephone");
    if(modifications->tel[0] != '\0') strcpy(contact->tel,modifications->tel);

    printf("ADRESSE : ");
    recupererChaine(modifications->adresse,SIZE_ADRESSE-1,SIZE_MIN,TRUE,"adresse");
    if(modifications->adresse[0] != '\0') strcpy(contact->adresse,modifications->adresse);

    printf("VILLE : ");
    recupererChaine(modifications->ville,SIZE_CITY-1,SIZE_MIN,TRUE,"ville");
    if(modifications->ville[0] != '\0') strcpy(contact->ville,modifications->ville);

    printf("CODE POSTAL : ");
    recupererChaine(modifications->cp,SIZE_CP-1,SIZE_CP-1,TRUE,"code postal");
    if(modifications->cp[0] != '\0') strcpy(contact->cp,modifications->cp);

    //on recopie les modifications faites sur modification (contact etant dans annuaire, on ne le veux pas le free)
    memcpy(modifications,contact,sizeof(struct sContact));

    //On demande l'ajout de contact et on affiche (demandeAjoutContact affiche une erreur dans le cas ou il y a un soucis)
    clrscr();
    error = demandeAjoutContact(modifications);
    if(!error) printf(GRN "\n>>> Contact modifié\n" RESET);
    sleep(1);

    return error;
}

int supprimerContact(annuaire annuaire, size_t tailleAnnuaire) {
    char mail[SIZE_MAIL];
    contact contact= NULL;
    int error = OK;

    printf(UWHT "\n\tSuppression contact\n" RESET);

    //On récupère le mail en prenant soin de vérifier qu'il existe bien dans l'annuaire
    do {
        printf("\nMAIL : ");
        recupererChaine(mail,SIZE_MAIL-1,SIZE_MIN,TRUE,"mail");
        if(mail[0] == '\0') return ANNULATION;
        contact = getContact(annuaire,tailleAnnuaire,mail);
        error = (contact == NULL);
        if(error) printf(MAG "\n>>> Le contact n'existe pas !" RESET);
    } while (error);

    //On demande la suppression de contact et on affiche (demandeSuppressionContact affiche une erreur dans le cas ou il y a un soucis)
    clrscr();
    error = demandeSuppressionContact(contact);
    if(!error) printf(GRN "\n>>> Contact supprimé\n" RESET);
    sleep(1);
    return error;
}

int ajouterPartage(char** accesAnnuaire, size_t tailleAcces) {
    char mail[SIZE_MAIL];
    int error = OK;

    printf(UWHT "\tAjout partage\n\n" RESET);
    
    //On récupère le mail de l'utilisateur à qui partager en prenant soin de vérifier qu'il existe pas deja
    do {
        if(error) printf(MAG "\n>>> Le partage existe deja !" RESET);
        printf("\nMAIL : ");
        recupererChaine(mail,SIZE_MAIL-1,SIZE_MIN,TRUE,"mail");
        if(mail[0] == '\0') return ANNULATION;
    } while ((error = existeChaine(accesAnnuaire,tailleAcces,mail)));

    //On demande l'ajout du partage et on affiche (demandeAjoutPartage affiche une erreur dans le cas ou il y a un soucis)
    clrscr();
    error = demandeAjoutPartage(mail);
    if(!error) printf(GRN "\n>>> Partage ajouté\n" RESET);
    sleep(1);

    return error;
}

int supprimerPartage(char** accesAnnuaire, size_t tailleAcces) {
    char mail[SIZE_MAIL];
    int error = OK;

    printf(UWHT "\n\tSupprimer partage\n" RESET);
    
    //On récupère le mail de l'utilisateur à supprimer du partage en prenant soin de vérifier qu'il existe
    do {
        if(error) printf(MAG "\n>>> Le partage n'existe pas !" RESET);
        printf("\nMAIL : ");  
        recupererChaine(mail,SIZE_MAIL-1,SIZE_MIN,TRUE,"mail");
        if(mail[0] == '\0') return ANNULATION;
    } while ((error = !existeChaine(accesAnnuaire,tailleAcces,mail)));

    //On demande la supression du partage et on affiche (demandeSuppressionPartage affiche une erreur dans le cas ou il y a un soucis)
    clrscr();
    error = demandeSuppressionPartage(mail);
    if(!error) printf(GRN "\n>>> Partage supprimé\n" RESET);
    sleep(1);

    return error;
}

annuaire selectionAutreAnnuaire(char** listePartage,size_t taillePartage , size_t* tailleAnnuaire) {
    char mail[SIZE_MAIL];
    int error = OK;
    
    printf(UWHT "\n\tSelection autre annuaire\n" RESET);
    
    //On recupère le mail de celui qui à partagé son annuaire avec l'utilisateur
    do {
        if(error) printf(MAG "\n>>> Cet utilisateur (si il existe) n'a pas partagé d'annuaires avec vous !\n" RESET);
        printf("\nMAIL : ");
        recupererChaine(mail,SIZE_MAIL-1,SIZE_MIN,TRUE,"mail");
        if(mail[0] == '\0') return NULL;
    } while ((error = !existeChaine(listePartage, taillePartage, mail)));

    return recupererAnnuaire(mail,tailleAnnuaire);
}

int rechercheGlobale (char *mail)
{
    char* chaineArechercher;
    char ** liste;
    size_t tailleListe;
    annuaire annuaire;
    size_t tailleAnnuaire;
    int i;
    int nbTrouvee = 0;
    int trouve = 0 ;

    chaineArechercher = malloc(sizeof(char)*SIZE_ADRESSE);

    clrscr();
    
    //On récupère le champ de recherche
    printf(UWHT "\n\tRecherche dans tous les annuaires\n" RESET);
    printf("\nRECHERCHE :");
    recupererChaine(chaineArechercher,SIZE_ADRESSE,3,FALSE,"chaine à rechercher");  
    clrscr();

    //On effectue la recherche sur l'annuaire de l'utilisateur
    annuaire = recupererAnnuaire(mail, &tailleAnnuaire);

    if (tailleAnnuaire == -1){
        fprintf(stderr,RED "\n>>> Erreur lors de la tentative de récupération de l'annuaire\n" RESET);
        free(chaineArechercher);
        getchar();
        return EXIT_FAILURE;
    }

    trouve |= rechercheAnnuaire( annuaire, tailleAnnuaire, chaineArechercher, &nbTrouvee);

    free(annuaire);

    //On effectue la recherche sur l'annuaire des personnes ayant partagé leurs annuaire avec l'utilisateur
    liste = recupererSharedAnnuaires(&tailleListe);

    for ( i = 0; i<tailleListe;i++){

        annuaire = recupererAnnuaire(liste[i],&tailleAnnuaire);

        if (tailleAnnuaire == -1){
            fprintf(stderr,RED "\n>>> Erreur lors de la tentative de récupération de l'annuaire\n" RESET);
            free(chaineArechercher);
            libererPointeurChaine(liste,tailleListe);
            sleep(1);
            return EXIT_FAILURE;
        }

        trouve |= rechercheAnnuaire( annuaire, tailleAnnuaire,chaineArechercher, &nbTrouvee);

        free(annuaire);

    }
    
    libererPointeurChaine(liste, tailleListe);
    free(chaineArechercher);

    if (!trouve){
        printf(MAG "\n>>> Pas de contact trouvé avec les paramètres renseignés \n" RESET);
    }
    getchar(); //blockage pour afficher l'annuaire
    return OK;
}



int rechercheAnnuaire(annuaire annuaire,size_t tailleAnnuaire, char *chaine, int* nbTrouvee){

    int i;
    int occ;
    int trouve=FALSE;
    
    //On recherche une occurance pour chaque contact
     for (i = 0; i < tailleAnnuaire; i++){
        occ = occurenceContact(chaine, &annuaire[i]);

        //Si il y a une occurence, on l'affiche
        if(occ){
            if(*nbTrouvee == 0) afficherIndicationAnnuaire();
            afficherContact(&annuaire[i], *nbTrouvee);
            trouve = TRUE;
            (*nbTrouvee)++;
        }      
     }

    return trouve;
}