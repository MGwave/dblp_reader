#!/bin/bash
k=5
experiment_type="recommend"
datasets=( "ACM" )
betas=( "0.3" )
#betas=( "0.1" )
tfidf_types=("S-M-S" "S-B-S" "M-S" "B-S" ) 
target_file="labels.txt"
outDir="output/"
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
			output_file=$outDir$experiment_type"_"$dataset"_1_"$tfidf_type"_"$beta"_"$k".txt"
		 	#./topKQueryTest --"$experiment_type" "$dataset" "$dataset"_pos_pairs.txt $target_file $k $tfidf_type 1 $beta > $output_file
		done

		output_file=$outDir$experiment_type"_"$dataset"_2_"$tfidf_type"_"$k".txt"
		#./topKQueryTest --"$experiment_type" "$dataset" "$dataset"_pos_pairs.txt $target_file $k $tfidf_type 2 > $output_file
	done
	#./topKQueryTest --"$experiment_type" "$dataset" "$dataset"_pos_pairs.txt $target_file $k SP 1 > $outDir$experiment_type"_"$dataset"_SP_"$k".txt"
	./topKQueryTest --"$experiment_type" "$dataset" "$dataset"_pos_pairs.txt $target_file $k SLV1 1 > $outDir$experiment_type"_"$dataset"_SLV1_"$k".txt"
	./topKQueryTest --"$experiment_type" "$dataset" "$dataset"_pos_pairs.txt $target_file $k SLV2 1 > $outDir$experiment_type"_"$dataset"_SLV2_"$k".txt"
done

