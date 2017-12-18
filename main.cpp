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

int main(int argc, const char * argv[]) {
        
    clock_t t1, t2, t3, t4, t5, t6;

    map<int, vector<Edge>> adj;
    map<int,string> node_name;
    map<int,string> node_type_name;
    map<int,int> node_type_num;
    map<int,vector<int>> node_id_to_type;
    map<int,string> edge_name;
    
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
        
	if(argc == 4){
		int src, dst, k;
		src = atoi(argv[1]);
                dst = atoi(argv[2]);
                k = atoi(argv[3]); 
		vector<pair<double, vector<int>>> topKMetaPaths = TopKCalculator::getTopKMetaPath_TFIDF(src, dst, k, DBLP_Graph);
		if(topKMetaPaths.size() > 0){
			bool invalid_edge_type_flag = false;
			cout << "TF-IDF" << "\t\t" << "Meta Path" << endl;
			for(vector<pair<double, vector<int>>>::iterator iter = topKMetaPaths.begin(); iter != topKMetaPaths.end(); iter++){
				cout << iter->first << "\t\t";
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
		}else{
			cout << "No Meta Pah Found in the HIN Graph." << endl;
		}
			
	}
    

    
    return 0;
}
