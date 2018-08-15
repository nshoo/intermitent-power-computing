from time import sleep
import sys, os

# if cmd arguments missing
if len(sys.argv) < 2:
    print(sys.argv[0] + ": Missing filename!")
    exit()

filename = sys.argv[1] # File to measure
# Does measurment file exist?
if os.path.isfile(filename):
    os.system("gcc -S " + filename + " -o MEASURE.s")
else:
    print(sys.argv[0] + ": File does not exist!")
    exit()

print("Compilig file...")
# Waiting for compiler
while(not os.path.isfile("MEASURE.s")):
    sleep(1)

measure = open("MEASURE.s", 'r')
lines = measure.readlines()
lines = [line.strip() for line in lines if line.strip()]
lines = [line for line in lines if line[0] not in ['#', '/'] or (len(line) >= 12 and line[:12] == "#MEASUREFLAG")]
measure.close()

positions = [i for i, line in enumerate(lines) if line[:12] == "#MEASUREFLAG"]
if len(positions) < 2:
    print(sys.argv[0] + ": Not enough flags!")
    os.remove("MEASURE.s")
    exit()
elif len(positions) % 2 == 1:
    print(sys.argv[0] + ": Unbalanced Flags!")
    os.remove("MEASURE.s")
    exit()

pairs = [(positions[i*2], positions[i*2+1]) for i in range(int(len(positions) / 2))]

for i, pair in enumerate(pairs):
    length = pair[1] - pair[0] - 1
    if length == 1:
        print("Block #{0}: is a line long.".format(i + 1))
    else:
        print("Block #{0}: is {1} lines long.".format(i + 1, length))

# CLeanup temp asm for next run
os.remove("MEASURE.s")
