from HIN import EntityInfo
from Similarity import *
import os, pickle, random, math, time

alpha = 0.5

#loading the edge type file and the strength file
def loadEdgeTypeStrength():
    fin = open('../../IMDB/IMDBEdgeType.txt','r')
    fin2 = open('../../cache/IMDBedgeTypeAvgDegree.txt','r')
    edgeTypeDict = dict()
    edgeTypeRecords = fin.readlines()
    for record in edgeTypeRecords:
        [edgeTypeStr, edgeTypeIdStr] = record.strip().split("\t")
        edgeTypeDict[int(edgeTypeIdStr)] = edgeTypeStr
    fin.close()

    edgeTypeStrength = dict()
    edgeTypeStrengthRecords = fin2.readlines()
    for record in edgeTypeStrengthRecords:
        [edgeTypeIdStr, inputStrengthStr, outputStrengthStr] = record.split("\t")
        edgeTypeStrength[int(edgeTypeIdStr)] = 1.0/math.pow(float(inputStrengthStr)*float(outputStrengthStr), alpha)
    fin2.close()
    return edgeTypeDict, edgeTypeStrength

# transfrom a edge type-based meta path to a node type based meta path
def transform(metaPath, edgeTypeDict):
    nodeTypeMetaPath = []
    pathLength = len(metaPath)
    for i in range(0, pathLength):
        edgeType = metaPath[i]
        nodeType = []
        if edgeType > 0:
            nodeType = edgeTypeDict[edgeType].strip().split("_")
        else:
            nodeType = edgeTypeDict[-edgeType].strip().split("_")
            nodeType.reverse()

        nodeTypeMetaPath.append(nodeType[0])
        if i == pathLength - 1:
            nodeTypeMetaPath.append(nodeType[1])
    return nodeTypeMetaPath


#loading the topK meta path for the given src and dst entity, the k and the method
def loadTopKMetaPath(src, dst, method, edgeTypeDict, k=5):
    filename = "../../topKResult/IMDB_" + method + "_" + str(src) + "_" + str(dst) + "_" + str(k) + ".txt"
    fin = open(filename, "r")
    topKMetaPathsRecords = fin.readlines()
    metaPaths = []
    for record in topKMetaPathsRecords:
        tmpMetaPathStr = record.strip().split("\t")
        tmpMetaPath = [int(edgeTypeStr) for edgeTypeStr in tmpMetaPathStr]

        metaPaths.append(tmpMetaPath)

    return metaPaths


# get the weight of a group of meta paths
def weight(metaPaths, edgeTypeStrength, methodFlag=1):
    weights = []
    for metaPath in metaPaths:
        length = len(metaPath)
        strength = 1
        for i in range(0, length):
            strength *= edgeTypeStrength[abs(metaPath[i])]
        if methodFlag == 1:
            weights.append(math.exp(strength - length))
        else:
            weights.append(strength/length)
    sumWeights = sum(weights)
    tmpWeights = [weight/sumWeights for weight in weights]
    return tmpWeights


# loading remaining user rating record to evaluae the similarity method
def loadRatingGroundTruth():
    fin = open('dataset/users_rate_test.txt','r')
    userRecords = fin.readlines()
    fin.close()

    userRateGroundTruth = dict()
    for userRecord in userRecords:
        [userStr, movieStr, ratingStr] = userRecord.strip().split(" ")
        userId = int(userStr)
        movieId = int(movieStr)
        if userId not in userRateGroundTruth:
            seenMovies = set()
            seenMovies.add(movieId)
            highRatingMovies = set()
            if float(ratingStr) > 3:
                highRatingMovies.add(movieId)
            userRateGroundTruth[userId] = [seenMovies, highRatingMovies]
        else:
            userRateGroundTruth[userId][0].add(movieId)
            if float(ratingStr) > 3:
                userRateGroundTruth[userId][1].add(movieId)

    return userRateGroundTruth


def getDstEntitiesInfo(hin, srcEntityInfo, metaPath):
    hinEntities = hin['Entities']
    hinEntityType = hin['EntityTypes']
    dstEntitiesInfo = set()
    dstEntitiesInfo.add(srcEntityInfo)
    metaPathLength = len(metaPath)
    #print(metaPath)
    for i in range(1, metaPathLength):
        tmpDstEntitiesInfo = set()
        currEntityType = metaPath[i]
        for entityInfo in dstEntitiesInfo:
            if currEntityType not in entityInfo.outRelations:
                continue
            tmpRelIndexDict = entityInfo.outRelations[currEntityType]['relIndexDict']
            for nextEntityId in tmpRelIndexDict.keys():
                tmpDstEntitiesInfo.add(hinEntities[hinEntityType[currEntityType][nextEntityId]])
        dstEntitiesInfo.clear()
        dstEntitiesInfo = tmpDstEntitiesInfo.copy()

    return dstEntitiesInfo


def getPrecisionAndRecall(groundTruth, predictResult):
    hitResult = groundTruth.intersection(predictResult)
    hit = len(hitResult)
    return hit/len(predictResult), hit/len(groundTruth)

def getF1Score(precision, recall, beta=0.5):
    if precision == 0 and recall == 0:
        return 0
    else:
        return (1+beta*beta)*precision*recall/(beta*beta*precision + recall)


def main():
    f = open('./HIN.pkl','rb')
    HIN = pickle.load(f)
    f.close()

    K = 10
    metaPathK = 5
    methods = ["SP", "S-M-S"]
    print('k ', K)

    # load meta info
    edgeTypeDict, edgeTypeStrength = loadEdgeTypeStrength()
    # load ground truth
    userRateGroundTruth = loadRatingGroundTruth()

    hinEntities = HIN['Entities']
    userEntityIdDict = HIN['EntityTypes']['user']
    movieEntityIdDict = HIN['EntityTypes']['movie']

    # input pairs for testing
    fin = open('./dataset/HeteRecomInputPairsPython.txt','r')
    testPairsRecords = fin.readlines()
    testPairsSize = len(testPairsRecords)
    for method in methods:
        print("Running method:" + method)
        sumLinkPrediction = sumLinkRecall = 0
        sumRatePrediction = sumRateRecall = 0

        j = 0
        for record in testPairsRecords:
            [srcStr, dstStr] = record.strip().split('\t')
            src = int(srcStr)
            dst = int(dstStr)
            metaPaths = loadTopKMetaPath(src+200100, dst, method, edgeTypeDict, metaPathK)
            metaPathsWeights = weight(metaPaths, edgeTypeStrength)
            userEntityInfo = HIN['Entities'][HIN['EntityTypes']['user'][src]]
            exclusionMovies = set(userEntityInfo.outRelations['movie'].keys())
            movieScores = dict()
            movieScores.clear()
            for i in range(metaPathK):
                currMetaPath = transform(metaPaths[i], edgeTypeDict)
                currMetaPathWeight = metaPathsWeights[i]
                movieEntitiesInfo = getDstEntitiesInfo(HIN, userEntityInfo, currMetaPath)
                for movieEntityInfo in movieEntitiesInfo:

                    currMovieId = movieEntityInfo.entity.entityId
                    # exclude those directly connected movies in training set
                    if currMovieId in exclusionMovies:
                        continue
                    heteSim = getHeteSim(HIN, userEntityInfo.entity, movieEntityInfo.entity, currMetaPath)
                    if currMovieId not in movieScores:
                        movieScores[currMovieId] = currMetaPathWeight*heteSim
                    else:
                        movieScores[currMovieId] += currMetaPathWeight*heteSim

            movieScoresList = [(score, movie) for movie, score in movieScores.items()]
            movieScoresList.sort(reverse=True)
            topKResult = [movie for _, movie in movieScoresList[:K]]

            linkPrediction, linkRecall = getPrecisionAndRecall(userRateGroundTruth[src][0], topKResult)
            sumLinkPrediction += linkPrediction
            sumLinkRecall += linkRecall
            ratePrediction, rateRecall = getPrecisionAndRecall(userRateGroundTruth[src][1], topKResult)
            sumRatePrediction += ratePrediction
            sumRateRecall += rateRecall

            print("record:" + str(j))
            j += 1

        avgLinkPrediction = sumLinkPrediction/testPairsSize
        avgLinkRecall = sumLinkRecall/testPairsSize
        avgRatePrediction = sumRatePrediction/testPairsSize
        avgRateRecall = sumRateRecall/testPairsSize

        print("method: " + method)
        print("Link: precision -> %.5f; recall -> %.5f; F1-score -> %.5f" % (avgLinkPrediction, avgLinkRecall, getF1Score(avgLinkPrediction, avgLinkRecall)))
        print("Rate: precision -> %.5f; recall -> %.5f; F1-score -> %.5f" % (avgRatePrediction, avgRateRecall, getF1Score(avgRatePrediction, avgRateRecall)))
        print()



if __name__ == '__main__':
    main()
