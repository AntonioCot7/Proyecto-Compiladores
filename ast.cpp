#include "ast.h"
#include <iostream>

using namespace std;

// ------------------ Program ------------------
Program::Program() {}

// ------------------ VarDec ------------------
VarDec::VarDec() {}

// ------------------ InstanceDec ------------------
InstanceDec::InstanceDec() {}

// ------------------ Body ------------------
Body::Body() {}

// ------------------ ParamDec ------------------
ParamDec::~ParamDec() {}

// ------------------ FunDec ------------------
FunDec::FunDec() {}

// ------------------ FcallExp ------------------
FcallExp::FcallExp() {}

// ------------------ Exp ------------------
Exp::~Exp() {}

string Exp::binopToChar(BinaryOp op) {
    switch (op) {
        case PLUS_OP:         return "+";
        case MINUS_OP:        return "-";
        case MUL_OP:          return "*";
        case DIV_OP:          return "/";
        case PLUS_ASSIGN_OP:  return "+=";
        case MINUS_ASSIGN_OP: return "-=";
        case GT_OP:           return ">";
        case LT_OP:           return "<";
        case GE_OP:           return ">=";
        case LE_OP:           return "<=";
        case EQ_OP:           return "==";
        case NE_OP:           return "!=";
        case ASSIGN_OP:       return "=";
        default:              return "?";
    }
}

// ------------------ BinaryExp ------------------
// Configuracion de BinaryExp (El cálculo del peso/etiqueta)
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp o) : left(l), right(r), op(o) {
    // ----- OPTIMIZACION: Lógica Sethi-Ullman -----
    // Si el hijo izquierdo es una hoja, le damos peso 1 temporalmente para la lógica
    if (left->hoja == 1) {
        left->et == 1;
    }
    // Lógica de Sethi-Ulman simplificada
    if (l->et == r->et) {
        et = l->et + 1;
    }
    else {
        et = max(l->et, r->et);
    }
    hoja = 0; // No es hoja

    // ----- OPTIMIZACION: Lógica de Constantes (Plegado) -----

    if (left->cont == 1 && right->cont == 1) {
        cont = 1;
        switch (op) {
            case PLUS_OP:  valor = left->valor + right->valor; break;
            case MINUS_OP: valor = left->valor - right->valor; break;
            case MUL_OP:   valor = left->valor * right->valor; break;
            case DIV_OP:   
                if(right->valor != 0) valor = left->valor / right->valor; 
                else { cont = 0; } // Evitar crash en compilación por div/0
                break;
            // Relacionales
            case LT_OP: valor = (left->valor < right->valor); break;
            case GT_OP: valor = (left->valor > right->valor); break;
            case LE_OP: valor = (left->valor <= right->valor); break;
            case GE_OP: valor = (left->valor >= right->valor); break;
            case EQ_OP: valor = (left->valor == right->valor); break;
            case NE_OP: valor = (left->valor != right->valor); break;
            default: cont = 0; break;
        }
    } else {
        cont = 0;
        valor = 0;
    }
}
BinaryExp::~BinaryExp() { delete left; delete right; }

// ------------------ NumberExp ------------------
NumberExp::NumberExp(int v) : value(v) {
    et = 0; hoja = 1;
    cont = 1; valor = v; // Es constante
}
NumberExp::~NumberExp() {
    et = 0; 
    hoja = 1;
}

// ------------------ FloatExp ------------------
FloatExp::FloatExp(double v) : value(v) {
    et = 0; hoja = 1;
    cont = 1; valor = (int)v; // Es constante (almacenamos parte entera en valor)
}
FloatExp::~FloatExp() {
    et = 0; 
    hoja = 1;
}

// ------------------ FcallExp ------------------
FcallExp::FcallExp(string n, vector<Exp*> args) 
    : name(n), arguments(args) {
        et = 0; hoja = 1;
        cont = 0; valor = 0; // Resultado de función desconocido
    }
FcallExp::~FcallExp() {
    for (Exp* e : arguments) delete e;
}

// ------------------ IdExp ------------------
IdExp::IdExp(string v) : value(v) {
    et = 0; hoja = 1;
    cont = 0; valor = 0; // Valor desconocido en compilación
}
IdExp::~IdExp() {}

// ------------------ BoolExp ------------------
BoolExp::BoolExp(bool v) : value(v) {
    et = 0; hoja = 1;
    cont = 1; valor = (v ? 1 : 0); // Es constante
}
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
    for (InitData* e : values) delete e;
}

// ------------------ AssignStm ------------------
AssignStm::AssignStm(string variable, Exp* expresion) : id(variable), e(expresion) {}
AssignStm::~AssignStm() { delete e; }

// ------------------ Body ------------------
Body::~Body() {
    for (VarDec* vd : declarations) delete vd;
    for (InstanceDec* id : intances) delete id;
    for (TypedefDec* td : tdlist) delete td;
    for (Stm* s : stmList) delete s;
}

// ------------------ Program ------------------
Program::~Program() {
    for (Include* inc : includes) delete inc;
    for (VarDec* vd : vdlist) delete vd;
    for (InstanceDec* id : intdlist) delete id;
    for (TypedefDec* td : tdlist) delete td;
    for (FunDec* fd : fdlist) delete fd;
}

// ------------------ ParamDec ------------------
ParamDec::ParamDec(string t, string i) : type(t), id(i) {}

// ------------------ FunDec ------------------
FunDec::FunDec(string rt, string n, vector<ParamDec*> p, Body* b) 
    : type(rt), id(n), params(p), body(b) {}
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

// ------------------ StepExp ------------------
StepExp::StepExp(Exp* var, StepType t, Exp* amt) 
    : variable(var), type(t), amount(amt) {}

StepExp::~StepExp() {
    delete variable;
    if (amount) delete amount;
}

// ------------------ ForStm ------------------
ForStm::ForStm(Stm* i, Exp* c, StepExp* s, Body* b)
    : init(i), condition(c), step(s), body(b) {}

ForStm::~ForStm() {
    if (init) delete init;
    delete condition;
    delete step;
    delete body;
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

// ------------------ Struct ------------------
StructInit::StructInit() {}
StructInit::~StructInit() {}

// ------------------ InitData ------------------
InitData::InitData() {}
InitData::~InitData() {}

// ------------------ StructDec ------------------
StructDec::StructDec() {}
StructDec::~StructDec() {}

// ------------------ TypedefDec ------------------
TypedefDec::TypedefDec() {}
TypedefDec::TypedefDec(string t, string a) : typeName(t), alias(a) {}
TypedefDec::~TypedefDec() {}

// ------------------ TernaryExp ------------------
TernaryExp::TernaryExp() {}
TernaryExp::~TernaryExp() {}
