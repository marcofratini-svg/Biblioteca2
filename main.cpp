#include <iostream>
#include <string>
#include <sqlite3.h>

using namespace std;

// Inizializzazione del database e popolamento dati di test
void inizializzaDatabase(sqlite3* db) {
    const char* sqlSchema = 
        "CREATE TABLE IF NOT EXISTS Autore ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  nome TEXT NOT NULL,"
        "  cognome TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS UtenteBiblioteca ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  nome TEXT NOT NULL,"
        "  cognome TEXT NOT NULL,"
        "  email TEXT"
        ");"
        "CREATE TABLE IF NOT EXISTS Libri ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  titolo TEXT NOT NULL,"
        "  autore_id INTEGER,"
        "  disponibile INTEGER DEFAULT 1,"
        "  utente_id INTEGER,"
        "  data_restituzione TEXT,"
        "  FOREIGN KEY(autore_id) REFERENCES Autore(id),"
        "  FOREIGN KEY(utente_id) REFERENCES UtenteBiblioteca(id)"
        ");";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sqlSchema, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "Errore creazione tabelle: " << errMsg << endl;
        sqlite3_free(errMsg);
        return;
    }

    // Popola i dati solo se la tabella Autore è vuota
    const char* checkQuery = "SELECT COUNT(*) FROM Autore;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, checkQuery, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt, 0) == 0) {
            const char* seedData = 
                // 4 Autori
                "INSERT INTO Autore (id, nome, cognome) VALUES (1, 'Umberto', 'Eco');"
                "INSERT INTO Autore (id, nome, cognome) VALUES (2, 'Elena', 'Ferrante');"
                "INSERT INTO Autore (id, nome, cognome) VALUES (3, 'Paolo', 'Giordano');"
                "INSERT INTO Autore (id, nome, cognome) VALUES (4, 'Alessandro', 'Baricco');"
                
                // 10 Utenti della biblioteca
                "INSERT INTO UtenteBiblioteca (nome, cognome, email) VALUES ('Mario', 'Rossi', 'mario.rossi@email.it');"
                "INSERT INTO UtenteBiblioteca (nome, cognome, email) VALUES ('Luca', 'Bianchi', 'luca.bianchi@email.it');"
                "INSERT INTO UtenteBiblioteca (nome, cognome, email) VALUES ('Giulia', 'Verdi', 'giulia.verdi@email.it');"
                "INSERT INTO UtenteBiblioteca (nome, cognome, email) VALUES ('Francesca', 'Neri', 'francesca.neri@email.it');"
                "INSERT INTO UtenteBiblioteca (nome, cognome, email) VALUES ('Marco', 'Gialli', 'marco.gialli@email.it');"
                "INSERT INTO UtenteBiblioteca (nome, cognome, email) VALUES ('Sara', 'Blu', 'sara.blu@email.it');"
                "INSERT INTO UtenteBiblioteca (nome, cognome, email) VALUES ('Andrea', 'Conti', 'andrea.conti@email.it');"
                "INSERT INTO UtenteBiblioteca (nome, cognome, email) VALUES ('Chiara', 'Romano', 'chiara.romano@email.it');"
                "INSERT INTO UtenteBiblioteca (nome, cognome, email) VALUES ('Matteo', 'Moretti', 'matteo.moretti@email.it');"
                "INSERT INTO UtenteBiblioteca (nome, cognome, email) VALUES ('Elena', 'Ricci', 'elena.ricci@email.it');"

                // 20 Libri famosi in Italia (5 per autore)
                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('Il nome della rosa', 1, 1);"
                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('Il pendolo di Foucault', 1, 1);"
                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('L''isola del giorno prima', 1, 1);"
                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('Baudolino', 1, 1);"
                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('La misteriosa fiamma della regina Loana', 1, 1);"

                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('L''amica geniale', 2, 1);"
                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('Storia del nuovo cognome', 2, 1);"
                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('Storia di chi fugge e di chi resta', 2, 1);"
                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('I giorni dell''abbandono', 2, 1);"
                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('La figlia oscura', 2, 1);"

                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('La solitudine dei numeri primi', 3, 1);"
                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('Il corpo umano', 3, 1);"
                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('Il nero e l''argento', 3, 1);"
                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('Divorare il cielo', 3, 1);"
                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('Tasman', 3, 1);"

                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('Oceano mare', 4, 1);"
                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('Seta', 4, 1);"
                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('Novecento', 4, 1);"
                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('Castelli di rabbia', 4, 1);"
                "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES ('Questa storia', 4, 1);";

            char* seedErr = nullptr;
            if (sqlite3_exec(db, seedData, nullptr, nullptr, &seedErr) != SQLITE_OK) {
                cerr << "Errore inserimento dati: " << seedErr << endl;
                sqlite3_free(seedErr);
            } else {
                cout << "[Sistema] Database 'biblioteca.db' inizializzato con 4 autori, 10 utenti e 20 libri.\n";
            }
        }
        sqlite3_finalize(stmt);
    }
}

// a1) Seleziona uno degli autori
void selezionaAutore(sqlite3* db, int &selected_id) {
    cout << "\n--- ELENCO AUTORI ---" << endl;
    const char* sql = "SELECT id, nome, cognome FROM Autore;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "ID: " << sqlite3_column_int(stmt, 0) 
                 << " | " << sqlite3_column_text(stmt, 1) 
                 << " " << sqlite3_column_text(stmt, 2) << endl;
        }
        sqlite3_finalize(stmt);
    }
    cout << "Inserisci l'ID dell'autore da selezionare: ";
    cin >> selected_id;
    cout << "Autore ID " << selected_id << " selezionato.\n";
}

// a2) Visualizza libri disponibili dell'autore selezionato
void mostraLibriDisponibiliAutore(sqlite3* db, int selected_id) {
    if (selected_id == -1) {
        cout << "\n[Attenzione] Nessun autore selezionato. Esegui prima l'opzione 'a1'.\n";
        return;
    }
    cout << "\n--- LIBRI DISPONIBILI PER L'AUTORE SELEZIONATO ---" << endl;
    string sql = "SELECT id, titolo FROM Libri WHERE autore_id = ? AND disponibile = 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, selected_id);
        bool found = false;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            found = true;
            cout << "[ID Libro: " << sqlite3_column_int(stmt, 0) << "] " << sqlite3_column_text(stmt, 1) << endl;
        }
        if (!found) cout << "Nessun libro disponibile per questo autore.\n";
        sqlite3_finalize(stmt);
    }
}

// b) Visualizza libri in prestito e date di restituzione
void mostraLibriInPrestito(sqlite3* db) {
    cout << "\n--- LIBRI ATTUALMENTE IN PRESTITO ---" << endl;
    const char* sql = 
        "SELECT L.id, L.titolo, U.nome, U.cognome, L.data_restituzione "
        "FROM Libri L JOIN UtenteBiblioteca U ON L.utente_id = U.id "
        "WHERE L.disponibile = 0;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        bool found = false;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            found = true;
            cout << "ID Libro: " << sqlite3_column_int(stmt, 0)
                 << " | Titolo: " << sqlite3_column_text(stmt, 1)
                 << " | Utente: " << sqlite3_column_text(stmt, 2) << " " << sqlite3_column_text(stmt, 3)
                 << " | Restituzione: " << sqlite3_column_text(stmt, 4) << endl;
        }
        if (!found) cout << "Nessun libro attualmente in prestito.\n";
        sqlite3_finalize(stmt);
    }
}

// c) Inserisci un nuovo autore
void inserisciAutore(sqlite3* db) {
    string nome, cognome;
    cout << "Nome nuovo autore: ";
    cin >> ws;
    getline(cin, nome);
    cout << "Cognome nuovo autore: ";
    getline(cin, cognome);

    string sql = "INSERT INTO Autore (nome, cognome) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, nome.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, cognome.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_DONE) cout << "Autore inserito con successo!\n";
        sqlite3_finalize(stmt);
    }
}

// d) Inserisci un nuovo libro associato a un autore
void inserisciLibro(sqlite3* db) {
    string titolo;
    int autore_id;
    cout << "Titolo nuovo libro: ";
    cin >> ws;
    getline(cin, titolo);
    cout << "ID dell'autore associato: ";
    cin >> autore_id;

    string sql = "INSERT INTO Libri (titolo, autore_id, disponibile) VALUES (?, ?, 1);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, titolo.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, autore_id);
        if (sqlite3_step(stmt) == SQLITE_DONE) cout << "Libro inserito con successo!\n";
        sqlite3_finalize(stmt);
    }
}

// e) Modifica un libro
void modificaLibro(sqlite3* db) {
    int id;
    string nuovoTitolo;
    cout << "ID del libro da modificare: ";
    cin >> id;
    cout << "Nuovo titolo del libro: ";
    cin >> ws;
    getline(cin, nuovoTitolo);

    string sql = "UPDATE Libri SET titolo = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, nuovoTitolo.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, id);
        if (sqlite3_step(stmt) == SQLITE_DONE) cout << "Libro aggiornato!\n";
        sqlite3_finalize(stmt);
    }
}

// f) Modifica un autore
void modificaAutore(sqlite3* db) {
    int id;
    string nuovoNome, nuovoCognome;
    cout << "ID dell'autore da modificare: ";
    cin >> id;
    cout << "Nuovo nome: ";
    cin >> ws;
    getline(cin, nuovoNome);
    cout << "Nuovo cognome: ";
    getline(cin, nuovoCognome);

    string sql = "UPDATE Autore SET nome = ?, cognome = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, nuovoNome.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, nuovoCognome.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, id);
        if (sqlite3_step(stmt) == SQLITE_DONE) cout << "Autore aggiornato!\n";
        sqlite3_finalize(stmt);
    }
}

// g) Dare un libro in prestito
void daiInPrestito(sqlite3* db) {
    int utente_id, libro_id;
    string dataRestituzione;

    cout << "\n--- UTENTI BIBLIOTECA ---" << endl;
    const char* sqlU = "SELECT id, nome, cognome FROM UtenteBiblioteca;";
    sqlite3_stmt* stmtU;
    if (sqlite3_prepare_v2(db, sqlU, -1, &stmtU, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmtU) == SQLITE_ROW) {
            cout << "ID Utente: " << sqlite3_column_int(stmtU, 0) 
                 << " | " << sqlite3_column_text(stmtU, 1) 
                 << " " << sqlite3_column_text(stmtU, 2) << endl;
        }
        sqlite3_finalize(stmtU);
    }

    cout << "Inserisci l'ID dell'utente: ";
    cin >> utente_id;

    cout << "\n--- LIBRI DISPONIBILI ---" << endl;
    const char* sqlL = "SELECT id, titolo FROM Libri WHERE disponibile = 1;";
    sqlite3_stmt* stmtL;
    if (sqlite3_prepare_v2(db, sqlL, -1, &stmtL, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmtL) == SQLITE_ROW) {
            cout << "ID Libro: " << sqlite3_column_int(stmtL, 0) 
                 << " | " << sqlite3_column_text(stmtL, 1) << endl;
        }
        sqlite3_finalize(stmtL);
    }

    cout << "Inserisci l'ID del libro da prestare: ";
    cin >> libro_id;

    cout << "Data prevista restituzione (es. 2026-12-15): ";
    cin >> ws;
    getline(cin, dataRestituzione);

    string sqlUpdate = "UPDATE Libri SET disponibile = 0, utente_id = ?, data_restituzione = ? WHERE id = ?;";
    sqlite3_stmt* stmtUp;
    if (sqlite3_prepare_v2(db, sqlUpdate.c_str(), -1, &stmtUp, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmtUp, 1, utente_id);
        sqlite3_bind_text(stmtUp, 2, dataRestituzione.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmtUp, 3, libro_id);
        if (sqlite3_step(stmtUp) == SQLITE_DONE) cout << "Prestito registrato!\n";
        sqlite3_finalize(stmtUp);
    }
}

int main() {
    sqlite3* db;
    if (sqlite3_open("biblioteca.db", &db)) {
        cerr << "Errore apertura database: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

    inizializzaDatabase(db);

    string scelta;
    int selected_autore_id = -1;

    while (true) {
        cout << "\n==========================================" << endl;
        cout << "        GESTIONE BIBLIOTECA - MENU" << endl;
        cout << "==========================================" << endl;
        cout << "a1) Seleziona uno degli autori" << endl;
        cout << "a2) Visualizza libri disponibili dell'autore" << endl;
        cout << "b)  Visualizza libri in prestito e scadenze" << endl;
        cout << "c)  Inserisci un nuovo autore" << endl;
        cout << "d)  Inserisci un nuovo libro" << endl;
        cout << "e)  Modifica un libro" << endl;
        cout << "f)  Modifica un autore" << endl;
        cout << "g)  Dare un libro in prestito" << endl;
        cout << "0)  Esci" << endl;
        cout << "Scegli un'opzione: ";
        cin >> scelta;

        if (scelta == "a1") selezionaAutore(db, selected_autore_id);
        else if (scelta == "a2") mostraLibriDisponibiliAutore(db, selected_autore_id);
        else if (scelta == "b") mostraLibriInPrestito(db);
        else if (scelta == "c") inserisciAutore(db);
        else if (scelta == "d") inserisciLibro(db);
        else if (scelta == "e") modificaLibro(db);
        else if (scelta == "f") modificaAutore(db);
        else if (scelta == "g") daiInPrestito(db);
        else if (scelta == "0") break;
        else cout << "Opzione non valida.\n";
    }

    sqlite3_close(db);
    return 0;
}
