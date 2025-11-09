#include "ast.h"
#include <iostream>

using namespace std;

// ------------------ Exp ------------------
Exp::~Exp() {}

string Exp::binopToChar(BinaryOp op) {
    switch (op) {
        case PLUS_OP:   return "+";
        case MINUS_OP:  return "-";
        case MUL_OP:    return "*";
        case DIV_OP:    return "/";
        case INC_OP:    return "++";
        case GT_OP:     return ">";
        case LT_OP:     return "<";
        case GE_OP:     return ">=";
        case LE_OP:     return "<=";
        case EQ_OP:     return "==";
        case NE_OP:     return "!=";
        case ASSIGN_OP: return "=";
        default:        return "?";
    }
}

// ------------------ BinaryExp ------------------
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp o) : left(l), right(r), op(o) {}
BinaryExp::~BinaryExp() { delete left; delete right; }

// ------------------ NumberExp ------------------
NumberExp::NumberExp(int v) : value(v) {}
NumberExp::~NumberExp() {}

// ------------------ IdExp ------------------
IdExp::IdExp(string v) : value(v) {}
IdExp::~IdExp() {}

// ------------------ BoolExp ------------------
BoolExp::BoolExp(bool v) : value(v) {}
BoolExp::~BoolExp() {}

// ------------------ Statement Base ------------------
Stm::~Stm() {}

// ------------------ Include ------------------
Include::Include(string h) : heade(h) {}
Include::~Include() {}

// ------------------ VarDec ------------------
VarDec::~VarDec() {}

// ------------------ InstanceDec ------------------
InstanceDec::~InstanceDec() {
    for (Exp* e : values) delete e;
}

// ------------------ AssignStm ------------------
AssignStm::AssignStm(string variable, Exp* expresion) : id(variable), e(expresion) {}
AssignStm::~AssignStm() { delete e; }

// ------------------ Body ------------------
Body::~Body() {
    for (VarDec* vd : declarations) delete vd;
    for (InstanceDec* id : intances) delete id;
    for (Stm* s : stmList) delete s;
}

// ------------------ Program ------------------
Program::~Program() {
    for (Include* inc : includes) delete inc;
    for (VarDec* vd : vdlist) delete vd;
    for (InstanceDec* id : intdlist) delete id;
    for (FunDec* fd : fdlist) delete fd;
}

// ------------------ ParamDec ------------------
ParamDec::ParamDec(string t, string i) : type(t), id(i) {}

// ------------------ FunDec ------------------
FunDec::FunDec(string rt, string n, vector<ParamDec*> p, Body* b) 
    : Type(rt), id(n), params(p), body(b) {}
FunDec::~FunDec() {
    for (ParamDec* p : params) delete p;
    delete body;
}

// ------------------ IfStm ------------------
IfStm::IfStm(Exp* c, Body* t, Body* e) 
    : condition(c), thenBody(t), elseBody(e) {}
IfStm::~IfStm() {
    delete condition;
    delete thenBody;
    delete elseBody;
}

// ------------------ WhileStm ------------------
WhileStm::WhileStm(Exp* c, Body* b) : condition(c), body(b) {}
WhileStm::~WhileStm() {
    delete condition;
    delete body;
}

// ------------------ ForStm ------------------
ForStm::ForStm(InstanceDec* i, Exp* c, string inc, Body* body)
    : init(i), condition(c), increment(inc), b(body) {}
ForStm::~ForStm() {
    delete init;
    delete condition;
    delete b;
}

// ------------------ PrintfStm ------------------
PrintfStm::PrintfStm(string fmt, list<Exp*> arguments) 
    : format(fmt), args(arguments) {}
PrintfStm::~PrintfStm() {
    for (Exp* e : args) delete e;
}

// ------------------ ReturnStm ------------------
ReturnStm::ReturnStm(Exp* exp) : e(exp) {}
ReturnStm::~ReturnStm() { if (e) delete e; }

// ------------------ FcallExp ------------------
FcallExp::FcallExp(string n, vector<Exp*> args) 
    : name(n), arguments(args) {}
FcallExp::~FcallExp() {
    for (Exp* e : arguments) delete e;
}


