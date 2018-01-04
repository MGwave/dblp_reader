#ifndef __DBLP_Reader__TopKCalculator__
#define __DBLP_Reader__TopKCalculator__

#include <vector>
#include <map>
#include <set>
#include <cstring>

#include "HIN_Graph.h"


using namespace std;

class TfIdfNode
{
public:
	int curr_edge_type_;
	map<int, set<int>> curr_nodes_with_parents_;// node id -> parent node ids
	double max_support_;
	vector<int> meta_path_;
	TfIdfNode* parent_;
	TfIdfNode(int curr_edge_type, map<int, set<int>> curr_nodes_with_parents, double max_support, vector<int> meta_path, TfIdfNode* parent);

};

class TfIdfNodePointerCmp
{
public:
	bool operator () (TfIdfNode* & node_p1, TfIdfNode* & node_p2);
};

class TopKCalculator
{
private:
	static set<int> getSimilarNodes(int eid, map<int, HIN_Node> & hin_nodes_, bool sample_flag=false);
	static double getRarity(int similarPairsSize, set<int> & srcSimilarNodes, set<int> & dstSimilarNodes, vector<int> meta_path, HIN_Graph & hin_graph_);
	static void updateTopKMetaPaths(double tfidf, double tf, double rarity, vector<int> meta_path, int k, vector<pair<vector<double>, vector<int>>> & topKMetaPath_);
	static double getSupport(int src, int dst, TfIdfNode* curr_tfidf_node, vector<int> meta_path, HIN_Graph & hin_graph_);
	static double getMaxSupport(double candidateSupport);
	static double getMetaPathScore(int src, int dst, vector<int> meta_path, int score_function, HIN_Graph & hin_graph_);
	static void getNextEntities(int eid, int edge_type, set<int> & next_entities, HIN_Graph & hin_graph_);
	static bool isConnectedMain(int src, int dst, set<int> src_next_entities, set<int> dst_next_entities, vector<int> meta_path, HIN_Graph & hin_graph_);
	static double getPCRWMain(int src, int dst, set<int> src_next_entities, set<int> dst_next_entities, vector<int> meta_path, HIN_Graph & hin_graph_);
	static double getPCRW_DFS(int src, int dst, vector<int> meta_path, HIN_Graph & hin_graph_);
public:
	static int penalty_type_;
	static int rarity_type_;// 1 -> true rarity; 0 -> 1(constant)
	static int support_type_;// 1 -> MNI; 2 -> PCRW; 0 -> 1(constant)
	static int sample_size_;
	static double penalty(int length);
	static bool isConnected(int src, int dst, vector<int> meta_path, HIN_Graph & hin_graph_);
	static double getPCRW(int src, int dst, vector<int> meta_path, HIN_Graph & hin_graph_);
	static vector<pair<vector<double>, vector<int>>> getTopKMetaPath_TFIDF(int src, int dst, int k, HIN_Graph & hin_graph_);	
	static vector<pair<vector<double>, vector<int>>> getTopKMetaPath_Refiner(int src, int dst, int k, vector<vector<int>> & meta_paths, int score_function, HIN_Graph & hin_graph_);
	static void saveToFile(vector<vector<int>> topKMetaPaths, string file_name);
	static vector<vector<int>> loadMetaPaths(string file_name);
};

#endif
