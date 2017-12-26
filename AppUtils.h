#ifndef __DBLP_Reader__AppUtils__
#define __DBLP_Reader__AppUtils__

#include "HIN_Graph.h"

#include <vector>
#include <cstring>
#include <map>
using namespace std;

HIN_Graph loadHinGraph(const char* dataset, map<int,string> & node_name, map<int, vector<Edge>> & adj, map<int,string> & node_type_name, map<int,int> & node_type_num, map<int,vector<int>> & node_id_to_type, map<int,string> & edge_name);

#endif
