#include <iostream>
#include <fstream>
#include <string>
#include "scanner.h"
#include "parser.h"
#include "ast.h"
#include "TypeChecker.h"
#include "visitor.h"

using namespace std;

int main(int argc, const char* argv[]) {
    // Verificar número de argumentos
    if (argc != 2) {
        cout << "Número incorrecto de argumentos.\n";
        cout << "Uso: " << argv[0] << " <archivo_de_entrada>" << endl;
        return 1;
    }

    // Abrir archivo de entrada
    ifstream infile(argv[1]);
    if (!infile.is_open()) {
        cout << "No se pudo abrir el archivo: " << argv[1] << endl;
        return 1;
    }

    // Leer contenido completo del archivo en un string
    string input, line;
    while (getline(infile, line)) {
        input += line + '\n';
    }
    infile.close();

    // Crear instancias de Scanner 
    Scanner scanner1(input.c_str());
    Scanner scanner2(input.c_str());

    // Ejecutar scanner en un scanner separado para generar archivo de tokens
    Scanner scanner_for_tokens(input.c_str());
    ejecutar_scanner(&scanner_for_tokens, string(argv[1]));

    // Crear instancias de Parser
    Parser parser(&scanner2);

    // Parsear y generar AST
    Program* ast = nullptr;

    try {
        ast = parser.parseProgram();
    } catch (const std::exception& e) {
        cerr << "Error al parsear: " << e.what() << endl;
        ast = nullptr;
    }

    // Si hubo error en el parsing, salir
    if (!ast) {
        return 1;
    }

    // Obtener el nombre base del archivo de entrada
    string inputFile(argv[1]);
    string baseName = inputFile;
    size_t lastSlash = baseName.find_last_of("/\\");
    if (lastSlash != string::npos) {
        baseName = baseName.substr(lastSlash + 1);
    }
    size_t dotPos = baseName.find_last_of('.');
    if (dotPos != string::npos) {
        baseName = baseName.substr(0, dotPos);
    }
    
    // Convertir formato input1, input2, etc. a input_1, input_2, etc.
    if (baseName.length() > 5 && baseName.substr(0, 5) == "input") {
        string suffix = baseName.substr(5);
        // Verificar si el sufijo es solo dígitos
        bool isAllDigits = true;
        for (char c : suffix) {
            if (c < '0' || c > '9') {
                isAllDigits = false;
                break;
            }
        }
        if (isAllDigits && !suffix.empty()) {
            baseName = "input_" + suffix;
        }
    }

    // Crear carpetas para las salidas
    system("mkdir -p outputs_interprete");
    system("mkdir -p outputs");
    
    // Generar archivo de salida del intérprete
    string interpreteOutput = "outputs_interprete/" + baseName + "_output.txt";
    ofstream outfileInterprete(interpreteOutput);
    if (!outfileInterprete.is_open()) {
        cerr << "Error al crear el archivo de salida del intérprete: " << interpreteOutput << endl;
        return 1;
    }

    // Ejecutar TypeChecker antes de cualquier visitor
    TypeChecker checker;
    checker.typecheck(ast);

    // Ejecutar y guardar la salida del PrintVisitor y EvalVisitor
    PrintVisitor impresion;
    EvalVisitor evaluador;
    // Redirigir la salida al archivo y ejecutar ambos visitantes: primero Print, luego Eval
    streambuf* oldCout = cout.rdbuf(outfileInterprete.rdbuf());
    impresion.imprimir(ast);
    // Ejecutar el evaluador para volcar los resultados del intérprete bajo la impresión
    evaluador.evaluar(ast);
    // Restaurar la salida estándar
    cout.rdbuf(oldCout);
    outfileInterprete.close();
    
    // Generar código ensamblador
    string asmOutput = "outputs/" + baseName + ".s";
    ofstream outfileAsm(asmOutput);
    if (!outfileAsm.is_open()) {
        cerr << "Error al crear el archivo de salida ensamblador: " << asmOutput << endl;
        return 1;
    }
    
    cout << "Generando código ensamblador en " << asmOutput << endl;
    GenCodeVisitor codigo(outfileAsm);
    codigo.generar(ast);  // Usar el mismo AST que ya tenemos
    outfileAsm.close();
    
    return 0;
}
