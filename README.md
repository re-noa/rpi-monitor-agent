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

---

## Tech Stack
* **Langage :** C++17 (POO, STL, Streams)
* **Build System :** CMake 3.10+
* **Bibliothèques :** 
    * `libcurl` : Transferts de données réseau.
    * `nlohmann/json` : Parsing et manipulation de données JSON.
* **Système :** Linux (Raspbian/Debian), Systemd.

---

## Architecture du Projet
L'architecture suit le principe de **Séparation des Préoccupations (SoC)** :
* `Collector` : Acquisition des métriques matérielles.
* `Notifier` : Logique d'expédition des alertes (abstraction de l'API Discord).
* `Core` : Gestionnaire de configuration et cycle de vie.

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
