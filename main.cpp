#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <time.h>
#include <vector>

#include "HIN_Graph.h"
#include "DBLP_Reader.h"
#include "Article.h"
#include "Meta-Structure.h"
#include "SimCalculator.h"
#include "yagoReader.h"
#include "TopKCalculator.h"

#define MAX_LENGTH 100
#define DEFAULT_PENALTY_TYPE 2
#define DEFAULT_ALGORITHM "TF-IDF"


HIN_Graph loadDBLPGraph(map<int,string> & edge_name) {
	clock_t t1, t2, t3, t4, t5, t6;

        map<int, vector<Edge>> adj;
        map<int,string> node_name;
        map<int,string> node_type_name;
        map<int,int> node_type_num;
        map<int,vector<int>> node_id_to_type;

        t1 = clock();
        YagoReader::readADJ("DBLP/dblpAdj.txt", adj);
        YagoReader::readNodeIdToType("DBLP/dblpTotalType.txt", node_id_to_type);
        YagoReader::readEdgeName("DBLP/dblpType.txt",edge_name);
        t2 = clock();

        cerr << "Take " << (0.0 + t2 - t1)/CLOCKS_PER_SEC << "s to read dblp_four_area" << endl;

        HIN_Graph DBLP_Graph;
        DBLP_Graph.buildYAGOGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
        t3 = clock();
        cerr << "Take " << (0.0 + t3 - t2)/CLOCKS_PER_SEC << "s to change dblp_four_area" << endl;

	return DBLP_Graph;	
}

HIN_Graph loadYagoGraph(map<int,string> & edge_name) {
	clock_t t1, t2, t3;

        map<int, vector<Edge>> adj;
        map<int,string> node_name;
        map<int,string> node_type_name;
        map<int,int> node_type_num;
        map<int,vector<int>> node_id_to_type;

	YagoReader::readADJ("Yago/yagoadj.txt", adj);
        YagoReader::readNodeName("Yago/yagoTaxID.txt",node_name,node_type_name);
        YagoReader::readNodeTypeNum("Yago/yagoTypeNum.txt", node_type_num);
        YagoReader::readNodeIdToType("Yago/totalType.txt", node_id_to_type);
        YagoReader::readEdgeName("Yago/yagoType.txt",edge_name);
        t2 = clock();
        
        cerr << "Take " << (0.0 + t2 - t1)/CLOCKS_PER_SEC << "s to read Yago" << endl;
        
        HIN_Graph Yago_Graph;
        Yago_Graph.buildYAGOGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
        t3 = clock();
        cerr << "Take " << (0.0 + t3 - t2)/CLOCKS_PER_SEC << "s to change Yago" << endl;

	return Yago_Graph;
}

void printUsage(const char* argv[]){
	cout << "Usage: " << endl;
        cout << argv[0] << "--default dataset entityId1 entityId2 k" << endl;
        cout << argv[0] << "--advance dataset entityId1 entityId2 k lengthPenalty algorithm (limitedLength)" << endl;
	cout << endl;
	cout << "--advance mode:" << endl;
        cout << "\t lengthPenalty(l is the meta-path's length): 1 -> 1/log(l); 2 -> 1/l; 3 -> 1/(l^2)" << endl;
        cout << "\t algorithm: TF-IDF, Shortest-Path, PathCount-based, PCRW-based" << endl;
        cout << "\t limitedLength: this parameter is needed only for PathCount-based and PCRW-based algorithm" << endl;
	cout << endl;
	cout << "--default mode:" << endl;
	cout << "\t lengthPenalty -> 1" << endl;
	cout << "\t algorithm -> TF-IDF" << endl;
	cout << endl;
}

int main(int argc, const char * argv[]) {
	clock_t t1, t2;
        
	HIN_Graph hin_graph_;
	map<int,string> edge_name;
	if(argc > 5){
		if(strcmp(argv[1], "--default") != 0 && strcmp(argv[1], "--advance") != 0){
			printUsage(argv);
			return -1;
		}

		if(strcmp(argv[2], "Yago") == 0){
			hin_graph_ = loadYagoGraph(edge_name);
		}else if(strcmp(argv[2], "DBLP") == 0){
			hin_graph_ = loadDBLPGraph(edge_name);
		}else{
			cout << "Unsupported data set" << endl;
			return -1;
		}

		int src, dst, k;
		src = atoi(argv[3]);
                dst = atoi(argv[4]);
                k = atoi(argv[5]); 
	
		int penalty_type = DEFAULT_PENALTY_TYPE;
		string algorithm = DEFAULT_ALGORITHM;
	
		if(strcmp(argv[1], "--advance") == 0){
			int limited_length = MAX_LENGTH;
			if(argc > 7){
				penalty_type = atoi(argv[6]);
				string algorithm = argv[7];	
			}
			
			if(argc == 9 && (strcmp(argv[7], "PathCount-based") == 0 || strcmp(argv[7], "PCRW-based") == 0)){
				limited_length = atoi(argv[8]);
			}
				
		}
		TopKCalculator::penalty_type_ = penalty_type;
		
		t1 = clock();
		vector<pair<vector<double>, vector<int>>> topKMetaPaths = TopKCalculator::getTopKMetaPath_TFIDF(src, dst, k, hin_graph_);
		t2 = clock();
		if(topKMetaPaths.size() > 0){
			bool invalid_edge_type_flag = false;
			cout << "TF-IDF" << "\t" << "tf" << "\t" << "idf" << "\t" << "length" << "\t" << "Meta Path" << endl;
			for(vector<pair<vector<double>, vector<int>>>::iterator iter = topKMetaPaths.begin(); iter != topKMetaPaths.end(); iter++){
				vector<double> tfidf_info = iter->first;
				if(tfidf_info[0] < 1)
					cout.precision(5);
				else
					cout.precision(6);
				cout << tfidf_info[0] << "\t" << tfidf_info[1] << "\t" << tfidf_info[2] << "\t" << iter->second.size() << "\t";
				vector<int> temp_meta_path = iter->second;
				for(vector<int>::iterator iter = temp_meta_path.begin(); iter != temp_meta_path.end(); iter++){
					if(edge_name.find(*iter) == edge_name.end() && edge_name.find(-(*iter)) == edge_name.end()){
						invalid_edge_type_flag = true;
						break;
					}
				}
				if(!invalid_edge_type_flag){
					for(vector<int>::iterator  iter = temp_meta_path.begin(); iter != temp_meta_path.end() - 1; iter++){
						int curr_edge_type = *iter;
						if(curr_edge_type > 0){
							cout << edge_name[curr_edge_type];
						}else{
							cout << "[" << edge_name[-curr_edge_type] << "]^(-1)";
						}
						cout << " -> ";
					}
					int curr_edge_type = temp_meta_path.back();
					if(curr_edge_type > 0){
                                               cout << edge_name[curr_edge_type];
                                        }else{
                                               cout << "[" << edge_name[-curr_edge_type] << "]^(-1)";
                                        }
					cout << endl;
				}else{
					cout << "Found some Invalid Edge Type" << endl;
				}	
					
			}
			cerr << "Take " << (0.0 + t2 - t1)/CLOCKS_PER_SEC << "s to calculate top k meta-paths" << endl;
		}else{
			cout << "No Meta Pah Found in the HIN Graph." << endl;
		}
			
	}else{
		printUsage(argv);
                return -1;
	}
    

    
    return 0;
}
