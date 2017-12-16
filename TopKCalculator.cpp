#include "TopKCalculator.h"

#include <vector>
#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>

using namespace std;

TfIdfNode::TfIdfNode(int curr_edge_type, set<int> curr_nodes, int min_instances_num, vector<int> meta_path):curr_edge_type_(curr_edge_type), curr_nodes_(curr_nodes), min_instances_num_(min_instances_num), meta_path_(meta_path){}

bool TfIdfNodeCmp::operator () (TfIdfNode & node1, TfIdfNode & node2)
{
	return node1.min_instances_num_*(TopKCalculator::penalty(node1.meta_path_.size())) > node2.min_instances_num_*(TopKCalculato::penalty(node2.meta_path_.size()));
}

double TopKCalculator::penalty(int length)
{
	return 1.0/length;
}

set<int> TopKCalculator::getSimilarNodes(int eid, HIN_Graph & hin_graph_)
{
	set<int> similarNodes;
	map<int, HIN_Node> hin_nodes_ = hin_graph_.nodes_;

	if(hin_nodes_.find(eid) == hin_nodes_.end()){
		return make_pair(0, similarNodesMap);
	}

	curr_node_types_id_ = hin_nodes_[eid].types_id_;

	for(map<int, HIN_Node>::iterator i = hin_nodes_.begin(); i != hin_nodes_.end(); i++){
		int temp_node_id = i->first;
		if(temp_node_id == eid){
			continue;
		}
		HIN_Node tempNode = i->second;
		vector<int> tempNode_types_id = tempNode.types_id_;
		for(vector<int>::iterator j = tempNode_types_id.begin(); j != tempNode_types_id.end(); j++){
			int temp_node_type = *j;
			vector<int>::iterator iter = find(curr_node_types_id_.begin(), curr_node_types_id_.end(), temp_node_type);
			if(iter != temp_node_type.end()){
				similarNodes.insert(temp_node_id);
				break;
			}
		}
	}

	return similarNodes;
}

double TopKCalculator::getRarity(int similarPairsSize, set<int> & srcSimilarNodes, set<int> & dstSimilarNodes, vector<int> meta_path, HIN_Graph & hin_graph_){

	map<int, vector<HIN_Edge> > hin_edges_src_ = hin_graph_.edges_src_;

	set<int> currNodes = srcSimilarNodes;
	set<int> nextNodes;
	for(vector<int>::iterator: iter = meta_path.begin(); iter != meta_path.end(); iter++){
		int curr_edge_type = *iter;
		for(vector<int>::iterator: i = currNodes.begin(); i != currNodes.end(); i++){
			int currNode = *i;
			if(hin_edges_src_.find(currNode) != end()){
				vector<HIN_Edge> tempEdges = hin_edges_src_[currNode];
				for(vector<HIN_Edge>::iterator: j = tempEdges.begin(); j != tempEdges.end(); j++){
					if(j->edge_type_ == curr_edge_type){
						nextNodes.insert(j->dst_);
						break;
					}
				}
			}
		}
		currNodes = nextNodes;
		nextNodes.clear();
	}

	int hit = 0;
	set<int> intersect;
	set_intersection(currNodes.begin(),currNodes.end(),dstSimilarNodes.begin(),dstSimilarNodes.end(), inserter(intersect,intersect.begin()));

	return log (similarPairsSize*1.0/intersect.size());
}

void TopKCalculator:: updateTopKMetaPaths(double tfidf, vector<int> meta_path, int k, vector<pair<double, vector<int>>> & topKMetaPath_){
	if(topKMetaPath_.size() == 0){
		topKMetaPath_.push_back(make_pair(tfidf, meta_path));
	}else{
		for(vector<pair<double, vector<int>>>::iterator iter = topKMetaPath_.begin(); iter != topKMetaPath_.end() - 1; iter++){
			if(*iter.first < tfidf){
				topKMetaPath_.insert(make_pair(tfidf, meta_path));
			}
		}
	}

	if(topKMetaPath_.size() > k){
		topKMetaPath_.erase(topKMetaPath_.end() - 1);
	}
}

vector<pair<double, vector<int>>> TopKCalculator::getTopKMetaPath_TFIDF(int src, int dst, int k, HIN_Graph & hin_graph_)
{
	vector<pair<double, vector<int>>> topKMetaPath_;
	map<int, vector<HIN_Edge> > hin_edges_src_ = hin_graph_.edges_src_;
	if(hin_edges_src_.find(src) == hin_edges_src_.end() || hin_edges_src_.find(dst) == hin_edges_src_.end())
	{
		return topKMetaPath_;
	}

	// similar pairs information
	set<int> srcSimilarNodes = getSimilarNodes(src, hin_graph_);
	set<int> dstSimilarNodes = getSimilarNodes(dst, hin_graph_);
	int similarPairsSize = srcSimilarNodes.size()*dstSimilarNodesInfo.first.size();
	double maxRarity = log (similarPairsSize);

	// queue initialize
	priority_queue<TfIdfNode, vector<TfIdfNode>, TfIdfNodeCmp> q;
	vector<HIN_Edge> curr_edges_src_ = hin_edges_src_[src];
	map<int, set<int>> curr_edges_dsts_; // edge type -> destinations
	for(vector<HIN_Edge>::iterator: iter = curr_edges_src_.begin() ; iter != curr_edges_src_.end(); iter++){
		int temp_edge_type_ = iter->edge_type_;
		if(curr_edges_dsts_.find(temp_edge_type_) == curr_edges_dsts_.end()){
			curr_edges_dsts_[temp_edge_type_] = set<int>();
		}
		curr_edges_dsts_[temp_edge_type_].insert(iter->dst_);
	}
	for(map<int, set<int>>::iterator: iter = curr_edges_dsts_.begin(); iter != curr_edges_dsts_.end(); iter++){
		int curr_edge_type = iter->edge_type_;
		set<int> next_nodes = iter->second;
		vector<int> meta_path (1, curr_edge_type);
		if(next_nodes.find(dst) != next_nodes.end()){
			double rarity = getRarity(similarPairsSize, srcSimilarNodes, dstSimilarNodes, meta_path, hin_graph_);
			double tfidf = curr_min_instances_num*rarity*penalty(meta_path.size());
			updateTopKMetaPaths(tfidf, meta_path, k, topKMetaPath_);
		}else{
			q.push(TfIdfNode(curr_edge_type, next_nodes, next_nodes.size(),meta_path));
		}
	}

	// BFS
	while(!q.empty()){
		TfIdfNode curr_tfidf_node = q.front();
		q.pop();

		vector<int> meta_path = curr_tfidf_node.meta_path_;
		int curr_min_instances_num = curr_tfidf_node.min_instances_num_;
		set<int> curr_nodes = curr_tfidf_node.curr_nodes_;

		if(topKMetaPath_.size() == k){ // stop if maximum tfidf in the queue is less than the minimum one in found meta paths
			if(curr_min_instances_num*penalty(meta_path.size())*maxRarity <= topKMetaPath_.back().first){
				break;
			}	
		}

		//expand
		curr_edges_dsts_.clear(); // edge type -> destinations 
		for(set<int>::iterator: iter = curr_nodes.begin(); iter != curr_nodes.end(); iter++){
			int temp_node = *iter;
			if(hin_edges_src_.find(temp_node) == hin_edges_src_.end()){
				continue;
			}
			vector<HIN_Edge> temp_edges_src_ = hin_edges_src_[temp_node];
			for(vector<HIN_Edge>::iterator: i = temp_edges_src_.begin(); i != temp_edges_src_.end(); i++){
				int temp_edge_type_ = i->edge_type_;
				if(curr_edges_dsts_.find(temp_edge_type_) == curr_edges_dsts_.end()){
					curr_edges_dsts_[temp_edge_type_] = set<int>();
				}
				curr_edges_dsts_[temp_edge_type_].insert(iter->dst_);
			}
		}

		for(map<int, set<int>>::iterator: iter = curr_edges_dsts_.begin(); iter != curr_edges_dsts_.end(); iter++){
			int curr_edge_type = iter->edge_type_;
			vector<int> temp_meta_path = meta_path;
			temp_meta_path.push_back(curr_edge_type);

			set<int> next_nodes = iter->second;
			if(next_nodes.find(dst) != next_nodes.end()){
				double rarity = getRarity(similarPairsSize, srcSimilarNodes, dstSimilarNodes, temp_meta_path, hin_graph_);
				double tfidf = curr_min_instances_num*rarity*penalty(meta_path.size());
				updateTopKMetaPaths(tfidf, temp_meta_path, k, topKMetaPath_);
			}else{
				int temp_min_instances_num = next_nodes.size();
				if(temp_min_instances_num > curr_min_instances_num){ // keep the minimum instance number in the meta path expanding
					temp_min_instances_num = curr_min_instances_num;
				}
				q.push(TfIdfNode(curr_edge_type, next_nodes, temp_min_instances_num, temp_meta_path));
			}
			
		}
	}


	return topKMetaPath_;
}
