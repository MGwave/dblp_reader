#include "DBLP_Reader.h"
#include "Meta-Structure.h"
#include "TopKCalculator.h"
#include "AppUtils.h"
#include "CommonUtils.h"

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <ctime>

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
        cout << argv[0] << " --recommend dataset (positive pairs file name) (entity label file name) k length-penalty TF-IDF-type" << endl;
        cout << argv[0] << " --classifier dataset (positive pairs file name) (negative pairs file name) k length-penalty TF-IDF-type" << endl;
        cout << argv[0] << " --refine_recommend dataset (positive pairs file name) (entity label file name) k score-function" << endl;
        cout << argv[0] << " --refine_classifier dataset (positive pairs file name) (negative pairs file name) k score-function" << endl;
        cout << endl;

        cout << "recommend && classifier mode:" << endl;
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


        cout << "refine_classifier && refine_recommend mode:" << endl;
        cout << "\t refine k meta-paths from previous generated meta-paths" << endl;
        cout << "\t default meta-paths file name: dataset_entityId1_entityId2.txt" << endl;
        cout << "\t score-function: 1 -> PCRW" << endl;
        cout << endl;
}

bool readClassifierSampleFile(string pos_file, string neg_file, vector<pair<int, int>> & pos_pairs, vector<pair<int, vector<int>>> & neg_pairs){
	ifstream posSamplesIn(pos_file.c_str(), ios::in);
	ifstream negSamplesIn(neg_file.c_str(), ios::in);
	if(!posSamplesIn.good() || !negSamplesIn.good()){
		cerr << "Error when reading sample files" << endl;
		return false;
	}

	pos_pairs.clear();	
	neg_pairs.clear();
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

			int pos_dst = atoi(pos_strs[1].c_str());
			pos_pairs.push_back(make_pair(src, pos_dst));

			vector<int> neg_dsts;
			vector<string> neg_dst_strs = split(neg_strs[1], ",");	
			for(int i = 0; i < neg_dst_strs.size(); i++){
				neg_dsts.push_back(atoi(neg_dst_strs[i].c_str()));
			}	
			neg_pairs.push_back(make_pair(src, neg_dsts));
		}

	}
	posSamplesIn.close();
	negSamplesIn.close();
	return true;
		
}

bool readRecommendSampleFile(string pos_file, string labels_file, vector<pair<int, int>> & pos_pairs, map<int, set<int>> & labeled_entities){
	string line;	
	pos_pairs.clear();
	labeled_entities.clear();
	ifstream posSamplesIn(pos_file.c_str(), ios::in);
	if(!posSamplesIn.good()){
		cerr << "Error when reading positive pairs files" << endl;
                return false;
	}
	while(getline(posSamplesIn, line)){
		vector<string> strs = split(line, "\t");	
		if(strs.size() != 2){
			cerr << "Unsupported format for the pair: " << line << endl;
		}else{
			int src = atoi(strs[0].c_str());
			int dst = atoi(strs[1].c_str());
			pos_pairs.push_back(make_pair(src, dst));
		}
	}
	posSamplesIn.close();
	
	cout << labels_file << endl;
	ifstream labelsEntitiesIn(labels_file.c_str(), ios::in);
	if(!labelsEntitiesIn.good()){
		cerr << "Error when reading label files" << endl;
		return false;
	}	
	while(getline(labelsEntitiesIn, line)){
		vector<string> strs = split(line, "\t");
		if(strs.size() != 2){
                        cerr << "Unsupported format for the labeled entity: " << line << endl;
		}else{
			int eid = atoi(strs[0].c_str());
			int label = atoi(strs[1].c_str());
			if(labeled_entities.find(label) == labeled_entities.end()){
				labeled_entities[label] = set<int>();
			}
			labeled_entities[label].insert(eid);
			
		}
	}
	labelsEntitiesIn.close();
	
	return true;
}

double getClassifierPrecision(vector<int> meta_path, vector<pair<int, int>> sample_pairs, HIN_Graph & hin_graph_){

	int hit_count = 0; 
	for(vector<pair<int, int>>::iterator iter = sample_pairs.begin(); iter != sample_pairs.end(); iter++){
		if(TopKCalculator::isConnected(iter->first, iter->second, meta_path, hin_graph_)){
			hit_count++;	
		}		
	}
	return 1.0/(1.0 + hit_count);
}



double getRecommendPrecision(vector<int> meta_path, int src, set<int> true_entities, HIN_Graph & hin_graph_){
	set<int> dst_entities;
	TopKCalculator::getDstEntities(src, meta_path, dst_entities, hin_graph_);
	
	int hit_count = 0;
	for(set<int>::iterator iter = dst_entities.begin(); iter != dst_entities.end(); iter++){
		if(true_entities.find(*iter) != true_entities.end()){
			hit_count++;
		}
	}	
	return hit_count*1.0/dst_entities.size();
}

int main(int argc, const char * argv[]) {

	if(argc > 5){
		
		int penalty_type = DEFAULT_PENALTY_TYPE;
                string tfidf_type = DEFAULT_TFIDF_TYPE;
                bool refine_flag = DEFAULT_REFINE_FLAG;
                int score_function = DEFAULT_SCORE_FUNCTION;
		string test_type;

                if(strcmp(argv[1], "--classifier") == 0 || strcmp(argv[1], "-c") == 0){
			if(argc > 7){
                                penalty_type = atoi(argv[6]);
                                tfidf_type = argv[7];
                        }
                        tfidfSetup(tfidf_type.c_str(), penalty_type);
			test_type = "classifier"; 
			
                }else if(strcmp(argv[1], "--recommend") == 0 || strcmp(argv[1], "-r") == 0){
                        if(argc > 7){
                                penalty_type = atoi(argv[6]);
                                tfidf_type = argv[7];
                        }
                        tfidfSetup(tfidf_type.c_str(), penalty_type);
			test_type = "recommend";
                }else if(strcmp(argv[1], "--refine_classifier") == 0 || strcmp(argv[1], "-rc") == 0){
                        if(argc > 6){
                                score_function = atoi(argv[6]);
                        }
                        refine_flag = true;
			test_type = "classifier";
		}else if(strcmp(argv[1], "--refine_recommend") == 0 || strcmp(argv[1], "-rr") == 0){
			if(argc > 6){
                                score_function = atoi(argv[6]);
                        }
                        refine_flag = true;
			test_type = "recommend";
                }else{
                        printUsage(argv);
                        return -1;
                }
		
		string dataset, pos_pairs_file_name;
        	int k;
        	dataset = argv[2];
        	pos_pairs_file_name = argv[3];
        	k = atoi(argv[5]);

		HIN_Graph hin_graph_;

        	map<int, vector<Edge>> adj;
        	map<int,string> node_name;
        	map<int,string> node_type_name;
        	map<int,int> node_type_num;
        	map<int,vector<int>> node_id_to_type;
        	map<int,string> edge_name;

        	hin_graph_ = loadHinGraph(dataset.c_str(), node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);

		vector<pair<int, int>> pos_pairs;

		vector<pair<int, vector<int>>> neg_entities;
        	if(strcmp(test_type.c_str(), "classifier") == 0 && !readClassifierSampleFile(pos_pairs_file_name, argv[4], pos_pairs, neg_entities)){
                	return -1;
        	}

		map<int, set<int>> labeled_entities;
		if(strcmp(test_type.c_str(), "recommend") == 0 && !readRecommendSampleFile(pos_pairs_file_name, argv[4], pos_pairs, labeled_entities)){
			return -1;
		} 
		
		int sample_size = pos_pairs.size();
		vector<double> precision_result (k, 0.0);

		double time_cost = 0.0;


		vector<pair<int, int>> neg_pairs; 
		set<int> candidate_entities;

		for(int i = 0; i < pos_pairs.size(); i++){
                	int src = pos_pairs[i].first;
                	int dst = pos_pairs[i].second;

			if(strcmp(test_type.c_str(), "classifier") == 0){
				vector<int> neg_dsts = neg_entities[i].second;
				neg_pairs.clear();
                        	for(int i = 0; i < neg_dsts.size();i++){
                                	neg_pairs.push_back(make_pair(src, neg_dsts[i]));
                        	}
			}else if(strcmp(test_type.c_str(), "recommend") == 0){
				for(map<int, set<int>>::iterator iter = labeled_entities.begin(); iter != labeled_entities.end(); iter++){
					if(iter->second.find(src) != iter->second.end()){
						candidate_entities = iter->second;
						break;
					}
				}	

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
	
				double precision;
				if(strcmp(test_type.c_str(), "classifier") == 0){
					precision = getClassifierPrecision(curr_meta_path, neg_pairs, hin_graph_);	
				}else if(strcmp(test_type.c_str(), "recommend") == 0){
					precision = getRecommendPrecision(curr_meta_path, src, candidate_entities, hin_graph_);
				}
	
				tmp_precision_result.push_back(precision);	

				for(int j = 0; j < curr_meta_path.size(); j++){
					int curr_edge_type = curr_meta_path[j];
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
			topKMetaPaths.clear();
			for(int j = 0; j < k; j++){
				precision_result[j] += tmp_precision_result[j];
			}
        	}

                        
			
		cout << "Average time cost is " << time_cost/sample_size << " seconds" << endl;

		// calculate the average value
		for(int j = 0; j < k; j++){
			precision_result[j] /= sample_size;
		}

		output(precision_result);

		
	}else{
		printUsage(argv);
		return -1;
	}		

	
	return 0;	

}
