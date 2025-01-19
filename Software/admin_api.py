from fastapi import FastAPI, HTTPException, Depends
from pydantic import BaseModel
from typing import Optional
from datetime import datetime
import sqlite3

# Métadonnées pour la documentation OpenAPI
tags_metadata = [
    {
        "name": "badges",
        "description": "Opérations sur les badges"
    },
    {
        "name": "logs",
        "description": "Opérations sur les logs"
    }
]

# Création de l'application FastAPI avec des informations de base et des tags pour la documentation
app = FastAPI(
    title="Gestion Emprunteurs - API Admin",
    description="API pour la gestion des emprunteurs et des badges",
    version="0.1.0",
    openapi_tags=tags_metadata
)

# Connexion à la base de données
DATABASE = "logs_and_badges.db"

def get_db_connection():
    conn = sqlite3.connect(DATABASE)
    conn.row_factory = sqlite3.Row
    return conn

# Modèles
class Badge(BaseModel):
    id_badge: str
    actif: Optional[bool] = True

class Log(BaseModel):
    id_machine: str
    operation: str
    data: Optional[str] = None
    timestamp: Optional[datetime] = None

# Initialiser la base de données
def init_db():
    conn = get_db_connection()
    cursor = conn.cursor()

    # Créer la table logs
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            id_machine TEXT NOT NULL,
            operation TEXT CHECK(operation IN ('E', 'R', 'A', 'D')),
            data TEXT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    ''')

    # Créer la table badges
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS badges (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            id_badge TEXT UNIQUE NOT NULL,
            actif BOOLEAN DEFAULT 1
        )
    ''')

    conn.commit()
    conn.close()

# Initialiser la base de données
init_db()

@app.get("/")
def read_root():
    return {"message": "Bienvenue sur l'API de gestion des emprunteurs", "status": "OK"}

# Routes
@app.get("/badges", tags=["badges"])
def get_badges():
    """Récupérer tous les badges."""
    conn = get_db_connection()
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM badges")
    badges = cursor.fetchall()
    conn.close()

    return [dict(badge) for badge in badges]
@app.get("/badges/{id_badge}", tags=["badges"])
def check_badge(id_badge: str):
    """Vérifier si un badge existe et retourner son statut."""
    conn = get_db_connection()
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM badges WHERE id_badge = ?", (id_badge,))
    badge = cursor.fetchone()
    conn.close()

    if badge is None:
        raise HTTPException(status_code=404, detail="Badge non trouvé")

    return {"id_badge": badge["id_badge"], "actif": badge["actif"]}

@app.put("/badges/", tags=["badges"])
def update_badge(badge: Badge):
    """Mettre à jour le statut d'un badge existant."""
    id_badge = badge.id_badge
    conn = get_db_connection()
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM badges WHERE id_badge = ?", (id_badge,))
    existing_badge = cursor.fetchone()

    if existing_badge is None:
        conn.close()
        raise HTTPException(status_code=404, detail="Badge non trouvé")

    cursor.execute(
        "UPDATE badges SET actif = ? WHERE id_badge = ?",
        (badge.actif, id_badge)
    )
    conn.commit()
    conn.close()
    return {"message": "Badge mis à jour avec succès", "id_badge": id_badge, "actif": badge.actif}

@app.post("/badges", tags=["badges"])
def add_badge(badge: Badge):
    """Ajouter un nouveau badge."""
    conn = get_db_connection()
    cursor = conn.cursor()
    try:
        cursor.execute(
            "INSERT INTO badges (id_badge, actif) VALUES (?, ?)",
            (badge.id_badge, badge.actif)
        )
        conn.commit()
    except sqlite3.IntegrityError:
        conn.close()
        raise HTTPException(status_code=400, detail="Le badge existe déjà")

    conn.close()
    return {"message": "Badge ajouté avec succès", "id_badge": badge.id_badge}

@app.post("/logs", tags=["logs"])
def add_log(log: Log):
    """Ajouter un nouveau log."""
    conn = get_db_connection()
    cursor = conn.cursor()
    cursor.execute(
        "INSERT INTO logs (id_machine, operation, data, timestamp) VALUES (?, ?, ?, ?)",
        (log.id_machine, log.operation, log.data, log.timestamp or datetime.utcnow())
    )
    conn.commit()
    conn.close()
    return {"message": "Log ajouté avec succès"}

@app.get("/logs", tags=["logs"])
def get_logs():
    """Récupérer tous les logs."""
    conn = get_db_connection()
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM logs")
    logs = cursor.fetchall()
    conn.close()

    return [dict(log) for log in logs]
