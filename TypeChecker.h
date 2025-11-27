#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include <unordered_map>
#include <string>
#include "ast.h"
#include "environment.h"
#include "semantic_types.h"

using namespace std;

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
class TernaryExp;


class TypeVisitor {
public:

    // --- Nodos de nivel superior ---
    virtual void visit(Program* p) = 0;
    virtual void visit(Body* b) = 0;
    virtual void visit(Include* inc) = 0;
    virtual void visit(VarDec* v) = 0;
    virtual void visit(InstanceDec* ind) = 0;
    virtual void visit(FunDec* f) = 0;
    virtual void visit(ParamDec* p) = 0;

    // Structs y typedefs
    virtual void visit(StructDec* sd) = 0;
    virtual Type* visit(StructInit* s) = 0;
    virtual Type* visit(InitData* id) = 0;
    virtual void visit(TypedefDec* td) = 0;

    // --- Sentencias ---
    virtual void visit(PrintfStm* stm) = 0;
    virtual void visit(IfStm* stm) = 0;
    virtual void visit(WhileStm* stm) = 0;
    virtual void visit(ForStm* stm) = 0;
    virtual void visit(AssignStm* stm) = 0;
    virtual void visit(ReturnStm* stm) = 0;

    // --- Expresiones ---
    virtual Type* visit(BinaryExp* e) = 0;
    virtual Type* visit(NumberExp* e) = 0;
    virtual Type* visit(FloatExp* e) = 0;
    virtual Type* visit(IdExp* e) = 0;
    virtual Type* visit(FcallExp* e) = 0;
    virtual Type* visit(BoolExp* e) = 0;
    virtual Type* visit(StepExp* e) = 0;
    virtual Type* visit(TernaryExp* e) = 0;
};



// ──────────────────────────────────────────────
//   CLASE TYPECHECKER
// ──────────────────────────────────────────────

class TypeChecker : public TypeVisitor {
private:
    Environment<Type*> env;                 // Entorno de variables y sus tipos
    unordered_map<string, Type*> functions; // Entorno de funciones
    // Tipos básicos
    Type* intType;
    Type* unsignedType;
    Type* boolType;
    Type* voidType;
    Type* floatType;
    // Guarda la definición de interna de los structs. Ej: "Punto" -> { "x"->int, "y"->int }
    unordered_map<string, unordered_map<string, Type*>> structs;
    // Guarda el objeto tipo geneal del struct para búsquedas rápidas
    unordered_map<string, Type*> struct_types;
    // Typedef registro: alias -> Type*
    unordered_map<string, Type*> typedefs;
    // Field types in declaration order for each struct
    unordered_map<string, vector<Type*>> struct_field_order;
    // Registro de funciones
    void add_function(FunDec* fd);
public:
    TypeChecker();

    // Método principal de verificación
    void typecheck(Program* program);

    // --- Visitas de alto nivel ---
    void visit(Program* p) override;
    void visit(Body* b) override;
    void visit(Include* inc) override;
    void visit(VarDec* v) override;
    void visit(InstanceDec* ind) override;
    void visit(StructDec* sd) override;
    void visit(TypedefDec* td) override;
    void visit(FunDec* f) override;
    void visit(ParamDec* p) override;
    Type* visit(StructInit* s) override;
    Type* visit(InitData* id) override;

    // --- Sentencias ---
    void visit(PrintfStm* stm) override;
    void visit(IfStm* stm) override;
    void visit(WhileStm* stm) override;
    void visit(ForStm* stm) override;
    void visit(AssignStm* stm) override;
    void visit(ReturnStm* stm) override;

    // --- Expresiones ---
    Type* visit(BinaryExp* e) override;
    Type* visit(NumberExp* e) override;
    Type* visit(FloatExp* e) override;
    Type* visit(IdExp* e) override;
    Type* visit(FcallExp* e) override;
    Type* visit(BoolExp* e) override;
    Type* visit(StepExp* e) override;
    Type* visit(TernaryExp* e) override;
};

#endif // TYPECHECKER_H
