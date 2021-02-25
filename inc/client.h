#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "client/requetes_client.h"

/**
 * @brief Main client
 * 
 * @param argc Nombre de paramètres du programme
 * @param argv Liste de paramètres du programme
 * @return int 0 si tout s'est bien passé 1 sinon
 */
int main(int argc, char *argv[]);

/**
 * @brief Fonction de gestion du signal SIGINT pour permettre l'interruption du client
 * @author Guillaume FOURCROY
 * 
 * @param num   Numero signal a gerer
 */
void signalHandler(int num);

/**
 * @brief Connecte le client au serveur en fonction des paramètre du programme
 * @author Guillaume FOURCROY
 * 
 * @param argc Nombre de paramètres du programme
 * @param argv Liste de paramètres du programme
 * @return int OK BAD_INITIALISATION
 */
int connexionServeur(int argc, char *argv[]);

/**
 * @brief Menu pour se connecter
 * @author Guillaume FOURCROY
 * 
 * @param mail Le mail de l'utilisateur connecté
 * @return int OK si connecté 1 sinon
 */
int menuConnexion(char* mail);

/**
 * @brief Menu principal de l'utilisateur
 * @author Guillaume FOURCROY
 * 
 * @param mail Le mail de l'utilisateur
 * @return int erreur
 * 
 */
int menuPrincipal(char* mail);

/**
 * @brief Menu de l'annuaire personnel de l'utilisateur
 * @author Guillaume FOURCROY
 * 
 * @param mail Le mail de l'utilisateur
 * @return int erreur
 */
int menuPerso(char* mail);

/**
 * @brief Menu de l'annuaire des annuaires partagés avec l'utilisateur
 * @author Guillaume FOURCROY
 * 
 * @param mail Le mail de l'utilisateur
 * @return int erreur
 */
int menuPartage(char* mail);

/**
 * @brief Menu pour consulter les autres annuaires
 * @author Guillaume FOURCROY
 * 
 * @param mail Le mail de l'utilisateur
 * @return int erreur
 */
int menuAutreAnnuaire(char* mail);

/**
 * @brief Menu d'ajout de contact
 * @author Guillaume FOURCROY
 * 
 * @param annuaire L'annuaire à modifier
 * @param tailleAnnuaire La taille de l'annuaire à modifier
 * @return int erreur
 */
int ajouterContact(annuaire annuaire, size_t tailleAnnuaire);

/**
 * @brief Menu de modification d'un contact
 * @author Guillaume FOURCROY
 * 
 * @param annuaire L'annuaire à modifier
 * @param tailleAnnuaire La taille de l'annuaire à modifier
 * @return int erreur
 */
int modifierContact(annuaire annuaire, size_t tailleAnnuaire);

/**
 * @brief Menu de suppression de contact
 * @author Guillaume FOURCROY
 * 
 * @param annuaire L'annuaire à modifier
 * @param tailleAnnuaire La taille de l'annuaire à modifier
 * @return int erreur
 */
int supprimerContact(annuaire annuaire, size_t tailleAnnuaire);

/**
 * @brief Menu d'ajout de partage
 * 
 * @param accesPartage La liste à modifier
 * @param tailleChaine La taille de la liste à modifier
 * @return int erreur
 */
int ajouterPartage(char** accesAnnuaire, size_t tailleAcces);

/**
 * @brief Menu de supression de partage
 * 
 * @param accesPartage La liste à modifier
 * @param tailleChaine La taille de la liste à modifier
 * @return int erreur
 */
int supprimerPartage(char** accesAnnuaire, size_t tailleAcces);

/**
 * @brief Menu pour selctionner un autre annuaire
 * 
 * @param accesPartage La liste d'annuaires disponibles
 * @param tailleChaine La taille de la liste d'annuaires disponible
 * @return int erreur
 */
annuaire selectionAutreAnnuaire(char** listePartage,size_t taillePartage , size_t* tailleAnnuaire);

/**
 * @brief Fonction qui recherche dans tous les annuaires de l'utilisateur
 * 
 * @param mail Le mail de l'utilisateur
 * @return int La valeur d'erreur
 */
int rechercheGlobale(char* mail);

/**
 * @brief  Fonction qui permet d'afficher les contacts incluant la chaine recherchée 
 * 
 * @param annuaire L'annuaire concerné par la recherche
 * @param tailleAnnuaire La taille de l'annuaire 
 * @param chaine La chaine recherché dans l'annuaire
 * @return int retourne FALSE si le parcours de l'ensemble des contacts  n'a pas produit de résultat concernant la recherhe 
 * et retourne TRUE sinon 
 */

int rechercheAnnuaire(annuaire annuaire,size_t tailleAnnuaire, char *chaine, int* nbTrouvee);


#endif