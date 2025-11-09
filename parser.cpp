#include<iostream>
#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"

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

    // Parsear includes
    if (match(Token::HASH)) {
        p->includes.push_back(parseInclude());
        while(match(Token::SEMICOL)) {
            if(check(Token::HASH)) {
                match(Token::HASH);
                p->includes.push_back(parseInclude());
            }
        }
    }

    // Parsear declaraciones globales (variables, inicializaciones y funciones)
    while (check(Token::INT) || check(Token::LONG)) {
        // Hacer lookahead: consumir tipo e ID para ver qué sigue
        string tipo;
        if (match(Token::INT)) {
            tipo = previous->text;
        } else if (match(Token::LONG)) {
            tipo = previous->text;
        }
        // Verificar que después del tipo viene un ID
        if (!check(Token::ID)) {
            throw runtime_error("Error sintáctico: se esperaba un identificador después del tipo");
        }
        match(Token::ID);  // Consumir ID
        string id = previous->text;
        
        // Determinar qué tipo de declaración es según el siguiente token
        if (check(Token::LPAREN)) {
            // Es una función: int id ( ... )
            // Los métodos de parsing esperan que el tipo ya esté consumido,
            // pero nosotros ya lo consumimos. Necesitamos parsear manualmente
            // o modificar el enfoque.
            // Solución: crear la función manualmente desde aquí
            FunDec* fd = new FunDec();
            fd->Type = tipo;
            fd->id = id;
            if (!match(Token::LPAREN)) {
                throw runtime_error("Error sintáctico: se esperaba '(' después del nombre de la función");
            }
            if(check(Token::INT) || check(Token::LONG)) {
                while(true) {
                    string ptipos;
                    if (match(Token::INT)) {
                        ptipos = previous->text;
                    } else if (match(Token::LONG)) {
                        ptipos = previous->text;
                    }
                    if (!match(Token::ID)) {
                        throw runtime_error("Error sintáctico: se esperaba un identificador en los parámetros");
                    }
                    string pnombres = previous->text;
                    ParamDec* pd = new ParamDec(ptipos, pnombres);
                    fd->params.push_back(pd);
                    if (!match(Token::COMMA)) {
                        break;
                    }
                }
            }
            if (!match(Token::RPAREN)) {
                throw runtime_error("Error sintáctico: se esperaba ')' después de los parámetros");
            }
            fd->body = parseBody();
            p->fdlist.push_back(fd);
        } else if (check(Token::ASSIGN)) {
            // Es una inicialización: int id = ...
            // Parsear como InstanceDec manualmente
            InstanceDec* id_dec = new InstanceDec();
            id_dec->type = tipo;
            id_dec->vars.push_back(id);
            if (!match(Token::ASSIGN)) {
                throw runtime_error("Error sintáctico: se esperaba '=' en la inicialización");
            }
            id_dec->values.push_back(parseCE());
            while (match(Token::COMMA)) {
                if (!match(Token::ID)) {
                    throw runtime_error("Error sintáctico: se esperaba un identificador después de la coma en la inicialización");
                }
                id_dec->vars.push_back(previous->text);
                if (!match(Token::ASSIGN)) {
                    throw runtime_error("Error sintáctico: se esperaba '=' después del identificador en la inicialización");
                }
                id_dec->values.push_back(parseCE());
            }
            p->intdlist.push_back(id_dec);
            if (!match(Token::SEMICOL)) {
                throw runtime_error("Error sintáctico: se esperaba ';' después de la inicialización");
            }
        } else if (check(Token::SEMICOL) || check(Token::COMMA)) {
            // Es una declaración de variable: int id ; o int id , ...
            VarDec* vd = new VarDec();
            vd->type = tipo;
            vd->vars.push_back(id);
            while (match(Token::COMMA)) {
                if (!match(Token::ID)) {
                    throw runtime_error("Error sintáctico: se esperaba un identificador después de la coma");
                }
                vd->vars.push_back(previous->text);
            }
            p->vdlist.push_back(vd);
            if (!match(Token::SEMICOL)) {
                throw runtime_error("Error sintáctico: se esperaba ';' después de la declaración de variable");
            }
        } else {
            string tokenInfo = isAtEnd() ? "fin de archivo" : ("token '" + (current ? current->text : "desconocido") + "'");
            throw runtime_error("Error sintáctico: después de '" + tipo + " " + id + "' se encontró " + tokenInfo + ", se esperaba '(', '=' o ';'");
        }
    }

    cout << "Parser exitoso" << endl;
    return p;
}

Include* Parser::parseInclude() {
    Include* inc;
    match(Token::INCLUDE);
    match(Token::LT);
    match(Token::ID);
    string h = previous->text;
    match(Token::GT);
    inc = new Include(h);
    return inc;
}

VarDec* Parser::parseVarDec(){
    VarDec* vd = new VarDec();
    if(match(Token::INT) || match(Token::LONG)) {
        vd->type = previous->text;
    }
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
    if (match(Token::INT) || match(Token::LONG)) {
        id->type = previous->text;
    }
    match(Token::ID);
    id->vars.push_back(previous->text);
    match(Token::ASSIGN);
    id->values.push_back(parseCE());
    while (match(Token::COMMA)) {
        match(Token::ID);
        id->vars.push_back(previous->text);
        match(Token::ASSIGN);
        id->values.push_back(parseCE());
    }
    return id;
}

FunDec* Parser::parseFunDec() {
    FunDec* fd = new FunDec();
    if (match(Token::INT) || match(Token::LONG)) {
        fd->Type = previous->text;
    }
    match(Token::ID);
    fd->id = previous->text;
    match(Token::LPAREN);
    if(check(Token::INT) || check(Token::LONG)) {
        while(true) {
            string ptipos;
            if (match(Token::INT) || match(Token::LONG)) {
                ptipos = previous->text;
            }
            match(Token::ID);
            string pnombres = previous->text;
            ParamDec* pd = new ParamDec(ptipos, pnombres);
            fd->params.push_back(pd);
            if (!match(Token::COMMA)) {
                break;
            }
        }
    }
    match(Token::RPAREN);
    fd->body = parseBody();
    return fd;
}

Body* Parser::parseBody() {
    Body* b = new Body();
    if (!match(Token::LBRACE)) {
        throw runtime_error("Error sintáctico: se esperaba '{'");
    }
    while (check(Token::INT) || check(Token::LONG)) {
        b->declarations.push_back(parseVarDec());
        if (!match(Token::SEMICOL)) {
            throw runtime_error("Error sintáctico: se esperaba ';' después de la declaración de variable");
        }
    }
    while (check(Token::INT) || check(Token::LONG)) {
        b->intances.push_back(paserInstanceDec());
        if (!match(Token::SEMICOL)) {
            throw runtime_error("Error sintáctico: se esperaba ';' después de la inicialización");
        }
    }
    while (!check(Token::RBRACE)) {
        if (isAtEnd()) {
            throw runtime_error("Error sintáctico: se esperaba '}' pero se llegó al final del archivo");
        }
        b->stmList.push_back(parseStm());
        // Los statements de bloque (if, while, for) no terminan con SEMICOL
        // Solo los statements simples (asignación, printf, return) terminan con SEMICOL
        // Si el siguiente token es RBRACE, no consumir SEMICOL (es el final del bloque)
        // Si el siguiente token no es RBRACE ni SEMICOL, el statement parseado debe ser un bloque
        // Si el siguiente token es SEMICOL, consumirlo (es un statement simple)
        if (check(Token::RBRACE)) {
            // Final del bloque, no consumir SEMICOL
            break;
        } else if (check(Token::SEMICOL)) {
            // Statement simple, consumir SEMICOL
            match(Token::SEMICOL);
        }
        // Si no es RBRACE ni SEMICOL, es un bloque (if/while/for) que ya fue parseado completamente
        // Continuar con el siguiente statement
    }
    if (!match(Token::RBRACE)) {
        throw runtime_error("Error sintáctico: se esperaba '}'");
    }
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
        if (!match(Token::ASSIGN)) {
            throw runtime_error("Error sintáctico: se esperaba '=' después del identificador '" + variable + "'");
        }
        e = parseCE();
        return new AssignStm(variable, e);
    }
    else if (match(Token::PRINTF)) {
        if (!match(Token::LPAREN)) {
            throw runtime_error("Error sintáctico: se esperaba '(' después de printf");
        }
        if (!match(Token::STRING)) {
            throw runtime_error("Error sintáctico: se esperaba una cadena después de '(' en printf");
        }
        string fmt = previous->text;
        // Quitar las comillas exteriores del string
        if (fmt.length() >= 2 && fmt[0] == '"' && fmt[fmt.length()-1] == '"') {
            fmt = fmt.substr(1, fmt.length() - 2);
        }
        list<Exp*> args;
        // Si hay una coma después del string, hay argumentos
        if (match(Token::COMMA)) {
            args.push_back(parseCE());
            while (match(Token::COMMA)) {
                // Si inmediatamente después de la coma viene ')' => coma final
                if (check(Token::RPAREN)) {
                    throw runtime_error("Error sintáctico: coma final en printf");
                }
                args.push_back(parseCE());
            }
        }
        if (!match(Token::RPAREN)) {
            throw runtime_error("Error sintáctico: se esperaba ')' en printf");
        }
        return new PrintfStm(fmt, args);
    }
    else if (match(Token::RETURN)) {
        ReturnStm* r = new ReturnStm();
        if (!check(Token::SEMICOL)) {
            r->e = parseCE();
        }
        return r;
    }
    else if (match(Token::IF)) {
        match(Token::LPAREN);
        e = parseCE();
        match(Token::RPAREN);
        tb = parseBody();
        if (match(Token::ELSE)) {
            fb = parseBody();
        }
        a = new IfStm(e, tb, fb);
    }
    else if (match(Token::WHILE)) {
        match(Token::LPAREN);
        e = parseCE();
        match(Token::RPAREN);
        tb = parseBody();
        a = new WhileStm(e, tb);
    }
    else if (match(Token::FOR)) {
        match(Token::LPAREN);
        InstanceDec* init = paserInstanceDec();
        match(Token::SEMICOL);
        Exp* condition = parseCE();
        match(Token::SEMICOL);
        
        // Parsear el paso (step)
        match(Token::ID);
        IdExp* var = new IdExp(previous->text);
        StepExp* step = nullptr;
        
        if (match(Token::INC)) {
            step = new StepExp(var, StepExp::INCREMENT);
        }
        else if (match(Token::PLUS_ASSIGN)) {
            Exp* amount = parseCE();
            step = new StepExp(var, StepExp::COMPOUND, amount);
        }
        else if (match(Token::MINUS_ASSIGN)) {
            Exp* amount = parseCE();
            step = new StepExp(var, StepExp::COMPOUND, amount);
        }
        else {
            throw runtime_error("Error sintáctico: expresión de incremento inválida");
        }
        
        match(Token::RPAREN);
        Body* body = parseBody();
        a = new ForStm(init, condition, step, body);
    }
    else {
        string tokenInfo = isAtEnd() ? "fin de archivo" : ("token '" + (current ? current->text : "desconocido") + "'");
        throw runtime_error("Error sintáctico: se esperaba un statement pero se encontró " + tokenInfo);
    }
    return a;
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
        return new NumberExp(stoi(previous->text));
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
            return new IdExp(nombre);
            }
    }
    else {
        string tokenInfo = isAtEnd() ? "fin de archivo" : ("token '" + (current ? current->text : "desconocido") + "'");
        throw runtime_error("Error sintáctico: se esperaba un factor (número, identificador, paréntesis o llamada a función) pero se encontró " + tokenInfo);
    }
}
