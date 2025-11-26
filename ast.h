#ifndef AST_H
#define AST_H

#include <string>
#include <unordered_map>
#include <list>
#include <ostream>
#include <vector>
#include "semantic_types.h"
using namespace std;

// Forward declarations
class Visitor;
class TypeVisitor;
class InitData;
class StructInit;
class StructDec;
class FunDec;
class TypedefDec;

// Operadores binarios soportados en el lenguaje
// Incluye operadores aritméticos, relacionales y de asignación
enum BinaryOp { 
    // Operadores aritméticos
    PLUS_OP,          // +
    MINUS_OP,         // -
    MUL_OP,          // *
    DIV_OP,          // /
    PLUS_ASSIGN_OP,  // +=
    MINUS_ASSIGN_OP, // -=
    
    // Operadores relacionales
    GT_OP,           // >
    LT_OP,           // <
    GE_OP,           // >=
    LE_OP,           // <=
    EQ_OP,           // ==
    NE_OP,           // !=
    
    // Operador de asignación
    ASSIGN_OP        // =
};


// Clase base abstracta para todas las expresiones
// Proporciona la interfaz común para todas las expresiones en el AST
class Exp {
public:

    // ----- OPTIMIZACION: Sethi-Ullman -----
    int et = 0;
    int hoja = 0;

    // ---- OPTIMIZACION: Dead Code Elimination - Constant Folding(If)
    int cont = 0; // 1 si es constante, 0 si no
    int valor = 0; // El valor pre-calculado si cont=1

    virtual int  accept(Visitor* visitor) = 0;
    virtual ~Exp() = 0;  // Destructor puro → clase abstracta
    static string binopToChar(BinaryOp op);  // Conversión operador → string

    // --- NUEVO ---
    virtual Type* accept(TypeVisitor* visitor) = 0; // Para verificador de tipos
};

// Representa una expresión binaria (e.g., a + b, x * y)
class BinaryExp : public Exp {
public:
    Exp* left;
    Exp* right;
    BinaryOp op;
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // nuevo
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    ~BinaryExp();
};

// Expresión numérica (enteros)
class NumberExp : public Exp {
public:
    int value;
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // nuevo
    NumberExp(int v);
    ~NumberExp();
};

// Expresión numérica (flotante de 64 bits)
class FloatExp : public Exp {
public:
    double value;
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // nuevo
    FloatExp(double v);
    ~FloatExp();
};

// Representa un identificador (variable)
class IdExp : public Exp {
public:
    string value;
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // nuevo
    IdExp(string v);
    ~IdExp();
};

// Representa un valor booleano
class BoolExp : public Exp {
public:
    bool value;
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // nuevo
    BoolExp(bool v);
    ~BoolExp();
};

// Clase base abstracta para todas las sentencias
class Stm {
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() = 0;

    // --- NUEVO ---
    virtual void accept(TypeVisitor* visitor) = 0;
};

// Representa una directiva include
// Ejemplo: #include <stdio.h>
class Include {
public:
    string heade;
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor); // nuevo
    Include(string h);
    ~Include();
};

// Representa una declaración de variables
// Ejemplo: int x, y, z;
class VarDec {
public:
    string type;          // Tipo de la variable (int, long)
    list<string> vars;    // Lista de nombres de variables
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor); // nuevo
    VarDec();
    ~VarDec();
};

// Representa una declaración con inicialización
// Ejemplo: int x = 5, y = 10;
class InstanceDec : public Stm { // <--- AGREGAR ": public Stm"
public:
    string type;
    list<string> vars;
    list<InitData*> values;
    
    int accept(Visitor* visitor) override;      // <--- AGREGAR override
    void accept(TypeVisitor* visitor) override; // <--- AGREGAR override
    InstanceDec();
    ~InstanceDec();
};
// Representa un bloque de código
// Contiene declaraciones y sentencias
class Body {
public:
    list<VarDec*> declarations;    // Declaraciones de variables
    list<InstanceDec*> intances;   // Declaraciones con inicialización
    list<TypedefDec*> tdlist;      // typedef locales
    list<Stm*> stmList;           // Lista de sentencias
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor); // nuevo
    Body();
    ~Body();
};

// Representa el programa completo
// Punto de entrada del AST
class Program { 
public:
    list<Include*> includes;       // Directivas include
    list<VarDec*> vdlist;         // Declaraciones globales
    list<StructDec*> strlist;
    list<TypedefDec*> tdlist;
    list<InstanceDec*> intdlist;   // Inicializaciones globales
    list<FunDec*> fdlist;         // Declaraciones de funciones
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor); // nuevo
    Program();
    ~Program();
};

// Representa un parámetro de función
// Ejemplo: int x en void foo(int x)
class ParamDec {
public:
    string type;    // Tipo del parámetro
    string id;      // Nombre del parámetro
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor); // nuevo
    ParamDec(string t, string i);
    ~ParamDec();
};

// Representa una declaración de función
// Ejemplo: int foo(int x, int y) { ... }

class FunDec {
public:
    string type;                  // Tipo de retorno
    string id;                    // Nombre de la función
    vector<ParamDec*> params;     // Lista de parámetros
    Body* body;                   // Cuerpo de la función
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor); // nuevo
    FunDec();
    FunDec(string rt, string n, vector<ParamDec*> p, Body* b);
    ~FunDec();
};

// Representa una asignación
// Ejemplo: x = 5;
class AssignStm: public Stm {
public:
    string id;     // Identificador a asignar
    Exp* e;       // Expresión a asignar
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor); // nuevo
    AssignStm(string id, Exp* e);
    ~AssignStm();
};

// Representa una sentencia if
// Ejemplo: if (x > 0) { ... } else { ... }
class IfStm : public Stm {
public:
    Exp* condition;      // Condición
    Body* thenBody;      // Bloque then
    Body* elseBody;      // Bloque else
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor); // nuevo
    IfStm(Exp* c, Body* t, Body* e = nullptr);
    ~IfStm();
};

// Representa un bucle while
// Ejemplo: while (x > 0) { ... }
class WhileStm : public Stm {
public:
    Exp* condition;      // Condición
    Body* body;         // Cuerpo del bucle
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor); // nuevo
    WhileStm(Exp* c, Body* b);
    ~WhileStm();
};

// Representa una expresión de incremento/paso
// Ejemplos: i++, i += n, i -= n
class StepExp : public Exp {
public:
    enum StepType {
        INCREMENT,    // i++
        DECREMENT,    // i--
        COMPOUND      // i += n, i -= n
    };
    
    Exp* variable;     // Variable a modificar (IdExp*)
    StepType type;     // Tipo de paso
    Exp* amount;       // Para COMPOUND: cantidad a incrementar/decrementar
    
    int accept(Visitor* visitor) override;
    Type* accept(TypeVisitor* visitor) override; // nuevo
    StepExp(Exp* var, StepType t, Exp* amt = nullptr);
    ~StepExp();
};

// Representa un bucle for
// Ejemplo: for (int i = 0; i < 10; i++) { ... }
class ForStm: public Stm {
public:
    Stm* init;    // Inicialización
    Exp* condition;       // Condición
    StepExp* step;        // Expresión de incremento
    Body* body;           // Cuerpo del bucle
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor); // nuevo
    ForStm(Stm* init, Exp* condition, StepExp* step, Body* body);   
    ~ForStm();
};

// Representa una llamada a printf
// Ejemplo: printf("Value: %d", x);
class PrintfStm: public Stm {
public:
    string format;        // String de formato
    list<Exp*> args;     // Argumentos
    int accept(Visitor* visitor) override;
    void accept(TypeVisitor* visitor) override; // nuevo
    PrintfStm(string fmt, list<Exp*> arguments);
    ~PrintfStm();
};

// Representa una sentencia return
// Ejemplo: return x + y;
class ReturnStm: public Stm {
public:
    Exp* e;              // Expresión a retornar
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor); // nuevo
    ReturnStm(Exp* exp = nullptr);
    ~ReturnStm();
};

// Representa una llamada a función
// Ejemplo: foo(1, 2);
class FcallExp: public Exp {
public:
    string name;                 // Nombre de la función
    vector<Exp*> arguments;      // Lista de argumentos
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // nuevo
    FcallExp();
    FcallExp(string n, vector<Exp*> args);
    ~FcallExp();
};

// Representa la inicialización de una estructura
// Ejemplo: { .field1 = value1, .field2 = value2 }
class StructInit {
public:
    vector<Exp*> argumentos;
    
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // nuevo
    StructInit();
    ~StructInit();
};


// Representa datos de inicialización (expresión o estructura)
// Ejemplo: int x = 5, y = 7;  or  MyStruct s1 = {1,2}, s2 = {3,4};
class InitData {
public:
    Exp * e;
    StructInit * st;
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // nuevo
    InitData();
    ~InitData();
};

// Representa una declaración de estructura
// Ejemplo: typedef struct { int a; bool b; } MyStruct;
class StructDec {
public:
    vector<VarDec*> VdList;
    string nombre; 

    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor); // nuevo
    StructDec();
    ~StructDec();
};

// Representa una declaración typedef
// Ejemplo: typedef unsigned int uint;
class TypedefDec {
public:
    string typeName;
    string alias;
    int accept(Visitor* visitor);
    void accept(TypeVisitor* visitor); // nuevo
    TypedefDec();
    TypedefDec(string t, string a);
    ~TypedefDec();
};

class TernaryExp : public Exp {
public:
    Exp* condition;
    Exp* trueExp;
    Exp* falseExp;
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // nuevo
    TernaryExp();
    ~TernaryExp();
};

#endif // AST_H
