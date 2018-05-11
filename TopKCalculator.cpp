#include "TopKCalculator.h"
#include "SimCalculator.h"
#include "Meta-Structure.h"
#include "CommonUtils.h"

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <vector>
#include <iterator>
#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

int TopKCalculator::penalty_type_ = 1;
double TopKCalculator::beta_ = 0.3;
int TopKCalculator::rarity_type_ = 1;
int TopKCalculator::support_type_ = 1;
int TopKCalculator::sample_size_ = 100;

TfIdfNode::TfIdfNode(int curr_edge_type, map<int, set<int>> curr_nodes_with_parents, double max_support, vector<int> meta_path, TfIdfNode* parent):curr_edge_type_(curr_edge_type), curr_nodes_with_parents_(curr_nodes_with_parents), max_support_(max_support), meta_path_(meta_path), parent_(parent){}

TfIdfNode::~TfIdfNode(){
	for(map<int, set<int>>::iterator iter = this->curr_nodes_with_parents_.begin(); iter != this->curr_nodes_with_parents_.end(); iter++){
		iter->second.clear();
	}
	this->curr_nodes_with_parents_.clear();
	this->meta_path_.clear(); 
	this->parent_ = NULL;
}

bool TfIdfNodePointerCmp::operator () (TfIdfNode* & node_p1, TfIdfNode* & node_p2)
{
	if(node_p1->max_support_ == 0 && node_p2->max_support_ == 0){
		return node_p1->meta_path_.size() > node_p2->meta_path_.size();
	}else{
		double tmp1 = node_p1->max_support_*(TopKCalculator::penalty(node_p1->meta_path_.size()));
		double tmp2 = node_p2->max_support_*(TopKCalculator::penalty(node_p2->meta_path_.size()));
		if(tmp1 < tmp2){
			return true;
		}else if(tmp1 > tmp2){
			return false;
		}else{
			double randomNumber = (double)rand()/RAND_MAX;
			return randomNumber <= 0.5;
		}
	}
}

class ScorePairCmp
{
public:
	bool operator () (pair<double, int> & scorePair1, pair<double, int> & scorePair2){
		if(scorePair1.first == scorePair2.first){
			return scorePair1.second > scorePair2.second;
		}else{
			return scorePair1.first < scorePair2.first;
		}
	}
};

int TopKCalculator::factorial(int n){
	return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

double TopKCalculator::getMetaPathScore(int src, int dst, vector<int> meta_path, int score_function, HIN_Graph & hin_graph_){
	if(score_function == 1){
		Meta_Paths tempmetapath(hin_graph_);
				map<int, double> id_sim;

				tempmetapath.weights_.push_back(1.0);
				tempmetapath.linkTypes_.push_back(meta_path);

				//double pcrw = SimCalculator::calSim_PCRW(src, dst, tempmetapath, id_sim);
		double pcrw = getPCRW(src, dst, meta_path, hin_graph_);
				return pcrw;
	}

	return 2.0;	
}

double TopKCalculator::penalty(int length)
{
	if(penalty_type_ == 0){
		return 1.0;
	}else if(penalty_type_ == 1){
		return pow(beta_, length);
	}else if(penalty_type_ == 2){
		return 1.0/factorial(length);
	}else if(penalty_type_ == 3){
		return 1.0/length;
	}else if(penalty_type_ == 4){
		return 1/exp(length);
	}else{
		return 1.0;
	}
}

set<int> TopKCalculator::getSimilarNodes(int eid, map<int, HIN_Node> & hin_nodes_, bool sample_flag, bool output_flag)
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
	if(output_flag){
		cout << eid << "\t";
		if(strcmp(hin_nodes_[eid].key_.c_str(), "") == 0){
			cout << "NA" << "\t";
		}else{
			cout << hin_nodes_[eid].key_ << "\t";
		} 

	}	
	
	similarNodes.insert(eid);

	if(sample_flag){
		set<int> samples;
		for(int i = 0; i < sample_size_; i++){
			int offset = (int) (rand() % similarNodes.size());
			set<int>::iterator iter(similarNodes.begin());
			advance(iter, offset);
			samples.insert(*iter);
			similarNodes.erase(*iter);	
		}
		similarNodes = samples;	
	}
	if(output_flag)
		cout << "candidate size: " << similarNodes.size() << endl;
	return similarNodes;
}

double TopKCalculator::getHit(set<int> & srcSimilarNodes, set<int> & dstSimilarNodes, vector<int> meta_path, HIN_Graph & hin_graph_) {
	int hit = 0;

	map<int, vector<HIN_Edge> > hin_edges_src_ = hin_graph_.edges_src_;
	map<int, vector<HIN_Edge> > hin_edges_dst_ = hin_graph_.edges_dst_;

	set<int> tmpSrcSimilarNodes;
	set<int> tmpDstSimilarNodes;
	if(srcSimilarNodes.size() < dstSimilarNodes.size()){
		tmpSrcSimilarNodes = srcSimilarNodes;
		tmpDstSimilarNodes = dstSimilarNodes;
	}else{
		tmpSrcSimilarNodes = dstSimilarNodes;
		tmpDstSimilarNodes = srcSimilarNodes;
		for(int m = 0; m < meta_path.size(); m++){
			meta_path[m] = -meta_path[m];
		}	
	}
	set<int> currNodes;
	set<int> nextNodes;	

	for (set<int>::iterator i = tmpSrcSimilarNodes.begin(); i != tmpSrcSimilarNodes.end(); i++) {
		currNodes.clear();
		nextNodes.clear();

		currNodes.insert(*i);

		for (vector<int>::iterator iter = meta_path.begin();
			iter != meta_path.end() && currNodes.size()>0; iter++) {
			int curr_edge_type = *iter;
			map<int, vector<HIN_Edge> > temp_hin_edges;
			if (curr_edge_type > 0) {
				temp_hin_edges = hin_edges_src_;
			}
			else {
				temp_hin_edges = hin_edges_dst_;
			}

			for (set<int>::iterator j = currNodes.begin(); j != currNodes.end(); j++) {
				int currNode = *j;
				if (temp_hin_edges.find(currNode) != temp_hin_edges.end()) {
					vector<HIN_Edge> tempEdges = temp_hin_edges[currNode];
					for (vector<HIN_Edge>::iterator e = tempEdges.begin(); e != tempEdges.end(); e++) {
						if (curr_edge_type > 0) {
							if (e->edge_type_ == curr_edge_type) {
								nextNodes.insert(e->dst_);
								//break;
							}
						}
						else {
							if (e->edge_type_ == -curr_edge_type) {
								nextNodes.insert(e->src_);
								//break;
							}
						}
					}
				}
			}
			currNodes = nextNodes;
			nextNodes.clear();
		}

		set<int> intersect;
		set_intersection(currNodes.begin(), currNodes.end(), tmpDstSimilarNodes.begin(), tmpDstSimilarNodes.end(), inserter(intersect, intersect.begin()));

		hit += intersect.size();
	}

	return hit;
}

// This is a light weight version of getRarity where we consider a smaller set of D
// D' = {(src, v)|v \in dstSimilarNodes} \cup {(u, dst)|u \in srcSimilarNodes}
double TopKCalculator::getRarity(int src, int dst, set<int> & srcSimilarNodes, set<int> & dstSimilarNodes, vector<int> & meta_path, HIN_Graph & hin_graph_) {
	if (rarity_type_ == 0) {
		return 1.0;
	}

	//cout << "Running with a light weight version of getRarity" << endl;

	// (src, dst) are counted twice, hence we have to substract 1
	int similarPairsSize = srcSimilarNodes.size() + dstSimilarNodes.size() - 1;

	set<int> tempNode;
	int hit = 0;

	tempNode.clear();tempNode.insert(src);dstSimilarNodes.erase(dst);
	hit += getHit(tempNode, dstSimilarNodes, meta_path, hin_graph_);
	dstSimilarNodes.insert(dst);

	tempNode.clear();tempNode.insert(dst);srcSimilarNodes.erase(src);
	hit += getHit(srcSimilarNodes, tempNode, meta_path, hin_graph_);
	srcSimilarNodes.insert(src);

	hit += 1;

	if(rarity_type_ == 1){
		return log(similarPairsSize*1.0 / hit);
	}else{
		double normalizedHit = hit*1.0;
		map<int, pair<double, double>> edgeTypeAvgDegree = hin_graph_.edge_type_avg_degree_;
		int meta_path_size = meta_path.size();
		for(int i = 0; i < meta_path_size; i++){
			int curr_edge_type = meta_path[i];
			if(curr_edge_type < 0){
				curr_edge_type = -curr_edge_type;
			}
			if(edgeTypeAvgDegree.find(curr_edge_type) != edgeTypeAvgDegree.end()){
				normalizedHit /= pow(edgeTypeAvgDegree[curr_edge_type].first*edgeTypeAvgDegree[curr_edge_type].second, STRENGTH_ALPHA);
				if(normalizedHit <= 1){
					break;
				}
			}
		}
		return log(similarPairsSize*1.0 / normalizedHit);
	}
}

double TopKCalculator::getRarity(int similarPairsSize, set<int> & srcSimilarNodes, set<int> & dstSimilarNodes, vector<int> & meta_path, HIN_Graph & hin_graph_){
	
	if(rarity_type_ != 1){
		return 1.0;
	}

	int hit = getHit(srcSimilarNodes, dstSimilarNodes, meta_path, hin_graph_);

	// To Zichen: Delete the commented code below if the above is correct
	/*for(vector<int>::iterator iter = meta_path.begin(); iter != meta_path.end(); iter++){
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

	return log (similarPairsSize*1.0/(intersect.size() + 1));*/

	// Since src is in srcSimilarNodes and dst is in dstSimilarNodes,
	// hence we already count (src, dst) in hit	
	return log (similarPairsSize*1.0/hit);
}

void TopKCalculator::getNextEntities(set<int> eids, int edge_type, set<int> & next_entities, HIN_Graph & hin_graph_){
	
	next_entities.clear();
	bool src_flag = true;
	map<int, vector<HIN_Edge> > temp_hin_edges;
	if(edge_type > 0){
		temp_hin_edges = hin_graph_.edges_src_;
	}else if(edge_type < 0){
		src_flag = false;
		edge_type = -edge_type;
		temp_hin_edges = hin_graph_.edges_dst_;
	}

	if(temp_hin_edges.size() != 0){
		for(set<int>::iterator iter = eids.begin(); iter != eids.end(); iter++){
			int eid = *iter;
			if(temp_hin_edges.find(eid) != temp_hin_edges.end()){
				vector<HIN_Edge> candidate_edges = temp_hin_edges[eid];
				for(int i = 0; i < candidate_edges.size(); i++){
					if(candidate_edges[i].edge_type_ == edge_type){
						if(src_flag){
							next_entities.insert(candidate_edges[i].dst_);
						}else{
							next_entities.insert(candidate_edges[i].src_);
						}
					}
				}
			} 
		}
	}

}
void TopKCalculator::getNextEntities(int eid, int edge_type, set<int> & next_entities, HIN_Graph & hin_graph_){
	set<int> eids;
	eids.insert(eid);
	getNextEntities(eids, edge_type, next_entities, hin_graph_);
}
double TopKCalculator::getPCRW(int src, int dst, vector<int> meta_path, HIN_Graph & hin_graph_){
	return getPCRWMain(src, dst, set<int>(), set<int>(), meta_path, hin_graph_); 
	//return getPCRW_DFS(src, dst, meta_path, hin_graph_);
}

double TopKCalculator::getPCRW_DFS(int src, int dst, vector<int> meta_path, HIN_Graph & hin_graph_){
	double result = 0.0;
	set<int> next_entities;
	getNextEntities(src, meta_path.front(), next_entities, hin_graph_);
	if(next_entities.size() == 0){
		return 0.0;
	}	
	double multiplier = 1.0/next_entities.size();
	if(meta_path.size() == 1){
		if(next_entities.find(dst) != next_entities.end()){
			return multiplier;
		}else{
			return 0.0;
		}
	}
	vector<int> temp_meta_path (meta_path.begin() + 1, meta_path.end());
	for(set<int>::iterator iter = next_entities.begin(); iter != next_entities.end(); iter++){
		result += multiplier*getPCRW_DFS(*iter, dst, temp_meta_path, hin_graph_); 
	}

	return result;
	
}
bool TopKCalculator::isConnected(int src, int dst, vector<int> meta_path, HIN_Graph & hin_graph_){
	return isConnectedMain(src, dst, set<int>(), set<int>(), meta_path, hin_graph_);
}
bool TopKCalculator::isConnectedMain(int src, int dst, set<int> src_next_entities, set<int> dst_next_entities, vector<int> meta_path, HIN_Graph & hin_graph_){
	int meta_path_size = meta_path.size();
		if(meta_path_size == 0){
				return false;
		}else if(meta_path_size == 1){
		if(src_next_entities.size() == 0){
						getNextEntities(src, meta_path.front(), src_next_entities, hin_graph_);
				}

		if(src_next_entities.size() != 0 && src_next_entities.find(dst) != src_next_entities.end()){
			return true;
		}

		return false;
	}else{
		if(src_next_entities.size() == 0){
						getNextEntities(src, meta_path.front(), src_next_entities, hin_graph_);
				}
		if(src_next_entities.size() == 0){
			return false;
		}
		
		if(dst_next_entities.size() == 0){
						getNextEntities(dst, -meta_path.back(), dst_next_entities, hin_graph_);
				}
				if(dst_next_entities.size() == 0){
						return false;
				}

		if(src_next_entities.size() > dst_next_entities.size()){
			vector<int> left_meta_path (meta_path.begin(), meta_path.end() - 1);
						for(set<int>:: iterator iter = dst_next_entities.begin(); iter != dst_next_entities.end(); iter++){	
				if(isConnectedMain(src, *iter, src_next_entities, set<int> (), left_meta_path, hin_graph_)){
					return true;
				}
						}
		}else{
						for(set<int>::iterator iter = src_next_entities.begin(); iter != src_next_entities.end(); iter++){
								if(isConnectedMain(*iter, dst, set<int> (), dst_next_entities, vector<int> (meta_path.begin() + 1, meta_path.end()), hin_graph_)){
					return true;
				}
						}

		}

		return false;	
					
	}

}
double TopKCalculator::getPCRWMain(int src, int dst, set<int> src_next_entities, set<int> dst_next_entities, vector<int> meta_path, HIN_Graph & hin_graph_){
	int meta_path_size = meta_path.size();
	if(meta_path_size == 0){
		return 0.0;	
	}else if(meta_path_size == 1){
		if(src_next_entities.size() == 0){
			getNextEntities(src, meta_path.front(), src_next_entities, hin_graph_);
		}

		if(src_next_entities.size() != 0 && src_next_entities.find(dst) != src_next_entities.end()){
			return 1.0/src_next_entities.size();
		}

		return 0.0;
	}else{
		if(src_next_entities.size() == 0){
			getNextEntities(src, meta_path.front(), src_next_entities, hin_graph_);
		}
		if(src_next_entities.size() == 0){
			return 0.0;
		}
	
		if(dst_next_entities.size() == 0){
			getNextEntities(dst, -meta_path.back(), dst_next_entities, hin_graph_);
		}
		if(dst_next_entities.size() == 0){
			return 0.0;
		}
		
		double result = 0.0;
		if(src_next_entities.size() > dst_next_entities.size()){
			vector<int> left_meta_path (meta_path.begin(), meta_path.end() - 1);
			vector<int> right_meta_path (1, meta_path.back());
			for(set<int>:: iterator iter = dst_next_entities.begin(); iter != dst_next_entities.end(); iter++){
				double tmp_result1 = getPCRWMain(src, *iter, src_next_entities, set<int> (), left_meta_path, hin_graph_);
				double tmp_result2 = getPCRWMain(*iter, dst, set<int> (), dst_next_entities, right_meta_path, hin_graph_);
				result += tmp_result1*tmp_result2;
			}	
		}else{
			double multiplier = 1.0/src_next_entities.size();
			for(set<int>::iterator iter = src_next_entities.begin(); iter != src_next_entities.end(); iter++){
				result += multiplier*getPCRWMain(*iter, dst, set<int> (), dst_next_entities, vector<int> (meta_path.begin() + 1, meta_path.end()), hin_graph_);
			}
		}
		return result;
		
	}
}

double TopKCalculator::getMaxSupport(double candidateSupport){
	if(support_type_ == 1 || support_type_ == 3){
		return candidateSupport;
	}else if(support_type_ == 0){
		return 1.0;
	}else if(support_type_ == 2){
		return 1.0;
	}

	return 0;
}

double TopKCalculator::getSupport(int src, int dst, TfIdfNode* curr_tfidf_node_p, vector<int> meta_path, HIN_Graph & hin_graph_){
	if(support_type_ == 0){// Binary Support
		return 1.0;
	}else if(support_type_ == 1 || support_type_ == 3 ||  support_type_ == 4 || support_type_ == 5){// 1 -> MNI Support; 3 -> Normalized MNI Support
		double strength_ratio = 1.0;
		if(support_type_ != 1){
			map<int, pair<double, double>> edgeTypeAvgDegree = hin_graph_.edge_type_avg_degree_;
			int meta_path_size = meta_path.size();
			for(int i = 0; i < meta_path_size; i++){
				int curr_edge_type = meta_path[i];
				if(curr_edge_type < 0){
					curr_edge_type = -curr_edge_type;
				}
				if(edgeTypeAvgDegree.find(curr_edge_type) != edgeTypeAvgDegree.end()){
					strength_ratio /= pow(edgeTypeAvgDegree[curr_edge_type].first*edgeTypeAvgDegree[curr_edge_type].second, STRENGTH_ALPHA);
				}
			}

			if(support_type_ == 4){
				return strength_ratio;
			}else if(support_type_ == 5){
				return exp(strength_ratio);
			}

		}

		if(curr_tfidf_node_p == NULL){
			return 1.0;
		}else if(curr_tfidf_node_p->parent_ == NULL){
			return 1.0;
		}

		TfIdfNode* temp_tfidf_node_p = curr_tfidf_node_p->parent_;
		set<int> last_nodes;
		last_nodes.insert(dst);
		set<int> curr_nodes = curr_tfidf_node_p->curr_nodes_with_parents_[dst];
		set<int> next_nodes;
		set<int> tmp_next_entities;
		int curr_edge_index = int(meta_path.size()) - 1;
		int min_instances_num = curr_nodes.size() + 1;

		set<int> min_curr_nodes;
		set<int> min_last_nodes;
		set<int> min_next_nodes;
		int min_last_edge_index, min_next_edge_index;
		min_next_edge_index = curr_edge_index;

		while(temp_tfidf_node_p != NULL){
			next_nodes.clear();
			map<int, set<int>> temp_nodes_with_parents = temp_tfidf_node_p->curr_nodes_with_parents_;
			curr_edge_index--;
			for(set<int>::iterator iter = curr_nodes.begin(); iter != curr_nodes.end(); iter++){
				int temp_node = *iter;
				tmp_next_entities.clear();	
				if(temp_nodes_with_parents.find(temp_node) != temp_nodes_with_parents.end()){
					set<int> parents = temp_nodes_with_parents[temp_node];
					next_nodes.insert(parents.begin(), parents.end());
				} 
			}		

			
			int curr_nodes_size = curr_nodes.size();
			if(min_instances_num > curr_nodes_size){
				min_instances_num = curr_nodes_size;
				min_last_nodes = last_nodes;	
				min_curr_nodes = curr_nodes;
				min_next_nodes = next_nodes;
				min_last_edge_index = curr_edge_index + 1;
				min_next_edge_index = curr_edge_index;
			}
			last_nodes = curr_nodes;	
			curr_nodes = next_nodes;
			temp_tfidf_node_p = temp_tfidf_node_p->parent_;
			
		}	
		//cout << min_instances_num << endl;	
		if(support_type_ == 1){
			return min_instances_num*1.0;
		}else{
			return min_instances_num*strength_ratio;
		}

	}else if(support_type_ == 2){// PCRW Support
		Meta_Paths tempmetapath(hin_graph_);
		map<int, double> id_sim;

		tempmetapath.weights_.push_back(1.0);
		tempmetapath.linkTypes_.push_back(meta_path);

		//double pcrw = SimCalculator::calSim_PCRW(src, dst, tempmetapath, id_sim);
		double pcrw = getPCRW(src, dst, meta_path, hin_graph_);
		return 1/(1.0 - log(pcrw));
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
    	map<int, pair<double, double>> edgeTypeAvgDegree = hin_graph_.edge_type_avg_degree_;	

	if((hin_edges_src_.find(src) == hin_edges_src_.end() && hin_edges_dst_.find(src) == hin_edges_dst_.end()) || (hin_edges_src_.find(dst) == hin_edges_src_.end() && hin_edges_dst_.find(dst) == hin_edges_dst_.end()))
	{
		return topKMetaPath_;
	}
	// /*
	bool inverse = false;
	int temp_node;
	int src_edges = hin_edges_src_[src].size() + hin_edges_dst_[src].size();
	//cout << src_edges << endl;
	int dst_edges = hin_edges_src_[dst].size() + hin_edges_dst_[dst].size();
	//cout << dst_edges << endl;
	if(dst_edges < src_edges && support_type_ != 2){
		inverse = true;
		int temp_node = src;
		src = dst;
		dst = temp_node;
	}
	// */	
	// similar pairs information
	set<int> srcSimilarNodes = getSimilarNodes(src, hin_graph_.nodes_);
	set<int> dstSimilarNodes = getSimilarNodes(dst, hin_graph_.nodes_);
	//int similarPairsSize = srcSimilarNodes.size()*dstSimilarNodes.size(); // orignal 
	int similarPairsSize = srcSimilarNodes.size() + dstSimilarNodes.size() - 1; // light version
	double maxRarity = log (similarPairsSize);
	if(rarity_type_ == 0){
		maxRarity = 1.0;
	}	
	double maxSupport = 1.0;
	/*
	if(support_type_ == 1){
		maxSupport = ???;	
	}
	*/
	
	// cout << maxRarity << endl; 
	// queue initialize
	clock_t t2, t1;
	t1 = clock();
	priority_queue<TfIdfNode*, vector<TfIdfNode*>, TfIdfNodePointerCmp> q;
	vector<TfIdfNode*> tmpTfIdfNodeList;
	vector<HIN_Edge> curr_edges_src_ = hin_edges_src_[src];
	vector<HIN_Edge> curr_edges_dst_ = hin_edges_dst_[src];
	map<int, map<int, set<int>>> next_nodes_id_; // edge type -> (destination id -> parent node ids)

	srand((unsigned)time(NULL));
	
	
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
		
		double candidate_max_support = next_nodes_with_parents.size()*1.0;

		TfIdfNode* temp_tfidf_node_p = new TfIdfNode(curr_edge_type, next_nodes_with_parents, getMaxSupport(candidate_max_support), meta_path, NULL);
		if(next_nodes_with_parents.find(dst) != next_nodes_with_parents.end()){
			double support = getSupport(src, dst, temp_tfidf_node_p, meta_path, hin_graph_);
			//double rarity = getRarity(similarPairsSize, srcSimilarNodes, dstSimilarNodes, meta_path, hin_graph_); // original version
			double rarity = getRarity(src, dst, srcSimilarNodes, dstSimilarNodes, meta_path, hin_graph_); // light weight
					double tfidf = support*rarity*penalty(meta_path.size());
					updateTopKMetaPaths(tfidf, support, rarity, meta_path, k, topKMetaPath_);
					temp_tfidf_node_p->curr_nodes_with_parents_.erase(dst);
		}
		q.push(temp_tfidf_node_p);
		
	}
	// BFS
	while(!q.empty()){
		t1 = clock();

		TfIdfNode* curr_tfidf_node_p = q.top();
		tmpTfIdfNodeList.push_back(curr_tfidf_node_p); 
		q.pop();

		vector<int> meta_path = curr_tfidf_node_p->meta_path_;		
		double curr_max_support = curr_tfidf_node_p->max_support_;
		
		int curr_edge_type = meta_path.back();
		
		curr_edge_type = curr_edge_type > 0 ? curr_edge_type: -curr_edge_type;
		if(edgeTypeAvgDegree.find(curr_edge_type) != edgeTypeAvgDegree.end()){
				curr_max_support /= pow(edgeTypeAvgDegree[curr_edge_type].first*edgeTypeAvgDegree[curr_edge_type].second, STRENGTH_ALPHA); 

		}

		if(topKMetaPath_.size() == k){ // stop if maximum tfidf in the queue is less than the minimum one in found meta paths
			//cout << curr_max_support << " " << meta_path.size() << " " << curr_max_support*penalty(meta_path.size())*maxRarity << " " << topKMetaPath_.back().first[0] << endl;
			if(curr_max_support*penalty(meta_path.size())*maxRarity < topKMetaPath_.back().first[0]){

				while(!q.empty()){
					TfIdfNode* tmp_tfidf_node_p = q.top();
							q.pop();			
					delete tmp_tfidf_node_p;
				}
				break;
			}	
		}
		
		
		map<int, set<int>> curr_nodes_with_parents = curr_tfidf_node_p->curr_nodes_with_parents_;
		// Zichen: Do we need to delete this object manually to save some memory ?
		map<int, set<int>> edge_max_instances_num; // edge type -> instances

		//expand
		next_nodes_id_.clear(); // edge type -> (destination id -> parent node id)
		for(map<int, set<int>>::iterator iter = curr_nodes_with_parents.begin(); iter != curr_nodes_with_parents.end(); iter++){
			int temp_node = iter->first;
			iter->second.clear();
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
				
				if (edge_max_instances_num.find(temp_edge_type_) == edge_max_instances_num.end()) {
					edge_max_instances_num[temp_edge_type_] = set<int>();

				}
				edge_max_instances_num[temp_edge_type_].insert(i->dst_);
				
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
				
				if (edge_max_instances_num.find(temp_edge_type_) == edge_max_instances_num.end()) {
					edge_max_instances_num[temp_edge_type_] = set<int>();

				}
				edge_max_instances_num[temp_edge_type_].insert(i->src_);

			}
		}
		curr_nodes_with_parents.clear();

		for(map<int, map<int,set<int>>>::iterator iter = next_nodes_id_.begin(); iter != next_nodes_id_.end(); iter++){
			int curr_edge_type = iter->first;
			vector<int> temp_meta_path = meta_path;
			temp_meta_path.push_back(curr_edge_type);
			map<int, set<int>> next_nodes_with_parents = iter->second;
			
			int candidate_max_support = edge_max_instances_num[curr_edge_type].size();	
			if (candidate_max_support > curr_max_support) { // keep the minimum instance number in the meta path expanding
				candidate_max_support = curr_max_support;
			}
			
			TfIdfNode* temp_tfidf_node_p = new TfIdfNode(curr_edge_type, next_nodes_with_parents, getMaxSupport(candidate_max_support), temp_meta_path, curr_tfidf_node_p);
						
			if (next_nodes_with_parents.find(dst) != next_nodes_with_parents.end()) {
				//double rarity = getRarity(similarPairsSize, srcSimilarNodes, dstSimilarNodes, temp_meta_path, hin_graph_); // original
				double rarity = getRarity(src, dst, srcSimilarNodes, dstSimilarNodes, temp_meta_path, hin_graph_); // light weight

				double support = getSupport(src, dst, temp_tfidf_node_p, temp_meta_path, hin_graph_);

				double tfidf = support * rarity * penalty(temp_meta_path.size());
				updateTopKMetaPaths(tfidf, support, rarity, temp_meta_path, k, topKMetaPath_);
				temp_tfidf_node_p->curr_nodes_with_parents_.erase(dst);
			}

			//clear memory
			for(map<int, set<int>>::iterator it = next_nodes_with_parents.begin(); it != next_nodes_with_parents.end(); it++){
				it->second.clear();
			}
			next_nodes_with_parents.clear();
			for(map<int, set<int>>::iterator it = iter->second.begin(); it != iter->second.end(); it++){
				it->second.clear();
			}
			iter->second.clear();
			/*			
			for(int i = 0; i < temp_meta_path.size() - 1; i++){
							cout << temp_meta_path[i] << "->";
					}
					cout << temp_meta_path.back() << endl;		
			*/ 
			q.push(temp_tfidf_node_p);
				
		}
		next_nodes_id_.clear();
		edge_max_instances_num.clear();

	}

	// delete TfIdfNodes
	for(int i = 0; i < tmpTfIdfNodeList.size(); i++){
		delete tmpTfIdfNodeList[i];
	}
	
	if(inverse){
		for(vector<pair<vector<double>, vector<int>>>::iterator iter = topKMetaPath_.begin(); iter != topKMetaPath_.end(); iter++){
			vector<int> temp_meta_path;
			int meta_path_size = iter->second.size();
			for(int i = iter->second.size() - 1; i >= 0; i--){
				temp_meta_path.push_back(-iter->second[i]);
			}
			iter->second.clear();
			iter->second = temp_meta_path;

		}	
	}

	return topKMetaPath_;
}

vector<pair<vector<double>, vector<int>>> TopKCalculator::getTopKMetaPath_Refiner(int src, int dst, int k, vector<vector<int>> & meta_paths, int score_function, HIN_Graph & hin_graph_){

	vector<pair<vector<double>, vector<int>>> topKMetaPath_;	
	priority_queue<pair<double, int>, vector<pair<double, int>>, ScorePairCmp> q;	

	for(int i = 0; i < meta_paths.size(); i++){
		double score = getMetaPathScore(src, dst, meta_paths[i], score_function, hin_graph_);
		cout << score << endl;
		q.push(make_pair(score, i));
	}

	for(int j = 0; j < k; j++){
		pair<double, int> currScorePair = q.top();
		topKMetaPath_.push_back(make_pair(vector<double> (1, currScorePair.first), meta_paths[currScorePair.second]));
		q.pop();
	}	
	
	return topKMetaPath_;

}

void TopKCalculator::getDstEntities(int src, vector<int> meta_path, set<int> & dst_entities, HIN_Graph & hin_graph_){
	bool full_flag = false;
	dst_entities.clear();
	set<int> curr_entities;
	set<int> next_entities;

	curr_entities.insert(src);
	for(int i = 0; i < meta_path.size(); i++){
		for(set<int>::iterator iter = curr_entities.begin(); iter != curr_entities.end(); iter++){
			set<int> tmp_next_entities;
			getNextEntities(*iter, meta_path[i], tmp_next_entities, hin_graph_);
			for(set<int>::iterator iter_n = tmp_next_entities.begin(); iter_n != tmp_next_entities.end(); iter_n++){
				next_entities.insert(*iter_n);
				if(full_flag){
					break;
				}
			}
			if(full_flag){
				break;
			}
		}

		if(!full_flag){

			int eid = *(next_entities.begin());	
			set<int> candidates = getSimilarNodes(eid, hin_graph_.nodes_, false, false);
			if(next_entities.size() >= candidates.size()){
				full_flag = true;	
			}
		}
		curr_entities = next_entities;
		next_entities.clear();
	}
	if(!full_flag){
		dst_entities = curr_entities;
	}else{
		dst_entities = getSimilarNodes(*(curr_entities.begin()), hin_graph_.nodes_, false, false);
	}
}

void TopKCalculator::saveToFile(vector<vector<int>> topKMetaPaths, string file_name){
	cerr << "start saving the top k meta-paths to " << file_name << "..." << endl;	
	ofstream topKMetaPathsOut(file_name, ios::out);
	for(int i = 0; i < topKMetaPaths.size(); i++){
		vector<int> currMetaPath = topKMetaPaths[i];
		for(int j = 0; j < currMetaPath.size() - 1; j++){
			topKMetaPathsOut << currMetaPath[j] << "\t";
		}
		topKMetaPathsOut << currMetaPath.back() << endl;

	} 
	topKMetaPathsOut.close();
	cerr << "finished saving meta-paths" << endl;

}

vector<vector<int>> TopKCalculator::loadMetaPaths(string file_name){
	cerr << "start loading the meta-paths..." << endl;
	ifstream topKMetaPathsIn(file_name.c_str(), ios::in);
	string line;
	vector<vector<int>> meta_paths;
	while(getline(topKMetaPathsIn, line)){
		vector<string> strs = split(line, "\t");
		meta_paths.push_back(vector<int>());
		for(int i = 0; i < strs.size(); i++){
			meta_paths.back().push_back(atoi(strs[i].c_str()));
		}

	}
	topKMetaPathsIn.close();
	return meta_paths;
}
