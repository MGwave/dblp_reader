
import scipy.io as sio
import numpy as np

def getRelation(relations, candidates):
	new_relations = []
	new_entities = []
	inv_index_entities = dict()
	for existed_entity, new_entity in relations:
		if existed_entity in candidates:
			new_relations.append((existed_entity, new_entity))
			if new_entity not in inv_index_entities:
				inv_index_entities[new_entity] = len(new_entities)
				new_entities.append(new_entity)
	return new_relations, new_entities, inv_index_entities

def getRelationMatrix(relations, inv_index_entities_1, inv_index_entities_2):
	matrix = np.zeros((len(inv_index_entities_1), len(inv_index_entities_2)))
	for entity1, entity2 in relations:
		entityIndex1 = inv_index_entities_1[entity1]
		entityIndex2 = inv_index_entities_2[entity2]
		matrix[entityIndex1][entityIndex2] = 1
	return matrix

fin1 = open('DBLP_labels.txt', 'r')
data1 = fin1.readlines()
inv_index_authors = dict()
authors = []

for line in data1:
	[authorStr, _ ] = line.strip().split('\t')
	author = int(authorStr)
	inv_index_authors[author] = len(authors)
	authors.append(author)

fin2 = open('dblpAdj.txt', 'r')
data2 = fin2.readlines()

adj = dict()
for line in data2:
	[paperStr, entityIdStr, edgeTypeStr] = line.strip().split('\t')
	paper = int(paperStr)
	edgeType = int(edgeTypeStr)
	entityId = int(entityIdStr)
	tmpPair = (paper, entityId)
	if edgeType == 1:
		tmpPair = (entityId, paper)	
	if edgeType not in adj:
		adj[edgeType] = [tmpPair]
	else:
		adj[edgeType].append(tmpPair)


author2paper, papers, inv_index_papers = getRelation(adj[1], inv_index_authors)
paper2topic, topics, inv_index_topics = getRelation(adj[2], inv_index_papers)
paper2venue, venues, inv_index_venues = getRelation(adj[3], inv_index_papers)

paper2paper = []
for paper1, paper2 in adj[4]:
	if paper1 in inv_index_papers and paper2 in inv_index_papers:
		paper2paper.append((paper1, paper2))


# A_P
author2paper_matrix = getRelationMatrix(author2paper, inv_index_authors, inv_index_papers)
paper2topic_matrix = getRelationMatrix(paper2topic, inv_index_papers, inv_index_topics)
paper2venue_matrix = getRelationMatrix(paper2venue, inv_index_papers, inv_index_venues)
paper2paper_matrix = getRelationMatrix(paper2paper, inv_index_papers, inv_index_papers)



sio.savemat('test.mat',{'A_P':author2paper_matrix, 'P_T':paper2topic_matrix, 'P_V':paper2venue_matrix, 'P_P':paper2paper_matrix}, do_compression=True)