# main.py
import ctypes
import platform
import argparse

# Load the shared library
match platform.system():
    case 'Windows':
        ditherlib = ctypes.CDLL('./lib/ditherlib.dll')
    case 'Linux':
        ditherlib = ctypes.CDLL('./lib/ditherlib.so')
    case _:
        raise Exception('Unsupported platform')

dither_algos = ['floyd-steinberg', 'atkinson', 'black-white', 'half-tone']

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('input', help='Input image')
    parser.add_argument('output', help='Output image')

    parser.add_argument('-p', '--passes', help='Number of passes', type=int, default=1)
    parser.add_argument('-n', '--negate', help='Negate image', action='store_true')
    parser.add_argument('-a', '--algo', help='Dithering algorithm', choices=dither_algos, default=dither_algos[0])
    parser.add_argument('-v', '--verbose', help='Verbose output', action='store_true')
    return parser.parse_args()

if __name__ == '__main__':
    from pgmify import readpgm
    args = parse_args()

    pixels, width, height, depth = readpgm(args.input)

    if(args.verbose):
        print("Image dimensions: " + str(width) + "x" + str(height) + "x" + str(depth))
        print("Input file: " + args.input)
        print("Output file: " + args.output)
        print("Number of passes: " + str(args.passes))
        print("Dithering algorithm: " + args.algo)
        
    # convert to c-compatible types
    output_file_c = ctypes.c_char_p(args.output.encode('utf-8'))
    algo = ctypes.c_char_p(args.algo.encode('utf-8'))
    pixels_c = (ctypes.POINTER(ctypes.c_uint8) * height)()
    for i in range(height):
        row_array = (ctypes.c_uint8 * width)(*pixels[i])
        pixels_c[i] = row_array

    ditherlib.itterativeDither(pixels_c, output_file_c, width, height, args.passes, algo, args.negate)
