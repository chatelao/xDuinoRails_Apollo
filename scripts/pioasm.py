import os
import subprocess
import glob
from SCons.Script import Import

Import("env")

# This script is executed by PlatformIO before the build process.

def compile_pio_files(environ):
    print("--- PIO Compilation ---")

    project_dir = environ.get("PROJECT_DIR")
    lib_dir = os.path.join(project_dir, "lib", "xDuinoRails_DccSounds", "src")

    # Find the pioasm executable
    pioasm_path = ""
    # A different potential location
    for root, dirs, files in os.walk(os.path.join(os.path.expanduser("~"), ".platformio", "packages")):
        if "pioasm" in files and "rp2040" in root:
            pioasm_path = os.path.join(root, "pioasm")
            break

    if not pioasm_path:
        print("PIOASM executable not found. Skipping compilation.")
        return

    # Find all .pio files in the library
    pio_files = glob.glob(os.path.join(lib_dir, "*.pio"))

    # Compile each file
    for pio_file in pio_files:
        output_dir = os.path.join(lib_dir, "generated")
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)

        header_file = os.path.join(output_dir, os.path.basename(pio_file) + ".h")
        command = [pioasm_path, pio_file, header_file]

        print(f"Compiling {pio_file} to {header_file}")
        subprocess.run(command, check=True)

# --- Entry Point ---
compile_pio_files(env)
