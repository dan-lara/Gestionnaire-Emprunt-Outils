# Gestionnaire-Emprunt-Outils

Ce dépôt contient toutes les ressources de développement pour le projet de Système de Prêt d'Outils. L'objectif est de créer un module complet pour gérer le prêt et le retour de petits outils dans des salles en libre-service, en utilisant la technologie RFID et des mécanismes de verrouillage à servomoteur.

# Architecture du Projet

```
└── 📁Docs
└── 📁Firmware
    └── main.c
    └── adxl345.h
    └── general.h
    └── logs.h
    └── manchester.h
    └── servo.h
    └── spi.h
    └── timer.h
    └── uart.h
└── 📁Hardware
    └── 📁Model3D
    └── 📁SystemRFID
└── 📁Software
    └── admin_api.py
    └── admin_st.py
    └── logs_and_badges.db
    └── tableaux.sql
```
Le projet a été développé sur la base de l'architecture décrite ci-dessus, chaque module étant spécifique à une tâche.

## Docs

Documents généraux tels que le rapport et les liens des démonstrations

## Firmware
Projet Keil avec les bibliothèques développées pour mettre en œuvre le projet. Chaque bibliothèque a sa propre fonction et peut être utilisée pour créer de nouveaux composants pour la machine afin d'augmenter sa fonctionnalité.

## Matériel

Contient le fichier stl pour le support du servomoteur et les fichiers de simulation pour la carte.

## Logiciel

Ce module contient toutes les solutions permettant la communication avec l'internet, les bases de données et la gestion par les administrateurs.


## Comment Contribuer

Pour ajouter des informations à ce dépôt, suivez ces étapes :

<!-- 1. **Forker le Dépôt** : Cliquez sur le bouton "Fork" en haut à droite de ce dépôt pour créer une copie du dépôt sur votre compte GitHub. -->

<!-- 2. **Cloner le Dépôt Forké** : Clonez le dépôt forké sur votre machine locale en utilisant la commande suivante : -->
2. **Cloner le Dépôt** : Clonez le dépôt sur votre machine locale en utilisant la commande suivante :
    ```sh
    git clone https://github.com/dan-lara/Gestionnaire-Emprunt-Outils.git
    cd Gestionnaire-Emprunt-Outils
    ```

3. **Créer une Nouvelle Branche** : Créez une nouvelle branche pour vos modifications :
    ```sh
    git checkout -b votre-nom-de-branche
    ```

4. **Apporter Vos Modifications** : Ajoutez les informations ou apportez les modifications que vous souhaitez contribuer.

5. **Valider Vos Modifications** : Validez vos modifications avec un message de validation descriptif :
    ```sh
    git add .
    git commit -m "Description des modifications"
    ```

6. **Pousser Vos Modifications** : Poussez vos modifications vers votre dépôt forké :
    ```sh
    git push origin votre-nom-de-branche
    ```

7. **Créer une Pull Request** : Allez sur le dépôt original sur GitHub et cliquez sur le bouton "New Pull Request". Sélectionnez votre branche et soumettez la pull request pour révision.

## Contact

Pour toute question ou problème, veuillez ouvrir une issue sur le dépôt ou contacter les mainteneurs du projet.