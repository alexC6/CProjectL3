#ifndef __SERVEUR_H__
#define __SERVEUR_H__

#include "serveur/requetes_serveur.h"


/**
 * @brief Main serveur
 * 
 * @param argc Nombre de paramètres du programme
 * @param argv Liste de paramètres du programme
 * @return int 0 si tout s'est bien passé 1 sinon
 */
int main(int argc, char *argv[]);

/**
 * @brief Fonction de gestion du signal SIGINT pour permettre l'interruption du serveur
 * @author Alexandre Coulais
 * 
 * @param num   Numero signal a gerer
 */
void signalHandler(int num);

/**
 * @brief Initialise le serveur en fonction des paramètre du programme
 * @author Guillaume FOURCROY
 * 
 * @param argc Nombre de paramètres du programme
 * @param argv Liste de paramètres du programme
 * @return int OK BAD_INITIALISATION
 */
int initialisationServeur(int argc, char *argv[]);

/**
 * @brief Lance le serveur pour être prêt à réponde à des clients
 * 
 * @return int erreur
 */
int lancementServeur();

/**
 * @brief Choisie l'action a faire en fonction de la requete recue
 * 
 * @param req requête à traiter
 * @param compte compte à traiter
 * @return int erreur
 */
int traiterRequete(requete req, compte compte);

/**
 * @brief Entre dans l'interface d'adninistration
 * 
 * @return erreur
 */
int administration();

/**
 * @brief Affiche et récupère les choix de l'utilisateur
 * 
 */
char menuAdministration();

/**
 * @brief Execute le choix du menu d'administration
 * 
 * @param choix Le choix a executer
 * @return int erreur
 */
int executerChoixMenuAdministration(char choix);

/**
 * @brief ajoute un compte utilisateur
 * 
 * @return int erreur
 */
int ajouterCompte();

/**
 * @brief modifie un compte utilisateur
 * 
 * @return int erreur
 */
int modifierCompte();

/**
 * @brief supprime un compte utilisateur
 * 
 * @return int erreur
 */
int supprimerCompte();

#endif