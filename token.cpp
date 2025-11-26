#include <iostream>
#include "token.h"

using namespace std;

// -----------------------------
// Constructores
// -----------------------------

Token::Token(Type type) 
    : type(type), text("") { }

Token::Token(Type type, char c) 
    : type(type), text(string(1, c)) { }

Token::Token(Type type, const string& source, int first, int last) 
    : type(type), text(source.substr(first, last)) { }

// -----------------------------
// Sobrecarga de operador <<
// -----------------------------

// Para Token por referencia
ostream& operator<<(ostream& outs, const Token& tok) {
    switch (tok.type) {
        // Literales booleanos
        case Token::TRUE:    outs << "TOKEN(TRUE, \""    << tok.text << "\")"; break;
        case Token::FALSE:   outs << "TOKEN(FALSE, \""   << tok.text << "\")"; break;

        // Operadores
        case Token::PLUS:    outs << "TOKEN(PLUS, \""    << tok.text << "\")"; break;
        case Token::MINUS:   outs << "TOKEN(MINUS, \""   << tok.text << "\")"; break;
        case Token::MUL:     outs << "TOKEN(MUL, \""     << tok.text << "\")"; break;
        case Token::DIV:     outs << "TOKEN(DIV, \""     << tok.text << "\")"; break;
        case Token::INC:     outs << "TOKEN(INC, \""     << tok.text << "\")"; break;
        case Token::GT:      outs << "TOKEN(GT, \""      << tok.text << "\")"; break;
        case Token::LT:      outs << "TOKEN(LT, \""      << tok.text << "\")"; break;
        case Token::GE:      outs << "TOKEN(GE, \""      << tok.text << "\")"; break;
        case Token::LE:      outs << "TOKEN(LE, \""      << tok.text << "\")"; break;
        case Token::EQ:      outs << "TOKEN(EQ, \""      << tok.text << "\")"; break;
        case Token::NE:      outs << "TOKEN(NE, \""      << tok.text << "\")"; break;
        case Token::ASSIGN:  outs << "TOKEN(ASSIGN, \""  << tok.text << "\")"; break;
        
        // Símbolos
        case Token::LPAREN:  outs << "TOKEN(LPAREN, \""  << tok.text << "\")"; break;
        case Token::RPAREN:  outs << "TOKEN(RPAREN, \""  << tok.text << "\")"; break;
        case Token::LBRACE:  outs << "TOKEN(LBRACE, \""  << tok.text << "\")"; break;
        case Token::RBRACE:  outs << "TOKEN(RBRACE, \""  << tok.text << "\")"; break;
        case Token::SEMICOL: outs << "TOKEN(SEMICOL, \"" << tok.text << "\")"; break;
        case Token::COMMA:   outs << "TOKEN(COMMA, \""   << tok.text << "\")"; break;
        case Token::HASH:    outs << "TOKEN(HASH, \""    << tok.text << "\")"; break;
        case Token::QUESTION:outs << "TOKEN(QUESTION, \"" << tok.text << "\")"; break;
        case Token::COLON:   outs << "TOKEN(COLON, \""   << tok.text << "\")"; break;
        
        // Palabras clave
        case Token::INT:     outs << "TOKEN(INT, \""     << tok.text << "\")"; break;
        case Token::LONG:    outs << "TOKEN(LONG, \""    << tok.text << "\")"; break;
        case Token::FLOAT:   outs << "TOKEN(FLOAT, \""   << tok.text << "\")"; break;
        case Token::IF:      outs << "TOKEN(IF, \""      << tok.text << "\")"; break;
        case Token::ELSE:    outs << "TOKEN(ELSE, \""    << tok.text << "\")"; break;
        case Token::WHILE:   outs << "TOKEN(WHILE, \""   << tok.text << "\")"; break;
        case Token::FOR:     outs << "TOKEN(FOR, \""     << tok.text << "\")"; break;
        case Token::RETURN:  outs << "TOKEN(RETURN, \""  << tok.text << "\")"; break;
        case Token::INCLUDE: outs << "TOKEN(INCLUDE, \"" << tok.text << "\")"; break;
        case Token::PRINTF:  outs << "TOKEN(PRINTF, \""  << tok.text << "\")"; break;
        // Nuevos tokens
        case Token::TYPEDEF: outs << "TOKEN(TYPEDEF, \"" << tok.text << "\")"; break;
        case Token::STRUCT:  outs << "TOKEN(STRUCT, \""  << tok.text << "\")"; break;
        case Token::UNSIGNED:outs << "TOKEN(UNSIGNED, \""<< tok.text << "\")"; break;
        case Token::DOT:     outs << "TOKEN(DOT, \""     << tok.text << "\")"; break;
        
        // Identificadores y literales
        case Token::ID:      outs << "TOKEN(ID, \""      << tok.text << "\")"; break;
        case Token::NUM:     outs << "TOKEN(NUM, \""     << tok.text << "\")"; break;
        case Token::STRING:  outs << "TOKEN(STRING, \""  << tok.text << "\")"; break;
        case Token::FORMAT:  outs << "TOKEN(FORMAT, \""  << tok.text << "\")"; break;
        
        // Especiales
        case Token::ERR:     outs << "TOKEN(ERR, \""     << tok.text << "\")"; break;
        case Token::END:     outs << "TOKEN(END)"; break;
    }
    return outs;
}

// Para Token puntero
ostream& operator<<(ostream& outs, const Token* tok) {
    if (!tok) return outs << "TOKEN(NULL)";
    return outs << *tok;  // delega al otro
}