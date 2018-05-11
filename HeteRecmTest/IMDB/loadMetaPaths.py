import pickle, math
alpha=0.5
#loading the edge type file and the strength file
def loadEdgeTypeStrength():
    fin = open('../../IMDB/IMDBEdgeType.txt','r')
    fin2 = open('../../cache/orgnIMDBedgeTypeAvgDegree.txt','r')
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

#loading the topK meta path for the given src and dst entity, the k and the method
def loadTopKMetaPath(src, dst, method, edgeTypeDict, k=5):
    filename = "../../topKResult/IMDB_" + method + "_" + str(src) + "_" + str(dst) + "_5.txt"
    fin = open(filename, "r")
    topKMetaPathsRecords = fin.readlines()
    fin.close()
    metaPaths = []
    i = 0
    for record in topKMetaPathsRecords:
        tmpMetaPathStr = record.strip().split("\t")
        tmpMetaPath = [int(edgeTypeStr) for edgeTypeStr in tmpMetaPathStr]
        if len(tmpMetaPath) != 1:
            metaPaths.append(tmpMetaPath)
            i += 1
        if i >= k:
            break

    return metaPaths

if __name__ == '__main__':
    # load meta info
    edgeTypeDict, edgeTypeStrength = loadEdgeTypeStrength()

    pairMetaPaths=dict()
    methods=["SP","S-M-S", "SLV1", "SLV2"]
 
    fin = open('./dataset/HeteRecomInputPairsPython.txt','r')
    testPairsRecords = fin.readlines()
    fin.close()
    for method in methods:
        pairMetaPaths[method]=dict()
        for record in testPairsRecords:
            [srcStr, dstStr] = record.strip().split('\t')
            print("src: " + srcStr + ";dst: " + dstStr)
            src = int(srcStr)
            dst = int(dstStr)
            metaPaths = loadTopKMetaPath(src+200100, dst, method, edgeTypeDict, 4)        
            pairMetaPaths[method][(src,dst)]=metaPaths
    with open("pairMetaPaths.pkl",'wb') as pairMetaPathsFile:
        pickle.dump(pairMetaPaths, pairMetaPathsFile)

