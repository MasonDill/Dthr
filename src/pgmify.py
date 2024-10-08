#turns a .pgm into a 2D array of bytes
def readpgm(name):
    pgmf = open(name, 'rb')
    pgm_header = pgmf.readline()
    if pgm_header not in [b'P5\n', b'P2\n']:
        raise ValueError("Not a valid PGM file")

    (width, height) = [int(i) for i in pgmf.readline().split()]
    #depth = int(pgmf.readline())
    pixels = []

    for _ in range(0, height):
        row = []
        for _ in range(0, width):
            byte_val = pgmf.read(1)
            row.append(int.from_bytes(byte_val, "little"))
        pixels.append(row)
    pgmf.close()

    return pixels, width, height

def pgmToDataFile(input_file, output_file):
    from numpy import savetxt as np_savetxt
    img, _, _ = readpgm(input_file)
    np_savetxt(output_file, img, fmt="%d")

if __name__ == "__main__":
    from sys import argv
    input_file = argv[1]
    output_file = argv[2]
    pgmToDataFile(input_file, output_file)