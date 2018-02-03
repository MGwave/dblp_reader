#!/bin/bash
k=3
experiment_type="recommend"
datasets=( "ACM" )
betas=( "0.1" "0.15" "0.2" "0.25" )
#betas=( "0.1" )
tfidf_types=( "B-S" "M-S" )
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
		for beta in "${betas[@]}"
		do
			output_file="output/"$experiment_type"_"$dataset"_1_"$tfidf_type"_"$beta"_"$k".txt"
		 	./topKQueryTest --"$experiment_type" "$dataset" "$dataset"_pos_pairs.txt $target_file $k $tfidf_type 1 $beta > $output_file
		done

		output_file="output/"$experiment_type"_"$dataset"_2_"$tfidf_type"_"$k".txt"
		#./topKQueryTest --"$experiment_type" "$dataset" "$dataset"_pos_pairs.txt $target_file $k $tfidf_type 2 > $output_file
	done
	./topKQueryTest --"$experiment_type" "$dataset" "$dataset"_pos_pairs.txt $target_file $k SP 1 > "output/"$experiment_type"_"$dataset"_SP_"$k".txt"
done

