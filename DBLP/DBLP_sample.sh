#!/bin/bash

save_rm(){
        if [ -f "$1" ]; then
                rm "$1"
        fi
}

sample_pairs_add(){
        for i in `seq 1 $2`
        do
                awk -F'\t' '{if($2 == '$1') print $1}' "author_label_new.txt" | sort -R | head -2 > pos_pair
                paste -s -d'\t' pos_pair >> "$pos_file"
                head -1 pos_pair > curr_src
                for area in $area_list
                do
                        if [ $area != $1 ]; then
                                if [ -f "tmp_area_$area.txt" ]; then
                                        cat "tmp_area_$area.txt" | sort -R | head -$m >> neg_pair
                                fi
                        fi
                done

                paste -s -d',' neg_pair > tmp_neg_pair
                paste -d'\t' curr_src tmp_neg_pair >> "$neg_file"

                save_rm pos_pair
                save_rm neg_pair
		save_rm tmp_neg_pair
                save_rm curr_src
        done
}


pos_file="DBLP_pos_pairs.txt"
neg_file="DBLP_neg_pairs.txt"
m=1
M=50

area_list=`awk -F'\t' '{print $2}' "author_label_new.txt" | sort | uniq`

for area in $area_list
do
        awk -F'\t' '{if($2 == '$area') print $1}' "author_label_new.txt" > "tmp_area_$area.txt"
done

# generate positive and negative pairs
save_rm $pos_file
save_rm $neg_file

for area in $area_list
do
	sample_pairs_add $area $M
done

for area in $area_list
do
	save_rm "tmp_area_$area.txt"
done

