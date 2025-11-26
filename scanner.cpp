#include <iostream>
#include <cstring>
#include <fstream>
#include "token.h"
#include "scanner.h"

using namespace std;

// -----------------------------
// Constructor
// -----------------------------
Scanner::Scanner(const char* s): input(s), first(0), current(0) { 
    }

// -----------------------------
// Función auxiliar
// -----------------------------

bool is_white_space(char c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

// -----------------------------
// nextToken: obtiene el siguiente token
// -----------------------------


Token* Scanner::nextToken() {
    Token* token;

    // Saltar espacios en blanco
    while (current < input.length() && is_white_space(input[current])) 
        current++;

    // Fin de la entrada
    if (current >= input.length()) 
        return new Token(Token::END);

    char c = input[current];
    first = current;

    // Números (enteros y decimales con punto)
    if (isdigit(c)) {
        current++;
        while (current < input.length() && isdigit(input[current]))
            current++;
        
        // Verificar si hay un punto decimal seguido de más dígitos
        if (current < input.length() && input[current] == '.' && 
            current + 1 < input.length() && isdigit(input[current + 1])) {
            current++; // saltar el punto
            while (current < input.length() && isdigit(input[current]))
                current++;
        }
        
        token = new Token(Token::NUM, input, first, current - first);
    }

    // Cadenas y formatos
    else if (c == '"') {
        int string_start = first;  // Guardamos el inicio de la cadena
        current++;  // Saltamos la comilla inicial

        while (current < input.length()) {
            // Procesar especificaciones de formato (solo avanzamos, no las separamos)
            if (input[current] == '%') {
                if (current + 1 < input.length()) {
                    if (input[current + 1] == 'd') {
                        current += 2;
                        continue;
                    }
                    else if (current + 2 < input.length() && input[current + 1] == 'l' && input[current +2] == 'd') {
                        current += 3;
                        continue;
                    }
                }
            }

            // Procesar escape sequences (como \n, \t, etc.)
            if (input[current] == '\\') {
                if (current + 1 < input.length()) {
                    if (input[current + 1] == 'n' || input[current + 1] == 't' || input[current + 1] == '"' || input[current + 1] == '\\') {
                        current += 2;
                        continue;
                    }
                }
            }

            // Buscar fin de cadena
            if (input[current] == '"') {
                current++; // Incluir la comilla final

                // SIEMPRE retornamos el string completo (con o sin formato)
                // Esto preserva espacios, \n y otros caracteres antes/después del formato
                return new Token(Token::STRING, input, string_start, current - string_start);
            }
            current++;
        }
        return new Token(Token::ERR, input[string_start]);
    }

    // ID y palabras clave
    else if (isalpha(c) || c == '_') {
        current++;
        while (current < input.length() && (isalnum(input[current]) || input[current] == '_'))
            current++;
        string lexema = input.substr(first, current - first);

        // Palabras clave
        if (lexema == "int") return new Token(Token::INT, input, first, current - first);
        else if (lexema == "true") return new Token(Token::TRUE, input, first, current - first);
        else if (lexema == "false") return new Token(Token::FALSE, input, first, current - first);
        else if (lexema == "long") return new Token(Token::LONG, input, first, current - first);
        else if (lexema == "float") return new Token(Token::FLOAT, input, first, current - first);
        else if (lexema == "if") return new Token(Token::IF, input, first, current - first);
        else if (lexema == "else") return new Token(Token::ELSE, input, first, current - first);
        else if (lexema == "while") return new Token(Token::WHILE, input, first, current - first);
        else if (lexema == "for") return new Token(Token::FOR, input, first, current - first);
        else if (lexema == "return") return new Token(Token::RETURN, input, first, current - first);
        else if (lexema == "include") return new Token(Token::INCLUDE, input, first, current - first);
        else if (lexema == "printf") return new Token(Token::PRINTF, input, first, current - first);
        else if (lexema == "typedef") return new Token(Token::TYPEDEF, input, first, current - first);
        else if (lexema == "struct") return new Token(Token::STRUCT, input, first, current - first);
        else if (lexema == "unsigned") return new Token(Token::UNSIGNED, input, first, current - first);

        // Id
        else return new Token(Token::ID, input, first, current - first);
    }
    // Operadores
    else if (strchr("+/-*(){};,=<>!.#?:", c)) {
        if (c == '+') {
            if (current + 1 < input.length()) {
                if (input[current + 1] == '+') {
                    current += 2;
                    return new Token(Token::INC, input, first, 2);
                }
                else if (input[current + 1] == '=') {
                    current += 2;
                    return new Token(Token::PLUS_ASSIGN, input, first, 2);
                }
            }
            current++;
            return new Token(Token::PLUS, c);
        }
        else if (c == '-') {
            if (current + 1 < input.length()) {
                if (input[current + 1] == '-') {
                    current += 2;
                    return new Token(Token::DEC, input, first, 2);
                }
                else if (input[current + 1] == '=') {
                    current += 2;
                    return new Token(Token::MINUS_ASSIGN, input, first, 2);
                }
            }
            current++;
            return new Token(Token::MINUS, c);
        }
        else if (c == '=' && current + 1 < input.length() && input[current + 1] == '=') {
            current += 2;
            return new Token(Token::EQ, input, first, 2);
        }
        else if (c == '>' && current + 1 < input.length() && input[current + 1] == '=') {
            current += 2;
            token = new Token(Token::GE, input, first, 2);
        }
        else if (c == '<' && current + 1 < input.length() && input[current + 1] == '=') {
            current += 2;
            token = new Token(Token::LE, input, first, 2);
        }
        else if (c == '!' && current + 1 < input.length() && input[current + 1] == '=') {
            current += 2;
            token = new Token(Token::NE, input, first, 2);
        }
        else {
            current++;
            switch (c) {
                case '#': token = new Token(Token::HASH, c); break;
                case '+': token = new Token(Token::PLUS, c); break;
                case '-': token = new Token(Token::MINUS, c); break;
                case '*': token = new Token(Token::MUL, c); break;
                case '/': token = new Token(Token::DIV, c); break;
                case '(': token = new Token(Token::LPAREN, c); break;
                case ')': token = new Token(Token::RPAREN, c); break;
                case '{': token = new Token(Token::LBRACE, c); break;
                case '}': token = new Token(Token::RBRACE, c); break;
                case ';': token = new Token(Token::SEMICOL, c); break;
                case ',': token = new Token(Token::COMMA, c); break;
                case '=': token = new Token(Token::ASSIGN, c); break;
                case '>': token = new Token(Token::GT, c); break;
                case '.': token = new Token(Token::DOT, c); break;
                case '<': token = new Token(Token::LT, c); break;
                case '?': token = new Token(Token::QUESTION, c); break;
                case ':': token = new Token(Token::COLON, c); break;
            }
        }
    }

    // Carácter inválido
    else {
        token = new Token(Token::ERR, c);
        current++;
    }

    return token;
}

// -----------------------------
// Destructor
// -----------------------------
Scanner::~Scanner() { }

// -----------------------------
// Función para ejecutar solo el scanner
// -----------------------------
int solo_scanner(const char* input_file) {
    // Leer archivo
    ifstream infile(input_file);
    if (!infile.is_open()) {
        cerr << "No se pudo abrir el archivo: " << input_file << endl;
        return 1;
    }

    string input, line;
    while (getline(infile, line)) {
        input += line + '\n';
    }
    infile.close();

    // Crear scanner y ejecutarlo
    Scanner scanner(input.c_str());
    return ejecutar_scanner(&scanner, string(input_file));
}

// -----------------------------
// Función de prueba
// -----------------------------

int ejecutar_scanner(Scanner* scanner, const string& InputFile) {
    Token* tok;

    // Obterner el nombre base del archivo de entrada
    string basename = InputFile;
    // Extraer solo el nombre del archivo (sin path)
    size_t slash = basename.find_last_of("/\\");
    if (slash != string::npos) {
        basename = basename.substr(slash + 1);
    }
    // Quitar extensión
    size_t pos = basename.find_last_of('.');
    if (pos != string::npos) {
        basename = basename.substr(0, pos);
    }

    // Crear archivo en la carpeta tokens/
    string OutputFileName = "tokens/" + basename + "_tokens.txt";
    ofstream outFile(OutputFileName);
    if (!outFile.is_open()) {
        // Intentar crear la carpeta tokens/ si no existe
        system("mkdir -p tokens");
        outFile.open(OutputFileName);
        if (!outFile.is_open()) {
            cerr << "Error: no se pudo abrir el archivo " << OutputFileName << endl;
            return 0;
        }
    }

    outFile << "Scanner\n" << endl;

    while (true) {
        tok = scanner->nextToken();

        if (tok->type == Token::END) {
            outFile << *tok << endl;
            delete tok;
            outFile << "\nScanner exitoso" << endl << endl;
            outFile.close();
            return 0;
        }

        if (tok->type == Token::ERR) {
            outFile << *tok << endl;
            delete tok;
            outFile << "Caracter invalido" << endl << endl;
            outFile << "Scanner no exitoso" << endl << endl;
            outFile.close();
            return 0;
        }

        outFile << *tok << endl;
        delete tok;
    }
}
