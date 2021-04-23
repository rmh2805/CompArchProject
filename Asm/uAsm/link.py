from sys import exit, argv

def main():
    if len(argv) < 5 or len(argv) % 2 != 1: # must have an odd number of args in the end
        print("Usage: " + argv[0] + " <out file> <entry point> <file 1> <load point 1> [...]")
        exit(1)

    outFile = open(argv[1], "w")

    i = 3
    while i < len(argv) - 1:
        obj = open(argv[i], 'r')
        loadPoint = int(argv[i + 1], 16)
        i += 2
        
        lines = obj.readlines()
        for line in lines:
            data = line.split(None, 1)
            
            offset = int(data[0], 16) + loadPoint
            line = str(hex(offset)[2:]) + ' ' + data[1]
            outFile.write(line)

        obj.close()

    outFile.write(hex(int(argv[2], 16))[2:])
    outFile.close()

main()