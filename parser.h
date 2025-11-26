#ifndef PARSER_H       
#define PARSER_H

#include "scanner.h"    // Incluye la definición del escáner (provee tokens al parser)
#include "ast.h"        // Incluye las definiciones para construir el Árbol de Sintaxis Abstracta (AST)

class Parser {
private:
    Scanner* scanner;       // Puntero al escáner, de donde se leen los tokens
    Token *current, *previous; // Punteros al token actual y al anterior
    bool match(Token::Type ttype);   // Verifica si el token actual coincide con un tipo esperado y avanza si es así
    bool check(Token::Type ttype);   // Comprueba si el token actual es de cierto tipo, sin avanzar
    bool advance();                  // Avanza al siguiente token
    bool isAtEnd();                  // Comprueba si ya se llegó al final de la entrada
    // Parsea un tipo y devuelve su representación como string (p.ej. "int", "unsigned int")
    string parseType();
public:
    Parser(Scanner* scanner);       
    Program* parseProgram();
    Include* parseInclude();
    StructDec* parserStructDec();
    VarDec* parseVarDec();
    InstanceDec* paserInstanceDec();
    InitData* parserInitData();
    FunDec* parseFunDec();
    Body* parseBody();
    Stm* parseStm();
    Exp* parseCE();
    Exp* parseBE();
    Exp* parseE();
    //Exp* parseT();
    Exp* parseF();
    // Helpers para reutilizar cuando el tipo ya fue consumido
    VarDec* parseVarDecFrom(const string& tipo, const string& firstId);
    InstanceDec* paserInstanceDecFrom(const string& tipo, const string& firstId);
};

#endif // PARSER_H      