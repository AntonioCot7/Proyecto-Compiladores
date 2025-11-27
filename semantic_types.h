#ifndef SEMANTIC_TYPES_H
#define SEMANTIC_TYPES_H

#include <iostream>
#include <string>
using namespace std;

// ===========================================================
//  Representación de tipos básicos del lenguaje
// ===========================================================

class Type {
public:
    enum TType { NOTYPE, VOID, INT, BOOL, UNSIGNED, FLOAT, STRUCT };
    enum { TYPE_NAME_COUNT = 7 };
    static const char* type_names[TYPE_NAME_COUNT];
    // Para struct types
    string struct_name;

    TType ttype;

    Type() : ttype(NOTYPE) {}
    Type(TType tt) : ttype(tt) {}

    // Comparación de tipos
    bool match(Type* t) const {
        if (!t) return false;
        if (this->ttype == STRUCT || t->ttype == STRUCT) {
            // ambos deben ser STRUCT y tener mismo nombre
            return this->ttype == STRUCT && t->ttype == STRUCT && this->struct_name == t->struct_name;
        }
        return this->ttype == t->ttype;
    }

    // Asignación de tipo básico desde string
    bool set_basic_type(const string& s) {
        TType tt = string_to_type(s);
        if (tt == NOTYPE) return false;
        ttype = tt; // ttype guarda el tipo actual del objeto Type
        return true;
    }

    // Conversión string 
    static TType string_to_type(const string& s) {
        if (s == "int") return INT;
        if (s == "unsigned" || s == "unsigned int") return UNSIGNED;
        if (s == "bool") return BOOL;
        if (s == "void") return VOID;
        if (s == "float") return FLOAT;
        return NOTYPE;
    }
};

inline const char* Type::type_names[Type::TYPE_NAME_COUNT] = { "notype", "void", "int", "bool", "unsigned", "float", "struct" };

#endif // SEMANTIC_TYPES_H
