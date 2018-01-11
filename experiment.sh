#!/bin/bash

experiment_type="classifier"
datasets=( "DBLP" )
penalty_types=( 1 2 3 4 )
tfidf_types=( "B-S" "P-S" "M-S" )
target_file="labels.txt"
for dataset in "${datasets[@]}"
do
	if [ $experiment_type == "classifier" ]; then
		target_file="$dataset"_neg_pairs.txt
	else
		target_file="$dataset"_labels.txt
	fi
	for tfidf_type in "${tfidf_types[@]}"
	do
		for penalty_type in "${penalty_types[@]}"
		do
			output_file="output/"$experiment_type"_"$dataset"_"$penalty_type"_"$tfidf_type"_5.txt"
		 	./topKQueryTest --"$experiment_type" "$dataset" "$dataset"_pos_pairs.txt $target_file 5 $penalty_type $tfidf_type > $output_file
		done
	done
	./topKQueryTest --"$experiment_type" "$dataset" "$dataset"_pos_pairs.txt $target_file 5 1 SP > "output/"$experiment_type"_"$dataset"_SP_5.txt"
done

