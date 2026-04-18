import sys
import shutil
import os

def prepare_example(example_name):
    source = f"firmware/lib/xDuinoRails_LocoFuncDecoder/examples/{example_name}/{example_name}.ino"
    target = "firmware/src/main.cpp"

    if not os.path.exists(source):
        print(f"Error: Example {example_name} not found at {source}")
        sys.exit(1)

    print(f"Copying {source} to {target}")
    shutil.copy2(source, target)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python prepare_example.py <example_name>")
        sys.exit(1)

    prepare_example(sys.argv[1])
