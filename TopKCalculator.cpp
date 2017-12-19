#include "TopKCalculator.h"

#include <vector>
#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>
#include <iostream>

using namespace std;

int TopKCalculator::penalty_type_ = 2;

TfIdfNode::TfIdfNode(int curr_edge_type, set<int> curr_nodes, int min_instances_num, vector<int> meta_path, set<int> reached_nodes):curr_edge_type_(curr_edge_type), curr_nodes_(curr_nodes), min_instances_num_(min_instances_num), meta_path_(meta_path), reached_nodes_(reached_nodes){
	found_ = false;
}

bool TfIdfNodeCmp::operator () (TfIdfNode & node1, TfIdfNode & node2)
{
	return node1.min_instances_num_*(TopKCalculator::penalty(node1.meta_path_.size())) <  node2.min_instances_num_*(TopKCalculator::penalty(node2.meta_path_.size()));
}

double TopKCalculator::penalty(int length)
{
	if(penalty_type_ == 1){
		return 1.0/(log (length));
	}else if(penalty_type_ == 2){
		return 1.0/length;
	}else if(penalty_type_ == 3){
		return 1.0/(length*length);
	}else{
		return 1.0/(length*length*length*length);		
	}
}

set<int> TopKCalculator::getSimilarNodes(int eid, map<int, HIN_Node> & hin_nodes_)
{
	set<int> similarNodes;

	if(hin_nodes_.find(eid) == hin_nodes_.end()){
		cout << eid << " Not Found" << endl;
		return similarNodes;
	}


	vector<int> curr_node_types_id_ = hin_nodes_[eid].types_id_;
	// cout << curr_node_types_id_.size() << " " << curr_node_types_id_[0] << endl;
	for(map<int, HIN_Node>::iterator i = hin_nodes_.begin(); i != hin_nodes_.end(); i++){
		int temp_node_id = i->first;
		if(temp_node_id == eid){
			continue;
		}
		HIN_Node tempNode = i->second;
		vector<int> tempNode_types_id = tempNode.types_id_;
		for(vector<int>::iterator j = tempNode_types_id.begin(); j != tempNode_types_id.end(); j++){
			vector<int>::iterator iter = find(curr_node_types_id_.begin(), curr_node_types_id_.end(), *j);
			if(iter != curr_node_types_id_.end()){
				similarNodes.insert(temp_node_id);
				break;
			}
		}
	}
	cout << eid << " candidate size: " << similarNodes.size() << endl;
	return similarNodes;
}

double TopKCalculator::getRarity(int similarPairsSize, set<int> & srcSimilarNodes, set<int> & dstSimilarNodes, vector<int> meta_path, HIN_Graph & hin_graph_){

	map<int, vector<HIN_Edge> > hin_edges_src_ = hin_graph_.edges_src_;
	map<int, vector<HIN_Edge> > hin_edges_dst_ = hin_graph_.edges_dst_;

	set<int> currNodes = srcSimilarNodes;
	set<int> nextNodes;
	for(vector<int>::iterator iter = meta_path.begin(); iter != meta_path.end(); iter++){
		int curr_edge_type = *iter;
		map<int, vector<HIN_Edge> > temp_hin_edges;
		if(curr_edge_type > 0){
			temp_hin_edges = hin_edges_src_;
		}else{
			temp_hin_edges = hin_edges_dst_;
		}
		for(set<int>::iterator i = currNodes.begin(); i != currNodes.end(); i++){
			int currNode = *i;
			if(temp_hin_edges.find(currNode) != temp_hin_edges.end()){
				vector<HIN_Edge> tempEdges = temp_hin_edges[currNode];
				for(vector<HIN_Edge>::iterator j = tempEdges.begin(); j != tempEdges.end(); j++){
					if(curr_edge_type > 0){
						if(j->edge_type_ == curr_edge_type){
                                                	nextNodes.insert(j->dst_);
                                                	break;
                                        	}
					}else{
						if(j->edge_type_ == -curr_edge_type){
							nextNodes.insert(j->src_);
							break;
						}
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

	return log (similarPairsSize*1.0/(intersect.size() + 1));
}

void TopKCalculator:: updateTopKMetaPaths(double tfidf, int tf, double rarity, vector<int> meta_path, int k, vector<pair<vector<double>, vector<int>>> & topKMetaPath_){
	vector<double> tfidf_info = vector<double>();
        tfidf_info.push_back(tfidf);
        tfidf_info.push_back(tf*1.0);
        tfidf_info.push_back(rarity);
	topKMetaPath_.push_back(make_pair(tfidf_info, meta_path));
	for(vector<pair<vector<double>, vector<int>>>::iterator iter = topKMetaPath_.begin(); iter != topKMetaPath_.end(); iter++){
		if(iter->first[0] <= tfidf){
			vector<double> tfidf_info = vector<double>();
			tfidf_info.push_back(tfidf);
			tfidf_info.push_back(tf*1.0);
			tfidf_info.push_back(rarity);
			topKMetaPath_.insert(iter, make_pair(tfidf_info, meta_path));
			break;
		}
		
	}
	topKMetaPath_.erase(topKMetaPath_.end() - 1);

	if(topKMetaPath_.size() > k){
		topKMetaPath_.erase(topKMetaPath_.end() - 1);
	}
}

vector<pair<vector<double>, vector<int>>> TopKCalculator::getTopKMetaPath_TFIDF(int src, int dst, int k, HIN_Graph & hin_graph_)
{
	vector<pair<vector<double>, vector<int>>> topKMetaPath_;
	map<int, vector<HIN_Edge> > hin_edges_src_ = hin_graph_.edges_src_;
	map<int, vector<HIN_Edge> > hin_edges_dst_ = hin_graph_.edges_dst_;
	if((hin_edges_src_.find(src) == hin_edges_src_.end() && hin_edges_dst_.find(src) == hin_edges_src_.end()) || (hin_edges_src_.find(dst) == hin_edges_src_.end() && hin_edges_dst_.find(dst) == hin_edges_src_.end()))
	{
		return topKMetaPath_;
	}

	// similar pairs information
	set<int> srcSimilarNodes = getSimilarNodes(src, hin_graph_.nodes_);
	set<int> dstSimilarNodes = getSimilarNodes(dst, hin_graph_.nodes_);
	int similarPairsSize = srcSimilarNodes.size()*dstSimilarNodes.size();
	double maxRarity = log (similarPairsSize);
	// cout << maxRarity << endl;

	// queue initialize
	priority_queue<TfIdfNode, vector<TfIdfNode>, TfIdfNodeCmp> q;
	vector<HIN_Edge> curr_edges_src_ = hin_edges_src_[src];
	vector<HIN_Edge> curr_edges_dst_ = hin_edges_dst_[src];
	map<int, set<int>> next_nodes_id_; // edge type -> destinations

	// out-relation:
	for(vector<HIN_Edge>::iterator iter = curr_edges_src_.begin() ; iter != curr_edges_src_.end(); iter++){
		int temp_edge_type_ = iter->edge_type_;
		if(next_nodes_id_.find(temp_edge_type_) == next_nodes_id_.end()){
			next_nodes_id_[temp_edge_type_] = set<int>();
		}
		next_nodes_id_[temp_edge_type_].insert(iter->dst_);
	}
	// in-relation
	for(vector<HIN_Edge>::iterator iter = curr_edges_dst_.begin() ; iter != curr_edges_dst_.end(); iter++){
		int temp_edge_type_ = -(iter->edge_type_);
		if(next_nodes_id_.find(temp_edge_type_) == next_nodes_id_.end()){
			next_nodes_id_[temp_edge_type_] = set<int>();
		}
		next_nodes_id_[temp_edge_type_].insert(iter->src_);
	}

	for(map<int, set<int>>::iterator iter = next_nodes_id_.begin(); iter != next_nodes_id_.end(); iter++){
		int curr_edge_type = iter->first;
		set<int> next_nodes = iter->second;
		vector<int> meta_path (1, curr_edge_type);
		TfIdfNode temp_tfidf_node = TfIdfNode(curr_edge_type, next_nodes, next_nodes.size(), meta_path, next_nodes);
		if(next_nodes.find(dst) != next_nodes.end()){
			double rarity = getRarity(similarPairsSize, srcSimilarNodes, dstSimilarNodes, meta_path, hin_graph_);
			double tfidf = rarity*penalty(meta_path.size());
			updateTopKMetaPaths(tfidf, 1, rarity, meta_path, k, topKMetaPath_);
			//temp_tfidf_node.found_ = true;
			temp_tfidf_node.curr_nodes_.erase(dst);
			//temp_tfidf_node.reached_nodes_.erase(dst);
		}
		q.push(temp_tfidf_node);
		
	}
	// BFS
	while(!q.empty()){
		TfIdfNode curr_tfidf_node = q.top();
		q.pop();

		vector<int> meta_path = curr_tfidf_node.meta_path_;
		//cout << meta_path.size() << endl;
		/*	
		for(int i = 0; i < meta_path.size() - 1; i++){
			cout << meta_path[i] << "->";
		}
		cout << meta_path.back() << endl;
		*/
			
		int curr_min_instances_num = curr_tfidf_node.min_instances_num_;
		//cout << curr_min_instances_num << endl;
		set<int> curr_nodes = curr_tfidf_node.curr_nodes_;
		bool found = curr_tfidf_node.found_;
		set<int> reached_nodes = curr_tfidf_node.reached_nodes_;

		if(topKMetaPath_.size() == k){ // stop if maximum tfidf in the queue is less than the minimum one in found meta paths
			//cout << curr_min_instances_num << " " << meta_path.size() << " " << curr_min_instances_num*penalty(meta_path.size())*maxRarity << " " << topKMetaPath_.back().first << endl;
			if(curr_min_instances_num*penalty(meta_path.size())*maxRarity <= topKMetaPath_.back().first[0]){
				break;
			}	
		}

		//expand
		next_nodes_id_.clear(); // edge type -> destinations 
		for(set<int>::iterator iter = curr_nodes.begin(); iter != curr_nodes.end(); iter++){
			int temp_node = *iter;
			if(hin_edges_src_.find(temp_node) == hin_edges_src_.end() && hin_edges_dst_.find(temp_node) == hin_edges_dst_.end()){
				continue;
			}
			vector<HIN_Edge> temp_edges_src_ = hin_edges_src_[temp_node];
			for(vector<HIN_Edge>::iterator i = temp_edges_src_.begin(); i != temp_edges_src_.end(); i++){
				int temp_edge_type_ = i->edge_type_;
				if(next_nodes_id_.find(temp_edge_type_) == next_nodes_id_.end()){
					next_nodes_id_[temp_edge_type_] = set<int>();
				}
				next_nodes_id_[temp_edge_type_].insert(i->dst_);
			}
			vector<HIN_Edge> temp_edges_dst_ = hin_edges_dst_[temp_node];
			for(vector<HIN_Edge>::iterator i = temp_edges_dst_.begin(); i != temp_edges_dst_.end(); i++){
				int temp_edge_type_ = -(i->edge_type_);
				if(next_nodes_id_.find(temp_edge_type_) == next_nodes_id_.end()){
					next_nodes_id_[temp_edge_type_] = set<int>();
				}
				next_nodes_id_[temp_edge_type_].insert(i->src_);
			}
		}

		for(map<int, set<int>>::iterator iter = next_nodes_id_.begin(); iter != next_nodes_id_.end(); iter++){
			int curr_edge_type = iter->first;
			vector<int> temp_meta_path = meta_path;
			temp_meta_path.push_back(curr_edge_type);

			set<int> next_nodes = iter->second;
		
			/*	
			if(includes(reached_nodes.begin(), reached_nodes.end(), next_nodes.begin(), next_nodes.end())){
				continue;
			}
			*/

			int temp_min_instances_num = next_nodes.size();
                        if(temp_min_instances_num > curr_min_instances_num){ // keep the minimum instance number in the meta path expanding
                                temp_min_instances_num = curr_min_instances_num;
                        }			
			set<int> new_reached_nodes;
			set_union(reached_nodes.begin(), reached_nodes.end(), next_nodes.begin(), next_nodes.end(), inserter(new_reached_nodes, new_reached_nodes.begin()));
			
			TfIdfNode temp_tfidf_node = TfIdfNode(curr_edge_type, next_nodes, temp_min_instances_num, temp_meta_path, new_reached_nodes);

			if(next_nodes.find(dst) != next_nodes.end()){
				double rarity = getRarity(similarPairsSize, srcSimilarNodes, dstSimilarNodes, temp_meta_path, hin_graph_);
				double tfidf = curr_min_instances_num*rarity*penalty(meta_path.size());
				updateTopKMetaPaths(tfidf, curr_min_instances_num, rarity, temp_meta_path, k, topKMetaPath_);
				//temp_tfidf_node.found_ = true;
				temp_tfidf_node.curr_nodes_.erase(dst);
                        	//temp_tfidf_node.reached_nodes_.erase(dst);
			}
			/*
			for(int i = 0; i < temp_meta_path.size() - 1; i++){
                        	cout << temp_meta_path[i] << "->";
                	}
                	cout << temp_meta_path.back() << endl;		
			*/
			q.push(temp_tfidf_node);
			
		}
	}


	return topKMetaPath_;
}
