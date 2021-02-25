#ifndef _UTILITAIRE_
#define _UTILITAIRE_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <unistd.h>
#include "error.h"
#include "structures.h"
#include "color.h"

#define REQUEST 0
#define REPONSE 1
#define NB_ARG_MAX 20

/**
 * @brief Structure de donnée arguments (utilisé dans la structure requete)
 * @author Guillaume FOURCROY
 */
typedef struct sArgument {
    char *nom;
    char *valeur;
} tArgument;

/**
 * @brief Redefinition argument
 * @author Guillaume FOURCROY
 */
typedef tArgument *argument;

/**
 * @brief Structure de donnée requete pour une meilleur gestion de celle ci
 * @author Guillaume FOURCROY
 */
typedef struct sRequete {
    int type;
    char* nom;
    tArgument arguments[NB_ARG_MAX];
    int nbArgs;
    char* donnees;
    size_t tDonnees;
} tRequete;

/**
 * @brief Redefinition d'une requete
 * @author Guillaume FOURCROY
 */
typedef tRequete *requete;

/**
 * @brief Formate des messages dans une structure requête
 * @author Guillaume FOURCROY
 * 
 * @param requete La requete finale
 * @param messages les messages à formater
 * @param nbMessages nombre de message transmit
 * @return int BAD_REQUEST ou OK
 */
int formaterEnRequete(requete requete, char** messages, int nbMessages);

/**
 * @brief 
 * @author Guillaume FOURCROY
 * 
 * @param requete requete à formater DOIT ETRE ALOOUE EN MEMOIRE
 * @param messages liste de messages, la liste doit être aloué en memoire avant
 * @param nbMessages le nombre de messages à envoyer
 * @return char* requete formaté
 */
int formaterEnChaine(requete requete, char** messages, int* nbMessages);

/**
 * @brief Recupère une partie d'une chaine de charactère en fonction d'un separateur
 * @author Guillaume FOURCROY
 * 
 * @param source chaine de base
 * @param destination1 première partie de la chaine
 * @param destination2 deuxième partie de la chaine
 * @param separateur séparateur manrquant la fin
 * @return int 1 si un des deux element sont vide 0 sinon
 */
int diviserChaine(char* source, char* destination1,char* destination2, char separateur);

/**
 * @brief Initialise la requete
 * @author Guillaume FOURCROY
 * 
 * @param type le type de la requete REPONSE ou REQUETE
 * @param nom le nom de la requete
 * @return requete la requete initialisé
 */
requete initRequete(int type, char* nom);

/**
 * @brief Ajoute un argument à une requete
 * @author Guillaume FOURCROY
 * 
 * @param requete requete à modifier
 * @param nom nom de l'argument
 * @param valeur valeur de l'argument
 * @return int 1 si tout s'est bien passé 0 sinon
 */
int ajouterArgument(requete requete, char *nom, char *valeur);

/**
 * @brief Récupère la valeur d'un argument d'une structure
 * @author Guillaume FOURCROY
 * 
 * @param requete requete ou l'on veut recupérer l'argument
 * @param nomArgument nom de l'argument à recuperer
 * @return char* valeur de l'argument récupéré
 */
char* recupererArgument(requete requete, char* nomArgument);

/**
 * @brief Libère la requete
 * @author Guillaume FOURCROY
 * 
 * @param requete requete à liberer
 */
void libererRequete(requete requete);

/**
 * @brief Alloue une requete et met tout ses variables a NULL
 * @author Guillaume FOURCROY
 * 
 * @return requete allouée
 */
requete allouerRequete();

/**
 * @brief libère un tableau de chaines
 * 
 */
void libererPointeurChaine(char** chaines, int nbChaines);

/**
 * @brief Convertie une chaine de caractère en structures de données
 * @author Guillaume FOURCROY
 * 
 * @param donnees Chaine de caractères en entrée
 * @param nbDonnees nb Chaine de caractères en entrée
 * @return char* Les données converties en structures
 */
void* charToData(char* donnees,size_t nbDonnees);

/**
 * @brief Convertie une structure de donnée en chaine de caractères
 * @author Guillaume FOURCROY
 * 
 * @param structures La structure à convertire
 * @param tailleStructure La taille de la structure
 * @param nbStructures Le nombre de structures entrées
 * @return char* Les données converties en char
 */
char* dataToChar(void* structures, size_t tailleStructure, size_t nbStructures);

/**
 * @brief Affiche une liste de chaine de caractères
 * @author Guillaume FOURCROY
 * 
 * @param liste La liste à afficher
 * @param taille La taille de la liste
 */
void afficherListe(char** liste, size_t taille);


/**
 * @brief Vide le buffeur de lecture avec getchar
 * @author Guillaume FOURCROY
 * 
 */
void freeBuffer();

/**
 * @brief Lire une chaine avec un buffeur d'une longueur donnée
 * @author Guillaume FOURCROY
 * 
 * @param chaine chaine lue
 * @param longueur buffer de la chaine
 * @return int 
 */
int lire(char *chaine, int longueur);

/**
 * @brief Recupère le premier charactère d'une chaine saisie et la verifie avec la liste donnée
 * @author Guillaume FOURCROY
 * 
 * @param liste liste de caractères possible pour l'entrée
 * @return char Le caractère récupéré
 */
char recupererChar(char* liste);

/**
 * @brief Recupère une chaine de longueur comprise entre deux valeurs
 * @author Guillaume FOURCROY
 * 
 * @param chaine la chaine à recupérer
 * @param longueurMax la valeur max de la chaine
 * @param longueurMin la valeur min de la chaine
 * @param skip notifie si on a le droit de skip la chaine
 * @param nomVariable nom de la variable a entrer
 */
void recupererChaine(char* chaine,int longueurMax,int longueurMin, int skip, char* nomVariable);

/**
 * @brief Pour les chaines qui sont mal découpé sur des retour a la lignes
 * @author Guillaume FOURCROY
 * 
 * @param message le message a verifier/decouper
 * @param dest1 la decoupe 1
 * @param dest2 la decoupe 2
 * @return int 
 */
int correctifChaine(char* message,char** destination);



/**
 * @brief Affiche un annuaire
 * @author Aurelien Delormas
 * @param annuaire L'annuaire à afficher
 * @param nbContacts le nombre de contacts dans l'annuaire
 */
void afficherAnnuaire(annuaire annuaire, size_t nbContacts);

/**
 * @brief Affiche la description en haut de l'affichage de l'annuaire
 * 
 */
void afficherIndicationAnnuaire();

/**
 * @brief Affiche un contact
 * @author Aurelien Delormas
 * @param contact Le contact a afficher
 * @param nba Le rang du contact a afficher
 */
void afficherContact(contact contact ,int nba);

/**
 * @brief Verifie si une chaine est présent dans une liste de chaines
 * @author Guillaume FOURCROY
 * 
 * @param listeChaines Liste de chaine à comparer
 * @param tailleChaine La taille de la liste a verifier
 * @param chaine La chaine a verifier
 * @return int TRUE si presente FALSE sinon
 */
int existeChaine(char** listeChaines, size_t tailleChaine, char* chaine);

/**
 * @brief Verifie si le contact existe et le renvoie
 * @author Guillaume FOURCROY
 * 
 * @param annuaire L'annuaire a verifier
 * @param tailleAnnuaire La taille de l'annuaire a verifier
 * @param mail Le mail du contact
 * @return contact 
 */
contact getContact(annuaire annuaire, size_t tailleAnnuaire, char* mail);


/**
 * @brief Transforme un tableau de chaine en chaine de caractères
 * @author Guillaume FOURCROY
 * 
 * @param liste La liste a transformer
 * @param tailleListe La taille de la liste
 * @param tailleChaine La taille de chaine
 * @return char* La chaine de caractère
 */
char* listeToChaine(char** liste, size_t tailleListe, size_t tailleChaine);

/**
 * @brief Transforme une chaine de caractères en tableau de chaine de caractères
 * @author Guillaume FOURCROY
 * 
 * @param chaine La chaine a transformer
 * @param tailleListe La taille de la liste
 * @param tailleChaine La taille de chaine
 * @return char** Le tableau de chaine de caractères
 */
char** chaineToListe(char*chaine, size_t* tailleListe, size_t tailleChaine);

/**
 * @brief Verifie si il y a une occurence de chaine dans le contact
 * 
 * @param chaine La chaine de verification
 * @param contact Le contact a verifier
 * @return int Renvoie TRUE si il y a une occurence de chaine dans contact, FALSE sinon
 */
int occurenceContact(char* chaine, contact contact);

/**
 * @brief Transforme tous les caractères majuscule d'une chaine en minuscule
 * 
 * @param chaine La chaine à transformer
 */
void toLowerCase(char* chaine);

/**
 * @brief Affiche des chaines avec des espaces
 * 
 * @param chaine la chaine a afficher
 * @param taille la taille max de la chaine
 */
void afficherChaineEspaces(char* chaine, int taille);

/**
 * @brief Verifie l'occurence de chaines de caractères sans tenir compte des minuscules/majuscules
 * 
 * @param chaine1 chaine a verifier
 * @param chaine2 occurence a trouver
 * @return TRUE si elle existe FALSE sinon
 */
int verifierOccurence(char*chaine1, char*chaine2);

#endif
