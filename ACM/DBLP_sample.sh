#!/bin/bash

save_rm(){
        if [ -f "$1" ]; then
                rm "$1"
        fi
}

sample_pairs_add(){
        for i in `seq 1 $2`
        do
                awk -F'\t' '{if($2 == '$1') print $1}' "ACM_labels.txt" | sort -R | head -2 > pos_pair
		awk -F'\t' '{if($2 == '$1') print $1}' "ACM_labels.txt" | sort -R | head -1 > neg_pair
		awk -F'\t' '{if($2 != '$1') print $1}' "ACM_labels.txt" | sort -R | head -1 >> neg_pair
                paste -s -d'\t' pos_pair >> "$pos_file"
                paste -s -d'\t' neg_pair >> "$neg_file"

                save_rm pos_pair
                save_rm neg_pair
        done
}


pos_file="ACM_pos_pairs.txt"
neg_file="ACM_neg_pairs.txt"
m=1
M=50

area_list=`awk -F'\t' '{print $2}' "ACM_labels.txt" | sort | uniq`


# generate positive and negative pairs
save_rm $pos_file
save_rm $neg_file

for area in $area_list
do
	sample_pairs_add $area $M
done


