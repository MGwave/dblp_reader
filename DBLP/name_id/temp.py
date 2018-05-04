
import sys

id2name = dict()

for line in open('authorN.txt'):
	line = line.strip()
	id2name[int(line.split(' ')[0])] = line.split('\t')[1]

for line in open('confN.txt'):
	line = line.strip()
	id2name[int(line.split(' ')[0])] = line.split('\t')[1]
	
for line in open('termN.txt'):
	line = line.strip()
	id2name[int(line.split(' ')[0])] = line.split('\t')[1]
	
for line in open('paperN.txt'):
	line = line.strip()
	id2name[int(line.split(' ')[0])] = line.split('\t')[1]
	
for id in id2name:
	print id2name[id]  + '\t' + str(id)