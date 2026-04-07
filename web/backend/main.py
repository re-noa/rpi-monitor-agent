from fastapi import FastAPI
import sqlite3
from fastapi.middleware.cors import CORSMiddleware
import os
import json

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)

DB_PATH = os.path.join(os.path.dirname(__file__), "../../metrics.db")

def get_db_connection():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn

@app.get("/api/metrics")
async def get_metrics():
    conn = get_db_connection()
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM metrics ORDER BY timestamp DESC LIMIT 60")
    rows = cursor.fetchall()
    conn.close()
    return [dict(row) for row in rows][::-1]

@app.get("/api/status")
async def get_status():
    conn = get_db_connection()
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM metrics ORDER BY timestamp DESC LIMIT 1")
    row = cursor.fetchone()
    conn.close()
    return dict(row) if row else {"error": "no data"}

@app.get("/api/config")
async def get_config():
    with open(os.path.join(os.path.dirname(__file__), "../../config.json"), "r") as f:
        return json.load(f)

@app.get("/api/sites")
async def get_sites_status():
    conn = get_db_connection()
    cursor = conn.cursor()
    cursor.execute("SELECT url, is_up FROM site_status")
    rows = cursor.fetchall()
    conn.close()
    return [dict(row) for row in rows]
