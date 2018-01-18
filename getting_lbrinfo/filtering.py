#!/usr/bin/python

import sys

filename_input = sys.argv[1] + '.txt'
filename_output = sys.argv[1] + '-filtering.txt'
file_to_write = open(filename_output, 'w+')
fields = []
filtering_fields = ''
num_of_lines = 0

with open(filename_input, 'r') as file_to_read:
    while True:
        lines = file_to_read.readline()
        if not lines:
            break
        fields = lines.split()
        if int(fields[1]) > 5 and int(fields[1]) < 10 :
            #filtering_fields = fields[1] + ', '
            num_of_lines += 1
        #file_to_write.write(filtering_fields)
    file_to_write.close()             
print 'number of lines: ' + str(num_of_lines)
#print 'Transforming done.'
