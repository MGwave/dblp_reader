#!/bin/bash

penalty_types=( 1 2 3 4 )
tfidf_types=( "B-S" "M-S" "P-S" )

for penalty_type in "${penalty_types[@]}"
do
	for tfidf_type in "${tfidf_types[@]}"
	do
		output_file="output/test_"$penalty_type"_"$tfidf_type"_5.txt"
		 ./topKQueryTest --advance DBLP DBLP_pos_pairs_10.txt DBLP_neg_pairs_10.txt 5 $penalty_type $tfidf_type > $output_file
	done
done

./topKQueryTest --advance DBLP DBLP_pos_pairs_10.txt DBLP_neg_pairs_10.txt 5 1 SP > "output/test_SP_5.txt"
