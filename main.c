//
// Autore: Lorenzo Rocca
// Data: 15/03/2025
//
// TODOs:
// - Controllo esistenza password con ID durante la creazione
//

// Inclusione librerie
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <dos.h>
#include <conio.h>

// Definizioni tasti
#define ENTER               13
#define BACKSLASH           8

// Definizioni varie
#define ALPHABET_SIZE       76
#define SECURITY_CODE       25022008

#pragma region Globals

// Alfabeto usato nel cifrario (76 lettere, 0 ... 75)
//                      |0                        |26                       |52       |62
const char* alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!$%&@#*.,:;()[";

#pragma endregion

#pragma region Utils

/**
 * Muove il cursore a una determinata posizione sullo schermo
 *
 * @param x La coordinata X
 * @param y La riga
 */
void moveCursor(const int x, const int y) {
    printf("\033[%d;%dH", y, x);
}

/**
 * Questa funzione pulisce lo schermo
 */
void clearScreen() {
    system("cls");
    moveCursor(1, 1);
}

/**
 * Stampa un numero di spazi bianchi
 *
 * @param spaces Il numero di spazi da stampare
 */
void printWhiteSpaces(const int spaces) {
    for (int i = 0; i < spaces; ++i) {
        printf(" ");
    }
}

/**
 * Ottiene l'indice di una lettera nell'alfabeto completo
 *
 * @param letter La lettera da trovare
 * @return L'indice nell'alfabeto
 */
int getLetterIndex(const char letter) {
    // Minuscole
    if (letter >= 'a' && letter <= 'z') {
        return letter - 'a';
    }

    // Maiuscole
    if (letter >= 'A' && letter <= 'Z') {
        return letter - 'A' + 26;
    }

    // Numeri
    if (letter >= '0' && letter <= '9') {
        return letter - '0' + 52;
    }

    // Simboli
    for (int i = 62; i < strlen(alphabet); i++) {
        if (letter == alphabet[i]) {
            return i;
        }
    }

    // Non trovata
    return -1;
}

/**
 * Verifica se una lettera è presente nell'alfabeto
 *
 * @param letter La lettera da verificare
 * @return Se la lettera è valida
 */
bool isValidLetter(const char letter) {
    return getLetterIndex(letter) != -1;
}

/**
 * Questa funzione richiede una stringa in modo sicuro e permettendo l'inserimento solo di
 * alcuni caratteri specifici contenuti nell'alfabeto scritto nella parte iniziale del codice.
 *
 * @param buffer Il buffer nel quale caricare la stringa letta
 * @param maxSize La dimensione massima della stringa
 * @param minSize La dimensione minima
 * @param acceptRangeS Inizio del range dei caratteri consentiti
 * @param acceptRangeE Fine del range dei caratteri consentiti
 * @param startX La coordinata X dalla quale iniziare a scrivere la stringa (SIMBOLI)
 * @param startY La riga nella quale scrivere
 * @param censure Se censurare la stringa
 * @param acceptSpaces Se accettare gli spazi
 */
void requestSafeInput(char* buffer, const int maxSize, const int minSize, const int acceptRangeS, const int acceptRangeE, const int startX, const int startY, const int censure, const int acceptSpaces) {
    // Cursore della scrittura
    int cursor = 0;

    // Sposta il cursore
    moveCursor(startX, startY);

    // Finché non viene inserito invio
    while (1) {
        // Carattere in input
        const int input = _getch();

        // Se si tratta di un invio esce dal ciclo e va a capo
        if (input == ENTER) {
            // Deve rispettare la dimensione minima
            if (cursor < minSize) {
                continue;
            }

            // Vai a capo ed esci
            printf("\n");
            break;
        }

        // Altrimenti, se è un backslash
        if (input == BACKSLASH) {
            if (cursor == 0) continue; // Non ci sta nulla da eliminare
            cursor--;
            buffer[cursor] = '\0';

            moveCursor(startX + cursor, startY);
            printf(" ");
            moveCursor(startX + cursor, startY);

            continue;
        }

        // Altrimenti, è un carattere

        // Se la dimensione massima è stata raggiunta, continua
        if (cursor >= maxSize)
            continue;

        // Se non è spazio, controlla se è valido
        if (input != ' ') {
            // Se il carattere non sta nel range, non te lo fa mettere
            if (getLetterIndex(input) < acceptRangeS || getLetterIndex(input) > acceptRangeE) // NOLINT(*-narrowing-conversions)
                continue;
        }

        // Se è spazio e non è accettato, continua
        if (input == ' ' && !acceptSpaces)
            continue;

        // Salva il carattere
        buffer[cursor] = input; // NOLINT(*-narrowing-conversions)
        buffer[cursor + 1] = '\0';

        // Incrementa il cursore e stampa il carattere
        ++cursor;

        if (censure) {
            printf("•");
        }
        else {
            printf("%c", input);
        }
    }
}

/**
 * Questa funzione stampa un numero di caratteri in un buffer
 *
 * @param buffer Il buffer dove stampare i caratteri
 * @param character Il carattere da stampare
 * @param size La dimensione del buffer
 */
void printCharactersIntoBuffer(char* buffer, const char character, const int size) {
    // Loop riempimento
    for (int i = 0; i < size; ++i) {
        buffer[i] = character;
    }

    // Termina la stringa
    buffer[size] = '\0';
}

/**
 * Questa funzione pulisce una stringa
 *
 * @param buffer Il buffer da pulire
 * @param size La dimensione della stringa
 */
void clearString(char* buffer, const int size) {
    printCharactersIntoBuffer(buffer, ' ', size - 1);
}

/**
 * Questa funzione scrive una stringa in un buffer
 *
 * @param buffer Il buffer dove scrivere la stringa
 * @param string La stringa da scrivere
 * @param size La dimensione del buffer + 1
 */
void printStringInBuffer(char* buffer, const char* string, const int size) {
    // Riempe la stringa di spazi
    printCharactersIntoBuffer(buffer, ' ', size);

    // Scrive nella stringa i primi caratteri fino alla dimensione
    for (int i = 0; i < strlen(string) && i < size; ++i) {
        buffer[i] = string[i];
    }

    // Termina la stringa
    buffer[size] = '\0';
}

#pragma endregion

#pragma region Encryption

/**
 * Questa funzione cifra una stringa con il cifrario di Cesare esteso e salva in un buffer
 * il risultato della cifratura
 *
 * @param input La stringa da cifrare
 * @param output Il buffer dove salvare la stringa cifrata
 * @param key La chiave di cifratura
 */
void encryptString(const char* input, char* output, const int key) {
    // Cifra con il cifrario di cesare esteso
    for (int i = 0; i < strlen(input); ++i) {
        // Codifica e salva
        const int currentCharIndex = getLetterIndex(input[i]);
        const int newCharIndex = (currentCharIndex + key) % ALPHABET_SIZE;
        output[i] = alphabet[newCharIndex];
    }

    output[strlen(input)] = '\0';
}

/**
 * Questa funzione decifra una stringa con il cifrario di Cesare esteso e salva in un buffer
 * il risultato della decifrazione
 *
 * @param input La stringa da decifrare
 * @param output Il buffer dove salvare la stringa decifrata
 * @param key La chiave di cifratura
 */
void decryptString(const char* input, char* output, const int key) {
    // Cifra con il cifrario di cesare esteso
    for (int i = 0; i < strlen(input); ++i) {
        // Ottiene l'indice della lettera corrente
        const int currentCharIndex = getLetterIndex(input[i]);

        // Decifra
        int newCharIndex = (currentCharIndex - key) % ALPHABET_SIZE;
        if (newCharIndex < 0) newCharIndex += ALPHABET_SIZE;

        // Salva
        output[i] = alphabet[newCharIndex];
    }

    // Termina la stringa
    output[strlen(input)] = '\0';
}

#pragma endregion

#pragma region Files

/**
 * Questa funzione crea tutti i files necessari per il funzionamento del programma
 */
void createPreliminaryFiles() {
    // Crea la cartella /data/ se non esiste
    CreateDirectory("data", nullptr);

    // Crea il file delle password .csv se non esiste
    FILE* passwordFile = fopen("data/passwords.csv", "r");
    if (passwordFile == nullptr) {
        passwordFile = fopen("data/passwords.csv", "w");
        fprintf(passwordFile, "ID,Dettagli,Password\n");
        fclose(passwordFile);
    }
}

/**
 * Questa funzione controlla se esiste il file utilizzato per la verifica della chiave
 * di cifratura
 *
 * @return Se il file esiste
 */
int checkKeyFile() {
    // Controlla se il file dove è salvata la chiave di cifratura esiste
    const FILE* keyFile = fopen("data/key.txt", "r");

    if (keyFile == nullptr) {
        // Se non esiste, ritorna 0
        return 0;
    }

    // Se esiste, ritorna 1
    return 1;
}

/**
 * Questa funzione verifica se la chiave di cifratura è valida.
 *
 * @param key La chiave da verificare
 * @return Se la chiave inserita corrisponde a quella impostata
 */
int isEncryptionKeyValid(const int key) {
    // Carica il codice di verifica cifrato dal file key.txt
    FILE* keyFile = fopen("data/key.txt", "r");
    char securityCodeEncrypted[128];
    fscanf(keyFile, "%s", securityCodeEncrypted);

    // Decifra il codice di verifica
    char securityCodeDecrypted[128];
    decryptString(securityCodeEncrypted, securityCodeDecrypted, key);

    // Controlla se il codice di verifica è corretto
    return atoi(securityCodeDecrypted) == SECURITY_CODE; // NOLINT(*-err34-c)
}

/**
 * Questa funzione salva la traccia della chiave di cifratura nel file key.txt,
 * dove viene inserito il codice di sicurezza salvato con tale chiave. Il codice
 * di sicurezza è noto, il che rende possibile rintracciare la chiave. A fini
 * dimostrativi, non ci interessa questo problema.
 *
 * @param key La chiave da salvare
 */
void saveEncryptionKey(const int key) {
    // Cifra il codice di verifica
    char securityCodeDecrypted[128];
    sprintf(securityCodeDecrypted, "%d", SECURITY_CODE);
    char securityCodeEncrypted[128];
    encryptString(securityCodeDecrypted, securityCodeEncrypted, key);

    // Salva il codice di verifica cifrato nel file key.txt
    FILE* keyFile = fopen("data/key.txt", "w");
    fprintf(keyFile, "%s", securityCodeEncrypted);
    fclose(keyFile);
}

/**
 * Questa funzione salva una password nel file delle password
 * @param id ID univoco della password
 * @param details I dettagli della password
 * @param password La password
 * @param key La chiave di cifratura
 */
void saveNewPassword(const char* id, const char* details, const char* password, const int key) {
    // Cifra la password
    char passwordEncrypted[128];
    encryptString(password, passwordEncrypted, key);

    // Salva la password nel file
    FILE* passwordFile = fopen("data/passwords.csv", "a");
    fprintf(passwordFile, "\"%s\",\"%s\",\"%s\"\n", id, details, passwordEncrypted);
    fclose(passwordFile);
}

#pragma endregion

#pragma region Headers

/**
 * Questa funzione scrive la testata del programma
 */
void printPasswordPlusText() {
    printf(" ____                                     _\n");
    printf("|  _ \\ __ _ ___ _____      _____  _ __ __| |  _\n");
    printf("| |_) / _` / __/ __\\ \\ /\\ / / _ \\| '__/ _` |_| |_\n");
    printf("|  __/ (_| \\__ \\__ \\\\ V  V / (_) | | | (_| |_   _|\n");
    printf("|_|   \\__,_|___/___/ \\_/\\_/ \\___/|_|  \\__,_| |_|  \n\n");
}

/**
 * Questa funzione scrive le informazioni del menu delle azioni
 */
void printActionsMenu() {
    // Scrive la testata del programma
    printPasswordPlusText();

    // Informazioni
    printf("Scegli un'azione da eseguire:\n\n");
    printf("1. Nuova password\n");
    printf("2. Lista password\n");
    printf("Q. Esci\n\n");

    // Attesa pulsante
    printf("[PREMI UN TASTO]\n");
}

/**
 * Questa funzione scrive la testata della lista delle password
 */
void printPasswordListHeader() {
    // Scrive la testata del programma
    printPasswordPlusText();

    // Informazioni
    printf("Lista delle password:\n\n");

    // Testate
    char id[33];
    char details[65];
    char password[33];
    printStringInBuffer(id, "ID", 32);
    printStringInBuffer(details, "Dettagli", 64);
    printStringInBuffer(password, "Password", 32);

    // Stampa le testate
    printf("%s | %s | %s\n", id, details, password);

    // Scrivi 118 trattini
    for (int i = 0; i < 148; ++i) {
        printf("-");
    }

    // Nuova riga
    printf("\n");
}

#pragma endregion

#pragma region Menus

/**
 * Il menu per la richiesta della chiave di cifratura
 *
 * @param encryptionKey La chiave di cifratura in uscita
 */
void generalKeyRequestMenu(int* encryptionKey) {
    // Scrive la testata del programma
    printPasswordPlusText();

    // Definisce la chiave
    char keyString[17];

    // Se non esiste il file di controllo della chiave
    if (!checkKeyFile()) {
        // Chiede la nuova chiave
        printf("Inserisci la nuova chiave di cifratura: \n>> ");
        requestSafeInput(keyString, 16, 8, 52, 61, 4, 8, 1, 0);

        // Converte la stringa nel numero
        *encryptionKey = atoi(keyString); // NOLINT(*-err34-c)

        // Salva la chiave nel file
        saveEncryptionKey(*encryptionKey);

        // Pulisce la stringa
        clearString(keyString, 17);
    }
    else {
        // Richiesta
        printf("Inserisci la chiave di cifratura: \n>> ");
        requestSafeInput(keyString, 16, 8, 52, 61, 4, 8, 1, 0);

        // Converte la stringa nel numero
        *encryptionKey = atoi(keyString); // NOLINT(*-err34-c)

        // Pulisce la stringa
        clearString(keyString, 17);

        // Se non è valida riprova
        while (!isEncryptionKeyValid(*encryptionKey)) {
            // Torna endre
            moveCursor(1, 7);

            // Richiesta
            printf("Chiave errata! Riprova:                                \n>>                                                        ");
            requestSafeInput(keyString, 16, 8, 52, 61, 4, 8, 1, 0);

            // Converte la stringa nel numero
            *encryptionKey = atoi(keyString); // NOLINT(*-err34-c)

            // Pulisce la stringa
            clearString(keyString, 17);
        }
    }

    // Scrivi messaggio di autenticazione riuscita
    printf("\nAutenticazione riuscita!\nPremi un tasto per continuare...");
    _getch();
}

/**
 * Questa funzione lancia il menu per la creazione di una nuova password
 *
 * @param encryptionKey La chiave per cifrare la nuova password
 */
void newPasswordMenu(const int encryptionKey) {
    // Dettagli password
    char id[128] = {0};
    char details[128] = {0};
    char password[33] = {0};

    // Intestazione
    printPasswordPlusText();

    // Richiesta ID
    printf("Inserisci l'ID univoco della password (1 Parola)\n>> ");
    printWhiteSpaces(128);
    requestSafeInput(id, 127, 1, 0, 61, 4, 8, 0, 0);
    moveCursor(1, 7);

    // Dettagli
    printf("Inserisci i dettagli della password                                                    \n>> ");
    printWhiteSpaces(128);
    requestSafeInput(details, 127, 1, 0, 61, 4, 8, 0, 1);
    moveCursor(1, 7);

    // Password
    printf("Inserisci la password                                                                   \n>> ");
    printWhiteSpaces(128);
    requestSafeInput(password, 32, 1, 0, 75, 4, 8, 1, 0);

    // Salva la password
    saveNewPassword(id, details, password, encryptionKey);

    // Notifica il successo
    printf("\nPassword salvata con successo!\nPremi un tasto per continuare...");
    _getch();
}

/**
 * Questa funzione stampa la lista delle password
 *
 * @param encryptionKey La chiave di cifratura
 */
void printPasswordsList(const int encryptionKey) {
    // Scrive la testata delle password
    printPasswordListHeader();

    // Legge riga per riga e scrive le password
    FILE* passwordFile = fopen("data/passwords.csv", "r");
    int row = 0;

    // Buffer riga
    char buf[512] = {0};

    // Legge riga per riga
    while (fgets(buf, 512, passwordFile)) {
        // Legge la riga
        char id[128];
        char details[128];
        char password[33];
        sscanf(buf, "\"%[^\"]\",\"%[^\"]\",\"%[^\"]\"", id, details, password);

        if (row == 0) {
            // Salta la prima riga
            ++row;
            continue;
        }

        // Decifra la password
        char passwordDecrypted[33];
        decryptString(password, passwordDecrypted, encryptionKey);

        // Crea nuovi buffer
        char idBuffer[33];
        char detailsBuffer[65];
        char passwordBuffer[33];
        printStringInBuffer(idBuffer, id, 32);
        printStringInBuffer(detailsBuffer, details, 64);
        printStringInBuffer(passwordBuffer, passwordDecrypted, 32);

        // Scrive le righe
        printf("%s | %s | %s\n", idBuffer, detailsBuffer, passwordBuffer);

        // Incrementa la riga
        ++row;
    }

    // Chiude il file
    printf("\nPremi un tasto per continuare...");
    _getch();
}

/**
 * Questa funzione lancia il menu per la scelta delle azioni da eseguire
 *
 * @param encryptionKey La chiave di cifratura
 */
void actionsMenu(const int encryptionKey) {
    // Prepara lo schermo
    printActionsMenu();

    while (1) {
        // Input
        const int input = _getch();

        // Se equivale a Q, esce
        if (input == 'Q' || input == 'q') {
            break;
        }

        if (input == '1') {
            // Menu creazione password
            clearScreen();
            newPasswordMenu(encryptionKey);

            // Stampa lo schermo
            clearScreen();
            printActionsMenu();
        }
        else if (input == '2') {
            // Menu lista delle password
            clearScreen();
            printPasswordsList(encryptionKey);

            // Stampa lo schermo
            clearScreen();
            printActionsMenu();
        }
    }
}

#pragma endregion

int main(void) {
    // Charset
    SetConsoleOutputCP(CP_UTF8);

    // Setup
    createPreliminaryFiles();

    // Valori globali
    int encryptionKey = 0;

    // Lancia il menù dell'applicazione
    generalKeyRequestMenu(&encryptionKey);

    // Pulisce lo schermo
    clearScreen();

    // Menu azioni
    actionsMenu(encryptionKey);

    // Pulisce lo schermo
    clearScreen();

    // OK
    return 0;
}