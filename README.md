# RPi-System-Guardian

**RPi-System-Guardian** est un agent de monitoring léger et modulaire écrit en **C++17** pour Raspberry Pi. Il surveille en temps réel les ressources critiques (CPU, RAM) et alerte l'administrateur via un **Bot Discord** privé en cas de dépassement de seuils configurables.

---

## Fonctionnalités
* **Surveillance CPU :** Lecture directe via l'interface `sysfs` du noyau Linux.
* **Gestion RAM :** Analyse dynamique de `/proc/meminfo` avec calcul d'utilisation en pourcentage.
* **Alertes Intelligentes :** 
    * Système d'**hystérésis** pour éviter le spam de notifications.
    * Persistance d'état (délai d'alerte) pour filtrer les pics de charge éphémères.
* **Communication Sécurisée :** Intégration de l'API REST Discord (v10) via `libcurl`.
* **Architecture Industrielle :** Gestion des secrets par template, configuration JSON et déploiement en tant que service `systemd`.
* **Monitoring d'Infrastructures Web :** Vérification périodique de la disponibilité de sites web via requêtes HTTP (`libcurl`).
* **Surveillance Espace Disque :** Analyse via `statvfs` pour prévenir la saturation du système de fichiers.
* **Architecture Découplée & Persistance :** Stockage continu des métriques dans une base **SQLite3** locale.
* **Dashboard Web Temps Réel :** Interface graphique asynchrone pour visualiser les constantes et l'état des services.

---

## Tech Stack
* **Agent Core :** C++17, CMake 3.10+, `libcurl`, `nlohmann/json`, `libsqlite3`
* **Base de données :** SQLite3
* **Backend Web :** Python 3, FastAPI, Uvicorn (Architecture REST)
* **Frontend :** HTML/JS, Tailwind CSS, Chart.js (Polling dynamique)
* **Déploiement :** Linux (Raspbian/Debian), Systemd, Nginx/Apache (Reverse Proxy)

---

## Architecture du Projet
L'architecture suit une logique de micro-services découplés autour d'une base de données commune :
* **Producer (C++) :** Daemon de bas niveau récoltant les métriques (Sysfs, Procfs, HTTP) et les persistant via SQLite.
* **Consumer API (Python) :** Serveur FastAPI asynchrone exposant les données au format JSON.
* **Dashboard (UI) :** Single Page Application interrogeant l'API pour un affichage dynamique (Graphiques lissés, LEDs d'état).
* **Notifier :** Module C++ gérant les alertes critiques via Webhooks Discord.

---

## Installation & Déploiement

### 1. Dépendances
```bash
sudo apt update && sudo apt install -y \
    cmake \
    build-essential \
    libcurl4-openssl-dev \
    nlohmann-json3-dev
```

### 2. Compilation
```bash
chmod +x build.sh
./build.sh clean
```

### 3. Configuration
1. Copier le template : 
```bash
cp config.json.template config.json
```
2. Renseigner vos identifiants (Discord Token, User ID) dans `config.json`.

---

## Sécurité
Les informations sensibles (Tokens, IDs) sont exclues du versionnage via `.gitignore`. Un fichier `config.json.template` est fourni pour faciliter le déploiement sans compromettre la sécurité des secrets.

---

## Auteur
**Noa** - Passionné par l'IoT et le développement système.
