lambda = 100;
seeds = 10;
[candidatesNum, clustersNum] = size(groundTruth);

methods = ["SP", "S-M-S"];
for method=methods
    testStr = strcat('../topKResult/DBLP_', method);
    disp(testStr);
end


SeedsMat = zeros(candidatesNum, clustersNum);
for i=1:clustersNum
    SeedsMat(find(groundTruth(:,i), seeds), i) = 1;
end

%%% construct edge type matrix %%%
load('DBLP/test.mat');
edgeTypeSet = {1,-1,2,2,3,-3,4,-4};
edgeTypeMatrixSet = {transpose(A_P), A_P, P_T, transpose(P_T), P_V, transpose(P_V), P_P, transpose(P_P)};
adjMatrixMap = containers.Map(edgeTypeSet,edgeTypeMatrixSet);


%%% read top k meta path file into a cell %%%
filename = '../topKResult/IMDB_SLV1_259629_1089_5.txt';
fid = fopen(filename);
tline = fgetl(fid);
tlines = cell(0,1);
while ischar(tline)
    tlines{end+1,1}=tline;
    tline = fgetl(fid);
end
fclose(fid);

K = size(tlines,1);
Scell = cell(K, 1);
for i=1:K
    W = eye(candidatesNum);
    metapath = str2num(tlines{i,1});
    for edgeType=metapath
        W = W*adjMatrixMap(edgeType);
    end
    Scell{i:1} = W;
end
disp(metapaths);

[thetaMat, betaCell, piVec, accuracy, nmi] = PathSelect_v3(Scell, SeedsMat, lambda, groundTruth);
 


