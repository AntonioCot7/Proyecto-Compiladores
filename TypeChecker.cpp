#include "TypeChecker.h"
#include <iostream>
#include <sstream>
#include <vector>
#include "struct_registry.h"

using namespace std;

// ===========================================================
//   Helpers (Funciones libres, no métodos de clase)
// ===========================================================

// Función para dividir strings "p.x.y" -> ["p", "x", "y"]
static vector<string> split(const string &s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// ===========================================================
//   Accepts (Boilerplate)
// ===========================================================
Type* NumberExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* FloatExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* IdExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* BinaryExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* FcallExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* BoolExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* StepExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* StructInit::accept(TypeVisitor* v) { return v->visit(this); }
Type* InitData::accept(TypeVisitor* v) { return v->visit(this); }
Type* TernaryExp::accept(TypeVisitor* v) { return v->visit(this); }

// Void accepts
void StructDec::accept(TypeVisitor* v) { v->visit(this); }
void TypedefDec::accept(TypeVisitor* v) { v->visit(this); }
void AssignStm::accept(TypeVisitor* v) { v->visit(this); }
void IfStm::accept(TypeVisitor* v) { v->visit(this); }
void WhileStm::accept(TypeVisitor* v) { v->visit(this); }
void ForStm::accept(TypeVisitor* v) { v->visit(this); }
void PrintfStm::accept(TypeVisitor* v) { v->visit(this); }
void ReturnStm::accept(TypeVisitor* v) { v->visit(this); }
void Include::accept(TypeVisitor* v) { v->visit(this); }
void InstanceDec::accept(TypeVisitor* v) { v->visit(this); }
void ParamDec::accept(TypeVisitor* v) { v->visit(this); }
void VarDec::accept(TypeVisitor* v) { v->visit(this); }
void FunDec::accept(TypeVisitor* v) { v->visit(this); }
void Body::accept(TypeVisitor* v) { v->visit(this); }
void Program::accept(TypeVisitor* v) { v->visit(this); }

// ===========================================================
//   Constructor
// ===========================================================
TypeChecker::TypeChecker() {
    intType = new Type(Type::INT);
    boolType = new Type(Type::BOOL);
    voidType = new Type(Type::VOID);
    unsignedType = new Type(Type::UNSIGNED);
    floatType = new Type(Type::FLOAT);
}

// ===========================================================
//   Método Helper Privado (Declarado en tu .h)
// ===========================================================
void TypeChecker::add_function(FunDec* fd) {
    if (functions.find(fd->id) != functions.end()) {
        cerr << "Error: funcion '" << fd->id << "' redefinida." << endl;
        exit(1);
    }
    Type* retType = new Type();
    // Intenta resolver tipo básico, o busca en structs/typedefs
    if (!retType->set_basic_type(fd->type)) {
        if (struct_types.find(fd->type) != struct_types.end()) retType = struct_types[fd->type];
        else if (typedefs.find(fd->type) != typedefs.end()) retType = typedefs[fd->type];
        else {
            cerr << "Error: tipo de retorno desconocido '" << fd->type << "' en funcion " << fd->id << endl;
            exit(1);
        }
    }
    functions[fd->id] = retType;
}

// ===========================================================
//   Visitas de Estructura (Program, Body, Decs)
// ===========================================================

void TypeChecker::typecheck(Program* program) {
    if (program) program->accept(this);
    cout << "TypeChecker: Revision exitosa" << endl;
}

void TypeChecker::visit(Program* p) {
    // 1. Registrar Typedefs PRIMERO (para que los structs puedan usar alias como 'money')
    for (auto td : p->tdlist) td->accept(this);

    // 2. Registrar Structs DESPUÉS
    for (auto sd : p->strlist) sd->accept(this);

    // 3. Registrar Funciones
    for (auto f : p->fdlist) add_function(f);

    env.add_level();
    // 4. Variables globales
    for (auto v : p->vdlist) v->accept(this);
    for (auto ind : p->intdlist) ind->accept(this);
    
    // 5. Cuerpos de funciones
    for (auto f : p->fdlist) f->accept(this);
    
    env.remove_level();
}

void TypeChecker::visit(Body* b) {
    env.add_level();
    // Importante: procesar typedefs locales primero
    for (auto td : b->tdlist) td->accept(this);
    
    for (auto v : b->declarations) v->accept(this);
    for (auto ind : b->intances) ind->accept(this);
    for (auto s : b->stmList) s->accept(this);
    env.remove_level();
}

void TypeChecker::visit(Include* inc) { /* Nada que chequear */ }

// --- Declaraciones de Tipos ---

void TypeChecker::visit(StructDec* sd) {
    if (structs.find(sd->nombre) != structs.end()) {
        cerr << "Error: struct '" << sd->nombre << "' ya existe." << endl;
        exit(1);
    }

    unordered_map<string, Type*> fields;
    vector<Type*> order;

    for (VarDec* vd : sd->VdList) {
        Type* baseType = new Type();
        if (!baseType->set_basic_type(vd->type)) {
            if (struct_types.count(vd->type)) baseType = struct_types[vd->type];
            else if (typedefs.count(vd->type)) baseType = typedefs[vd->type];
            else { cerr << "Error: tipo desconocido '" << vd->type << "' en struct " << sd->nombre << endl; exit(1); }
        }

        for (string fieldName : vd->vars) {
            if (fields.count(fieldName)) { cerr << "Error: campo duplicado '" << fieldName << "'" << endl; exit(1); }
            fields[fieldName] = baseType;
            order.push_back(baseType);
            // collect names in order for runtime registry
            // we'll register names after building all fields
        }
    }

    structs[sd->nombre] = fields;
    struct_field_order[sd->nombre] = order;
    // Registrar nombres de campos en orden en el registry global
    {
        vector<string> names;
        for (VarDec* vd : sd->VdList) {
            for (string fieldName : vd->vars) names.push_back(fieldName);
        }
        // registrar en el registry
        StructRegistry::register_struct(sd->nombre, names);
    }
    
    // Registrar el tipo struct para lookup
    Type* t = new Type(Type::STRUCT);
    t->struct_name = sd->nombre;
    struct_types[sd->nombre] = t;
}

void TypeChecker::visit(TypedefDec* td) {
    if (typedefs.count(td->alias)) { cerr << "Error: typedef '" << td->alias << "' redefinido." << endl; exit(1); }
    
    Type* t = new Type();
    if (!t->set_basic_type(td->typeName)) {
        if (struct_types.count(td->typeName)) t = struct_types[td->typeName];
        else if (typedefs.count(td->typeName)) t = typedefs[td->typeName];
        else { cerr << "Error: typedef source '" << td->typeName << "' desconocido." << endl; exit(1); }
    }
    typedefs[td->alias] = t;
}

// --- Declaraciones de Variables ---

void TypeChecker::visit(VarDec* v) {
    Type* t = new Type();
    if (!t->set_basic_type(v->type)) {
        if (struct_types.count(v->type)) t = struct_types[v->type];
        else if (typedefs.count(v->type)) t = typedefs[v->type];
        else { cerr << "Error: tipo desconocido '" << v->type << "'" << endl; exit(1); }
    }

    for (string var : v->vars) {
        if (env.check(var)) { cerr << "Error: variable '" << var << "' redefinida." << endl; exit(1); }
        env.add_var(var, t);
    }
}

void TypeChecker::visit(InstanceDec* ind) {
    Type* declType = new Type();
    if (!declType->set_basic_type(ind->type)) {
        if (struct_types.count(ind->type)) declType = struct_types[ind->type];
        else if (typedefs.count(ind->type)) declType = typedefs[ind->type];
        else { cerr << "Error: tipo desconocido '" << ind->type << "'" << endl; exit(1); }
    }

    // Resolver alias si es necesario para saber si es STRUCT
    Type* actualType = declType;
    while (actualType->ttype != Type::STRUCT && typedefs.count(actualType->struct_name)) {
        // Nota: esto asume que guardas el nombre del typedef en struct_name o similar para tipos no basicos
        // Si tu implementación de Type es simple, este while puede ser complejo. 
        // Por ahora confiamos en la resolución inicial.
        break; 
    }

    auto itVar = ind->vars.begin();
    auto itVal = ind->values.begin();

    while(itVar != ind->vars.end()) {
        string name = *itVar;
        InitData* init = *itVal;

        if (env.check(name)) { cerr << "Error: variable '" << name << "' redefinida." << endl; exit(1); }

        if (declType->ttype == Type::STRUCT) {
            string sname = declType->struct_name;
            if (init->st) {
                // Inicialización {a, b}
                vector<Type*>& fields = struct_field_order[sname];
                if (fields.size() != init->st->argumentos.size()) {
                    cerr << "Error: numero de campos incorrecto en inicializacion de struct " << sname << endl; exit(1);
                }
                for(size_t i=0; i<fields.size(); i++) {
                    Type* argType = init->st->argumentos[i]->accept(this);
                    if (!fields[i]->match(argType)) { cerr << "Error: tipo incorrecto en campo struct." << endl; exit(1); }
                }
            } else if (init->e) {
                // Inicialización por copia
                Type* rval = init->e->accept(this);
                if (!declType->match(rval)) { cerr << "Error: tipos incompatibles en asignacion struct." << endl; exit(1); }
            } else {
                cerr << "Error: inicializador invalido para struct." << endl; exit(1);
            }
        } else {
            // Tipo básico
            if (init->st) { cerr << "Error: no se puede inicializar primitivo con {}." << endl; exit(1); }
            Type* rval = init->e->accept(this);
            if (!declType->match(rval)) {
                // Permitir int->unsigned
                if (!(declType->match(unsignedType) && rval->match(intType))) {
                    cerr << "Error: tipos incompatibles en " << name << endl; exit(1);
                }
            }
        }
        env.add_var(name, declType);
        itVar++; itVal++;
    }
}

void TypeChecker::visit(FunDec* f) {
    env.add_level();
    for(auto p : f->params) p->accept(this);
    f->body->accept(this);
    env.remove_level();
}

void TypeChecker::visit(ParamDec* p) {
    Type* t = new Type();
    if (!t->set_basic_type(p->type)) {
        if (struct_types.count(p->type)) t = struct_types[p->type];
        else if (typedefs.count(p->type)) t = typedefs[p->type];
        else { cerr << "Error param type: " << p->type << endl; exit(1); }
    }
    env.add_var(p->id, t);
}

// ===========================================================
//   Sentencias
// ===========================================================

void TypeChecker::visit(AssignStm* stm) {
    // RESOLUCIÓN MANUAL DE LVALUE (p.x)
    vector<string> parts = split(stm->id, '.');
    if (parts.empty()) return;

    if (!env.check(parts[0])) { cerr << "Error: var '" << parts[0] << "' no declarada." << endl; exit(1); }
    Type* currentType = env.lookup(parts[0]);

    for (size_t i = 1; i < parts.size(); ++i) {
        // Desempaquetar typedefs si es necesario
        // (Dependiendo de tu implementación de Type, verifica si necesitas 'bajar' a través de alias)
        
        if (currentType->ttype != Type::STRUCT) {
            cerr << "Error: '" << parts[i-1] << "' no es un struct, no se puede acceder a ." << parts[i] << endl;
            exit(1);
        }
        string sname = currentType->struct_name;
        if (structs.find(sname) == structs.end()) { cerr << "Error interno: struct def no encontrada." << endl; exit(1); }
        
        if (structs[sname].find(parts[i]) == structs[sname].end()) {
            cerr << "Error: campo '" << parts[i] << "' no existe en struct '" << sname << "'" << endl; exit(1);
        }
        currentType = structs[sname][parts[i]];
    }
    
    Type* lvalueType = currentType;
    Type* rvalueType = stm->e->accept(this);

    if (!lvalueType->match(rvalueType)) {
        if (lvalueType->match(unsignedType) && rvalueType->match(intType)) return;
        cerr << "Error: asignacion incompatible a '" << stm->id << "'" << endl;
        exit(1);
    }
}

void TypeChecker::visit(IfStm* stm) {
    Type* condType = stm->condition->accept(this);
    // PERMITIR: Bool, Int o Unsigned (Lógica estilo C)
    if (!condType->match(boolType) && !condType->match(intType) && !condType->match(unsignedType)) { 
        cerr << "Error: If requiere una condición lógica o numérica." << endl; 
        exit(1); 
    }
    stm->thenBody->accept(this);
    if (stm->elseBody) stm->elseBody->accept(this);
}

void TypeChecker::visit(WhileStm* stm) {
    Type* condType = stm->condition->accept(this);
    // PERMITIR: Bool, Int o Unsigned
    if (!condType->match(boolType) && !condType->match(intType) && !condType->match(unsignedType)) { 
        cerr << "Error: While requiere una condición lógica o numérica." << endl; 
        exit(1); 
    }
    stm->body->accept(this);
}

void TypeChecker::visit(ForStm* stm) {
    env.add_level();
    if (stm->init) stm->init->accept(this);
    
    Type* condType = stm->condition->accept(this);
    // PERMITIR: Bool, Int o Unsigned
    if (!condType->match(boolType) && !condType->match(intType) && !condType->match(unsignedType)) { 
        cerr << "Error: For condition requiere una condición lógica o numérica." << endl; 
        exit(1); 
    }
    
    stm->body->accept(this);
    if (stm->step) stm->step->accept(this);
    env.remove_level();
}

void TypeChecker::visit(ReturnStm* stm) {
    if (stm->e) stm->e->accept(this);
}

void TypeChecker::visit(PrintfStm* stm) {
    for (auto arg : stm->args) {
        Type* t = arg->accept(this);
        if (!(t->match(intType) || t->match(boolType) || t->match(unsignedType) || t->match(floatType))) {
            cerr << "Error: printf solo soporta tipos primitivos." << endl; exit(1);
        }
    }
}

// ===========================================================
//   Expresiones
// ===========================================================

Type* TypeChecker::visit(IdExp* e) {
    // RESOLUCIÓN MANUAL DE RVALUE (p.x)
    vector<string> parts = split(e->value, '.');
    if (parts.empty()) return voidType;

    if (!env.check(parts[0])) { cerr << "Error: var '" << parts[0] << "' no declarada." << endl; exit(1); }
    Type* currentType = env.lookup(parts[0]);

    for (size_t i = 1; i < parts.size(); ++i) {
        if (currentType->ttype != Type::STRUCT) {
            cerr << "Error: '" << parts[i-1] << "' no es struct." << endl; exit(1);
        }
        string sname = currentType->struct_name;
        if (structs[sname].find(parts[i]) == structs[sname].end()) {
            cerr << "Error: campo '" << parts[i] << "' no existe en struct." << endl; exit(1);
        }
        currentType = structs[sname][parts[i]];
    }
    return currentType;
}

Type* TypeChecker::visit(BinaryExp* e) {
    Type* l = e->left->accept(this);
    Type* r = e->right->accept(this);

    if (e->op == EQ_OP || e->op == NE_OP) {
        if (!l->match(r)) { 
             // Permitir int vs unsigned
             if ((l->match(intType) && r->match(unsignedType)) || (l->match(unsignedType) && r->match(intType))) return boolType;
             cerr << "Error: comparacion tipos distintos." << endl; exit(1); 
        }
        return boolType;
    }
    // Relacionales y Aritméticas
    bool isInt = l->match(intType) && r->match(intType);
    bool isUint = l->match(unsignedType) && r->match(unsignedType);
    // Mix int/uint
    bool isMix = (l->match(intType) && r->match(unsignedType)) || (l->match(unsignedType) && r->match(intType));

    if (!isInt && !isUint && !isMix) {
        cerr << "Error: operacion binaria requiere int o unsigned." << endl; exit(1);
    }

    if (e->op == LT_OP || e->op == LE_OP || e->op == GT_OP || e->op == GE_OP) return boolType;
    
    // Aritmetica
    if (isUint || isMix) return unsignedType; 
    return intType;
}

Type* TypeChecker::visit(NumberExp* e) { return intType; }
Type* TypeChecker::visit(FloatExp* e) { return floatType; }
Type* TypeChecker::visit(BoolExp* e) { return boolType; }

Type* TypeChecker::visit(FcallExp* e) {
    if (functions.find(e->name) == functions.end()) { cerr << "Error: funcion '" << e->name << "' no existe." << endl; exit(1); }
    return functions[e->name];
}

Type* TypeChecker::visit(StepExp* e) {
    return e->variable->accept(this);
}

Type* TypeChecker::visit(TernaryExp* e) {
    // Validar que la condición sea booleana O numérica
    Type* condType = e->condition->accept(this);
    if (!condType || (!condType->match(boolType) && !condType->match(intType) && !condType->match(unsignedType))) {
        cerr << "Error: La condición del operador ternario debe ser lógica o numérica." << endl;
        exit(1);
    }
    
    // Validar tipos de ambas ramas
    Type* trueType = e->trueExp->accept(this);
    Type* falseType = e->falseExp->accept(this);
    
    if (!trueType || !falseType) {
        cerr << "Error: tipos indefinidos en expresión ternaria" << endl;
        exit(1);
    }
    
    // Permitir mezcla de int/unsigned como retorno
    if (!trueType->match(falseType)) {
        bool compat = (trueType->match(intType) && falseType->match(unsignedType)) ||
                      (trueType->match(unsignedType) && falseType->match(intType));
        if (!compat) {
            cerr << "Error: tipos inconsistentes en operador ternario" << endl;
            exit(1);
        }
    }
    
    return trueType;
}

// StructInit e InitData no devuelven tipo por sí mismos en este contexto de visitor
// La validación ocurre en InstanceDec
Type* TypeChecker::visit(StructInit* s) { return nullptr; }
Type* TypeChecker::visit(InitData* id) { 
    if(id->e) return id->e->accept(this);
    return nullptr; 
}