import matplotlib.pyplot as plt
import numpy as np

img_plt = plt.imread("./role8x8.png")
print(img_plt.shape)
# input size
h = 8
l = 8

outputfile = open("e:\PythonProject\out.txt", mode='w', encoding='utf-8')

# print(img_plt[3, 120] )

for i in range(h):
    for j in range(l):
        if i == h - 1 and j == l - 1:
            break
        val = ((int(img_plt[i, j, 0] * 31) & 0b11111) << 11)
        val += ((int(img_plt[i, j, 1] * 63) & 0b111111) << 5)
        val += int(img_plt[i, j, 2] * 31) & 0b11111
        val = "0x{:X}, ".format(val)
        outputfile.write(val)
    outputfile.write("\n")

val = ((int(img_plt[h - 1, l - 1, 0] * 31) & 31) << 11) + ((int(img_plt[h - 1, l - 1, 1] * 64) & 31) << 5) +\
      int(img_plt[h - 1, l - 1, 2] * 31)
val = "0x{:X}".format(val)

outputfile.write(val)
outputfile.close()
