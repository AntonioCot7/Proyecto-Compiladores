#ifndef VISITOR_H
#define VISITOR_H
#include "ast.h"
#include "environment.h"
#include <list>
#include <vector>
#include <unordered_map>
#include <string>
using namespace std;

class BinaryExp;
class NumberExp;
class IdExp;
class BoolExp;
class Include;
class VarDec;
class InstanceDec;
class Body;
class Program;
class ParamDec;
class FunDec;
class AssignStm;
class IfStm;
class WhileStm;
class ForStm;
class PrintfStm;
class ReturnStm;
class FcallExp;
class StepExp;


class Visitor {
public:
    // 
    virtual int visit(BinaryExp* exp) = 0;
    virtual int visit(NumberExp* exp) = 0;
    virtual int visit(IdExp* exp) = 0;
    virtual int visit(BoolExp* exp) = 0;
    virtual int visit(Include* inc) = 0;
    virtual int visit(VarDec* vd) = 0;
    virtual int visit(InstanceDec* ind) = 0;
    virtual int visit(Body* body) = 0;
    virtual int visit(Program* p) = 0;
    virtual int visit(ParamDec* pd) = 0;
    virtual int visit(FunDec* fd) = 0;
    virtual int visit(AssignStm* stm) = 0;
    virtual int visit(IfStm* stm) = 0;
    virtual int visit(WhileStm* stm) = 0;
    virtual int visit(ForStm* stm) = 0;
    virtual int visit(PrintfStm* stm) = 0;
    virtual int visit(ReturnStm* r) = 0;
    virtual int visit(FcallExp* fcall) = 0;
    virtual int visit(StepExp* step) = 0;

};

class PrintVisitor : public Visitor {
public:
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(IdExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(Include* inc) override;
    int visit(VarDec* vd) override;
    int visit(InstanceDec* ind) override;
    int visit(Body* body) override;
    int visit(Program* p) override;
    int visit(ParamDec* pd) override;
    int visit(FunDec* fd) override;
    int visit(AssignStm* stm) override;
    int visit(IfStm* stm) override;
    int visit(WhileStm* stm) override;
    int visit(ForStm* stm) override;
    int visit(PrintfStm* stm) override;
    int visit(ReturnStm* r) override;
    int visit(FcallExp* fcall) override;
    int visit(StepExp* step) override;
    void imprimir(Program* program);
};

class EvalVisitor : public Visitor {
private:

    // El ambiente para almacenar variables y manejar el scope
    Environment* env;
    unordered_map<string, FunDec*> envfun;
    int return_value; // Para manejar el valor de retorno de las funciones
    bool returning;   // Bandera para saber si se ha ejecutado un return
public:
    //EvalVisitor(Environment* environment) : env(environment), return_value(0), returning(false) {}
    //virtual ~EvalVisitor() {}
    void evaluar(Program* program);
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(IdExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(FcallExp* fcall) override;
    int visit(Include* inc) override;
    int visit(VarDec* vd) override;
    int visit(InstanceDec* ind) override;
    int visit(ParamDec* pd) override;
    int visit(FunDec* fd) override;
    int visit(Body* body) override;
    int visit(AssignStm* stm) override;
    int visit(IfStm* stm) override;
    int visit(WhileStm* stm) override;
    int visit(ForStm* stm) override;
    int visit(PrintfStm* stm) override;
    int visit(ReturnStm* r) override;
    int visit(Program* p) override;
    int visit(StepExp* step) override; 
};

class GenCodeVisitor : public Visitor {
private:
    std::ostream& out;
public:
    GenCodeVisitor(std::ostream& out) : out(out) {}
    virtual ~GenCodeVisitor() {}
    void generar(Program* program);

    // Declaraciones de los métodos virtuales (serán implementados luego)
    unordered_map<string, int> memoria;
    unordered_map<string, bool> memoriaGlobal;
    int offset = -8;
    int labelcont = 0;
    bool entornoFuncion = false;
    string nombreFuncion;
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(IdExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(Include* inc) override;
    int visit(VarDec* vd) override;
    int visit(InstanceDec* ind) override;
    int visit(Body* body) override;
    int visit(Program* p) override;
    int visit(ParamDec* pd) override;
    int visit(FunDec* fd) override;
    int visit(AssignStm* stm) override;
    int visit(PrintfStm* stm) override;
    int visit(IfStm* stm) override;
    int visit(WhileStm* stm) override;
    int visit(ForStm* stm) override;
    int visit(ReturnStm* r) override;
    int visit(StepExp* step) override;
    int visit(FcallExp* fcall) override;
};

#endif // VISITOR_H