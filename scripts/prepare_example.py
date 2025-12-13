import sys
import shutil
import os

def prepare_example(example_name):
    # Paths
    base_dir = "firmware"
    examples_dir = os.path.join(base_dir, "lib", "xDuinoRails_LocoFuncDecoder", "examples")
    target_file = os.path.join(base_dir, "src", "main.cpp")

    # Construct source path
    # Assuming example folder matches example name, and contains {ExampleName}.ino
    source_file = os.path.join(examples_dir, example_name, f"{example_name}.ino")

    if not os.path.exists(source_file):
        print(f"Error: Example file not found at {source_file}")
        sys.exit(1)

    print(f"Copying {source_file} to {target_file}...")
    try:
        shutil.copyfile(source_file, target_file)
        print("Success.")
    except Exception as e:
        print(f"Error copying file: {e}")
        sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python prepare_example.py <ExampleName>")
        sys.exit(1)

    prepare_example(sys.argv[1])
