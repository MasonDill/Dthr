import numpy as np
from sys import argv

def byteArrToIntArr(byteArr):
    intArr = []
    for r in byteArr:
        intArr.append([])
        for c in r:
            intArr[-1].append(int.from_bytes(c, "little"))
    return intArr

def intToByteArr(intArr):
    byteArr = []
    for r in intArr:
        byteArr.append([])
        for c in r:
            n = int(c)
            byteArr[-1].append(n.to_bytes(1, 'little'))

    return byteArr

def scrapePgmMetadata(name):
    pgmf = open(name, 'rb')
    pgmf.readline()
    (width, height) = [int(i) for i in pgmf.readline().split()]
    pgmf.close()
    return width, height

#turns a .pgm into a 2D array of bytes
def readpgm(name):
    pgmf = open(name, 'rb')
    pgmf.readline()
    (width, height) = [int(i) for i in pgmf.readline().split()]
    #depth = int(pgmf.readline())
    bytevals = []

    for _ in range(0, height):
        row = []
        for _ in range(0, width):
            row.append((pgmf.read(1)))
        bytevals.append(row)
    pgmf.close()

    return bytevals

def pgmToDataFile(input_file, output_file):
    img = pgmToArr(input_file)
    np.savetxt(output_file, img, fmt="%d")

def pgmToArr(name):
    img = readpgm(name)
    img = byteArrToIntArr(img)
    img = np.array(img)
    return img

if __name__ == "__main__":
    input_file = argv[1]
    output_file = argv[2]
    pgmToDataFile(input_file, output_file)