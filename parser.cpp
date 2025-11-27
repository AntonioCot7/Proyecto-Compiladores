#include<iostream>
#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"
#include <cmath>

using namespace std;

// =============================
// Métodos de la clase Parser
// =============================

Parser::Parser(Scanner* sc) : scanner(sc) {
    previous = nullptr;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        throw runtime_error("Error léxico");
    }
}

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;

        if (check(Token::ERR)) {
            throw runtime_error("Error lexico");
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}


// =============================
// Reglas gramaticales
// =============================

Program* Parser::parseProgram() {
    Program* p = new Program();

    // 1. Parsear Includes
    if (match(Token::HASH)) {
        p->includes.push_back(parseInclude());
        while(match(Token::SEMICOL)) {
            if(check(Token::HASH)) {
                match(Token::HASH);
                p->includes.push_back(parseInclude());
            }
        }
    }

    // Parsear Typedefs y Structs (ANTES de variables/funciones)
    // Se ejecuta mientras encuentre 'typedef' o 'struct'
    while (check(Token::TYPEDEF) || check(Token::STRUCT)) {
        if (check(Token::TYPEDEF)) {
            match(Token::TYPEDEF);
            if (check(Token::STRUCT)) {
                // typedef struct { ... } Name;
                p->strlist.push_back(parserStructDec()); 
            } else {
                // typedef int uint;
                string underlying = parseType();
                if (!match(Token::ID)) throw runtime_error("Error sintáctico: ID esperado en typedef");
                string alias = previous->text;
                if (!match(Token::SEMICOL)) throw runtime_error("Error sintáctico: ';' esperado");
                TypedefDec* td = new TypedefDec(underlying, alias);
                p->tdlist.push_back(td);
            }
        } else if (check(Token::STRUCT)) {
            p->strlist.push_back(parserStructDec());
        }
    }

    // Parsear Declaraciones Globales y Funciones
    // Ahora aceptamos ID (tipos personalizados) como inicio
    while (check(Token::INT) || check(Token::LONG) || check(Token::FLOAT) || check(Token::UNSIGNED) || check(Token::ID)) {
        
        string tipo = parseType(); // Consume int, long, float, o un ID (ej: "Punto")
        
        if (!match(Token::ID)) {
            throw runtime_error("Error sintáctico: se esperaba un identificador después del tipo '" + tipo + "'");
        }
        string id = previous->text;
        
        if (check(Token::LPAREN)) {
            // Función: Tipo ID ( ... )
            FunDec* fd = new FunDec();
            fd->type = tipo;
            fd->id = id;
            match(Token::LPAREN);
            // ... (Lógica de parámetros igual a tu código original) ...
            if(check(Token::INT) || check(Token::LONG) || check(Token::FLOAT) || check(Token::UNSIGNED) || check(Token::ID)) {
                while(true) {
                    string ptipos = parseType();
                    if (!match(Token::ID)) throw runtime_error("Parametro sin nombre");
                    fd->params.push_back(new ParamDec(ptipos, previous->text));
                    if (!match(Token::COMMA)) break;
                }
            }
            if (!match(Token::RPAREN)) throw runtime_error("Falta ')'");
            fd->body = parseBody();
            p->fdlist.push_back(fd);
        } 
        else if (check(Token::ASSIGN)) {
            // Instancia global: Tipo ID = ...
            InstanceDec* id_dec = paserInstanceDecFrom(tipo, id); // Usar tu helper
            p->intdlist.push_back(id_dec);
            if (!match(Token::SEMICOL)) throw runtime_error("Falta ';'");
        }
        else if (check(Token::SEMICOL) || check(Token::COMMA)) {
            // Variable global: Tipo ID;
            VarDec* vd = parseVarDecFrom(tipo, id); // Usar tu helper
            p->vdlist.push_back(vd);
            if (!match(Token::SEMICOL)) throw runtime_error("Falta ';'");
        }
        else {
            throw runtime_error("Declaración global mal formada para: " + id);
        }
    }
    return p;
}

Include* Parser::parseInclude() {
    Include* inc;
    match(Token::INCLUDE);
    match(Token::LT);
    // Allow header names with dots: ID ('.' ID)*
    string h = "";
    if (match(Token::ID)) {
        h = previous->text;
        while (match(Token::DOT)) {
            if (!match(Token::ID)) throw runtime_error("Error sintáctico: se esperaba identificador en include");
            h += "." + previous->text;
        }
    }
    match(Token::GT);
    inc = new Include(h);
    return inc;
}

StructDec* Parser::parserStructDec() {
    StructDec* Std = new StructDec();
    match(Token::STRUCT);
    match(Token::LBRACE);
    // VarDecList: uno o más VarDec terminados en ';'
    while (!check(Token::RBRACE)) {
        VarDec* vd = parseVarDec();
        Std->VdList.push_back(vd);
        if (!match(Token::SEMICOL)) {
            throw runtime_error("Error sintáctico: se esperaba ';' después de la declaración de variable en struct");
        }
    }
    match(Token::RBRACE);
    match(Token::ID);
    Std->nombre = previous->text;
    if (!match(Token::SEMICOL)) {
        throw runtime_error("Error sintáctico: se esperaba ';' después de la declaración de struct");
    }
    return Std;
}

VarDec* Parser::parseVarDec(){
    VarDec* vd = new VarDec();
    // El tipo puede ser 'int', 'long' o 'unsigned int'
    string tipo = parseType();
    vd->type = tipo;
    if (!match(Token::ID)) {
        throw runtime_error("Error sintáctico: se esperaba un identificador en la declaración de variable");
    }
    vd->vars.push_back(previous->text);
    while(match(Token::COMMA)) {
        if (!match(Token::ID)) {
            throw runtime_error("Error sintáctico: se esperaba un identificador después de la coma");
        }
        vd->vars.push_back(previous->text);
    }
    return vd;
}

InstanceDec* Parser::paserInstanceDec() {
    InstanceDec* id = new InstanceDec();
    // Espera: Type ID (= InitData) (',' ID '=' InitData)*
    string tipo = parseType();
    id->type = tipo;
    if (!match(Token::ID)) {
        throw runtime_error("Error sintáctico: se esperaba identificador en inicialización en el tipo: "+tipo);

    }
    id->vars.push_back(previous->text);
    match(Token::ASSIGN);
    id->values.push_back(parserInitData());
    while (match(Token::COMMA)) {
        match(Token::ID);
        id->vars.push_back(previous->text);
        match(Token::ASSIGN);
        id->values.push_back(parserInitData());
    }
    return id;
}

InitData* Parser::parserInitData() {
    InitData* data = new InitData();
    if( check(Token::LBRACE) ){
        match(Token::LBRACE);
        StructInit* estructura = new StructInit(); 
        if (!check(Token::RBRACE)) {
            estructura->argumentos.push_back(parseCE());
            while (match(Token::COMMA)) {
                estructura->argumentos.push_back(parseCE());
            }
        }
        match(Token::RBRACE);
        data->st = estructura; 
        data->e = nullptr;
        return data; 
    }
    else{
        data->e = parseCE();
        data->st = nullptr;
        return data;
    }
}

Body* Parser::parseBody() {
    Body* b = new Body();
    if (!match(Token::LBRACE)) throw runtime_error("Se esperaba '{'");

    // Bucle de declaraciones locales
    while (true) {
        // Casos claros de declaración
        if (check(Token::INT) || check(Token::LONG) || check(Token::FLOAT) || check(Token::UNSIGNED) || check(Token::TYPEDEF)) {
            if (check(Token::TYPEDEF)) {
                // ... (Tu lógica de typedef local) ...
                match(Token::TYPEDEF);
                // ... copiar tu logica existente ...
                string underlying = parseType();
                match(Token::ID);
                string alias = previous->text;
                match(Token::SEMICOL);
                b->tdlist.push_back(new TypedefDec(underlying, alias));
            } else {
                string tipo = parseType();
                if (!match(Token::ID)) throw runtime_error("ID faltante");
                string nombre = previous->text;

                if (check(Token::ASSIGN)) {
                    InstanceDec* idd = paserInstanceDecFrom(tipo, nombre);
                    match(Token::SEMICOL);
                    b->intances.push_back(idd);
                } else {
                    VarDec* vd = parseVarDecFrom(tipo, nombre);
                    match(Token::SEMICOL);
                    b->declarations.push_back(vd);
                }
            }
        } 
        // Caso ambiguo: Empieza con ID (puede ser "Punto p;" o "x = 5;")
        else if (check(Token::ID)) {
            Token* posibleTipo = current;
            advance(); // Consumimos el primer ID (ej: "Punto" o "x")

            if (check(Token::ID)) {
                // Tenemos "ID ID" -> Es una declaración (ej: "Punto p")
                string tipo = posibleTipo->text;
                string nombre = current->text;
                advance(); // Consumimos el segundo ID

                if (check(Token::ASSIGN)) {
                    InstanceDec* idd = paserInstanceDecFrom(tipo, nombre);
                    match(Token::SEMICOL);
                    b->intances.push_back(idd);
                } else {
                    VarDec* vd = parseVarDecFrom(tipo, nombre);
                    match(Token::SEMICOL);
                    b->declarations.push_back(vd);
                }
            } else {
                // Tenemos "ID =" o "ID ." -> Es una sentencia (assignment)
                // NO es una declaración. Terminamos el bucle de declaraciones.
                string lvalueName = posibleTipo->text;
                while (match(Token::DOT)) {
                    if(!match(Token::ID)) throw runtime_error("ID esperado tras punto");
                    lvalueName += "." + previous->text;
                }
                if (!match(Token::ASSIGN)) throw runtime_error("Se esperaba '=' en asignación");
                Exp* e = parseCE();
                if (!match(Token::SEMICOL)) throw runtime_error("Falta ';'");
                b->stmList.push_back(new AssignStm(lvalueName, e));
                break; 
            }
        } else {
            // Ni tipo básico ni ID, fin de declaraciones
            break;
        }
    }

    // Parsear resto de sentencias
    while (!check(Token::RBRACE)) {
        b->stmList.push_back(parseStm());
        // Manejo de punto y coma para sentencias simples vs bloques
        if (!check(Token::RBRACE) && check(Token::SEMICOL)) match(Token::SEMICOL); 
    }
    match(Token::RBRACE);
    return b;
}

Stm* Parser::parseStm() {
    Stm* a;
    Exp* e;
    string variable;
    Body* tb = nullptr;
    Body* fb = nullptr;

    if (match(Token::ID)) {
        variable = previous->text;
        while (match(Token::DOT)) {
            if (!match(Token::ID)) throw runtime_error("Error sintáctico: se esperaba identificador después de '.'");
            variable += "." + previous->text;
        }
        if (!match(Token::ASSIGN)) {
            throw runtime_error("Error sintáctico: se esperaba '=' después del identificador '" + variable + "'");
        }
        e = parseCE();
        if (check(Token::SEMICOL)) match(Token::SEMICOL);
        return new AssignStm(variable, e);
    }
    else if (match(Token::PRINTF)) {
        if (!match(Token::LPAREN)) throw runtime_error("Error: falta '(' en printf");
        if (!match(Token::STRING)) throw runtime_error("Error: falta cadena en printf");
        string fmt = previous->text;
        if (fmt.length() >= 2 && fmt.front() == '"' && fmt.back() == '"') {
            fmt = fmt.substr(1, fmt.length() - 2);
        }
        list<Exp*> args;
        if (match(Token::COMMA)) {
            args.push_back(parseCE());
            while (match(Token::COMMA)) args.push_back(parseCE());
        }
        if (!match(Token::RPAREN)) throw runtime_error("Error: falta ')' en printf");
        if (check(Token::SEMICOL)) match(Token::SEMICOL);
        return new PrintfStm(fmt, args);
    }
    else if (match(Token::RETURN)) {
        ReturnStm* r = new ReturnStm();
        if (!check(Token::SEMICOL)) r->e = parseCE();
        match(Token::SEMICOL);
        return r;
    }
    else if (match(Token::IF)) {
        match(Token::LPAREN);
        e = parseCE();
        match(Token::RPAREN);
        tb = parseBody();
        if (match(Token::ELSE)) fb = parseBody();
        return new IfStm(e, tb, fb);
    }
    else if (match(Token::WHILE)) {
        match(Token::LPAREN);
        e = parseCE();
        match(Token::RPAREN);
        tb = parseBody();
        return new WhileStm(e, tb);
    }
    else if (match(Token::FOR)) {
        match(Token::LPAREN);
        Stm* init = nullptr;
        // Verificamos TODOS los tipos posibles para saber si es declaración
        if (check(Token::INT) || check(Token::LONG) || check(Token::UNSIGNED) || 
            check(Token::TYPEDEF) || check(Token::STRUCT) || check(Token::ID)) {
            // Si es ID y el siguiente es ID, es declaración (Ej: uint n).
            // Si es ID y el siguiente es =, ., etc, es asignación.
            bool esDeclaracion = true;
            if (check(Token::ID) && !check(Token::INT) && !check(Token::UNSIGNED)) {
                // Caso borde: "i = 0". parseStm maneja esto.
                init = parseStm();
            } else {
                 // Caso declaración: "int i = 0"
                init = paserInstanceDec();
                if (check(Token::SEMICOL)) match(Token::SEMICOL);
            }
        } else {
            init = parseStm();
        }
        // Asegurar que el primer ; se consumió
        if (check(Token::SEMICOL)) match(Token::SEMICOL);
        Exp* condition = parseCE();
        match(Token::SEMICOL); // Segundo ;
        // Parsear paso (Step)
        match(Token::ID);
        IdExp* var = new IdExp(previous->text);
        StepExp* step = nullptr;
        if (match(Token::INC)) step = new StepExp(var, StepExp::INCREMENT);
        else if (match(Token::DEC)) step = new StepExp(var, StepExp::DECREMENT);
        else if (match(Token::PLUS_ASSIGN)) {
            Exp* amt = parseCE();
            step = new StepExp(var, StepExp::COMPOUND, amt);
        }
        else if (match(Token::MINUS_ASSIGN)) {
            Exp* amt = parseCE();
            step = new StepExp(var, StepExp::COMPOUND, amt);
        } 
        else {
             // Si usaron =, lo atrapamos para dar un error útil
            if(match(Token::ASSIGN)) throw runtime_error("Error: En for usa 'i++', no 'i=...'");
        }
        
        match(Token::RPAREN);
        Body* body = parseBody();
        return new ForStm(init, condition, step, body);
    }

    else {
        string tokenInfo = isAtEnd() ? "fin de archivo" : ("token '" + current->text + "'");
        throw runtime_error("Error sintáctico: se esperaba un statement pero se encontró " + tokenInfo);
    }
}


Exp* Parser::parseCE() {
    Exp* l = parseBE();
    if (match(Token::LE) || match(Token::LT) || match(Token::GE) || match(Token::GT) || match(Token::EQ) || match(Token::NE)) {
        BinaryOp op;
        switch (previous->type) {
            case Token::LE:
                op = LE_OP;
                break;
            case Token::LT:
                op = LT_OP;
                break;
            case Token::GE:
                op = GE_OP;
                break;
            case Token::GT:
                op = GT_OP;
                break;
            case Token::EQ:
                op = EQ_OP;
                break;
            case Token::NE:
                op = NE_OP;
                break;
            default:
                throw runtime_error("Error sintáctico: operador relacional no reconocido");
        }
        Exp* r = parseBE();
        l = new BinaryExp(l, r, op);
    }
    //------------------NUEVO: PARA PARSEAR IF TERNARIO------------------
    if (check(Token::QUESTION)) {
        match(Token::QUESTION);
        TernaryExp* ternaryNode = new TernaryExp();
        Exp* trueExp = parseCE();
        if (!match(Token::COLON)) {
            throw runtime_error("Error sintáctico: se esperaba ':' en expresión ternaria");
        }
        Exp* falseExp = parseCE();
        ternaryNode->condition = l;
        ternaryNode->trueExp = trueExp;
        ternaryNode->falseExp = falseExp;
        l = ternaryNode;
    }
    return l;
}

Exp* Parser::parseBE() {
    Exp* l = parseE();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op;
        if (previous->type == Token::PLUS){
            op = PLUS_OP;
        }
        else{
            op = MINUS_OP;
        }
        Exp* r = parseE();
        l = new BinaryExp(l, r, op);
    }
    return l;
}


Exp* Parser::parseE() {
    Exp* l = parseF();
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op;
        if (previous->type == Token::MUL){
            op = MUL_OP;
        }
        else{
            op = DIV_OP;
        }
        Exp* r = parseF();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

/*
// NO SE USA ESTA FUNCIÓN
Exp* Parser::parseT() {
    Exp* l = parseF();
    if (match(Token::POW)) {
        BinaryOp op = POW_OP;
        Exp* r = parseF();
        l = new BinaryExp(l, r, op);
    }
    return l;
}
*/

Exp* Parser::parseF() {
    Exp* e;
    string nombre;
    if (match(Token::NUM)) {
        // Verificar si es un número flotante (contiene punto)
        string numText = previous->text;
        if (numText.find('.') != string::npos) {
            return new FloatExp(stod(numText));
        } else {
            return new NumberExp(stoi(numText));
        }
    }
    else if (match(Token::TRUE)) {
        return new NumberExp(1);
    }
    else if (match(Token::FALSE)) {
        return new NumberExp(0);
    }
    else if (match(Token::LPAREN))
    {
        e = parseCE();
        match(Token::RPAREN);
        return e;
    }
    else if (match(Token::ID)) {
        nombre = previous->text;
        if(check(Token::LPAREN)) {
            match(Token::LPAREN);
            FcallExp* fcall = new FcallExp();
            fcall->name = nombre;
            if (!check(Token::RPAREN)) {
                fcall->arguments.push_back(parseCE());
                while (match(Token::COMMA)) {
                    if (check(Token::RPAREN)) {
                        throw runtime_error("Error sintáctico: coma final en llamada a función");
                    }
                    fcall->arguments.push_back(parseCE());
                }
            }
            match(Token::RPAREN);
            return fcall;
        }
        else {
            // Soportar acceso a campos: id(.id)*
            while (match(Token::DOT)) {
                if (!match(Token::ID)) throw runtime_error("Error sintáctico: se esperaba identificador después de '.'");
                nombre += "." + previous->text;
            }
            return new IdExp(nombre);
        }
    }
    else {
        string tokenInfo = isAtEnd() ? "fin de archivo" : ("token '" + (current ? current->text : "desconocido") + "'");
        throw runtime_error("Error sintáctico: se esperaba un factor (número, identificador, paréntesis o llamada a función) pero se encontró " + tokenInfo);
    }
}

// Helpers cuando ya consumimos el tipo y el primer identificador
VarDec* Parser::parseVarDecFrom(const string& tipo, const string& firstId) {
    VarDec* vd = new VarDec();
    vd->type = tipo;
    vd->vars.push_back(firstId);
    while (match(Token::COMMA)) {
        if (!match(Token::ID)) throw runtime_error("Error sintáctico: se esperaba identificador después de la coma");
        vd->vars.push_back(previous->text);
    }
    return vd;
}

InstanceDec* Parser::paserInstanceDecFrom(const string& tipo, const string& firstId) {
    InstanceDec* id = new InstanceDec();
    id->type = tipo;
    id->vars.push_back(firstId);
    if (!match(Token::ASSIGN)) throw runtime_error("Error sintáctico: se esperaba '=' en inicialización");
    id->values.push_back(parserInitData());
    while (match(Token::COMMA)) {
        if (!match(Token::ID)) throw runtime_error("Error sintáctico: se esperaba identificador después de la coma");
        id->vars.push_back(previous->text);
        if (!match(Token::ASSIGN)) throw runtime_error("Error sintáctico: se esperaba '=' en inicialización");
        id->values.push_back(parserInitData());
    }
    return id;
}

// Parsea un tipo y devuelve su representación textual.
string Parser::parseType() {
    if (match(Token::INT)) return previous->text;
    if (match(Token::LONG)) return previous->text;
    if (match(Token::FLOAT)) return previous->text;
    if (match(Token::UNSIGNED)) {
        if (!match(Token::INT)) throw runtime_error("Error sintáctico: se esperaba 'int' después de 'unsigned'");
        return string("unsigned int");
    }
    // Permitir tipos referenciados por identificador (typedef/struct names)
    if (match(Token::ID)) return previous->text;
    throw runtime_error("Error sintáctico: se esperaba un tipo");
}
