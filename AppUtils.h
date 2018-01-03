#ifndef __DBLP_Reader__AppUtils__
#define __DBLP_Reader__AppUtils__

#include "HIN_Graph.h"
#include "TopKCalculator.h"

#include <vector>
#include <cstring>
#include <map>

#define DEFAULT_PENALTY_TYPE 2
#define DEFAULT_TFIDF_TYPE "M-S"
#define DEFAULT_REFINE_FLAG false
#define DEFAULT_OUTPUT_TYPE 1
#define DEFAULT_SCORE_FUNCTION 1
#define DEFAULT_OUTPUT_DIR "topKResult"
#define DEFAULT_SAMPLE_SIZE 100

using namespace std;

HIN_Graph loadHinGraph(const char* dataset, map<int,string> & node_name, map<int, vector<Edge>> & adj, map<int,string> & node_type_name, map<int,int> & node_type_num, map<int,vector<int>> & node_id_to_type, map<int,string> & edge_name);

string getFileName(int src, int dst, string dataset);

void tfidfSetup(const char* tfidf_type, int penalty_type, int sample_size=DEFAULT_SAMPLE_SIZE);

#endif
