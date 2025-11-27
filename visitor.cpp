#include <iostream>
#include "ast.h"
#include "visitor.h"
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iomanip>

using namespace std;

// ==========================================
// Helper Local para Structs
// Clave: Nombre del Struct (ej: "Punto")
// Valor: Lista de pares <TipoCampo, NombreCampo> (ej: <"int", "x">)
// ==========================================
static unordered_map<string, vector<pair<string, string>>> global_struct_defs;

// Función recursiva para crear un Value por defecto dado un tipo
Value createDefaultValue(const string& type) {
    // Tipos Básicos
    if (type == "int" || type == "long") return Value::make_int(0);
    if (type.find("unsigned") != string::npos || type == "uint") return Value::make_unsigned(0);
    if (type == "bool") return Value::make_bool(false);
    
    // Structs
    // Si el tipo existe en nuestro registro de structs...
    if (global_struct_defs.find(type) != global_struct_defs.end()) {
        vector<Value> fieldValues;
        const auto& fields = global_struct_defs[type];
        
        // Inicializamos cada campo recursivamente
        for (const auto& field : fields) {
            string fieldType = field.first;
            // Llamada recursiva: si el campo es otro struct, se construirá correctamente
            fieldValues.push_back(createDefaultValue(fieldType));
        }

        // Retornamos el Struct ya armado
        return Value::make_struct(fieldValues, type);
    }
    return Value::make_int(0);
}

///////////////////////////////////////////////////////////////////////////////////
//                    SECCIÓN 1: MÉTODOS accept()
///////////////////////////////////////////////////////////////////////////////////

int BinaryExp::accept(Visitor* visitor)   { return visitor->visit(this); }
int NumberExp::accept(Visitor* visitor)   { return visitor->visit(this); }
int FloatExp::accept(Visitor* visitor)    { return visitor->visit(this); }
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
int StructInit::accept(Visitor* visitor)  { return visitor->visit(this); }
int InitData::accept(Visitor* visitor)    { return visitor->visit(this); }
int StructDec::accept(Visitor* visitor)   { return visitor->visit(this); }
int TypedefDec::accept(Visitor* visitor)  { return visitor->visit(this); }
int TernaryExp::accept(Visitor* visitor)  { return visitor->visit(this); }

///////////////////////////////////////////////////////////////////////////////////
//                    SECCIÓN 2: PrintVisitor
///////////////////////////////////////////////////////////////////////////////////

void PrintVisitor::imprimir(Program* program) {
    if (program) program->accept(this);
}

int PrintVisitor::visit(BinaryExp* exp) {
    cout << "("; 
    exp->left->accept(this);
    switch(exp->op) {
        case PLUS_OP: cout << " + "; break;
        case MINUS_OP: cout << " - "; break;
        case MUL_OP: cout << " * "; break;
        case DIV_OP: cout << " / "; break;
        case GT_OP: cout << " > "; break;
        case LT_OP: cout << " < "; break;
        case GE_OP: cout << " >= "; break;
        case LE_OP: cout << " <= "; break;
        case EQ_OP: cout << " == "; break;
        case NE_OP: cout << " != "; break;
        default: cout << " op "; break;
    }
    exp->right->accept(this); 
    cout << ")"; 
    return 0;
}

int PrintVisitor::visit(NumberExp* exp) { cout << exp->value; return 0; }
int PrintVisitor::visit(FloatExp* exp) { cout << exp->value; return 0; }
int PrintVisitor::visit(IdExp* exp) { cout << exp->value; return 0; }
int PrintVisitor::visit(BoolExp* exp) { cout << (exp->value ? "true" : "false"); return 0; }
int PrintVisitor::visit(Include* inc) { cout << "#include <" << inc->heade << ">" << endl; return 0; }

int PrintVisitor::visit(VarDec* vd) {
    cout << vd->type << " ";
    bool first = true;
    for(const string& var : vd->vars) {
        if (!first) cout << ", ";
        cout << var;
        first = false;
    }
    cout << ";" << endl; 
    return 0;
}

int PrintVisitor::visit(StructDec* sd) {
    cout << "typedef struct {" << endl;
    for (VarDec* vd : sd->VdList) {
        cout << "    " << vd->type << " ";
        bool first = true;
        for(const string& var : vd->vars) {
            if (!first) cout << ", ";
            cout << var;
            first = false;
        }
        cout << ";" << endl;
    }
    cout << "} " << sd->nombre << ";" << endl; 
    return 0;
}

int PrintVisitor::visit(StructInit* si) { 
    cout << "{";
    bool first = true;
    for (Exp* e : si->argumentos) {
        if (!first) cout << ", ";
        e->accept(this);
        first = false;
    }
    cout << "}"; 
    return 0; 
}

int PrintVisitor::visit(InitData* id) { 
    if(id->e) id->e->accept(this); 
    else if(id->st) id->st->accept(this); 
    return 0; 
}

int PrintVisitor::visit(TernaryExp* exp) {
    cout << "(";
    exp->condition->accept(this);
    cout << " ? ";
    exp->trueExp->accept(this);
    cout << " : ";
    exp->falseExp->accept(this);
    cout << ")";
    return 0;
}

int PrintVisitor::visit(InstanceDec* ind) { 
    cout << ind->type << " ";
    auto itVar = ind->vars.begin();
    auto itVal = ind->values.begin();
    bool first = true;
    while (itVar != ind->vars.end() && itVal != ind->values.end()) {
        if (!first) cout << ", ";
        cout << *itVar << " = ";
        (*itVal)->accept(this);
        itVar++; itVal++; first = false;
    }
    cout << ";" << endl; 
    return 0; 
}

int PrintVisitor::visit(Body* body) {
    cout << "{" << endl;
    for(auto t: body->tdlist) t->accept(this);
    for(auto v: body->declarations) v->accept(this);
    for(auto i: body->intances) i->accept(this);
    for(auto s: body->stmList) s->accept(this);
    cout << "}" << endl; 
    return 0;
}

int PrintVisitor::visit(Program* p) {
    for(auto i: p->includes) i->accept(this);
    for(auto s: p->strlist) s->accept(this);
    for(auto t: p->tdlist) t->accept(this);
    for(auto v: p->vdlist) v->accept(this);
    for(auto i: p->intdlist) i->accept(this);
    for(auto f: p->fdlist) f->accept(this);
    return 0;
}

int PrintVisitor::visit(TypedefDec* td) { 
    cout << "typedef " << td->typeName << " " << td->alias << ";" << endl; 
    return 0; 
}

int PrintVisitor::visit(ParamDec* pd) { cout << pd->type << " " << pd->id; return 0; }

int PrintVisitor::visit(FunDec* fd) {
    cout << fd->type << " " << fd->id << "("; 
    bool first = true;
    for (ParamDec* p : fd->params) {
        if (!first) cout << ", ";
        p->accept(this);
        first = false;
    }
    cout << ") ";
    fd->body->accept(this); 
    return 0;
}

int PrintVisitor::visit(AssignStm* stm) { 
    cout << stm->id << " = "; stm->e->accept(this); cout << ";" << endl; return 0; 
}

int PrintVisitor::visit(IfStm* stm) { 
    cout << "if ("; stm->condition->accept(this); cout << ") ";
    stm->thenBody->accept(this);
    if (stm->elseBody) { cout << " else "; stm->elseBody->accept(this); }
    return 0; 
}

int PrintVisitor::visit(WhileStm* stm) { 
    cout << "while ("; stm->condition->accept(this); cout << ") ";
    stm->body->accept(this);
    return 0; 
}

int PrintVisitor::visit(StepExp* step) { 
    IdExp* id = dynamic_cast<IdExp*>(step->variable);
    if(id) cout << id->value; else cout << "var";
    if (step->type == StepExp::INCREMENT) cout << "++";
    else if (step->type == StepExp::DECREMENT) cout << "--";
    else if (step->type == StepExp::COMPOUND) { cout << " += "; step->amount->accept(this); }
    return 0; 
}

int PrintVisitor::visit(ForStm* stm) { 
    cout << "for (";
    if (stm->init) stm->init->accept(this); else cout << "; ";
    stm->condition->accept(this); cout << "; ";
    if (stm->step) stm->step->accept(this);
    cout << ") "; stm->body->accept(this);
    return 0; 
}

int PrintVisitor::visit(PrintfStm* stm) { 
    cout << "printf(\"" << stm->format << "\"";
    for (Exp* e : stm->args) { cout << ", "; e->accept(this); }
    cout << ");" << endl; return 0; 
}

int PrintVisitor::visit(ReturnStm* r) { 
    cout << "return"; if (r->e) { cout << " "; r->e->accept(this); }
    cout << ";" << endl; return 0; 
}

int PrintVisitor::visit(FcallExp* fcall) { 
    cout << fcall->name << "(";
    bool first = true;
    for (Exp* arg : fcall->arguments) {
        if (!first) cout << ", ";
        arg->accept(this);
        first = false;
    }
    cout << ")"; return 0; 
}


///////////////////////////////////////////////////////////////////////////////////
//                    SECCIÓN 3: EvalVisitor
///////////////////////////////////////////////////////////////////////////////////

void EvalVisitor::evaluar(Program* program) {
    env.clear();
    env.add_level();
    envfun.clear();
    global_struct_defs.clear(); 
    return_value = 0;
    returning = false;
    last_value_valid = false;

    if (program) {
        for (FunDec* fd : program->fdlist) envfun[fd->id] = fd;
        cout << "Interprete:" << endl;
        program->accept(this);
    }
    env.clear();
}

int EvalVisitor::visit(BinaryExp* exp) {
    int leftVal = exp->left->accept(this);
    int rightVal = exp->right->accept(this);
    int res = 0;
    switch (exp->op) {
        case PLUS_OP: res = leftVal + rightVal; break;
        case MINUS_OP: res = leftVal - rightVal; break;
        case MUL_OP: res = leftVal * rightVal; break;
        case DIV_OP: 
            if (rightVal == 0) { cerr << "Error: Div 0" << endl; exit(1); }
            res = leftVal / rightVal; break;
        case GT_OP: res = leftVal > rightVal; break;
        case LT_OP: res = leftVal < rightVal; break;
        case GE_OP: res = leftVal >= rightVal; break;
        case LE_OP: res = leftVal <= rightVal; break;
        case EQ_OP: res = leftVal == rightVal; break;
        case NE_OP: res = leftVal != rightVal; break;
        default: break;
    }
    last_value = Value::make_int(res);
    last_value_valid = true;
    return res;
}

int EvalVisitor::visit(NumberExp* exp) {
    last_value = Value::make_int(exp->value);
    last_value_valid = true;
    return exp->value;
}

int EvalVisitor::visit(FloatExp* exp) {
    last_value = Value::make_float(exp->value);
    last_value_valid = true;
    return (int)exp->value;
}

int EvalVisitor::visit(IdExp* exp) {
    string name = exp->value;
    size_t pos = name.find('.');
    
    // CASO 1: Variable simple (sin puntos)
    if (pos == string::npos) {
        Value v = env.lookup(name);
        last_value = v;
        last_value_valid = true;
        if (v.kind == Value::INT) return v.i;
        if (v.kind == Value::UNSIGNED) return (int)v.u;
        if (v.kind == Value::BOOL) return v.b ? 1 : 0;
        return 0; // Si es un struct completo, retornamos 0 (el valor viaja en last_value)
    } 
    
    // CASO 2: Acceso a Struct (p.x, rect.centro.y)
    vector<string> parts;
    string token;
    std::istringstream ss(name);
    while (getline(ss, token, '.')) parts.push_back(token);

    // Obtener la variable raíz
    if (!env.check(parts[0])) {
        cerr << "Error: Variable '" << parts[0] << "' no encontrada." << endl;
        exit(1);
    }
    Value v = env.lookup(parts[0]);

    // Navegar por los campos
    for (size_t i = 1; i < parts.size(); ++i) {
        if (v.kind != Value::STRUCT) { 
            cerr << "Error: '" << parts[i-1] << "' no es un struct." << endl; 
            exit(1); 
        }
        
        string structName = v.type_name;
        string fieldName = parts[i];
        
        // Verificar definición del struct
        if (global_struct_defs.find(structName) == global_struct_defs.end()) { 
            cerr << "Error interno: Definicion de struct '" << structName << "' no encontrada." << endl; 
            exit(1); 
        }
        
        // Buscar el campo en la lista de pares <Tipo, Nombre>
        const auto& fields = global_struct_defs[structName];
        
        // Usamos find_if para buscar solo por el nombre (.second)
        auto it = find_if(fields.begin(), fields.end(), 
            [&fieldName](const pair<string, string>& element) { 
                return element.second == fieldName; 
            });

        if (it == fields.end()) { 
            cerr << "Error: Campo '" << fieldName << "' no existe en '" << structName << "'" << endl; 
            exit(1); 
        }
        
        // Calcular índice y avanzar
        int index = distance(fields.begin(), it);
        v = v.fields[index];
    }

    // Retornar el valor final
    last_value = v;
    last_value_valid = true;
    if (v.kind == Value::INT) return v.i;
    if (v.kind == Value::UNSIGNED) return (int)v.u;
    if (v.kind == Value::BOOL) return v.b ? 1 : 0;
    return 0;
}

int EvalVisitor::visit(BoolExp* exp) {
    last_value = Value::make_bool(exp->value);
    last_value_valid = true;
    return exp->value ? 1 : 0;
}

int EvalVisitor::visit(FcallExp* fcall) {
    if (envfun.find(fcall->name) == envfun.end()) {
        cerr << "Error: Funcion no declarada " << fcall->name << endl; exit(1);
    }
    FunDec* func = envfun[fcall->name];

    vector<Value> argValues;
    for (size_t i = 0; i < func->params.size(); ++i) {
        last_value_valid = false;
        int val = fcall->arguments[i]->accept(this);
        Value v = last_value_valid ? last_value : Value::make_int(val);
        argValues.push_back(v);
    }

    env.add_level();
    for (size_t i = 0; i < func->params.size(); ++i) {
        env.add_var(func->params[i]->id, argValues[i]);
    }
    
    return_struct_valid = false;
    func->body->accept(this);
    
    int ret = return_value;
    env.remove_level();
    
    // Si hubo retorno de struct explícito, úsalo
    if (return_struct_valid) {
        last_value = return_struct;
        last_value_valid = true;
        return_struct_valid = false; // Limpiar flag
    } else {
        last_value = Value::make_int(ret);
        last_value_valid = true;
    }
    
    returning = false;
    return_value = 0;
    return ret;
}

int EvalVisitor::visit(Program* p) {
    for (StructDec* sd : p->strlist) sd->accept(this);
    for (TypedefDec* td : p->tdlist) td->accept(this);
    for (VarDec* vd : p->vdlist) vd->accept(this);
    for (InstanceDec* ind : p->intdlist) ind->accept(this);

    if (envfun.find("main") != envfun.end()) {
        envfun["main"]->body->accept(this);
    } else {
        cerr << "Error: main no encontrado." << endl;
        exit(1);
    }
    return 0;
}

int EvalVisitor::visit(StructDec* sd) {
    vector<pair<string, string>> fields;
    for (VarDec* vd : sd->VdList) {
        // vd->type tiene el tipo (ej: "int"), vd->vars tiene los nombres (ej: "x", "y")
        for (const string& name : vd->vars) {
            fields.push_back({vd->type, name});
        }
    }
    // Guardamos la definición completa
    global_struct_defs[sd->nombre] = fields;
    return 0;
}

int EvalVisitor::visit(TypedefDec* td) { return 0; }

int EvalVisitor::visit(TernaryExp* exp) {
    int cond = exp->condition->accept(this);
    if (cond) {
        return exp->trueExp->accept(this);
    } else {
        return exp->falseExp->accept(this);
    }
}

int EvalVisitor::visit(Body* body) {
    for (TypedefDec* td : body->tdlist) td->accept(this);
    for (VarDec* vd : body->declarations) vd->accept(this);
    for (InstanceDec* ind : body->intances) ind->accept(this);
    for (Stm* stm : body->stmList) {
        stm->accept(this);
        if (returning) return return_value;
    }
    return 0;
}

int EvalVisitor::visit(VarDec* vd) {
    for (const string& var : vd->vars) {
        env.add_var(var, createDefaultValue(vd->type));
    }
    return 0;
}

int EvalVisitor::visit(InstanceDec* ind) {
    auto varIt = ind->vars.begin();
    auto valIt = ind->values.begin();
    for (; varIt != ind->vars.end(); ++varIt, ++valIt) {
        last_value_valid = false;
        int val = (*valIt)->accept(this);
        Value v = last_value_valid ? last_value : Value::make_int(val);
        if ((ind->type.find("unsigned") != string::npos || ind->type == "uint") && v.kind == Value::INT) {
            v = Value::make_unsigned((unsigned)v.i);
        }
        if (v.kind == Value::STRUCT) v.type_name = ind->type;
        env.add_var(*varIt, v);
    }
    return 0;
}

int EvalVisitor::visit(ParamDec* pd) { return 0; }
int EvalVisitor::visit(FunDec* fd) { return 0; }
int EvalVisitor::visit(Include* inc) { return 0; }

int EvalVisitor::visit(AssignStm* stm) {
    // Evaluar el valor a asignar (RHS)
    last_value_valid = false;
    int val = stm->e->accept(this);
    Value newVal = last_value_valid ? last_value : Value::make_int(val);

    string name = stm->id;
    size_t dotPos = name.find('.');

    // CASO A: Asignación simple
    if (dotPos == string::npos) {
        if (!env.update(name, newVal)) {
            cerr << "Error: Asignacion fallida a " << name << endl;
            exit(1);
        }
    } 
    // CASO B: Asignación a Struct (p.x = ...)
    else {
        vector<string> parts;
        string token;
        std::istringstream ss(name);
        while (getline(ss, token, '.')) {
            parts.push_back(token);
        }

        string rootVar = parts[0];
        if (!env.check(rootVar)) {
            cerr << "Error: Variable '" << rootVar << "' no encontrada." << endl;
            exit(1);
        }
        
        // IMPORTANTE: Copia para modificar
        Value structVal = env.lookup(rootVar);
        Value* currentVal = &structVal;

        for (size_t i = 1; i < parts.size(); ++i) {
            if (currentVal->kind != Value::STRUCT) {
                cerr << "Error: '" << parts[i-1] << "' no es un struct." << endl;
                exit(1);
            }

            string structTypeName = currentVal->type_name;
            string fieldName = parts[i];

            if (global_struct_defs.find(structTypeName) == global_struct_defs.end()) {
                cerr << "Error interno: Definicion de struct '" << structTypeName << "' no encontrada." << endl;
                exit(1);
            }

            const auto& fields = global_struct_defs[structTypeName];
            
            // Búsqueda corregida usando find_if para pares
            auto it = find_if(fields.begin(), fields.end(), 
                [&fieldName](const pair<string, string>& element) { 
                    return element.second == fieldName; 
                });
            
            if (it == fields.end()) {
                cerr << "Error: Campo '" << fieldName << "' no existe en '" << structTypeName << "'" << endl;
                exit(1);
            }
            
            int index = distance(fields.begin(), it);

            // Asignar o profundizar
            if (i == parts.size() - 1) {
                // Estamos en el campo final -> Asignar
                currentVal->fields[index] = newVal;
            } else {
                // Aún falta camino -> Profundizar
                currentVal = &(currentVal->fields[index]);
            }
        }

        // Actualizar la memoria con el struct modificado
        env.update(rootVar, structVal);
    }
    return 0;
}

int EvalVisitor::visit(IfStm* stm) {
    if (stm->condition->accept(this)) {
        env.add_level(); stm->thenBody->accept(this); env.remove_level();
    } else if (stm->elseBody) {
        env.add_level(); stm->elseBody->accept(this); env.remove_level();
    }
    return 0;
}

int EvalVisitor::visit(WhileStm* stm) {
    while (stm->condition->accept(this)) {
        env.add_level(); stm->body->accept(this); env.remove_level();
        if (returning) return return_value;
    }
    return 0;
}

int EvalVisitor::visit(ForStm* stm) {
    env.add_level();
    if (stm->init) stm->init->accept(this);
    while (stm->condition->accept(this)) {
        env.add_level(); stm->body->accept(this); env.remove_level();
        if (returning) break;
        if (stm->step) stm->step->accept(this);
    }
    env.remove_level();
    return 0;
}

int EvalVisitor::visit(StepExp* step) {
    IdExp* id = dynamic_cast<IdExp*>(step->variable);
    if (!id) return 0;
    Value v = env.lookup(id->value);
    int val = v.i;
    if (step->type == StepExp::INCREMENT) val++;
    else if (step->type == StepExp::DECREMENT) val--;
    else if (step->type == StepExp::COMPOUND) val += step->amount->accept(this);
    env.update(id->value, Value::make_int(val));
    return 0;
}

int EvalVisitor::visit(PrintfStm* stm) {
    string fmt = stm->format;
    vector<Value> args;
    for (Exp* e : stm->args) {
        last_value_valid = false;
        int val = e->accept(this);
        args.push_back(last_value_valid ? last_value : Value::make_int(val));
    }
    int argIdx = 0;
    for (size_t i = 0; i < fmt.size(); i++) {
        if (fmt[i] == '%' && argIdx < args.size()) {
            Value v = args[argIdx++];
            if (i+1 < fmt.size() && fmt[i+1] == 'd') { cout << v.i; i++; }
            else if (i+1 < fmt.size() && fmt[i+1] == 'u') { 
                if (v.kind == Value::UNSIGNED) cout << v.u; else cout << (unsigned)v.i;
                i++; 
            }
            else if (i+1 < fmt.size() && fmt[i+1] == 'f') { 
                if (v.kind == Value::FLOAT) cout << v.f; else cout << (double)v.i;
                i++; 
            }
            else if (i+2 < fmt.size() && fmt[i+1] == 'l' && fmt[i+2] == 'd') { 
                if (v.kind == Value::FLOAT) cout << v.f; else cout << v.i; 
                i+=2; 
            }
            else cout << "%";
        } else if (fmt[i] == '\\') {
            if (i+1 < fmt.size() && fmt[i+1] == 'n') { cout << endl; i++; }
            else cout << "\\";
        } else {
            cout << fmt[i];
        }
    }
    return 0;
}

int EvalVisitor::visit(ReturnStm* r) {
    if (r->e) {
        last_value_valid = false;
        return_struct_valid = false; // Reset

        return_value = r->e->accept(this); // Evalúa
        
        // Si el resultado fue un struct, guárdalo en la variable específica
        if (last_value_valid && last_value.kind == Value::STRUCT) {
            return_struct = last_value;
            return_struct_valid = true;
        }
    }
    returning = true;
    return 0;
}

int EvalVisitor::visit(StructInit* si) {
    vector<Value> fields;
    for (Exp* e : si->argumentos) {
        last_value_valid = false;
        int val = e->accept(this);
        fields.push_back(last_value_valid ? last_value : Value::make_int(val));
    }
    last_value = Value::make_struct(fields);
    last_value_valid = true;
    return 0;
}

int EvalVisitor::visit(InitData* id) {
    if (id->e) return id->e->accept(this);
    if (id->st) return id->st->accept(this);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////
//                    SECCIÓN 4: GenCodeVisitor
///////////////////////////////////////////////////////////////////////////////////

int GenCodeVisitor::getMemory(string name) {
    if (memoria.find(name) == memoria.end()) {
        cerr << "Error GenCode: Variable " << name << " no encontrada." << endl;
        exit(1);
    }
    return memoria[name];
}

void GenCodeVisitor::generar(Program* program) {
    if (program) program->accept(this);
}

int GenCodeVisitor::visit(Program* p) {
    out << ".data" << endl;
    out << "print_fmt_int: .string \"%ld\\n\"" << endl;
    
    out << ".text" << endl;
    out << ".global main" << endl;

    // Structs (offsets)
    for (auto s : p->strlist) s->accept(this);
    // Funciones
    for (FunDec* fd : p->fdlist) fd->accept(this);
    
    out << ".section .note.GNU-stack,\"\",@progbits" << endl;
    return 0;
}

int GenCodeVisitor::visit(StructDec* sd) {
    int currentOffset = 0;
    for (VarDec* vd : sd->VdList) {
        for (string field : vd->vars) {
            structLayouts[sd->nombre][field] = currentOffset;
            currentOffset += 8;
        }
    }
    structSizes[sd->nombre] = currentOffset;
    return 0;
}

int GenCodeVisitor::visit(TypedefDec* td) { return 0; }

// -------------------- IF TERNARIO OPTIMIZADO --------------------
int GenCodeVisitor::visit(TernaryExp* exp) {
    // Optimización: Si la condición es constante, solo generamos la rama necesaria
    if (exp->condition->cont == 1) {
        if (exp->condition->valor != 0) exp->trueExp->accept(this);
        else exp->falseExp->accept(this);
        return 0;
    }
    int id = count_ternary++;
    string labelFalse = "ternary_" + to_string(id) + "_false";
    string labelEnd = "ternary_" + to_string(id) + "_end";
    // 1. Evaluar condición (BinaryExp pone 0 o 1 en %rax)
    exp->condition->accept(this);
    out << "    cmpq $0, %rax" << endl;
    out << "    je " << labelFalse << endl; // Si es 0 (Falso), salta
    // 2. Caso Verdadero
    exp->trueExp->accept(this);
    out << "    jmp " << labelEnd << endl;
    // 3. Caso Falso
    out << labelFalse << ":" << endl;
    exp->falseExp->accept(this);
    // 4. Fin
    out << labelEnd << ":" << endl;
    return 0;
}

int GenCodeVisitor::visit(FunDec* fd) {
    nombreFuncion = fd->id;
    memoria.clear();
    varTypes.clear();
    offset = 0; 

    out << fd->id << ":" << endl;
    out << "    pushq %rbp" << endl;
    out << "    movq %rsp, %rbp" << endl;

    vector<string> regs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    
    offset = -8;
    for (size_t i = 0; i < fd->params.size(); ++i) {
        string pName = fd->params[i]->id;
        string pType = fd->params[i]->type;
        memoria[pName] = offset;
        varTypes[pName] = pType;
        
        int paramSize = 8;  // Por defecto, 8 bytes
        if (structSizes.count(pType)) {
            paramSize = structSizes[pType];  // Tamaño del struct
        }
        
        if (i < regs.size()) {
            // Si el parámetro es un struct, copiar sus campos desde la dirección
            if (structSizes.count(pType)) {
                int structSize = structSizes[pType];
                cerr << "DEBUG FunDec: param " << pName << " is STRUCT " << pType << " size=" << structSize << endl;
                // regs[i] contiene la dirección del struct
                out << "    movq " << regs[i] << ", %rax" << endl;  // Dirección en %rax
                
                // Copiar cada campo en orden de offset: 0, 8, 16, ...
                vector<pair<int, string>> orderedFields;
                for (auto& field : structLayouts[pType]) {
                    orderedFields.push_back({field.second, field.first});  // {offset, fieldName}
                }
                sort(orderedFields.begin(), orderedFields.end());
                
                int fieldStackOffset = offset;
                for (auto& p : orderedFields) {
                    int fieldStructOffset = p.first;   // Offset dentro del struct (0, 8, ...)
                    string fieldName = p.second;
                    // Convertir a offset negativo para stack que crece hacia abajo
                    int negOffset = -fieldStructOffset;
                    out << "    movq " << negOffset << "(%rax), %rcx" << endl;
                    out << "    movq %rcx, " << fieldStackOffset << "(%rbp)" << endl;
                    fieldStackOffset -= 8;
                }
            } else {
                out << "    movq " << regs[i] << ", " << offset << "(%rbp)" << endl;
            }
        }
        offset -= paramSize;  // Reducir por el tamaño del parámetro, no siempre 8
    }

    // Calcular locales
    int espacioLocales = 0;
    if (fd->body) {
        for (VarDec* vd : fd->body->declarations) {
            int size = 8;
            if (structSizes.count(vd->type)) size = structSizes[vd->type];
            espacioLocales += vd->vars.size() * size;
        }
        for (InstanceDec* ind : fd->body->intances) {
            int size = 8;
            if (structSizes.count(ind->type)) size = structSizes[ind->type];
            espacioLocales += ind->vars.size() * size;
        }
    }
    int totalStack = ((-offset) + espacioLocales + 15) & ~15; 
    out << "    subq $" << totalStack << ", %rsp" << endl;

    fd->body->accept(this);

    out << ".end_" << nombreFuncion << ":" << endl;
    out << "    leave" << endl;
    out << "    ret" << endl;
    return 0;
}

int GenCodeVisitor::visit(Body* body) {
    for (VarDec* vd : body->declarations) vd->accept(this);
    for (InstanceDec* ind : body->intances) ind->accept(this);
    for (Stm* stm : body->stmList) stm->accept(this);
    return 0;
}

int GenCodeVisitor::visit(VarDec* vd) {
    int typeSize = 8;
    if (structSizes.count(vd->type)) typeSize = structSizes[vd->type];

    for (string var : vd->vars) {
        memoria[var] = offset;
        varTypes[var] = vd->type;
        
        // Inicializar memoria a 0 (limpieza)
        for(int k=0; k<typeSize; k+=8) {
            out << "    movq $0, " << (offset - k) << "(%rbp)" << endl; 
        }
        
        // Reservar espacio en el stack map
        offset -= typeSize; 
    }
    return 0;
}

int GenCodeVisitor::visit(InstanceDec* ind) {
    int typeSize = 8;
    if (structSizes.count(ind->type)) typeSize = structSizes[ind->type];

    auto itVar = ind->vars.begin();
    auto itVal = ind->values.begin();
    
    while (itVar != ind->vars.end()) {
        string var = *itVar;
        InitData* init = *itVal;
        
        memoria[var] = offset;
        varTypes[var] = ind->type;
        
        if (structSizes.count(ind->type)) {
            // --- INICIALIZACIÓN DE STRUCT ---
            if (init->e) {
                if (FcallExp* call = dynamic_cast<FcallExp*>(init->e)) {
                    // Caso: Cualquier llamada a función que retorna struct
                    // Generar parámetros
                    for (int j = 0; j < call->arguments.size(); j++) {
                        Exp* arg = call->arguments[j];
                        arg->accept(this);
                        out << "    pushq %rax" << endl;
                    }
                    
                    vector<string> regs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
                    for (int i = call->arguments.size() - 1; i >= 0; --i) {
                        if (i < 6) {
                            out << "    popq " << regs[i] << endl;
                        }
                    }
                    out << "    movl $0, %eax" << endl;
                    out << "    call " << call->name << endl;
                    
                    // Recibir struct retornado en %rax + %rdx (+ más registros si necesario)
                    vector<string> retRegs = {"%rax", "%rdx", "%rcx", "%rsi", "%r8", "%r9"};
                    int regIdx = 0;
                    int fieldStackOffset = offset;
                    
                    vector<pair<int, string>> orderedFields;
                    for (auto& field : structLayouts[ind->type]) {
                        orderedFields.push_back({field.second, field.first});
                    }
                    sort(orderedFields.begin(), orderedFields.end());
                    
                    for (auto& p : orderedFields) {
                        if (regIdx < retRegs.size()) {
                            out << "    movq " << retRegs[regIdx] << ", " << fieldStackOffset << "(%rbp)" << endl;
                            regIdx++;
                        }
                        fieldStackOffset -= 8;
                    }
                } else {
                    // Expresión normal que retorna struct (menos común)
                    init->e->accept(this);
                    out << "    movq %rax, " << offset << "(%rbp)" << endl;
                }
            } else if (init->st) {
                // Caso: Punto p = {1, 2};
                int fieldIdx = 0;
                for (Exp* arg : init->st->argumentos) {
                    arg->accept(this);
                    int dest = offset + (fieldIdx * -8);
                    out << "    movq %rax, " << dest << "(%rbp)" << endl;
                    fieldIdx++;
                }
            }
        } else {
            // --- INICIALIZACIÓN BÁSICA (int, uint, float) ---
            if (init->e) {
                init->e->accept(this); // Evalúa expresión -> %rax
                out << "    movq %rax, " << offset << "(%rbp)" << endl;
            }
        }
        
        offset -= typeSize;
        itVar++; itVal++;
    }
    return 0;
}

int GenCodeVisitor::visit(IdExp* exp) {
    string name = exp->value;
    size_t dotPos = name.find('.');
    
    if (dotPos != string::npos) {
        string varName = name.substr(0, dotPos);
        string fieldName = name.substr(dotPos + 1);
        int baseOffset = getMemory(varName);
        string type = varTypes[varName];
        int relative = structLayouts[type][fieldName]; 
        int finalOffset = baseOffset - relative;
        cerr << "DEBUG IdExp: " << varName << "." << fieldName << " -> offset: " << finalOffset << endl;
        out << "    movq " << finalOffset << "(%rbp), %rax" << endl;
    } else {
        int off = getMemory(name);
        string type = varTypes[name];
        cerr << "DEBUG IdExp: " << name << " type=" << type << " offset=" << off;
        if (structSizes.count(type)) {
            cerr << " (STRUCT size=" << structSizes[type] << ")";
        }
        cerr << endl;
        out << "    movq " << off << "(%rbp), %rax" << endl;
    }
    return 0;
}

int GenCodeVisitor::visit(AssignStm* stm) {
    stm->e->accept(this);
    
    string name = stm->id;
    size_t dotPos = name.find('.');
    
    if (dotPos != string::npos) {
        // Asignación a campo de struct: alice.balance = ...
        string varName = name.substr(0, dotPos);
        string fieldName = name.substr(dotPos + 1);
        int baseOffset = getMemory(varName);
        string type = varTypes[varName];
        int relative = structLayouts[type][fieldName]; 
        int finalOffset = baseOffset - relative;
        out << "    movq %rax, " << finalOffset << "(%rbp)" << endl;
    } else {
        // Asignación a variable completa
        string type = varTypes[name];
        if (structSizes.count(type)) {
            // Struct: copiar desde múltiples registros
            int structSize = structSizes[type];
            int baseOffset = getMemory(name);
            cerr << "DEBUG Assign STRUCT " << type << " to " << baseOffset << endl;
            
            // Los registros de retorno son %rax, %rdx, %rcx, ...
            vector<pair<int, string>> orderedFields;
            for (auto& field : structLayouts[type]) {
                orderedFields.push_back({field.second, field.first});
            }
            sort(orderedFields.begin(), orderedFields.end());
            
            vector<string> retRegs = {"%rax", "%rdx", "%rcx", "%rsi", "%r8", "%r9"};
            int regIdx = 0;
            int fieldStackOffset = baseOffset;
            for (auto& p : orderedFields) {
                if (regIdx < retRegs.size()) {
                    out << "    movq " << retRegs[regIdx] << ", " << fieldStackOffset << "(%rbp)" << endl;
                    regIdx++;
                }
                fieldStackOffset -= 8;
            }
        } else {
            // Valor simple
            int off = getMemory(name);
            out << "    movq %rax, " << off << "(%rbp)" << endl;
        }
    }
    return 0;
}int GenCodeVisitor::visit(NumberExp* exp) {
    out << "    movq $" << exp->value << ", %rax" << endl;
    return 0;
}

int GenCodeVisitor::visit(FloatExp* exp) {
    // Para floats de 64 bits (double), cargamos la representación como entero de 64 bits
    // usando la representación binaria del double
    unsigned long long bits = *(unsigned long long*)(&exp->value);
    out << "    movabsq $0x" << hex << bits << dec << ", %rax" << endl;
    return 0;
}

int GenCodeVisitor::visit(BoolExp* exp) {
    out << "    movq $" << (exp->value ? 1 : 0) << ", %rax" << endl;
    return 0;
}

int GenCodeVisitor::visit(BinaryExp* exp) {
    // OPTIMIZACIÓN: Si la expresión es una constante (constant folding), usar directamente el valor
    if (exp->cont == 1) {
        out << "    movq $" << exp->valor << ", %rax" << endl;
        return 0;
    }
    // MANTENEMOS LA LÓGICA DE SETHI-ULLMAN (Decidir orden basado en 'et')
    // CASO 1: El hijo derecho es "ligero" (hoja o llamada simple).
    if (exp->right->et == 0) {
        // Orden: Izquierda -> Derecha
        exp->left->accept(this);
        out << "    pushq %rax" << endl; 
        exp->right->accept(this);
        out << "    movq %rax, %rcx" << endl;
        out << "    popq %rax" << endl;
    }
    // CASO 2: El hijo izquierdo es más pesado o igual.
    else if (exp->left->et >= exp->right->et) {
        exp->left->accept(this);
        out << "    pushq %rax" << endl;
        exp->right->accept(this);
        out << "    movq %rax, %rcx" << endl;
        out << "    popq %rax" << endl;
    }
    // CASO 3: El hijo derecho es más pesado (Optimización real de S-U).
    else {
        exp->right->accept(this);       
        out << "    pushq %rax" << endl;
        exp->left->accept(this);
        out << "    popq %rcx" << endl;
    }

    // OPERACIONES
    switch (exp->op) {
        case PLUS_OP: out << "    addq %rcx, %rax" << endl; break;
        case MUL_OP:  out << "    imulq %rcx, %rax" << endl; break;
        case MINUS_OP: 
            out << "    subq %rcx, %rax" << endl; 
            break;
        case DIV_OP: 
            out << "    cqo" << endl;
            out << "    idivq %rcx" << endl;
            break;
            
        // Comparaciones
        case EQ_OP: 
        case NE_OP:
        case GT_OP:
        case LT_OP:
        case GE_OP:
        case LE_OP:
            out << "    cmpq %rcx, %rax" << endl;
            if (exp->op == EQ_OP) out << "    sete %al" << endl;
            if (exp->op == NE_OP) out << "    setne %al" << endl;
            if (exp->op == GT_OP) out << "    setg %al" << endl;
            if (exp->op == LT_OP) out << "    setl %al" << endl;
            if (exp->op == GE_OP) out << "    setge %al" << endl;
            if (exp->op == LE_OP) out << "    setle %al" << endl;
            
            out << "    movzbq %al, %rax" << endl;
            break;
            
        default: break;
    }
    return 0;
}

int GenCodeVisitor::visit(ReturnStm* stm) {
    if (stm->e) {
        stm->e->accept(this);
        
        // Si es un struct, necesitamos copiar múltiples registros
        if (IdExp* id = dynamic_cast<IdExp*>(stm->e)) {
            string varName = id->value;
            string varType = varTypes[varName];
            if (structSizes.count(varType)) {
                // Struct: copiar a registros de retorno (%rax, %rdx, etc.)
                int structSize = structSizes[varType];
                int varOffset = getMemory(varName);
                cerr << "DEBUG Return STRUCT " << varType << " from " << varOffset << endl;
                
                // Copiar cada campo a registros
                vector<pair<int, string>> orderedFields;
                for (auto& field : structLayouts[varType]) {
                    orderedFields.push_back({field.second, field.first});
                }
                sort(orderedFields.begin(), orderedFields.end());
                
                vector<string> retRegs = {"%rax", "%rdx", "%rcx", "%rsi", "%r8", "%r9"};
                int regIdx = 0;
                int fieldStackOffset = varOffset;
                for (auto& p : orderedFields) {
                    if (regIdx < retRegs.size()) {
                        out << "    movq " << fieldStackOffset << "(%rbp), " << retRegs[regIdx] << endl;
                        regIdx++;
                    }
                    fieldStackOffset -= 8;
                }
            }
        }
    }
    out << "    jmp .end_" << nombreFuncion << endl;
    return 0;
}

// -------------------- IF OPTIMIZADO --------------------
int GenCodeVisitor::visit(IfStm* stm) {
    // CASO 1: La condición es CONSTANTE (Optimizacion)
    if (stm->condition->cont == 1) {
        if (stm->condition->valor != 0) {
            stm->thenBody->accept(this);
        } else {
            if (stm->elseBody) {
                stm->elseBody->accept(this);
            }
        }
        return 0;
    }
    // CASO 2: La condición es DINÁMICA (Normal)
    int id = count_if++;
    string labelElse = "if_" + to_string(id) + "_else";
    string labelEnd = "if_" + to_string(id) + "_end";
    stm->condition->accept(this);
    out << "    cmpq $0, %rax" << endl;
    out << "    je " << labelElse << endl;
    // Bloque THEN
    stm->thenBody->accept(this);
    out << "    jmp " << labelEnd << endl;
    // Bloque ELSE
    out << labelElse << ":" << endl;
    if (stm->elseBody) {
        stm->elseBody->accept(this);
    }
    out << labelEnd << ":" << endl;
    return 0;
}

int GenCodeVisitor::visit(WhileStm* stm) {
    int id = count_while++;
    string labelStart = "while_" + to_string(id) + "_start";
    string labelEnd = "while_" + to_string(id) + "_end";

    out << labelStart << ":" << endl;
    
    stm->condition->accept(this);
    out << "    cmpq $0, %rax" << endl;
    out << "    je " << labelEnd << endl;

    stm->body->accept(this);
    
    out << "    jmp " << labelStart << endl;
    out << labelEnd << ":" << endl;
    return 0;
}

int GenCodeVisitor::visit(ForStm* stm) {
    // Init (solo una vez)
    if (stm->init) stm->init->accept(this);
    int id = count_for++;
    string labelStart = "for_" + to_string(id) + "_start";
    string labelEnd = "for_" + to_string(id) + "_end";
    out << labelStart << ":" << endl;
    // Condición
    stm->condition->accept(this);
    out << "    cmpq $0, %rax" << endl;
    out << "    je " << labelEnd << endl;
    // Cuerpo
    stm->body->accept(this);
    // Step
    if (stm->step) stm->step->accept(this);
    out << "    jmp " << labelStart << endl;
    out << labelEnd << ":" << endl;
    return 0;
}

int GenCodeVisitor::visit(StepExp* step) {
    IdExp* id = dynamic_cast<IdExp*>(step->variable);
    if (!id) return 0;
    
    int off = getMemory(id->value);
    out << "    movq " << off << "(%rbp), %rax" << endl;
    
    if (step->type == StepExp::INCREMENT) out << "    incq %rax" << endl;
    else if (step->type == StepExp::DECREMENT) out << "    decq %rax" << endl;
    else if (step->type == StepExp::COMPOUND) {
        out << "    pushq %rax" << endl;
        step->amount->accept(this);
        out << "    movq %rax, %rcx" << endl;
        out << "    popq %rax" << endl;
        out << "    addq %rcx, %rax" << endl;
    }
    out << "    movq %rax, " << off << "(%rbp)" << endl;
    return 0;
}

int GenCodeVisitor::visit(PrintfStm* stm) {
    for (Exp* e : stm->args) {
        e->accept(this);
        out << "    movq %rax, %rsi" << endl;
        out << "    leaq print_fmt_int(%rip), %rdi" << endl;
        out << "    movl $0, %eax" << endl;
        out << "    call printf@PLT" << endl;
    }
    return 0;
}

int GenCodeVisitor::visit(FcallExp* exp) {
    if (exp->name == "crearPunto") return 0;
    
    cerr << "DEBUG FcallExp: " << exp->name << " with " << exp->arguments.size() << " args" << endl;
    
    // Procesar argumentos y push a la pila
    for (int j = 0; j < exp->arguments.size(); j++) {
        Exp* arg = exp->arguments[j];
        cerr << "  arg[" << j << "] = ";
        if (IdExp* id = dynamic_cast<IdExp*>(arg)) {
            cerr << "IdExp(" << id->value << ")";
            string varName = id->value;
            string type = varTypes[varName];
            if (structSizes.count(type)) {
                int structSize = structSizes[type];
                cerr << " -> STRUCT size=" << structSize;
            }
            cerr << endl;
            
            // Si es un struct, cargar la dirección
            if (structSizes.count(type)) {
                int baseOffset = getMemory(varName);
                cerr << "    Loading struct address: " << baseOffset << "(%rbp)" << endl;
                out << "    leaq " << baseOffset << "(%rbp), %rax" << endl;
            } else {
                arg->accept(this);
            }
        } else {
            cerr << "other" << endl;
            arg->accept(this);
        }
        out << "    pushq %rax" << endl;
    }
    
    vector<string> regs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    for (int i = exp->arguments.size() - 1; i >= 0; --i) {
        if (i < 6) {
            cerr << "  popq " << regs[i] << endl;
            out << "    popq " << regs[i] << endl;
        }
    }
    out << "    movl $0, %eax" << endl;
    out << "    call " << exp->name << endl;
    return 0;
}

// Stubs vacíos
int GenCodeVisitor::visit(Include* inc) { return 0; }
int GenCodeVisitor::visit(ParamDec* pd) { return 0; }
int GenCodeVisitor::visit(StructInit* si) { return 0; }
int GenCodeVisitor::visit(InitData* id) { return 0; }