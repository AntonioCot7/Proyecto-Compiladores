import os
import subprocess
import shutil

# Archivos c++
programa = ["main.cpp", "scanner.cpp", "token.cpp", "parser.cpp", "ast.cpp", "visitor.cpp"]

# Compilar
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

for i in range(1, 5):  # Solo procesar hasta input4
    filename = f"input{i}.txt"
    filepath = os.path.join(input_dir, filename)

    if os.path.isfile(filepath):
        print(f"\nProcesando {filename}")
        run_cmd = ["./a.out", filepath]
        result = subprocess.run(run_cmd, capture_output=True, text=True)

        if result.returncode != 0:
            print(f"Error al procesar {filename}:")
            print(result.stderr)
            continue

        base_name = f"input_{i}"

        # Verificar y mover archivos generados
        # 1. Código ensamblador - buscamos en el directorio actual
        asm_file = f"{base_name}.s"
        if os.path.isfile(asm_file):
            dest_asm = os.path.join(output_dir, asm_file)
            # Si el archivo ya existe en el destino, lo sobrescribimos
            if os.path.exists(dest_asm):
                os.remove(dest_asm)
            shutil.move(asm_file, dest_asm)
            print(f"- Código ensamblador generado en: {dest_asm}")

        # 2. Salida del intérprete (usar el mismo base_name que usamos para el .s)
        interprete_file = os.path.join(interprete_dir, f"{base_name}_output.txt")
        if os.path.isfile(interprete_file):
            print(f"- Salida del intérprete generada en: {interprete_file}")
    else:
        print(f"Error: {filename} no encontrado en {input_dir}")
        exit(1)  # Terminar si falta algún archivo de entrada requerido
