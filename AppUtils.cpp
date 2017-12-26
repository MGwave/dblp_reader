#include "HIN_Graph.h"
#include "AppUtils.h"

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

        cerr << "Take " << (0.0 + t2 - t1)/CLOCKS_PER_SEC << "s to read dblp_four_area" << endl;

        HIN_Graph DBLP_Graph;
        DBLP_Graph.buildYAGOGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
        t3 = clock();
        cerr << "Take " << (0.0 + t3 - t2)/CLOCKS_PER_SEC << "s to change dblp_four_area" << endl;

        return DBLP_Graph;
}

HIN_Graph loadYagoGraph(map<int,string> & node_name, map<int, vector<Edge>> & adj, map<int,string> & node_type_name, map<int,int> & node_type_num, map<int,vector<int>> & node_id_to_type, map<int,string> & edge_name) {
        clock_t t1, t2, t3;


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

HIN_Graph loadHinGraph(const char* dataset, map<int,string> & node_name, map<int, vector<Edge>> & adj, map<int,string> & node_type_name, map<int,int> & node_type_num, map<int,vector<int>> & node_id_to_type, map<int,string> & edge_name){
	if(strcmp(dataset, "Yago") == 0){
		return loadYagoGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
	}else if(strcmp(dataset, "DBLP") == 0){
		return loadDBLPGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
	}else{
		cerr << "Unsupported dataset" << endl;
		return HIN_Graph ();
	}
}
