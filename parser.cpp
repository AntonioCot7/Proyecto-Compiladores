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

    if (match(Token::HASH)) {
        p->includes.push_back(parseInclude());
        while(match(Token::SEMICOL)) {
            if(check(Token::HASH)) {
                match(Token::HASH);
                p->includes.push_back(parseInclude());
            }
        }
    }

    if (check(Token::INT) || check(Token::LONG)) {
        p->vdlist.push_back(parseVarDec());
        while(match(Token::SEMICOL)) {
            if(check(Token::INT) || check(Token::LONG)) {
                p->vdlist.push_back(parseVarDec());
            }
        }
        
    }

    if (check(Token::INT) || check(Token::LONG)) {
        p->intdlist.push_back(paserInstanceDec());
        while(match(Token::SEMICOL)) {
            if(check(Token::INT) || check(Token::LONG)) {
                p->intdlist.push_back(paserInstanceDec());
            }
        }
    }

    if (check(Token::INT) || check(Token::LONG)) {
        while (check(Token::INT) || check(Token::LONG)) {
            p->fdlist.push_back(parseFunDec());
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
    match(Token::ID);
    vd->vars.push_back(previous->text);
    while(match(Token::COMMA)) {
        match(Token::ID);
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
    match(Token::LBRACE);
    while (check(Token::INT) || check(Token::LONG)) {
        b->declarations.push_back(parseVarDec());
        match(Token::SEMICOL);
    }
    while (check(Token::INT) || check(Token::LONG)) {
        b->intances.push_back(paserInstanceDec());
        match(Token::SEMICOL);
    }
    while (!check(Token::RBRACE)) {
        b->stmList.push_back(parseStm());
        match(Token::SEMICOL);
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
        match(Token::ASSIGN);
        e = parseCE();
        return new AssignStm(variable, e);
    }
    else if (match(Token::PRINTF)) {
        match(Token::LPAREN);
        match(Token::FORMAT);
        string fmt = previous->text;
        list<Exp*> args;
        if (!check(Token::RPAREN)) {
            args.push_back(parseCE());
            while (match(Token::COMMA)) {
                // Si inmediatamente después de la coma viene ')' => coma final
                if (check(Token::RPAREN)) {
                    throw runtime_error("Error sintáctico: coma final en printf");
                }
                args.push_back(parseCE());
            }
        }
        match(Token::RPAREN);
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
        match(Token::ID);
        string inc = previous->text;
        match(Token::INC);
        match(Token::RPAREN);
        Body* body = parseBody();
        a = new ForStm(init, condition, inc, body);
    }
    else {
        throw runtime_error("Error sintáctico");
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
                throw runtime_error("Error sintáctico");
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
        throw runtime_error("Error sintáctico");
    }
}
