# main.py
import ctypes
import platform
import argparse

# Load the shared library
if platform.system() == 'Windows':
    ditherlib = ctypes.CDLL('./ditherlib.dll')
elif platform.system() == 'Linux':
    ditherlib = ctypes.CDLL('./ditherlib.so')
else:
    raise Exception('Unsupported platform')

def parse_args():
    dither_algos = ['floyd-steinberg']

    parser = argparse.ArgumentParser()
    parser.add_argument('input', help='Input image')
    parser.add_argument('output', help='Output image')

    parser.add_argument('--passes', help='Number of passes', type=int, default=1)
    parser.add_argument('--algo', help='Dithering algorithm', choices=dither_algos, default=dither_algos[0])

    return parser.parse_args()

if __name__ == '__main__':
    from pgmify import pgmToDataFile, scrapePgmMetadata
    args = parse_args()

    width, height = scrapePgmMetadata(args.input)
    pgmToDataFile(args.input, './temp/temp.txt')
    
    ditherlib.dither(args.input, args.output, width, height, args.passes)
