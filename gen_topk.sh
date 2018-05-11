#!/bin/bash
inputFileName="/home/zczhu2/dblp_reader/HeteRecmTest/IMDB/dataset/HeteRecomInputPairsCPP.txt"
#inputFileName="/home/zczhu2/dblp_reader/Clustering/DBLP/ClusteringInputPairs.txt"
k=5
dataset="IMDB"
betas=( "0.1" )
tfidf_types=( "S-M-S" )

for tfidf_type in $tfidf_types
do
	for beta in $betas
	do
		x=1
		awk -F'\t' '{system("./topKQuery --advance '$dataset' " $1 " " $2 " '$k' 2 '$tfidf_type' 1 '$beta'")}' $inputFileName
	done
	#awk -F'\t' '{system("./topKQuery --advance '$dataset' " $1 " " $2 "'$k' 2 '$tfidf_type' 2")}' $inputFileName
done


awk -F'\t' '{system("./topKQuery --advance '$dataset' " $1 " " $2 " '$k' 2 SP 1")}' $inputFileName
#awk -F'\t' '{system("./topKQuery --advance '$dataset' " $1 " " $2 " '$k' 2 SLV1 1")}' $inputFileName
#awk -F'\t' '{system("./topKQuery --advance '$dataset' " $1 " " $2 " '$k' 2 SLV2 1")}' $inputFileName

