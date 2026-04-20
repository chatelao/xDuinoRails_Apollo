import sys
import wave

def wav_to_header(wav_file, header_file):
    with wave.open(wav_file, 'rb') as w:
        frames = w.readframes(w.getnframes())

    with open(header_file, 'w') as f:
        f.write("#ifndef BEEP_SOUND_H\n")
        f.write("#define BEEP_SOUND_H\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write("const uint8_t beep_sound[] = {\n")

        for i, byte in enumerate(frames):
            if i % 16 == 0:
                f.write("    ")
            f.write(f"0x{byte:02x}, ")
            if (i + 1) % 16 == 0:
                f.write("\n")

        f.write("\n};\n\n")
        f.write(f"const size_t beep_sound_len = {len(frames)};\n\n")
        f.write("#endif // BEEP_SOUND_H\n")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python wav_to_header.py <input.wav> <output.h>")
        sys.exit(1)

    wav_to_header(sys.argv[1], sys.argv[2])
