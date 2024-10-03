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

#turns a .pgm into a 2D array of bytes
def readpgm(name):
    pgmf = open(name, 'rb')
    pgmf.readline()
    (width, height) = [int(i) for i in pgmf.readline().split()]
    #depth = int(pgmf.readline())
    bytevals = []

    for r in range(0, height):
        row = []
        for c in range(0, width):
            row.append((pgmf.read(1)))
        bytevals.append(row)
    pgmf.close()

    return bytevals

if __name__ == "__main__":
    input_file = argv[1]
    output_file = argv[2]

    img = readpgm(input_file)
    img = byteArrToIntArr(img)
    img = np.array(img)
    
    #write img to array
    np.savetxt(output_file, img, fmt="%d")