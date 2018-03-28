import sys
import random


fin = open("users_rate_train.txt","r")
data = fin.readlines()
usersDict = dict()

for line in data:
	[userStr, movieStr, rateStr] = line.strip().split(' ')
	user = int(userStr)
	movie = int(movieStr)
	rate = float(rateStr)
	if user not in usersDict:
		usersDict[user] = (movie, rate)
	else:
		(_, tmp_rate) = usersDict[user]
		if tmp_rate < rate:
			usersDict[user] = (movie, rate)
fin.close()
sampleNum = 100
if len(sys.argv) == 2:
	sampeNum = int(sys.argv[1])

fout1 = open("HeteRecomInputPairsPython.txt","w")
fout2 = open("HeteRecomInputPairsCPP.txt","w")

random_keys = random.sample(usersDict, sampleNum)
for key in random_keys:
	fout1.write(str(key)+"\t"+str(usersDict[key][0])+"\n")	
	fout2.write(str(key+200100)+"\t"+str(usersDict[key][0])+"\n")
fout1.close()
fout2.close()
