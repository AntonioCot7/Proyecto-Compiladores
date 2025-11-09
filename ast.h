#ifndef AST_H
#define AST_H

#include <string>
#include <list>
#include <ostream>
#include <vector>
using namespace std;

// Forward declarations
class Visitor;
class VarDec;
class FunDec;
class ParamDec;
class Body;
class Include;
class InstanceDec;
class Stm;
class Exp;
class FunDec;
class ParamDec;
class Body;
class Include;
class InstanceDec;
class Stm;
class Exp;

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
    virtual int  accept(Visitor* visitor) = 0;
    virtual ~Exp() = 0;  // Destructor puro → clase abstracta
    static string binopToChar(BinaryOp op);  // Conversión operador → string
};

// Representa una expresión binaria (e.g., a + b, x * y)
class BinaryExp : public Exp {
public:
    Exp* left;
    Exp* right;
    BinaryOp op;
    int accept(Visitor* visitor);
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    ~BinaryExp();
};

// Expresión numérica
class NumberExp : public Exp {
public:
    int value;
    int accept(Visitor* visitor);
    NumberExp(int v);
    ~NumberExp();
};

// Representa un identificador (variable)
class IdExp : public Exp {
public:
    string value;
    int accept(Visitor* visitor);
    IdExp(string v);
    ~IdExp();
};

// Representa un valor booleano
class BoolExp : public Exp {
public:
    bool value;
    int accept(Visitor* visitor);
    BoolExp(bool v);
    ~BoolExp();
};

// Clase base abstracta para todas las sentencias
class Stm {
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() = 0;
};

// Representa una directiva include
// Ejemplo: #include <stdio.h>
class Include {
public:
    string heade;
    int accept(Visitor* visitor);
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
    VarDec();
    ~VarDec();
};

// Representa una declaración con inicialización
// Ejemplo: int x = 5, y = 10;
class InstanceDec {
public:
    string type;
    list<string> vars;    // Nombres de variables
    list<Exp*> values;    // Valores de inicialización
    int accept(Visitor* visitor);
    InstanceDec();
    ~InstanceDec();
};

// Representa un bloque de código
// Contiene declaraciones y sentencias
class Body {
public:
    list<VarDec*> declarations;    // Declaraciones de variables
    list<InstanceDec*> intances;   // Declaraciones con inicialización
    list<Stm*> stmList;           // Lista de sentencias
    int accept(Visitor* visitor);
    Body();
    ~Body();
};

// Representa el programa completo
// Punto de entrada del AST
class Program { 
public:
    list<Include*> includes;       // Directivas include
    list<VarDec*> vdlist;         // Declaraciones globales
    list<InstanceDec*> intdlist;   // Inicializaciones globales
    list<FunDec*> fdlist;         // Declaraciones de funciones
    int accept(Visitor* visitor);
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
    ParamDec(string t, string i);
    ~ParamDec();
};

// Representa una declaración de función
// Ejemplo: int foo(int x, int y) { ... }

class FunDec {
public:
    string Type;                  // Tipo de retorno
    string id;                    // Nombre de la función
    vector<ParamDec*> params;     // Lista de parámetros
    Body* body;                   // Cuerpo de la función
    int accept(Visitor* visitor);
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
    StepExp(Exp* var, StepType t, Exp* amt = nullptr);
    ~StepExp();
};

// Representa un bucle for
// Ejemplo: for (int i = 0; i < 10; i++) { ... }
class ForStm: public Stm {
public:
    InstanceDec* init;    // Inicialización
    Exp* condition;       // Condición
    StepExp* step;        // Expresión de incremento
    Body* body;           // Cuerpo del bucle
    int accept(Visitor* visitor);
    ForStm(InstanceDec* init, Exp* condition, StepExp* step, Body* body);
    ~ForStm();
};

// Representa una llamada a printf
// Ejemplo: printf("Value: %d", x);
class PrintfStm: public Stm {
public:
    string format;        // String de formato
    list<Exp*> args;     // Argumentos
    int accept(Visitor* visitor) override;
    PrintfStm(string fmt, list<Exp*> arguments);
    ~PrintfStm();
};

// Representa una sentencia return
// Ejemplo: return x + y;
class ReturnStm: public Stm {
public:
    Exp* e;              // Expresión a retornar
    int accept(Visitor* visitor);
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
    FcallExp();
    FcallExp(string n, vector<Exp*> args);
    ~FcallExp();
};

#endif // AST_H
