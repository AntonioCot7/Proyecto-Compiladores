#include <iostream>
#include "ast.h"
#include "visitor.h"
#include <unordered_map>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////
//                    SECCIÓN 0: MÉTODOS DE IMPRESIÓN Y GENERACIÓN
///////////////////////////////////////////////////////////////////////////////////

void PrintVisitor::imprimir(Program* program) {
    if (program) {
        program->accept(this);
    }
}

// Por ahora solo un stub - se implementará después
void GenCodeVisitor::generar(Program* program) {
    if (program) {
        cerr << "GenCodeVisitor::generar - No implementado aún" << endl;
    }
}

// Implementaciones vacías de los métodos virtuales de GenCodeVisitor
int GenCodeVisitor::visit(BinaryExp* exp) { return 0; }
int GenCodeVisitor::visit(NumberExp* exp) { return 0; }
int GenCodeVisitor::visit(IdExp* exp) { return 0; }
int GenCodeVisitor::visit(BoolExp* exp) { return 0; }
int GenCodeVisitor::visit(Include* inc) { return 0; }
int GenCodeVisitor::visit(VarDec* vd) { return 0; }
int GenCodeVisitor::visit(InstanceDec* ind) { return 0; }
int GenCodeVisitor::visit(Body* body) { return 0; }
int GenCodeVisitor::visit(Program* p) { return 0; }
int GenCodeVisitor::visit(ParamDec* pd) { return 0; }
int GenCodeVisitor::visit(FunDec* fd) { return 0; }
int GenCodeVisitor::visit(AssignStm* stm) { return 0; }
int GenCodeVisitor::visit(PrintfStm* stm) { return 0; }
int GenCodeVisitor::visit(IfStm* stm) { return 0; }
int GenCodeVisitor::visit(WhileStm* stm) { return 0; }
int GenCodeVisitor::visit(ForStm* stm) { return 0; }
int GenCodeVisitor::visit(ReturnStm* r) { return 0; }
int GenCodeVisitor::visit(StepExp* step) { return 0; }
int GenCodeVisitor::visit(FcallExp* fcall) { return 0; }

///////////////////////////////////////////////////////////////////////////////////
//                    SECCIÓN 1: MÉTODOS accept() DEL AST
///////////////////////////////////////////////////////////////////////////////////

int BinaryExp::accept(Visitor* visitor)   { return visitor->visit(this); }

int NumberExp::accept(Visitor* visitor)   { return visitor->visit(this); }

int IdExp::accept(Visitor* visitor)       { return visitor->visit(this); }

int BoolExp::accept(Visitor* visitor)     { return visitor->visit(this); }

int Include::accept(Visitor* visitor)     { return visitor->visit(this); }

int VarDec::accept(Visitor* visitor)      { return visitor->visit(this); }

int InstanceDec::accept(Visitor* visitor) { return visitor->visit(this); }

int Body::accept(Visitor* visitor)        { return visitor->visit(this); }

int Program::accept(Visitor* visitor)     { return visitor->visit(this); }

int ParamDec::accept(Visitor* visitor)    { return visitor->visit(this); }

int FunDec::accept(Visitor* visitor)      { return visitor->visit(this); }

int AssignStm::accept(Visitor* visitor)   { return visitor->visit(this); }

int IfStm::accept(Visitor* visitor)       { return visitor->visit(this); }

int WhileStm::accept(Visitor* visitor)    { return visitor->visit(this); }

int ForStm::accept(Visitor* visitor)      { return visitor->visit(this); }

int PrintfStm::accept(Visitor* visitor)   { return visitor->visit(this); }

int ReturnStm::accept(Visitor* visitor)   { return visitor->visit(this); }

int FcallExp::accept(Visitor* visitor)    { return visitor->visit(this); }

int StepExp::accept(Visitor* visitor)     { return visitor->visit(this); }

///////////////////////////////////////////////////////////////////////////////////
//                    SECCIÓN 2: IMPLEMENTACIÓN DE PrintVisitor
///////////////////////////////////////////////////////////////////////////////////

int PrintVisitor::visit(BinaryExp* exp) {
    cout << "(";
    exp->left->accept(this);
    cout << " " << Exp::binopToChar(exp->op) << " ";
    exp->right->accept(this);
    cout << ")";
    return 0;
}

int PrintVisitor::visit(NumberExp* exp) {
    cout << exp->value;
    return 0;
}

int PrintVisitor::visit(IdExp* exp) {
    cout << exp->value;
    return 0;
}

int PrintVisitor::visit(BoolExp* exp) {
    cout << (exp->value ? "true" : "false");
    return 0;
}

int PrintVisitor::visit(Include* inc) {
    cout << "#include <" << inc->heade << ">" << endl;
    return 0;
}

int PrintVisitor::visit(VarDec* vd) {
    cout << vd->type << " ";
    bool first = true;
    for (const string& var : vd->vars) {
        if (!first) cout << ", ";
        cout << var;
        first = false;
    }
    cout << ";" << endl;
    return 0;
}

int PrintVisitor::visit(InstanceDec* ind) {
    cout << ind->type << " ";
    auto varIt = ind->vars.begin();
    auto valIt = ind->values.begin();
    bool first = true;
    for (; varIt != ind->vars.end() && valIt != ind->values.end(); ++varIt, ++valIt) {
        if (!first) cout << ", ";
        cout << *varIt << " = ";
        (*valIt)->accept(this);
        first = false;
    }
    cout << ";" << endl;
    return 0;
}

int PrintVisitor::visit(Body* body) {
    cout << "{" << endl;
    for (VarDec* vd : body->declarations) {
        vd->accept(this);
    }
    for (InstanceDec* ind : body->intances) {
        ind->accept(this);
    }
    for (Stm* stm : body->stmList) {
        stm->accept(this);
    }
    cout << "}" << endl;
    return 0;
}

int PrintVisitor::visit(Program* p) {
    for (Include* inc : p->includes) {
        inc->accept(this);
    }
    cout << endl;
    for (VarDec* vd : p->vdlist) {
        vd->accept(this);
    }
    for (InstanceDec* ind : p->intdlist) {
        ind->accept(this);
    }
    for (FunDec* fd : p->fdlist) {
        fd->accept(this);
    }
    return 0;
}

int PrintVisitor::visit(ParamDec* pd) {
    cout << pd->type << " " << pd->id;
    return 0;
}

int PrintVisitor::visit(FunDec* fd) {
    cout << fd->Type << " " << fd->id << "(";
    for (size_t i = 0; i < fd->params.size(); ++i) {
        fd->params[i]->accept(this);
        if (i < fd->params.size() - 1) cout << ", ";
    }
    cout << ") ";
    fd->body->accept(this);
    cout << endl;
    return 0;
}

int PrintVisitor::visit(AssignStm* stm) {
    cout << stm->id << " = ";
    stm->e->accept(this);
    cout << ";" << endl;
    return 0;
}

int PrintVisitor::visit(IfStm* stm) {
    cout << "if (";
    stm->condition->accept(this);
    cout << ") ";
    stm->thenBody->accept(this);
    if (stm->elseBody) {
        cout << "else ";
        stm->elseBody->accept(this);
    }
    cout << endl;
    return 0;
}

int PrintVisitor::visit(WhileStm* stm) {
    cout << "while (";
    stm->condition->accept(this);
    cout << ") ";
    stm->body->accept(this);
    cout << endl;
    return 0;
}

int PrintVisitor::visit(StepExp* step) {
    step->variable->accept(this);
    switch(step->type) {
        case StepExp::INCREMENT:
            cout << "++";
            break;
        case StepExp::DECREMENT:
            cout << "--";
            break;
        case StepExp::COMPOUND:
            cout << " += ";
            step->amount->accept(this);
            break;
    }
    return 0;
}

int PrintVisitor::visit(ForStm* stm) {
    cout << "for (";
    stm->init->accept(this);
    cout << " ";
    stm->condition->accept(this);
    cout << "; ";
    stm->step->accept(this);
    cout << ") ";
    stm->body->accept(this);
    cout << endl;
    return 0;
}

/*
// Representa una llamada a printf
// Ejemplo: printf("Value: %d", x);
class PrintfStm: public Stm {
public:
    string format;        // String de formato
    list<Exp*> args;     // Argumentos
    int accept(Visitor* visitor) override;
    PrintfStm(string fmt, list<Exp*> arguments);
    ~PrintfS
*/

int PrintVisitor::visit(PrintfStm* stm) {
    cout << "printf(\"" << stm->format << "\"";
    for (Exp* arg : stm->args) {
        cout << ", ";
        arg->accept(this);
    }
    cout << ");" << endl;
    return 0;
}

int PrintVisitor::visit(ReturnStm* r) {
    cout << "return ";
    r->e->accept(this);
    cout << ";" << endl;
    return 0;
}

int PrintVisitor::visit(FcallExp* fcall) {
    cout << fcall->name << "(";
    for (size_t i = 0; i < fcall->arguments.size(); ++i) {
        fcall->arguments[i]->accept(this);
        if (i < fcall->arguments.size() - 1) cout << ", ";
    }
    cout << ")";
    return 0;
}


///////////////////////////////////////////////////////////////////////////////////
/*
int GenCodeVisitor::generar(Program* program) {
    program->accept(this);
        return 0;
}

int GenCodeVisitor::visit(Program* program) {
out << ".data\nprint_fmt: .string \"%ld \\n\""<<endl;

    for (auto dec : program->vdlist){
        dec->accept(this);
    }

    for (auto& [var, _] : memoriaGlobal) {
        out << var << ": .quad 0"<<endl;
    }

    out << ".text\n";
    
    for (auto dec : program->fdlist){
        dec->accept(this);
    }

    out << ".section .note.GNU-stack,\"\",@progbits"<<endl;
        return 0;
}

int GenCodeVisitor::visit(VarDec* stm) {
    for (auto var : stm->vars) {
        if (!entornoFuncion) {
            memoriaGlobal[var] = true;
        } else {
            memoria[var] = offset;
            offset -= 8;
        }
    }
        return 0;
}



int GenCodeVisitor::visit(NumberExp* exp) {
    out << " movq $" << exp->value << ", %rax"<<endl;
    return 0;
}

int GenCodeVisitor::visit(IdExp* exp) {
    if (memoriaGlobal.count(exp->value))
        out << " movq " << exp->value << "(%rip), %rax"<<endl;
    else
        out << " movq " << memoria[exp->value] << "(%rbp), %rax"<<endl;
    return 0;
}

int GenCodeVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    out << " pushq %rax\n";
    exp->right->accept(this);
    out << " movq %rax, %rcx\n popq %rax\n";

    switch (exp->op) {
        case PLUS_OP:  out << " addq %rcx, %rax\n"; break;
        case MINUS_OP: out << " subq %rcx, %rax\n"; break;
        case MUL_OP:   out << " imulq %rcx, %rax\n"; break;
        case LE_OP:
            out << " cmpq %rcx, %rax\n"
                    << " movl $0, %eax\n"
                    << " setle %al\n"
                    << " movzbq %al, %rax\n";
            break;
    }
    return 0;
}


int GenCodeVisitor::visit(AssignStm* stm) {
    stm->e->accept(this);
    if (memoriaGlobal.count(stm->id))
        out << " movq %rax, " << stm->id << "(%rip)"<<endl;
    else
        out << " movq %rax, " << memoria[stm->id] << "(%rbp)"<<endl;
            return 0;
}

int GenCodeVisitor::visit(PrintStm* stm) {
    stm->e->accept(this);
    out <<
        " movq %rax, %rsi\n"
        " leaq print_fmt(%rip), %rdi\n"
        " movl $0, %eax\n"
        " call printf@PLT\n";
            return 0;
}



int GenCodeVisitor::visit(Body* b) {
    for (auto dec : b->declarations){
        dec->accept(this);
    }
    for (auto s : b->StmList){
        s->accept(this);
    }
        return 0;
}

int GenCodeVisitor::visit(IfStm* stm) {
    int label = labelcont++;
    stm->condition->accept(this);
    out << " cmpq $0, %rax"<<endl;
    out << " je else_" << label << endl;
    stm->then->accept(this);
    out << " jmp endif_" << label << endl;
    out << " else_" << label << ":"<< endl;
    if (stm->els) stm->els->accept(this);
    out << "endif_" << label << ":"<< endl;
    return 0;
}

int GenCodeVisitor::visit(WhileStm* stm) {
    int label = labelcont++;
    out << "while_" << label << ":"<<endl;
    stm->condition->accept(this);
    out << " cmpq $0, %rax" << endl;
    out << " je endwhile_" << label << endl;
    stm->b->accept(this);
    out << " jmp while_" << label << endl;
    out << "endwhile_" << label << ":"<< endl;
    return 0;
}


int GenCodeVisitor::visit(ReturnStm* stm) {
    stm->e->accept(this);
    out << " jmp .end_"<<nombreFuncion << endl;
    return 0;
}

int GenCodeVisitor::visit(FunDec* f) {
    entornoFuncion = true;
    memoria.clear();
    offset = -8;
    nombreFuncion = f->nombre;
    vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    out << ".globl " << f->nombre << endl;
    out << f->nombre <<  ":" << endl;
    out << " pushq %rbp" << endl;
    out << " movq %rsp, %rbp" << endl;
    int size = f->Pnombres.size();
    for (int i = 0; i < size; i++) {
        memoria[f->Pnombres[i]]=offset;
        out << " movq " << argRegs[i] << "," << offset << "(%rbp)" << endl;
        offset -= 8;
    }
    for (auto i: f->cuerpo->declarations){
        i->accept(this);
    }
    int reserva = -offset - 8;
    out << " subq $" << reserva << ", %rsp" << endl;
    for (auto i: f->cuerpo->StmList){
        i->accept(this);
    }
    out << ".end_"<< f->nombre << ":"<< endl;
    out << "leave" << endl;
    out << "ret" << endl;
    entornoFuncion = false;
    return 0;
}

int GenCodeVisitor::visit(FcallExp* exp) {
    vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    int size = exp->argumentos.size();
    for (int i = 0; i < size; i++) {
        exp->argumentos[i]->accept(this);
        out << " mov %rax, " << argRegs[i] <<endl;
    }
    out << "call " << exp->nombre << endl;
    return 0;
}
*/