fin1=open('dblpAdj.txt','r')
data1=fin1.readlines()
fin1.close()

id1=10190
id2=10174
VP=dict()
AP=dict()
PA=dict()
for line in data1:
	tmp = line.strip().split("\t")
	paper = int(tmp[0])
	relation = int(tmp[2])
	if relation == 1:
		author = int(tmp[1])
		if paper not in PA:
			PA[paper]= {author}
		else:
			PA[paper].add(author)
		if author not in AP:
			AP[author] = {paper}
		else:
			AP[author].add(paper)
	elif relation == 3:
		venue = int(tmp[1])
		if venue not in VP:
			VP[venue]={paper}
		else:
			VP[venue].add(paper)

papers1 = VP[id1]
papers2 = VP[id2]

authors1 = set()
authors2 = set()

for paper in papers1:
	authors1.update(PA[paper])
for paper in papers2:
	authors2.update(PA[paper])
authors = authors1 & authors2
print(authors)

for author in authors:
	papers = AP[author]
	print(str(author) + ":" + str(len(papers & papers1)) + "," + str(len(papers & papers2)))

id3=1033
papers3=AP[id3]
venues3=dict()
for venue in VP.keys():
	tmp = VP[venue] & papers3
	if len(tmp) != 0:
		venues3[venue] = tmp
print(venues3)	
