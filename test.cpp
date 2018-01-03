#include "DBLP_Reader.h"
#include "Meta-Structure.h"
#include "TopKCalculator.h"
#include "AppUtils.h"
#include "CommonUtils.h"

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>

#define F1_BETA 1

using namespace std;

void output(vector<double> precision_result){
	cout << endl;
	if(precision_result.size() > 0){
		cout << "Metrics" << "\t" << "Precision" << "\t" << "Precision@k" << endl;
		cout.precision(6);
		cout << fixed;
		for(int i = 0; i < precision_result.size(); i++){
			cout << i+1 << "\t" << precision_result[i] << "\t";
			double sum = 0.0;
			for(int j = 0; j <= i; j++){
				sum += precision_result[j]; 
			}
			cout << sum/(i + 1) << endl;	
		}

	}else{
		cerr << "No Meta Pah Found in the HIN Graph." << endl;
	}

}

void printUsage(const char* argv[]){
	cout << "Usage: " << endl;
        cout << argv[0] << " --default dataset (positive pairs file name) (negative pairs file name) k" << endl;
        cout << argv[0] << " --advance dataset (positive pairs file name) (negative pairs file name) k length-penalty TF-IDF-type" << endl;
        cout << argv[0] << " --refine dataset (positive pairs file name) (negative pairs file name) k score-function" << endl;
        cout << endl;

        cout << "--advance mode:" << endl;
        cout << "\t length-penalty(l is the meta-path's length): " << endl;
        cout << "\t\t 0 -> 1" << endl;
        cout << "\t\t 1 -> 1/log(l)" << endl;
        cout << "\t\t 2 -> 1/l" << endl;
        cout << "\t\t 3 -> 1/(l^2)" << endl;
        cout << "\t\t 4 -> 1/(e^l)" << endl;

        cout << "\t TF-IDF-type:" << endl;
        cout << "\t\t M-S -> MNI-based Support" << endl;
        cout << "\t\t B-S -> Binary-based Support" << endl;
        cout << "\t\t P-S -> PCRW-based Support" << endl;
        cout << "\t\t SP -> Shortest Path" << endl;
	cout << endl;

        cout << "--default mode:" << endl;
        cout << "\t length-penalty -> 2" << endl;
        cout << "\t TF-IDF-type -> M-S" << endl;
        cout << endl;


        cout << "--refine mode:" << endl;
        cout << "\t refine k meta-paths from previous generated meta-paths" << endl;
        cout << "\t default meta-paths file name: dataset_entityId1_entityId2.txt" << endl;
        cout << "\t score-function: 1 -> PCRW" << endl;
        cout << endl;
}

bool readSampleFile(string pos_file, string neg_file, vector<pair<int, pair<int, vector<int>>>> & sample_pairs){
	ifstream posSamplesIn(pos_file.c_str(), ios::in);
	ifstream negSamplesIn(neg_file.c_str(), ios::in);
	if(!posSamplesIn.good() || !negSamplesIn.good()){
		cerr << "Error when reading sample files" << endl;
		return false;
	}

	sample_pairs.clear();	
	string pos_line, neg_line;
	while(getline(posSamplesIn, pos_line) && getline(negSamplesIn, neg_line)){
		vector<string> pos_strs = split(pos_line, "\t");
		vector<string> neg_strs = split(neg_line, "\t");	
		if(pos_strs.size() != 2){
			cerr << "Unsupported format for the pair: " << pos_line << endl;
		}else if(strcmp(pos_strs[0].c_str(), neg_strs[0].c_str()) != 0){
			cerr << "Unmatched pairs: " << pos_strs[0] << " and " << neg_strs[0] << endl;
		}else{
			int src = atoi(pos_strs[0].c_str());
			vector<int> neg_dsts;
			vector<string> neg_dst_strs = split(neg_strs[1], ",");	
			for(int i = 0; i < neg_dst_strs.size(); i++){
				neg_dsts.push_back(atoi(neg_dst_strs[i].c_str()));
			}	
			int pos_dst = atoi(pos_strs[1].c_str());
			sample_pairs.push_back(make_pair(src, make_pair(pos_dst, neg_dsts)));
		}

	}
	posSamplesIn.close();
	negSamplesIn.close();
	return true;
		
}

int getHitCount(vector<int> meta_path, vector<pair<int, int>> sample_pairs, HIN_Graph & hin_graph_){

	int hit_count = 0;
	
	for(vector<pair<int, int>>::iterator iter = sample_pairs.begin(); iter != sample_pairs.end(); iter++){
		double pcrw = TopKCalculator::isConnected(iter->first, iter->second, meta_path, hin_graph_);
		if(pcrw != 0){
			hit_count++;	
		}		
	}
	return hit_count;
}

int main(int argc, const char * argv[]) {

	if(argc > 5){
		
		int penalty_type = DEFAULT_PENALTY_TYPE;
                string tfidf_type = DEFAULT_TFIDF_TYPE;
                bool refine_flag = DEFAULT_REFINE_FLAG;
                int score_function = DEFAULT_SCORE_FUNCTION;

                if(strcmp(argv[1], "--default") == 0 || strcmp(argv[1], "-d") == 0){
                        tfidfSetup(tfidf_type.c_str(), penalty_type);
                }else if(strcmp(argv[1], "--advance") == 0 || strcmp(argv[1], "-a") == 0){
                        if(argc > 7){
                                penalty_type = atoi(argv[6]);
                                tfidf_type = argv[7];
                        }
                        tfidfSetup(tfidf_type.c_str(), penalty_type);
                }else if(strcmp(argv[1], "--refine") == 0 || strcmp(argv[1], "-r") == 0){
                        if(argc > 6){
                                score_function = atoi(argv[6]);
                        }
                        refine_flag = true;
                }else{
                        printUsage(argv);
                        return -1;
                }
		
		string dataset, pos_pairs_file_name, neg_pairs_file_name;
        	int k;
        	dataset = argv[2];
        	pos_pairs_file_name = argv[3];
        	neg_pairs_file_name = argv[4];
        	k = atoi(argv[5]);

		HIN_Graph hin_graph_;

        	map<int, vector<Edge>> adj;
        	map<int,string> node_name;
        	map<int,string> node_type_name;
        	map<int,int> node_type_num;
        	map<int,vector<int>> node_id_to_type;
        	map<int,string> edge_name;

        	hin_graph_ = loadHinGraph(dataset.c_str(), node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);

		vector<pair<int, pair<int, vector<int>>>> sample_pairs;
        	if(!readSampleFile(pos_pairs_file_name, neg_pairs_file_name, sample_pairs)){
                	return -1;
        	}
		
		int sample_size = sample_pairs.size();
		vector<double> precision_result (k, 0.0);

		double time_cost = 0.0;
		vector<pair<int, int>> neg_pairs; 

		for(vector<pair<int, pair<int, vector<int>>>>::iterator iter = sample_pairs.begin(); iter != sample_pairs.end(); iter++){
                	int src = iter->first;
                	int dst = iter->second.first;
			vector<int> neg_dsts = iter->second.second;
			neg_pairs.clear();
			for(int i = 0; i < neg_dsts.size();i++){
				neg_pairs.push_back(make_pair(src, neg_dsts[i]));	
			}


                	vector<pair<vector<double>, vector<int>>> topKMetaPaths;
			clock_t t2, t1;
			t1 = clock();
                	if(!refine_flag){
                        	topKMetaPaths = TopKCalculator::getTopKMetaPath_TFIDF(src, dst, k, hin_graph_);
                	}else{
                        	string file_name = getFileName(src, dst, dataset);
                        	vector<vector<int>> meta_paths = TopKCalculator::loadMetaPaths(file_name);
                        	topKMetaPaths = TopKCalculator::getTopKMetaPath_Refiner(src, dst, k, meta_paths, score_function, hin_graph_);
                	}
			t2 = clock();
			time_cost += (double) ((0.0 + t2 - t1)/CLOCKS_PER_SEC);
		
			vector<double> tmp_precision_result;	
			for(vector<pair<vector<double>, vector<int>>>::iterator iter_path = topKMetaPaths.begin(); iter_path != topKMetaPaths.end(); iter_path++){
				
				vector<int> curr_meta_path = iter_path->second;

				int false_positive = getHitCount(curr_meta_path, neg_pairs, hin_graph_); 	
				double precision = 1.0/(1.0 + false_positive);
	
				tmp_precision_result.push_back(precision);	

				for(int i = 0; i < curr_meta_path.size(); i++){
					int curr_edge_type = curr_meta_path[i];
					if(curr_edge_type < 0){
						curr_edge_type = -curr_edge_type;
						if(edge_name.find(curr_edge_type) != edge_name.end()){
							cout << "[" << edge_name[curr_edge_type] << "]^(-1)" << "\t";
						}else{
							cout << "NA" << "\t";
						}
					}else{
						if(edge_name.find(curr_edge_type) != edge_name.end()){
							cout << "[" << edge_name[curr_edge_type] << "]" << "\t";	
						}else{
							cout << "NA" << "\t";
						}
					}
				}	
				cout << endl;	
				cout.precision(4);
                		cout << fixed;
				cout << "precision: " << precision << endl;
				
				
			}			
			cout << endl;
			for(int i = 0; i < k; i++){
				precision_result[i] += tmp_precision_result[i];
			}
        	}

                        
			
		cout << "Average time cost is " << time_cost/sample_size << " seconds" << endl;

		// calculate the average value
		for(int i = 0; i < k; i++){
			precision_result[i] /= sample_size;
		}

		output(precision_result);

		
	}else{
		printUsage(argv);
		return -1;
	}		

	
	return 0;	

}
