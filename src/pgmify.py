# Read a .pgm file given in P2 form (ASCII)
def readP2(pgmf, width, height):
    pixels = []
    for _ in range(0, height):
        row = []
        read_pixels = 0
        while(read_pixels < width):
            nums = pgmf.readline().split()
            read_pixels += len(nums)
            for num in nums:
                row.append(int(num))
        pixels.append(row)
    return pixels

# Read a .pgm file given in P5 form (binary)
def readP5(pgmf, width, height):
    pixels = []
    for _ in range(0, height):
        row = []
        for _ in range(0, width):
            byte_val = pgmf.read(1)
            row.append(int.from_bytes(byte_val, "little"))
        pixels.append(row)
    return pixels

#turns a .pgm into a 2D array of integers
def readpgm(name):
    PGM_TYPES = {b'P2\n': readP2, b'P5\n': readP5}

    pgmf = open(name, 'rb')
    pgm_header = pgmf.readline()
    if pgm_header not in PGM_TYPES:
        raise ValueError("Only P2 and P5 pgm file formats are supported")

    description = pgmf.readline()
    if not b'#' in description:
        (width, height) = [int(i) for i in description.split()]
    else:
        (width, height) = [int(i) for i in pgmf.readline().split()]
    
    depth = int(pgmf.readline())
    if depth != 255:
        raise ValueError("Only 8-bit depths are supported")

    pixels = PGM_TYPES[pgm_header](pgmf, width, height)
    pgmf.close()

    return pixels, width, height, depth

def pgmToDataFile(input_file, output_file):
    from numpy import savetxt as np_savetxt
    img, _, _ = readpgm(input_file)
    np_savetxt(output_file, img, fmt="%d")

if __name__ == "__main__":
    from sys import argv
    input_file = argv[1]
    output_file = argv[2]
    pgmToDataFile(input_file, output_file)