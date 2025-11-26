#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include "token.h"
using namespace std;

class Scanner {
private:
    string input;
    int first;
    int current;

public:
    // Constructor
    Scanner(const char* in_s);

    // Retorna el siguiente token
    Token* nextToken();

    // Destructor
    ~Scanner();

};

// Ejecutar scanner y guardar tokens en archivo
int ejecutar_scanner(Scanner* scanner,const string& InputFile);

int solo_scanner(const char* input_file);

#endif // SCANNER_H