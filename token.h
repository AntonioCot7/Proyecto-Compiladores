#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>

using namespace std;

class Token {
public:
    // Tipos de token
    enum Type {
        // Literales booleanos
        TRUE,    // true
        FALSE,   // false

        // Operadores
        PLUS,    // +
        MINUS,   // -
        MUL,     // *
        DIV,     // /
        INC,     // ++
        DEC,     // --
        GT,      // >
        LT,      // <
        GE,      // >=
        LE,      // <=
        EQ,      // ==
        NE,      // !=
        ASSIGN,  // =
        PLUS_ASSIGN,  // +=
        MINUS_ASSIGN, // -=
        
        // Símbolos
        LPAREN,  // (
        RPAREN,  // )
        LBRACE,  // {
        RBRACE,  // }
        SEMICOL, // ;
        COMMA,   // ,
        HASH,    // #
        QUESTION, // ?
        COLON,   // :

        // Palabras clave
        INT,    // int
        LONG,    // long
        FLOAT,   // float
        IF,      // if
        ELSE,    // else
        WHILE,   // while
        FOR,     // for
        RETURN,  // return
        INCLUDE, // include
        PRINTF,  // printf
        // Nuevos tokens
        TYPEDEF, // typedef
        STRUCT,  // struct
        UNSIGNED, // unsigned
        DOT,     // .

        // Identificadores y literales
        ID,      // ID
        NUM,     // Número
        STRING,  // cadenas ("...")
        FORMAT,  // especificadores formato (%d, %ld)

        // Especiales
        ERR,     // Error
        END,     // Fin de entrada
    };

    // Atributos
    Type type;
    string text;

    // Constructores
    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const string& source, int first, int last);

    // Sobrecarga de operadores de salida
    friend ostream& operator<<(ostream& outs, const Token& tok);
    friend ostream& operator<<(ostream& outs, const Token* tok);
};

#endif // TOKEN_H