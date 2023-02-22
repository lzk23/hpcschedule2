from random import randint
import re

def main():
    file = open("lublin-10000.txt", 'r')
    new_file = open('lublin_converted', 'w')
    data = file.readlines()
    for line in data:
        line = line.strip()
        s_array = re.split("\\s+", line)
        N = randint(3, 0)
        res = s_array[0] + ',' + str(N) + ',' + s_array[1]+ ',' +s_array[3]
        new_file.write(res+'\n')
    file.close()
    new_file.close()

if __name__ == '__main__':
    main()