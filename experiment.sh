#!/bin/bash
datasets=( "DBLP" )
penalty_types=( 1 2 3 4 )
tfidf_types=( "B-S" "M-S" "P-S" )

for dataset in "${datasets[@]}"
do
	for penalty_type in "${penalty_types[@]}"
	do
		for tfidf_type in "${tfidf_types[@]}"
		do
			output_file="output/"$dataset"_"$penalty_type"_"$tfidf_type"_5.txt"
		 	./topKQueryTest --advance "$dataset" "$dataset"_pos_pairs.txt "$dataset"_neg_pairs.txt 5 $penalty_type $tfidf_type > $output_file
		done
	done
	./topKQueryTest --advance "$dataset" "$dataset"_pos_pairs.txt "$dataset"_neg_pairs.txt 5 1 SP > "output/"$dataset"_SP_5.txt"
done

