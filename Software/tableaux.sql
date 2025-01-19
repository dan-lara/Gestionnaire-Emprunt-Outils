CREATE TABLE logs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    id_machine TEXT NOT NULL,
    operation TEXT CHECK (operation IN ('E', 'R', 'A', 'D')),
    data TEXT,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE badges (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    id_badge TEXT UNIQUE NOT NULL,
    actif BOOLEAN DEFAULT 1
);
