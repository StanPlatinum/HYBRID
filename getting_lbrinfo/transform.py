#!/usr/bin/python

import sys

filename_input = sys.argv[1] + '.txt'
filename_output = sys.argv[1] + '.json'
file_to_write = open(filename_output, 'w+')
fields = []
json_fields = []
with open(filename_input, 'r') as file_to_read:
    while True:
        lines = file_to_read.readline()
        if not lines:
            break
        fields = lines.split()
        json_field = '{"dom_id":' + fields[0] + ',"MaxLength":' + fields[1] + ',"cpu_id":' + fields[2] + ',"cr3":"' + fields[3] + '"}\n'
        #print json_field
        file_to_write.write(json_field)
    file_to_write.close()             
#print 'Transforming done.'
