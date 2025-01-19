import streamlit as st
import requests

# URL de base de FastAPI
API_BASE_URL = "http://localhost:8000"

# Identifiants administrateur
UTILISATEUR_ADMIN = "admin"
MOT_DE_PASSE_ADMIN = "password"

def connexion():
    """Fonction de connexion administrateur."""
    st.title("Connexion Administrateur")

    with st.form("formulaire_connexion"):
        nom_utilisateur = st.text_input("Nom d'utilisateur")
        mot_de_passe = st.text_input("Mot de passe", type="password")
        bouton_connexion = st.form_submit_button("Connexion")

        if bouton_connexion:
            if nom_utilisateur == UTILISATEUR_ADMIN and mot_de_passe == MOT_DE_PASSE_ADMIN:
                st.session_state["connecté"] = True
                st.success("Connexion réussie!")
            else:
                st.error("Identifiants invalides")

def afficher_journaux():
    """Afficher tous les journaux."""
    st.title("Journaux")

    réponse = requests.get(f"{API_BASE_URL}/logs")
    if réponse.status_code == 200:
        journaux = réponse.json()
        st.write("### Tableau des journaux")
        st.table(journaux)
    else:
        st.error("Échec de la récupération des journaux")

def gérer_badges():
    """Gérer les badges (voir et modifier)."""
    st.title("Gestion des Badges")

    # Récupérer les badges
    id_badge = st.text_input("Entrez l'ID du Badge à vérifier")
    if st.button("Vérifier le Badge"):
        réponse = requests.get(f"{API_BASE_URL}/badges/{id_badge}")
        if réponse.status_code == 200:
            badge = réponse.json()
            st.write(f"ID du Badge: {badge['id_badge']}")
            st.write(f"Actif: {'Oui' if badge['actif'] else 'Non'}")

            nouveau_statut = st.checkbox("Définir comme Actif", value=badge['actif'])
            if st.button("Mettre à jour le statut"):
                réponse = requests.put(f"{API_BASE_URL}/badges", json={"id_badge": id_badge, "actif": nouveau_statut})
                if réponse.status_code == 200:
                    st.success("Statut du badge mis à jour avec succès")
                else:
                    st.error("Échec de la mise à jour du statut du badge")
        else:
            st.error("Badge non trouvé")

    # Ajouter un nouveau badge
    st.write("### Ajouter un nouveau Badge")
    nouvel_id_badge = st.text_input("Nouvel ID de Badge")
    nouveau_badge_actif = st.checkbox("Actif", value=True)

    if st.button("Ajouter le Badge"):
        réponse = requests.post(f"{API_BASE_URL}/badges", json={"id_badge": nouvel_id_badge, "actif": nouveau_badge_actif})
        if réponse.status_code == 200:
            st.success("Badge ajouté avec succès")
        else:
            st.error("Échec de l'ajout du badge")

# Application principale
if "connecté" not in st.session_state:
    st.session_state["connecté"] = False

if not st.session_state["connecté"]:
    connexion()
else:
    st.sidebar.title("Navigation")
    page = st.sidebar.radio("Aller à", ["Journaux", "Gérer les Badges"])

    if page == "Journaux":
        afficher_journaux()
    elif page == "Gérer les Badges":
        gérer_badges()
