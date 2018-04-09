lambda = 100;
seeds = 10;
[candidatesNum, clustersNum] = size(groundTruth);


%%% construct edge type matrix %%%
load('DBLP/test.mat');
edgeTypeSet = {1,-1,2,-2,3,-3,4,-4};
edgeTypeMatrixSet = {transpose(A_P), A_P, P_T, transpose(P_T), P_V, transpose(P_V), P_P, transpose(P_P)};
adjMatrixMap = containers.Map(edgeTypeSet,edgeTypeMatrixSet);

methods = ["SP" "S-M-S"];

for method=methods
    numiTotal = 0;
    for i=1:size(testPairs,1)
        src = testPairs(i, 1);
        dst = testPairs(i, 2);
        fileName = strcat('../topKResult/DBLP_', method, '_',  int2str(src), '_',  int2str(dst), '_5.txt');  
        %%% read top k meta path file into a cell %%%
        fid = fopen(fileName);
        tline = fgetl(fid);
        tlines = cell(0,1);
        while ischar(tline)
            tlines{end+1,1}=tline;
            tline = fgetl(fid);
        end
        fclose(fid);

        %K = size(tlines,1);
        K = 3;
        Scell = cell(K, 1);
        for i=1:K
            W = eye(candidatesNum);
            metapath = str2num(tlines{i,1});
            for edgeType=metapath
                if(size(W,2) ~= size( adjMatrixMap(edgeType), 1))
                    disp(size(W));
                    disp(edgeType);
                    disp(size(adjMatrixMap(edgeType)));
                end
                W = W*adjMatrixMap(edgeType);
            end
            Scell{i,1} = W;
        end
        %disp(metapaths);
        SeedsMat = zeros(candidatesNum, clustersNum);
        for j=1:clustersNum
            SeedsMat(find(groundTruth(:,j), seeds), j) = 1;
        end

        [thetaMat, betaCell, piVec, accuracy, nmi] = PathSelect_v3(Scell, SeedsMat, lambda, groundTruth);
        disp(nmi);
    end
    nmiTotal = nmiTotal + nmi;
    disp(method);
    disp(nmiTotal/size(testPairs,1));
end
 


