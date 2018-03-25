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

void output(vector<double> result, string metric){
	cout << endl;
	if(result.size() > 0){
		cout << "Metrics" << "\t" << metric << "\t" << metric << "@k" << endl;
		cout.precision(6);
		cout << fixed;
		for(int i = 0; i < result.size(); i++){
			cout << i+1 << "\t" << result[i] << "\t";
			double sum = 0.0;
			for(int j = 0; j <= i; j++){
				sum += result[j]; 
			}
			cout << sum/(i + 1) << endl;	
		}

	}else{
		cerr << "No Meta Pah Found in the HIN Graph." << endl;
	}

}

void printUsage(const char* argv[]){
	cout << "Usage: " << endl;
	cout << argv[0] << " --recommend dataset (positive pairs file name) (entity label file name) k TF-IDF-type length-penalty (beta)" << endl;
	cout << argv[0] << " --classifier dataset (positive pairs file name) (negative pairs file name) k TF-IDF-type length-penalty (beta)" << endl;
	cout << argv[0] << " --refine_recommend dataset (positive pairs file name) (entity label file name) k score-function" << endl;
	cout << argv[0] << " --refine_classifier dataset (positive pairs file name) (negative pairs file name) k score-function" << endl;
	cout << endl;

	cout << "\t TF-IDF-type:" << endl;
	cout << "\t\t M-S -> MNI-based Support" << endl;
	cout << "\t\t S-M-S -> Strength-based M-S" << endl;
	cout << "\t\t B-S -> Binary-based Support" << endl;
	cout << "\t\t S-B-S -> Strength-based Binary Support"<< endl;
	cout << "\t\t P-S -> PCRW-based Support" << endl;
	cout << "\t\t SLV1 -> Strength & Length based Version 1" << endl;
        cout << "\t\t SLV2 -> Strength & Length based Version 2" << endl;
	cout << "\t\t SP -> Shortest Path" << endl;
	cout << "\t\t S-SP -> Strengrg-based Shortest Path" << endl;
	
	cout << "recommend && classifier mode:" << endl;
	cout << "\t length-penalty(l is the meta-path's length): " << endl;
	cout << "\t\t 1 -> 1/beta^l" << endl;
	cout << "\t\t 2 -> 1/factorial(l)" << endl;
	cout << "\t\t 3 -> 1/l" << endl;
	cout << "\t\t 4 -> 1/e^l" << endl;

	cout << endl;


	cout << "refine_classifier && refine_recommend mode:" << endl;
	cout << "\t refine k meta-paths from previous generated meta-paths" << endl;
	cout << "\t default meta-paths file name: dataset_entityId1_entityId2.txt" << endl;
	cout << "\t score-function: 1 -> PCRW" << endl;
	cout << endl;
}

bool readClassifierSampleFile(string pos_file, string neg_file, vector<pair<int, int>> & pos_pairs, vector<pair<int, int>> & neg_pairs){
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
		}else{
			int pos_src = atoi(pos_strs[0].c_str());
			int pos_dst = atoi(pos_strs[1].c_str());
			pos_pairs.push_back(make_pair(pos_src, pos_dst));

			int neg_src = atoi(neg_strs[0].c_str());	
			int neg_dst = atoi(neg_strs[1].c_str());
			neg_pairs.push_back(make_pair(neg_src, neg_dst));
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

pair<double, double> getClassifierPrecisionAndRecall(vector<int> meta_path, vector<pair<int, int>> pos_pairs, vector<pair<int, int>> neg_pairs, HIN_Graph & hin_graph_){

	int pos_hit_count = 0; 
	int neg_hit_count = 0;
	for(vector<pair<int, int>>::iterator iter = pos_pairs.begin(); iter != pos_pairs.end(); iter++){
		if(TopKCalculator::isConnected(iter->first, iter->second, meta_path, hin_graph_)){
			pos_hit_count++;	
		}		
	}
	for(vector<pair<int, int>>::iterator iter = neg_pairs.begin(); iter != neg_pairs.end(); iter++){
		if(TopKCalculator::isConnected(iter->first, iter->second, meta_path, hin_graph_)){
			neg_hit_count++;	
		}		
	}
	return make_pair(pos_hit_count*1.0/(pos_hit_count + neg_hit_count), pos_hit_count*1.0/pos_pairs.size());
}



pair<double, double> getRecommendPrecisionAndRecall(vector<int> meta_path, int src, set<int> true_entities, HIN_Graph & hin_graph_){
	set<int> dst_entities;
	TopKCalculator::getDstEntities(src, meta_path, dst_entities, hin_graph_);
	
	int hit_count = 0;
	for(set<int>::iterator iter = dst_entities.begin(); iter != dst_entities.end(); iter++){
		if(true_entities.find(*iter) != true_entities.end()){
			hit_count++;
		}
	}	
	return make_pair(hit_count*1.0/dst_entities.size(), hit_count*1.0/true_entities.size());
}

int main(int argc, const char * argv[]) {

	if(argc > 5){
		
		int penalty_type = DEFAULT_PENALTY_TYPE;
		double beta = DEFAULT_BETA;
		string tfidf_type = DEFAULT_TFIDF_TYPE;
		bool refine_flag = DEFAULT_REFINE_FLAG;
		int score_function = DEFAULT_SCORE_FUNCTION;
		string test_type;

				if(strcmp(argv[1], "--classifier") == 0 || strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--recommend") == 0 || strcmp(argv[1], "-r") == 0){
					if(strcmp(argv[1], "--classifier") == 0 || strcmp(argv[1], "-c") == 0){
						test_type = "classifier";
					}else{
						test_type = "recommend";
					}
					if(argc > 7){
						tfidf_type = argv[6];
						penalty_type = atoi(argv[7]);
						if(penalty_type != 1 && penalty_type != 2){
							cerr << "The penalty_type parameter must be 1 or 2" << endl;
							return -1;
                                		}
					}
						
					if(argc > 8 && penalty_type == 1){
						beta = atof(argv[8]);
						if(beta <= 0 || beta >= 1){ 
							cerr << "The beta parameter must be greater than 0 and less than 1" << endl; 
							return -1;
						}							
					}
					tfidfSetup(tfidf_type.c_str(), penalty_type, beta);
			
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

			 if(strcmp(tfidf_type.c_str(), "S-M-S") == 0 || strcmp(tfidf_type.c_str(), "S-B-S") == 0|| strcmp(tfidf_type.c_str(), "SLV1") == 0 || strcmp(tfidf_type.c_str(), "SLV2") == 0 ){	
				loadMetaInfo(dataset, hin_graph_);
			}
		vector<pair<int, int>> pos_pairs;

		vector<pair<int, int>> neg_pairs;
			if(strcmp(test_type.c_str(), "classifier") == 0 && !readClassifierSampleFile(pos_pairs_file_name, argv[4], pos_pairs, neg_pairs)){
					return -1;
			}

		map<int, set<int>> labeled_entities;
		if(strcmp(test_type.c_str(), "recommend") == 0 && !readRecommendSampleFile(pos_pairs_file_name, argv[4], pos_pairs, labeled_entities)){
			return -1;
		} 
		
		int sample_size = pos_pairs.size();
		vector<double> precision_result (k, 0.0);
		vector<double> recall_result (k, 0.0);
		vector<double> f1_result (k, 0.0);

		double time_cost = 0.0;


		set<int> candidate_entities;

		for(int i = 0; i < pos_pairs.size(); i++){
					int src = pos_pairs[i].first;
					int dst = pos_pairs[i].second;

			if(strcmp(test_type.c_str(), "recommend") == 0){
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
			double curr_time_cost = (double) ((0.0 + t2 - t1)/CLOCKS_PER_SEC);
			cout << "Time cost for this pair is " << curr_time_cost << " seconds" << endl;
			time_cost += curr_time_cost;
	
			vector<double> tmp_precision_result;	
			vector<double> tmp_recall_result;
			for(vector<pair<vector<double>, vector<int>>>::iterator iter_path = topKMetaPaths.begin(); iter_path != topKMetaPaths.end(); iter_path++){
				
				vector<int> curr_meta_path = iter_path->second;
	
				double precision, recall;
				if(strcmp(test_type.c_str(), "classifier") == 0){
					pair<double, double> result = getClassifierPrecisionAndRecall(curr_meta_path, pos_pairs, neg_pairs, hin_graph_);	
					precision = result.first;
					recall = result.second;
				}else if(strcmp(test_type.c_str(), "recommend") == 0){
					pair<double, double> result = getRecommendPrecisionAndRecall(curr_meta_path, src, candidate_entities, hin_graph_);
					precision = result.first;
					recall = result.second;
				}
	
				tmp_precision_result.push_back(precision);	
				tmp_recall_result.push_back(recall);
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
				cout << "recall: " << recall << endl;
								
				
			}			
			cout << endl;
			topKMetaPaths.clear();
			for(int j = 0; j < k; j++){
				precision_result[j] += tmp_precision_result[j];
				recall_result[j] += tmp_recall_result[j];
			}
		}

						
			
		cout << "Average time cost is " << time_cost/sample_size << " seconds" << endl;

		// calculate the average value
		for(int j = 0; j < k; j++){
			precision_result[j] /= sample_size;
			recall_result[j] /= sample_size;
			f1_result[j] = recall_result[j]*precision_result[j]*(F1_BETA*F1_BETA+1)/(F1_BETA*F1_BETA*precision_result[j] + recall_result[j]);
		}

		output(precision_result, "Precision");
		output(recall_result, "Recall");
		output(f1_result, "F1-score");	

		
	}else{
		printUsage(argv);
		return -1;
	}		

	
	return 0;	

}
