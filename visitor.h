#ifndef VISITOR_H
#define VISITOR_H
#include "ast.h"
#include "environment.h"
#include <list>
#include <vector>
#include <unordered_map>
#include <string>

using namespace std;

// Valor en tiempo de ejecución usado por EvalVisitor y Environment
struct Value {
    enum Kind { INT, UNSIGNED, BOOL, FLOAT, STRUCT } kind;
    int i;
    unsigned u;
    bool b;
    double f;  // Para valores float de 64 bits
    std::vector<Value> fields;
    std::string type_name; // nombre del tipo struct (si kind == STRUCT)
    Value(): kind(INT), i(0), u(0), b(false), f(0.0) {}
    static Value make_int(int v) { Value x; x.kind = INT; x.i = v; return x; }
    static Value make_unsigned(unsigned v) { Value x; x.kind = UNSIGNED; x.u = v; return x; }
    static Value make_bool(bool v) { Value x; x.kind = BOOL; x.b = v; return x; }
    static Value make_float(double v) { Value x; x.kind = FLOAT; x.f = v; return x; }
    static Value make_struct(const std::vector<Value>& f, const std::string& tname = std::string()) { Value x; x.kind = STRUCT; x.fields = f; x.type_name = tname; return x; }
};

class BinaryExp;
class NumberExp;
class FloatExp;
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
class StructDec;
class StructInit;
class InitData;
class TypedefDec;
class TernaryExp;

class Visitor {
public:
    // 
    virtual int visit(BinaryExp* exp) = 0;
    virtual int visit(NumberExp* exp) = 0;
    virtual int visit(FloatExp* exp) = 0;
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
    virtual int visit(StructDec* sd) = 0;
    virtual int visit(TypedefDec* td) = 0;
    virtual int visit(StructInit* si) = 0;
    virtual int visit(InitData* id) = 0;
    virtual int visit(TernaryExp* exp) = 0;

};

class PrintVisitor : public Visitor {
public:
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(FloatExp* exp) override;
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
    int visit(StructDec* sd) override;
    int visit(TypedefDec* td) override;
    int visit(StructInit* si) override;
    int visit(InitData* id) override;
    int visit(TernaryExp* exp) override;
    void imprimir(Program* program);
};

class EvalVisitor : public Visitor {
private:
    Environment<Value> env;
    unordered_map<string, FunDec*> envfun;
    int return_value; // Para manejar el valor de retorno de las funciones
    bool returning;   // Bandera para saber si se ha ejecutado un return
    // Último valor evaluado (útil para inicializadores de struct)
    Value last_value;
    bool last_value_valid = false;
    // Para capturar struct devueltos por funciones
    Value return_struct;
    bool return_struct_valid = false;
public:
    //EvalVisitor(Environment* environment) : env(environment), return_value(0), returning(false) {}
    //virtual ~EvalVisitor() {}
    void evaluar(Program* program);
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(FloatExp* exp) override;
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
    int visit(StructDec* sd) override;
    int visit(StructInit* si) override;
    int visit(InitData* id) override;
    int visit(TypedefDec* td) override;
    int visit(TernaryExp* exp) override;
};

// --- GENCODE VISITOR ---
class GenCodeVisitor : public Visitor {
private:
    std::ostream& out;
    
    // Gestión de Memoria
    unordered_map<string, int> memoria; // Offset base de variables
    unordered_map<string, string> varTypes; // Tipo de cada variable (para saber si es struct)
    
    // Gestión de Structs: NombreStruct -> (NombreCampo -> OffsetRelativo)
    unordered_map<string, unordered_map<string, int>> structLayouts;
    unordered_map<string, int> structSizes; // Tamaño total del struct

    int offset;
    std::string nombreFuncion;

    // NUEVOS CONTADORES ESPECÍFICOS
    int count_if = 0;
    int count_while = 0;
    int count_for = 0;
    int count_ternary = 0;
    
    // Helpers
    int getMemory(string name);

public:
    // Inicializamos los contadores en 0
    GenCodeVisitor(std::ostream& out) : out(out), offset(-8), count_if(0), count_while(0), count_for(0), count_ternary(0) {}
    virtual ~GenCodeVisitor() {}

    void generar(Program* program); 

    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(FloatExp* exp) override;
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
    int visit(ReturnStm* stm) override;
    int visit(FcallExp* exp) override;
    int visit(StepExp* step) override;
    
    // Implementados para Structs
    int visit(StructDec* sd) override;
    int visit(StructInit* si) override;
    int visit(InitData* id) override;
    int visit(TypedefDec* td) override;
    int visit(TernaryExp* exp) override;
};

#endif // VISITOR_H