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

void output(vector<vector<double>> resolution_result){
	if(resolution_result.size() > 0){
		cout << "Metrics" << "\t" << "Precision" << "\t" << "Recall" << "\t" << "F1-measre" << endl;
		cout.precision(4);
		cout << fixed;
		for(int i = 0; i < resolution_result.size(); i++){
			cout << i+1;
			for(int m = 0; m < resolution_result[i].size(); m++){
				cout << "\t" << resolution_result[i][m]; 
			}
			cout << endl;	
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

bool readSampleFile(string file_name, vector<pair<int, int> > & sample_pairs){
	ifstream samplesIn(file_name.c_str(), ios::in);
	if(!samplesIn.good()){
		cerr << "Error when reading " << file_name << endl;
		return false;
	}

	sample_pairs.clear();	
	string line;
	while(getline(samplesIn, line)){
		vector<string> strs = split(line, "\t");
		if(strs.size() != 2){
			cerr << "Unsupported format for the pair: " << line << endl;
		}else{
			sample_pairs.push_back(make_pair(atoi(strs[0].c_str()), atoi(strs[1].c_str())));
		}

	}
	samplesIn.close();
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

		vector<pair<int, int> > pos_pairs, neg_pairs;
        	if(!readSampleFile(pos_pairs_file_name, pos_pairs) || !readSampleFile(neg_pairs_file_name, neg_pairs)){
                	return -1;
        	}

		int pos_pairs_size = pos_pairs.size();
		int neg_pairs_size = neg_pairs.size();
        	if(pos_pairs_size != neg_pairs_size){
                	cerr << "Unmatched size between positive pairs and negative pairs" << endl;
                	return -1;
        	}

		vector<vector<double>> resolution_result;
		int metrics_size = 3;
		for(int i = 0; i < k; i++){
			resolution_result.push_back(vector<double> (metrics_size, 0.0));
		}

		for(vector<pair<int, int>>::iterator iter = pos_pairs.begin(); iter != pos_pairs.end(); iter++){
			vector<vector<double>> tmp_resolution_result;
                	int src = iter->first;
                	int dst = iter->second;
                	vector<pair<vector<double>, vector<int>>> topKMetaPaths;
                	if(!refine_flag){
                        	topKMetaPaths = TopKCalculator::getTopKMetaPath_TFIDF(src, dst, k, hin_graph_);
                	}else{
                        	string file_name = getFileName(src, dst, dataset);
                        	vector<vector<int>> meta_paths = TopKCalculator::loadMetaPaths(file_name);
                        	topKMetaPaths = TopKCalculator::getTopKMetaPath_Refiner(src, dst, k, meta_paths, score_function, hin_graph_);
                	}
				
			for(vector<pair<vector<double>, vector<int>>>::iterator iter_path = topKMetaPaths.begin(); iter_path != topKMetaPaths.end(); iter_path++){
				
				vector<double> tmp_result;
				vector<int> curr_meta_path = iter_path->second;

				int true_positive = getHitCount(curr_meta_path, pos_pairs, hin_graph_);
				int false_positive = getHitCount(curr_meta_path, neg_pairs, hin_graph_); 
			
				double precision = true_positive*1.0/(true_positive + false_positive);
				double recall = true_positive*1.0/pos_pairs_size;
				double f1_measure = (1.0 + F1_BETA*F1_BETA*1.0)*precision*recall/(F1_BETA*F1_BETA*precision + recall);

				tmp_result.push_back(precision);
				tmp_result.push_back(recall);
				tmp_result.push_back(f1_measure);
				
				tmp_resolution_result.push_back(tmp_result);	

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
				cout << "precision: " << precision << "\t" << "recall: " << recall << "\t" << "f1_measure: " << f1_measure << endl;
				
				
			}			
			cout << endl;

			for(int j = 0; j < tmp_resolution_result.size(); j++){
				for(int m = 0; m < metrics_size; m++){
					resolution_result[j][m] += tmp_resolution_result[j][m];
				} 
			}
        	}

		// calculate the average value
		for(int i = 0; i < k; i++){
			for(int m = 0; m < metrics_size; m++){
				resolution_result[i][m] /= pos_pairs_size;
			}
		}

		output(resolution_result);

		
	}else{
		printUsage(argv);
		return -1;
	}		

	
	return 0;	

}
