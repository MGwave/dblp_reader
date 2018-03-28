import random
fin = open("user_ratedmovies-timestamps.dat",'r')
data = fin.readlines()
data = data[1:]
userDict = dict()
for record in data:
	[userStr, movieStr, ratingStr, timestampStr] = record.split('\t')
	if userStr not in userDict:
		userDict[userStr] = [(int(timestampStr), movieStr, ratingStr)]
	else:
		userDict[userStr].append((int(timestampStr), movieStr, ratingStr))

TRAINING_RATE = 0.7
fout1 = open('users_rate_train.txt','w')
fout2 = open('users_rate_test.txt','w')
for userStr in userDict:
	userRecords = userDict[userStr].copy()
	userRecords.sort()
	trainingLen = int(TRAINING_RATE*len(userRecords))
	trainUser = userDict[userStr][:trainingLen]
	for _, movieStr, ratingStr in trainUser:
		fout1.write(userStr + ' ' + movieStr + ' ' + ratingStr + '\n')
	testUser = userDict[userStr][trainingLen:]
	for _, movieStr, ratingStr in testUser:
		fout2.write(userStr + ' ' + movieStr + ' ' + ratingStr + '\n')

fin.close()
fout1.close()
fout2.close()
