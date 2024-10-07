# main.py
import ctypes
import platform
import argparse

# Load the shared library
if platform.system() == 'Windows':
    ditherlib = ctypes.CDLL('./lib/ditherlib.dll')
elif platform.system() == 'Linux':
    ditherlib = ctypes.CDLL('./lib/ditherlib.so')
else:
    raise Exception('Unsupported platform')

def parse_args():
    dither_algos = ['floyd-steinberg']

    parser = argparse.ArgumentParser()
    parser.add_argument('input', help='Input image')
    parser.add_argument('output', help='Output image')

    parser.add_argument('-p', '--passes', help='Number of passes', type=int, default=1)
    parser.add_argument('-a', '--algo', help='Dithering algorithm', choices=dither_algos, default=dither_algos[0])
    parser.add_argument('-v', '--verbose', help='Verbose output', action='store_true')
    return parser.parse_args()

if __name__ == '__main__':
    from pgmify import pgmToDataFile, scrapePgmMetadata
    args = parse_args()

    if(args.verbose):
        print("Input file: " + args.input)
        print("Output file: " + args.output)
        print("Number of passes: " + str(args.passes))
        print("Dithering algorithm: " + args.algo)

    width, height = scrapePgmMetadata(args.input)
    pgmToDataFile(args.input, './temp/temp.txt')

    # convert to c-compatible strings
    input_file_c = ctypes.c_char_p(b"./temp/temp.txt")
    output_file_c = ctypes.c_char_p(args.output.encode('utf-8'))
    
    ditherlib.dither(input_file_c, output_file_c, width, height, args.passes, args.verbose)
