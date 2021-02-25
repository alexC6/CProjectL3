#ifndef __REQUETES_CLIENT_H__
#define __REQUETES_CLIENT_H__
#include "TCP_serveur.h"
#include "../global/utilitaire.h"
#include "../global/structures.h"
#include "fichier.h"

/**
 * @brief Recupère une requete (et les éventuelles données) du client en utilisant TCP
 * @note la requête doit être allouée
 * @author Guillaume Fourcroy
 * 
 * @param requete requete à recupérer
 * @return int OK NO_MESSAGES BAD_REQUEST
 */
int recupererRequete(requete requete);

/**
 * @brief Envoie une requete au client avec les eventuelles données au client
 * @author Guillaume FOURCROY
 * 
 * @param requete requete à envoyer qui doit être initilisé et remplie en utilisant initRequete et ajouterArgument
 * @return int OK BAD_REQUEST BAD_SEND BAD_SEND_DATA
 */
int envoyerRequete(requete requete);

/**
 * @brief Traite une requete d'authentification, le compte est celui qui sera connecté
 * @author Guillaume FOURCROY
 * 
 * @param req La requete dont on souhaite traiter : doit être verifié au niveau du nom et du type
 * @param compte Compte de l'utilisateur connecté
 * @return int OK BAD_REQUEST BAD_ARGUMENT ou un erreur de envoyerRequete
 */
int traiterAutentification(requete req, compte *compte);

/**
 * @brief Traite une requete de deconnexion
 * 
 * @param req La requete dont on souhaite traiter : doit être verifié au niveau du nom et du type
 * @param compte Le compte de l'utilisateur connecté que l'on va deconnecter
 * @return int Retourne OK ou erreur de envoyerRequete
 */
int traiterDeconnexion(requete req, compte compte);

/**
 * @brief Traite la requete de consultation d'annuaire
 * @author Guillaume FOURCROY
 * 
 * @param req La requete dont on souhaite traiter : doit être verifié au niveau du nom et du type
 * @param compte Le compte de l'utilisateur connecté
 * @return int Retourne BAD_REQUEST BAD_ARGUMENT
 */
int traiterConsultationAnnuaire(requete req, compte compte);

/**
 * @brief Traite la requete de la consultation de listes d'annuaires
 * 
 * @param req La requete dont on souhaite traiter : doit être verifié au niveau du nom et du type
 * @param compte Le compte de l'utilisateur connecté
 * @return int erreur
 */
int traiterListeAnnuaires(requete req, compte compte);

/**
 * @brief Traite la requête des accès de l'annuaire de l'utilisateur
 * 
 * @param req La requete dont on souhaite traiter : doit être verifié au niveau du nom et du type
 * @param compte Le compte de l'utilisateur connecté
 * @return int erreur
 */
int traiterAccesAnnuaire(requete req, compte compte);

/**
 * @brief Traite  la requete d'ajout ou de modification d'un contact de l'utilisateur
 * 
 * @param req La requete dont on souhaite traiter : doit être verifié au niveau du nom et du type
 * @param compte Le compte de l'utilisateur connecté
 * @return int erreur
 */
int traiterAjouterContact(requete req, compte compte);

/**
 * @brief Traite la requete de supression d'un contact de l'utilisateur
 * 
 * @param req La requete dont on souhaite traiter : doit être verifié au niveau du nom et du type
 * @param compte Le compte de l'utilisateur connecté
 * @return int erreur
 */
int traiterSupprimerContact(requete req, compte compte);

/**
 * @brief Traite la requete d'ajout de partage d'annuaire
 * 
 * @param req La requete dont on souhaite traiter : doit être verifié au niveau du nom et du type
 * @param compte Le compte de l'utilisateur connecté
 * @return int erreur
 */
int traiterAjouterPartage(requete req, compte compte);

/**
 * @brief Traite la requete de supression de partage d'annuaire
 * 
 * @param req La requete dont on souhaite traiter : doit être verifié au niveau du nom et du type
 * @param compte Le compte de l'utilisateur connecté
 * @return int erreur
 */
int traiterSupprimerPartage(requete req, compte compte);

#endif