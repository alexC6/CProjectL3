/**
 * @file structures.h
 * @author Alexandre Coulais - Guillaume Fourcroy
 * @brief Description des structures des comptes utilisateur et des contacts
 * @version 0.1
 * @date 2021-01-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef _STRUCTURES_
#define _STRUCTURES_

#define SIZE_MAIL 50
#define SIZE_NOM 30
#define SIZE_PRENOM 30
#define SIZE_PASSWORD 30
#define SIZE_SHARED 100
#define SIZE_VIEW 100
#define SIZE_TEL 11
#define SIZE_ADRESSE 100
#define SIZE_CITY 30
#define SIZE_CP 6

#define SIZE_MIN 3
#define SIZE_MIN_PASSWORD 3

#define TRUE 1
#define FALSE 0


/**
 * @brief Structure de données d'un compte
 * 
 */
typedef struct sCompte {
    int id;
    char mail[SIZE_MAIL];
    char nom[SIZE_NOM];
    char prenom[SIZE_PRENOM];
    char mdp[SIZE_PASSWORD];
    int nbContact;
    int idShared[SIZE_SHARED];
    int nbShared;
    int idView[SIZE_VIEW];
    int nbView;
} tCompte;

typedef tCompte *compte;

/**
 * @brief Structure de donnée d'un contact
 * 
 */
typedef struct sContact {
    int id;
    char mail[SIZE_MAIL];
    char nom[SIZE_NOM];
    char prenom[SIZE_PRENOM];
    char tel[SIZE_TEL];
    char adresse[SIZE_ADRESSE];
    char ville[SIZE_CITY];
    char cp[SIZE_CP];
} tContact;

/**
 * @brief Redéfinition du contact
 * 
 */
typedef tContact *contact;

/**
 * @brief Redefinition d'un annuaire
 * 
 */
typedef contact annuaire;

#endif