#include "HIN_Graph.h"
#include "AppUtils.h"
#include "TopKCalculator.h"

#include <cstring>
#include <vector>
#include <map>
#include <ctime>
#include <iostream>
using namespace std;

HIN_Graph loadDBLPGraph(map<int,string> & node_name, map<int, vector<Edge>> & adj, map<int,string> & node_type_name, map<int,int> & node_type_num, map<int,vector<int>> & node_id_to_type, map<int,string> & edge_name) {
		clock_t t1, t2, t3;

		t1 = clock();
		YagoReader::readADJ("DBLP/dblpAdj.txt", adj);
		YagoReader::readNodeIdToType("DBLP/dblpTotalType.txt", node_id_to_type);
		YagoReader::readEdgeName("DBLP/dblpType.txt",edge_name);
		t2 = clock();

		cerr << "Take " << (0.0 + t2 - t1)/CLOCKS_PER_SEC << "s to read DBLP dataset" << endl;

		HIN_Graph DBLP_Graph;
		DBLP_Graph.buildYAGOGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
		t3 = clock();
		cerr << "Take " << (0.0 + t3 - t2)/CLOCKS_PER_SEC << "s to construct DBLP graph" << endl;

		return DBLP_Graph;
}

HIN_Graph loadIMDBGraph(map<int,string> & node_name, map<int, vector<Edge>> & adj, map<int,string> & node_type_name, map<int,int> & node_type_num, map<int,vector<int>> & node_id_to_type, map<int,string> & edge_name) {
		clock_t t1, t2, t3;

		t1 = clock();
		YagoReader::readADJ("IMDB/IMDBAdj.txt", adj);
		YagoReader::readNodeIdToType("IMDB/IMDBEntityType.txt", node_id_to_type);
		YagoReader::readEdgeName("IMDB/IMDBEdgeType.txt",edge_name);
		t2 = clock();

		cerr << "Take " << (0.0 + t2 - t1)/CLOCKS_PER_SEC << "s to read IMDB dataset" << endl;

		HIN_Graph IMDB_Graph;
		IMDB_Graph.buildYAGOGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
		t3 = clock();
		cerr << "Take " << (0.0 + t3 - t2)/CLOCKS_PER_SEC << "s to construct IMDB graph" << endl;

		return IMDB_Graph;
}

HIN_Graph loadACMGraph(map<int,string> & node_name, map<int, vector<Edge>> & adj, map<int,string> & node_type_name, map<int,int> & node_type_num, map<int,vector<int>> & node_id_to_type, map<int,string> & edge_name) {
		clock_t t1, t2, t3;

		t1 = clock();
		YagoReader::readADJ("ACM/ACMAdj.txt", adj);
		YagoReader::readNodeName("ACM/ACMEntityName.txt", node_name, node_type_name);
		YagoReader::readNodeIdToType("ACM/ACMEntityType.txt", node_id_to_type);
		YagoReader::readEdgeName("ACM/ACMEdgeType.txt",edge_name);
		t2 = clock();

		cerr << "Take " << (0.0 + t2 - t1)/CLOCKS_PER_SEC << "s to read ACM dataset" << endl;

		HIN_Graph ACM_Graph;
		ACM_Graph.buildYAGOGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
		t3 = clock();
		cerr << "Take " << (0.0 + t3 - t2)/CLOCKS_PER_SEC << "s to construct ACM graph" << endl;

		return ACM_Graph;
}

HIN_Graph loadYagoGraph(map<int,string> & node_name, map<int, vector<Edge>> & adj, map<int,string> & node_type_name, map<int,int> & node_type_num, map<int,vector<int>> & node_id_to_type, map<int,string> & edge_name) {
		clock_t t1, t2, t3;


		YagoReader::readADJ("Yago/yagoadj.txt", adj);
		YagoReader::readNodeName("Yago/yagoTaxID.txt",node_name,node_type_name);
		YagoReader::readNodeTypeNum("Yago/yagoTypeNum.txt", node_type_num);
		YagoReader::readNodeIdToType("Yago/totalType.txt", node_id_to_type);
		YagoReader::readEdgeName("Yago/yagoType.txt",edge_name);
		t2 = clock();

		cerr << "Take " << (0.0 + t2 - t1)/CLOCKS_PER_SEC << "s to read Yago dataset" << endl;

		HIN_Graph Yago_Graph;
		Yago_Graph.buildYAGOGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
		t3 = clock();
		cerr << "Take " << (0.0 + t3 - t2)/CLOCKS_PER_SEC << "s to build Yago graph" << endl;

		return Yago_Graph;
}

HIN_Graph loadHinGraph(const char* dataset, map<int,string> & node_name, map<int, vector<Edge>> & adj, map<int,string> & node_type_name, map<int,int> & node_type_num, map<int,vector<int>> & node_id_to_type, map<int,string> & edge_name){
	if(strcmp(dataset, "Yago") == 0){
		return loadYagoGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
	}else if(strcmp(dataset, "DBLP") == 0){
		return loadDBLPGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
	}else if(strcmp(dataset, "ACM") == 0){
		return loadACMGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
	}else if(strcmp(dataset, "IMDB") == 0){
		return loadIMDBGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name); 
	}else{
		cerr << "Unsupported dataset" << endl;
		return HIN_Graph ();
	}
}

string getFileName(int src, int dst, string dataset){
		return string(DEFAULT_OUTPUT_DIR) + "/" + dataset + "_" + to_string(src) + "_" + to_string(dst) + ".txt";
}

void tfidfSetup(const char* tfidf_type, int penalty_type, int sample_size){

		TopKCalculator::penalty_type_ = penalty_type;
	TopKCalculator::sample_size_ = sample_size;

		if(strcmp(tfidf_type, "M-S") == 0){
				TopKCalculator::support_type_ = 1;
				TopKCalculator::rarity_type_ = 1;
		}else if(strcmp(tfidf_type, "B-S") == 0){
				TopKCalculator::support_type_ = 0;
				TopKCalculator::rarity_type_ = 1;
		}else if(strcmp(tfidf_type, "P-S") == 0){
				TopKCalculator::support_type_ = 2;
				TopKCalculator::rarity_type_ = 1;
		}else if(strcmp(tfidf_type, "SP") == 0){
				TopKCalculator::support_type_ = 0;
				TopKCalculator::rarity_type_ = 0;
		}
}
