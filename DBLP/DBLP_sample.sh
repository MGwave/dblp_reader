#!/bin/sh
pos_file="DBLP_pos_pairs.txt"
neg_file="DBLP_neg_pairs.txt"

save_rm(){
	if [ -f "$1" ]; then
		rm "$1"
	fi
}

sample_pairs_add(){
	if [ $1 = "pos" ]; then
		awk -F'\t' '{if($2 == '$2') print $1}' author_label_new.txt | sort -R | head -$3 > 1
		awk -F'\t' '{if($2 == '$2') print $1}' author_label_new.txt | sort -R | head -$3 > 2
		paste 1 2 >> "$pos_file"		
	else
		awk -F'\t' '{if($2 == '$2') print $1}' author_label_new.txt | sort -R | head -$3 > 1
                awk -F'\t' '{if($2 != '$2') print $1}' author_label_new.txt | sort -R | head -$3 > 2
		paste 1 2 >> "$neg_file"
	fi

	save_rm 1
	save_rm 2			
}

# generate positive pairs
save_rm $pos_file
sample_pairs_add "pos" 0 13
sample_pairs_add "pos" 1 13
sample_pairs_add "pos" 2 12
sample_pairs_add "pos" 3 12


save_rm $neg_file
sample_pairs_add "neg" 0 13
sample_pairs_add "neg" 1 13
sample_pairs_add "neg" 2 12
sample_pairs_add "neg" 3 12
