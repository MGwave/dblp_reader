#include "TopKCalculator.h"
#include "SimCalculator.h"
#include "Meta-Structure.h"

#include <vector>
#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>

using namespace std;

int TopKCalculator::penalty_type_ = 2;
int TopKCalculator::rarity_type_ = 1; // 1 -> true rarity; 0 -> 1(constant)
int TopKCalculator::support_type_ = 1; // 1 -> MNI; 2 -> PCRW; 0 -> 1(constant)

TfIdfNode::TfIdfNode(int curr_edge_type, map<int, set<int>> curr_nodes_with_parents, double max_support, vector<int> meta_path, TfIdfNode* parent):curr_edge_type_(curr_edge_type), curr_nodes_with_parents_(curr_nodes_with_parents), max_support_(max_support), meta_path_(meta_path), parent_(parent){}

bool TfIdfNodePointerCmp::operator () (TfIdfNode* & node_p1, TfIdfNode* & node_p2)
{
	return node_p1->max_support_*(TopKCalculator::penalty(node_p1->meta_path_.size())) <  node_p2->max_support_*(TopKCalculator::penalty(node_p2->meta_path_.size()));
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
		return 1.0/(exp (length));		
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
	cout << eid << "\t";
	if(strcmp(hin_nodes_[eid].key_.c_str(), "") == 0){
		cout << "NA" << "\t";
	}else{
		cout << hin_nodes_[eid].key_ << "\t";
	} 
	cout << "candidate size: " << similarNodes.size() << endl;
	return similarNodes;
}

double TopKCalculator::getRarity(int similarPairsSize, set<int> & srcSimilarNodes, set<int> & dstSimilarNodes, vector<int> meta_path, HIN_Graph & hin_graph_){
	
	if(rarity_type_ != 1){
		return 1.0;
	}

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

	return log10 (similarPairsSize*1.0/(intersect.size() + 1));
}

double TopKCalculator::getMaxSupport(double candidateSupport){
	if(support_type_ == 1){
		return candidateSupport;
	}else if(support_type_ == 0 || support_type_ == 2){
		return 0;
	}

	return 0;
}

double TopKCalculator::getSupport(int src, int dst, TfIdfNode* curr_tfidf_node_p, vector<int> meta_path, HIN_Graph & hin_graph_){
	if(support_type_ == 0){// Binary Support
		return 1.0;
	}else if(support_type_ == 1){// MNI Support

		if(curr_tfidf_node_p == NULL){
			return 1.0;
		}else if(curr_tfidf_node_p->parent_ == NULL){
			return 1.0;
		}

		TfIdfNode* temp_tfidf_node_p = curr_tfidf_node_p;
		set<int> curr_nodes;
		set<int> next_nodes;
		curr_nodes.insert(dst);
		int min_instances_num = temp_tfidf_node_p->curr_nodes_with_parents_.size();


		while(temp_tfidf_node_p->parent_ != NULL){
			next_nodes.clear();
			map<int, set<int>> temp_nodes_with_parents = temp_tfidf_node_p->curr_nodes_with_parents_;
			for(set<int>::iterator iter = curr_nodes.begin(); iter != curr_nodes.end(); iter++){
				int temp_node = *iter;	
				if(temp_nodes_with_parents.find(temp_node) != temp_nodes_with_parents.end()){
					set<int> parents = temp_nodes_with_parents[temp_node]; 
					next_nodes.insert(parents.begin(), parents.end());
				} 
			}		
			
			curr_nodes = next_nodes;
			int curr_nodes_size = curr_nodes.size();
			if(min_instances_num > curr_nodes_size){
				min_instances_num = curr_nodes_size;
			}	
			temp_tfidf_node_p = temp_tfidf_node_p->parent_;
			
		}	
		//cout << min_instances_num << endl;	
		return min_instances_num*1.0;

	}else if(support_type_ == 2){// PCRW Support
		Meta_Paths tempmetapath(hin_graph_);
		map<int, double> id_sim;

		tempmetapath.weights_.push_back(1.0);
		tempmetapath.linkTypes_.push_back(meta_path);

		double pcrw = SimCalculator::calSim_PCRW(src, dst, tempmetapath, id_sim);
		return pcrw;
	}
}

void TopKCalculator:: updateTopKMetaPaths(double tfidf, double support, double rarity, vector<int> meta_path, int k, vector<pair<vector<double>, vector<int>>> & topKMetaPath_){
	vector<double> tfidf_info = vector<double>();
        tfidf_info.push_back(tfidf);
        tfidf_info.push_back(support);
        tfidf_info.push_back(rarity);
	topKMetaPath_.push_back(make_pair(tfidf_info, meta_path));
	for(vector<pair<vector<double>, vector<int>>>::iterator iter = topKMetaPath_.begin(); iter != topKMetaPath_.end(); iter++){
		if(iter->first[0] <= tfidf){
			vector<double> tfidf_info = vector<double>();
			tfidf_info.push_back(tfidf);
			tfidf_info.push_back(support);
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
	// /*
	int temp_node;
	int src_edges = hin_edges_src_[src].size() + hin_edges_dst_[src].size();
	//cout << src_edges << endl;
	int dst_edges = hin_edges_src_[dst].size() + hin_edges_dst_[dst].size();
	//cout << dst_edges << endl;
	if(dst_edges < src_edges){
		int temp_node = src;
		src = dst;
		dst = temp_node;
	}
	// */	
	// similar pairs information
	set<int> srcSimilarNodes = getSimilarNodes(src, hin_graph_.nodes_);
	set<int> dstSimilarNodes = getSimilarNodes(dst, hin_graph_.nodes_);
	int similarPairsSize = srcSimilarNodes.size()*dstSimilarNodes.size();
	double maxRarity = log10 (similarPairsSize);
	double maxSupport = 1.0;
	/*
	if(support_type_ == 1){
		maxSupport = ???;	
	}
	*/

	// cout << maxRarity << endl; 
	// queue initialize
	priority_queue<TfIdfNode*, vector<TfIdfNode*>, TfIdfNodePointerCmp> q;
	vector<HIN_Edge> curr_edges_src_ = hin_edges_src_[src];
	vector<HIN_Edge> curr_edges_dst_ = hin_edges_dst_[src];
	map<int, map<int, set<int>>> next_nodes_id_; // edge type -> (destination id -> parent node ids)
	

	// out-relation:
	for(vector<HIN_Edge>::iterator iter = curr_edges_src_.begin() ; iter != curr_edges_src_.end(); iter++){
		int temp_edge_type_ = iter->edge_type_;
		if(next_nodes_id_.find(temp_edge_type_) == next_nodes_id_.end()){
			next_nodes_id_[temp_edge_type_] = map<int, set<int>>();
		}
		if(next_nodes_id_[temp_edge_type_].find(iter->dst_) == next_nodes_id_[temp_edge_type_].end()){
			next_nodes_id_[temp_edge_type_][iter->dst_] = set<int>();
		}
		next_nodes_id_[temp_edge_type_][iter->dst_].insert(src);
	}
	// in-relation
	for(vector<HIN_Edge>::iterator iter = curr_edges_dst_.begin() ; iter != curr_edges_dst_.end(); iter++){
		int temp_edge_type_ = -(iter->edge_type_);
		if(next_nodes_id_.find(temp_edge_type_) == next_nodes_id_.end()){
			next_nodes_id_[temp_edge_type_] = map<int,set<int>>();
		}
		if(next_nodes_id_[temp_edge_type_].find(iter->src_) == next_nodes_id_[temp_edge_type_].end()){
                        next_nodes_id_[temp_edge_type_][iter->src_] = set<int>();
                }
		next_nodes_id_[temp_edge_type_][iter->src_].insert(src);
	}
	for(map<int, map<int, set<int>> >::iterator iter = next_nodes_id_.begin(); iter != next_nodes_id_.end(); iter++){
		int curr_edge_type = iter->first;
		vector<int> meta_path (1, curr_edge_type);
		map<int, set<int>> next_nodes_with_parents = iter->second;
		TfIdfNode* temp_tfidf_node_p = new TfIdfNode(curr_edge_type, next_nodes_with_parents, getMaxSupport(next_nodes_with_parents.size()), meta_path, NULL);
		if(next_nodes_with_parents.find(dst) != next_nodes_with_parents.end()){
			double support = getSupport(src, dst, temp_tfidf_node_p, meta_path, hin_graph_);
			double rarity = getRarity(similarPairsSize, srcSimilarNodes, dstSimilarNodes, meta_path, hin_graph_);
                	double tfidf = support*rarity*penalty(meta_path.size());
                	updateTopKMetaPaths(tfidf, support, rarity, meta_path, k, topKMetaPath_);
                	temp_tfidf_node_p->curr_nodes_with_parents_.erase(dst);
		}
		q.push(temp_tfidf_node_p);
		
	}

	// BFS
	while(!q.empty()){
		TfIdfNode* curr_tfidf_node_p = q.top();
		q.pop();

		vector<int> meta_path = curr_tfidf_node_p->meta_path_;
		//cout << meta_path.size() << endl;
		/*		
		for(int i = 0; i < meta_path.size() - 1; i++){
			cout << meta_path[i] << "->";
		}
		cout << meta_path.back() << endl;
		*/	
			
		double curr_max_support = curr_tfidf_node_p->max_support_;
		//cout << curr_max_support << endl;
		map<int, set<int>> curr_nodes_with_parents = curr_tfidf_node_p->curr_nodes_with_parents_;
		map<int, map<int, int>> edge_max_instances_num; // edge type -> (node id -> instances number)

		if(topKMetaPath_.size() == k){ // stop if maximum tfidf in the queue is less than the minimum one in found meta paths
			//cout << curr_max_support << " " << meta_path.size() << " " << curr_max_support*penalty(meta_path.size())*maxRarity << " " << topKMetaPath_.back().first[0] << endl;
			if(curr_max_support*penalty(meta_path.size())*maxRarity <= topKMetaPath_.back().first[0]){
				break;
			}	
		}

		//expand
		next_nodes_id_.clear(); // edge type -> (destination id -> parent node id)
		for(map<int, set<int>>::iterator iter = curr_nodes_with_parents.begin(); iter != curr_nodes_with_parents.end(); iter++){
			int temp_node = iter->first;
			if(hin_edges_src_.find(temp_node) == hin_edges_src_.end() && hin_edges_dst_.find(temp_node) == hin_edges_dst_.end()){
				continue;
			}
			vector<HIN_Edge> temp_edges_src_ = hin_edges_src_[temp_node];
			for(vector<HIN_Edge>::iterator i = temp_edges_src_.begin(); i != temp_edges_src_.end(); i++){
				int temp_edge_type_ = i->edge_type_;
				if(next_nodes_id_.find(temp_edge_type_) == next_nodes_id_.end()){
					next_nodes_id_[temp_edge_type_] = map<int, set<int>>();
				}
				if(next_nodes_id_[temp_edge_type_].find(i->dst_) == next_nodes_id_[temp_edge_type_].end()){
					next_nodes_id_[temp_edge_type_][i->dst_] = set<int>();
				}
				next_nodes_id_[temp_edge_type_][i->dst_].insert(temp_node);

				if(edge_max_instances_num.find(temp_edge_type_) == edge_max_instances_num.end()){
                                        edge_max_instances_num[temp_edge_type_] = map<int, int>();

                                }
				if(edge_max_instances_num[temp_edge_type_].find(temp_node) == edge_max_instances_num[temp_edge_type_].end()){
					edge_max_instances_num[temp_edge_type_][temp_node] = 0;

				}
				edge_max_instances_num[temp_edge_type_][temp_node]++;
			}
			vector<HIN_Edge> temp_edges_dst_ = hin_edges_dst_[temp_node];
			for(vector<HIN_Edge>::iterator i = temp_edges_dst_.begin(); i != temp_edges_dst_.end(); i++){
				int temp_edge_type_ = -(i->edge_type_);
				if(next_nodes_id_.find(temp_edge_type_) == next_nodes_id_.end()){
					next_nodes_id_[temp_edge_type_] = map<int, set<int>>();
				}
				if(next_nodes_id_[temp_edge_type_].find(i->src_) == next_nodes_id_[temp_edge_type_].end()){
                                        next_nodes_id_[temp_edge_type_][i->src_] = set<int>();
                                }
				next_nodes_id_[temp_edge_type_][i->src_].insert(temp_node);
			
				if(edge_max_instances_num.find(temp_edge_type_) == edge_max_instances_num.end()){
                                        edge_max_instances_num[temp_edge_type_] = map<int, int>();

                                }
                                if(edge_max_instances_num[temp_edge_type_].find(temp_node) == edge_max_instances_num[temp_edge_type_].end()){
                                        edge_max_instances_num[temp_edge_type_][temp_node] = 0;

                                }
                                edge_max_instances_num[temp_edge_type_][temp_node]++;	
			}
		}

		for(map<int, map<int,set<int>>>::iterator iter = next_nodes_id_.begin(); iter != next_nodes_id_.end(); iter++){
			int curr_edge_type = iter->first;
			vector<int> temp_meta_path = meta_path;
			temp_meta_path.push_back(curr_edge_type);
			map<int, set<int>> next_nodes_with_parents = iter->second;
			
			int temp_max_support = 1;
			map<int, int> nodes_next_instances_num = edge_max_instances_num[curr_edge_type];
			for(map<int, int>::iterator i = nodes_next_instances_num.begin(); i != nodes_next_instances_num.end(); i++){
				if(i->second > temp_max_support){
					temp_max_support = i->second;
				}
			}

                        if(temp_max_support > curr_max_support){ // keep the minimum instance number in the meta path expanding
                                temp_max_support = curr_max_support;
                        }			
			
			TfIdfNode* temp_tfidf_node_p = new TfIdfNode(curr_edge_type, next_nodes_with_parents, getMaxSupport(temp_max_support), temp_meta_path, curr_tfidf_node_p);

			if(next_nodes_with_parents.find(dst) != next_nodes_with_parents.end()){
				double support = getSupport(src, dst, temp_tfidf_node_p, temp_meta_path, hin_graph_);
				double rarity = getRarity(similarPairsSize, srcSimilarNodes, dstSimilarNodes, temp_meta_path, hin_graph_);
                        	double tfidf = support*rarity*penalty(temp_meta_path.size());
                        	updateTopKMetaPaths(tfidf, support, rarity, temp_meta_path, k, topKMetaPath_);
                        	temp_tfidf_node_p->curr_nodes_with_parents_.erase(dst);
			}
			/*			
			for(int i = 0; i < temp_meta_path.size() - 1; i++){
                        	cout << temp_meta_path[i] << "->";
                	}
                	cout << temp_meta_path.back() << endl;		
			*/ 
			q.push(temp_tfidf_node_p);
			
		}
	}


	return topKMetaPath_;
}
