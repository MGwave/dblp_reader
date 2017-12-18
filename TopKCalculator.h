#ifndef __DBLP_Reader__TopKCalculator__
#define __DBLP_Reader__TopKCalculator__

#include <vector>
#include <map>
#include <set>

#include "HIN_Graph.h"

using namespace std;

class TfIdfNode
{
public:
	int curr_edge_type_;
	set<int> curr_nodes_;
	int min_instances_num_;
	vector<int> meta_path_;
	set<int> reached_nodes_;
	bool found_;
	TfIdfNode(int curr_edge_type, set<int> curr_nodes, int min_instances_num, vector<int> meta_path, set<int> reached_nodes);

};

class TfIdfNodeCmp
{
public:
	bool operator () (TfIdfNode & node1, TfIdfNode & node2);
};

class TopKCalculator
{
private:
	static set<int> getSimilarNodes(int eid, map<int, HIN_Node> & hin_nodes_);
	static double getRarity(int similarPairsSize, set<int> & srcSimilarNodes, set<int> & dstSimilarNodes, vector<int> meta_path, HIN_Graph & hin_graph_);
	static void updateTopKMetaPaths(double tfidf, vector<int> meta_path, int k, vector<pair<double, vector<int>>> & topKMetaPath_);
public:
	static double penalty(int length);
	static vector<pair<double, vector<int>>> getTopKMetaPath_TFIDF(int src, int dst, int k, HIN_Graph & hin_graph_);
};

#endif
