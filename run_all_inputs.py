import os
import subprocess
import shutil

# Archivos c++ (incluye TypeChecker y semantic_types si aplican)
programa = ["main.cpp", "scanner.cpp", "token.cpp", "parser.cpp", "ast.cpp", "visitor.cpp", "TypeChecker.cpp", "struct_registry.cpp"]

# Compilar (comando simple, genera ./a.out)
compile = ["g++"] + programa
print("Compilando:", " ".join(compile))
result = subprocess.run(compile, capture_output=True, text=True)

if result.returncode != 0:
    print("Error en compilación:\n", result.stderr)
    exit(1)

print("Compilación exitosa")

# Ejecutar
input_dir = "inputs"
output_dir = "outputs"
interprete_dir = "outputs_interprete"

# Crear directorios de salida
os.makedirs(output_dir, exist_ok=True)
os.makedirs(interprete_dir, exist_ok=True)

# Procesar todos los archivos inputs/input*.txt
for filename in sorted(os.listdir(input_dir)):
    if not filename.startswith("input") or not filename.endswith(".txt"):
        continue
    filepath = os.path.join(input_dir, filename)
    print(f"\nProcesando {filename}")
    run_cmd = ["./a.out", filepath]
    result = subprocess.run(run_cmd, capture_output=True, text=True)

    if result.returncode != 0:
        print(f"Error al procesar {filename}:")
        print(result.stderr)
        continue

    # Derivar base_name (input5.txt -> input_5)
    base = filename
    base = base.replace('.txt','')
    if base.startswith('input'):
        suffix = base[5:]
        if suffix.isdigit():
            base_name = f"input_{suffix}"
        else:
            base_name = base
    else:
        base_name = base

    # Verificar archivos generados en `outputs` y `outputs_interprete`
    asm_file = os.path.join(output_dir, f"{base_name}.s")
    if os.path.isfile(asm_file):
        print(f"- Código ensamblador generado en: {asm_file}")

    interprete_file = os.path.join(interprete_dir, f"{base_name}_output.txt")
    if os.path.isfile(interprete_file):
        print(f"- Salida del intérprete generada en: {interprete_file}")
