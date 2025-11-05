#!/usr/bin/env python3
import os
import subprocess
import sys
from pathlib import Path

def create_main_scanner():
    """Crea un archivo main temporal para probar solo el scanner."""
    content = """#include "scanner.h"

int main(int argc, const char* argv[]) {
    if (argc != 2) return 1;
    return solo_scanner(argv[1]);  // Usa la función que ya está en scanner.cpp
}"""
    with open("main_scanner.cpp", "w") as f:
        f.write(content)

def main():
    # Crear carpeta tokens/ (aunque ejecutar_scanner ya lo hace)
    os.makedirs("tokens", exist_ok=True)
    
    # Compilar scanner
    create_main_scanner()
    if subprocess.run(["g++", "-o", "scanner_test", "main_scanner.cpp", "scanner.cpp", "token.cpp"]).returncode != 0:
        print("Error en compilación")
        return 1

    # Procesar archivo específico o todos
    input_path = Path(sys.argv[1]) if len(sys.argv) > 1 else None
    
    if input_path:
        if not input_path.exists():
            print(f"No existe el archivo: {input_path}")
            return 1
        return subprocess.run(["./scanner_test", str(input_path)]).returncode
    
    # Procesar todos los archivos
    success = True
    for i in range(1, 15):
        input_file = Path("inputs") / f"input{i}.txt"
        if input_file.exists():
            print(f"Procesando {input_file}")
            if subprocess.run(["./scanner_test", str(input_file)]).returncode != 0:
                success = False
    return 0 if success else 1

if __name__ == "__main__":
    try:
        sys.exit(main())
    finally:
        # Limpieza
        for temp_file in ["main_scanner.cpp", "scanner_test"]:
            if os.path.exists(temp_file):
                os.remove(temp_file)