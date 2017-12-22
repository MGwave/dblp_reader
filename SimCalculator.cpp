//
//  SimCalculator.cpp
//  DBLP_Reader
//
//  Created by 黄智鹏 on 15/10/8.
//  Copyright (c) 2015年 黄智鹏. All rights reserved.
//

#include "SimCalculator.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <queue>
#include <fstream>
#include <map>
#include <sstream>
#include <algorithm>
#include <ctime>

using namespace std;

SCRW_Node::SCRW_Node(const SCRW_Node & n)
{
    
    src_id_ = n.src_id_;
    pro_ = n.pro_;
    for(int i = 0; i < n.positionV_.size(); ++i){
        positionV_.push_back(n.positionV_[i]);
    }
    
}
SCRW_Node::SCRW_Node(int ide, double pro, vector<double> v):src_id_(ide), pro_(pro)
{
    for(int i = 0; i < v.size(); ++i)
        positionV_.push_back(v[i]);
}
SCRW_Node::SCRW_Node()
{
    src_id_ = 0;
    pro_ = 0.0;
}

vector< vector<int> > SimCalculator::mergeSetIntoJoinResult(vector<HIN_Edge> &theSet, vector<vector<int> > &tempJoinResult, int s_position, int d_position, Meta_Structure & m_struct)
{
    clock_t t1, t2;
    t1 = clock();
    vector< vector<int> > rtn;
    if(theSet.size() == 0)
        return rtn;
    if(tempJoinResult.size() == 0){
        for(int i = 0; i < theSet.size(); ++i){
            vector<int> tempV(m_struct.meta_nodes_.size(), -1);
            tempV[s_position] = theSet[i].src_;
            tempV[d_position] = theSet[i].dst_;
            rtn.push_back(tempV);
        }
        // return rtn;
    }else{
        if(tempJoinResult[0][s_position] == -1){
            if(tempJoinResult[0][d_position] == -1){
                for(int i = 0; i < theSet.size(); ++i){
                    for(int j = 0; j < tempJoinResult.size(); ++j){
                        vector<int> tempV = tempJoinResult[j];
                        tempV[s_position] = theSet[i].src_;
                        tempV[d_position] = theSet[i].dst_;
                        rtn.push_back(tempV);
                    }
                }
            }else{
                map<int, vector<int> > tempTheSet;
                for(int k = 0; k < theSet.size(); ++k)
                    tempTheSet[theSet[k].dst_].push_back(theSet[k].src_);
                for(int i = 0; i < tempJoinResult.size(); ++i){
                    map<int, vector<int> >::iterator iter = tempTheSet.find(tempJoinResult[i][d_position]);
                    if(iter == tempTheSet.end())
                        continue;
                    vector<int> tempV = tempJoinResult[i];
                    for(int j = 0; j < iter->second.size(); ++j){
                        tempV[s_position] = iter->second[j];
                        rtn.push_back(tempV);
                    }
                }
            }
        }else{
            if(tempJoinResult[0][d_position] == -1){
                map<int, vector<int> > tempTheSet;
                for(int k = 0; k < theSet.size(); ++k)
                    tempTheSet[theSet[k].src_].push_back(theSet[k].dst_);
                for(int i = 0; i < tempJoinResult.size(); ++i){
                    map<int, vector<int> >::iterator iter = tempTheSet.find(tempJoinResult[i][s_position]);
                    if(iter == tempTheSet.end())
                        continue;
                    vector<int> tempV = tempJoinResult[i];
                    for(int j = 0; j < iter->second.size(); ++j){
                        tempV[d_position] = iter->second[j];
                        rtn.push_back(tempV);
                    }
                }
                
            }else{
                set< pair<int , int> > tempS;
                for(int i = 0; i < theSet.size(); ++i){
                    tempS.insert(make_pair(theSet[i].src_, theSet[i].dst_));
                }
                for(int i = 0 ; i < tempJoinResult.size(); ++i){
                    if(tempS.find(make_pair(tempJoinResult[i][s_position], tempJoinResult[i][d_position])) != tempS.end()){
                        rtn.push_back(tempJoinResult[i]);
                    }
                }
            }
            
        }
    }
    t2 = clock();
    //cerr << (t2 - t1 + 1.0) / CLOCKS_PER_SEC  << "s in merging" << endl;
    return rtn;
}

vector< vector<int> > SimCalculator::joinEdges(int src, int dst, vector<Meta_Edge> tobe_Edges, Meta_Structure &m_struct)
{
    vector< vector<int> > rtn;
    if(tobe_Edges.size() == 0){
        return rtn;
    }
    Meta_Edge theEdge = tobe_Edges[tobe_Edges.size() - 1];
    tobe_Edges.pop_back();
    vector< vector<int> > subJoin;
    if(tobe_Edges.size() > 0){
        subJoin = joinEdges(src, dst, tobe_Edges, m_struct);
        if(subJoin.size() == 0)
            return subJoin;
    }
    vector<HIN_Edge> hin_edges;
    
    if(theEdge.src_ == 0){
        if(theEdge.dst_ == m_struct.meta_nodes_.size() - 1){
            vector<HIN_Edge> & tempV_ = m_struct.hin_graph_.edges_src_[src];
            bool FOUND = false;
            HIN_Edge tempE;
            for(int i = 0; i < tempV_.size(); ++i){
                if(tempV_[i].dst_ == dst && tempV_[i].edge_type_ == theEdge.type_){
                    FOUND = true;
                    tempE = tempV_[i];
                    break;
                }
            }
            if(FOUND)
                hin_edges.push_back(tempE);
        }else{
            vector<HIN_Edge> & temp_edges_ = m_struct.hin_graph_.edges_src_[src];
            for(int i = 0; i < temp_edges_.size(); ++i){
                if(temp_edges_[i].edge_type_ == theEdge.type_){
                    hin_edges.push_back(temp_edges_[i]);
                }
            }
        }
    }else if(theEdge.src_ == m_struct.meta_nodes_.size() - 1){
        if(theEdge.dst_ == 0){
            vector<HIN_Edge> & tempV_ = m_struct.hin_graph_.edges_dst_[src];
            bool FOUND = false;
            HIN_Edge tempE;
            for(int i = 0; i < tempV_.size(); ++i){
                if(tempV_[i].src_ == dst && theEdge.type_ == tempV_[i].edge_type_){
                    FOUND = true;
                    tempE = tempV_[i];
                    break;
                }
            }
            if(FOUND)
                hin_edges.push_back(tempE);
        }else{
            vector<HIN_Edge> & temp_edges_ = m_struct.hin_graph_.edges_src_[dst];
            for(int i = 0; i < temp_edges_.size(); ++i){
                if(temp_edges_[i].edge_type_ == theEdge.type_){
                    hin_edges.push_back(temp_edges_[i]);
                }
            }
        }
        
    }else{
        if(theEdge.dst_ == m_struct.meta_nodes_.size() - 1){
            vector<HIN_Edge> & temp_edges_ = m_struct.hin_graph_.edges_dst_[dst];
            for(int i = 0; i < temp_edges_.size(); ++i){
                if(temp_edges_[i].edge_type_ == theEdge.type_){
                    hin_edges.push_back(temp_edges_[i]);
                }
            }
        }else if(theEdge.dst_ == 0){
            vector<HIN_Edge> & temp_edges_ = m_struct.hin_graph_.edges_dst_[src];
            for(int i = 0; i < temp_edges_.size(); ++i){
                if(temp_edges_[i].edge_type_ == theEdge.type_){
                    hin_edges.push_back(temp_edges_[i]);
                }
            }
        }else{
            for(map<int, vector<HIN_Edge> >::iterator iter = m_struct.hin_graph_.edges_src_.begin(); iter != m_struct.hin_graph_.edges_src_.end(); ++iter){
                vector<HIN_Edge> & temp_edges_ = iter->second;
                for(int j = 0; j < temp_edges_.size(); ++j){
                    if(temp_edges_[j].edge_type_ == theEdge.type_){
                        hin_edges.push_back(temp_edges_[j]);
                    }
                }
            }
        }
    }
    rtn = mergeSetIntoJoinResult(hin_edges, subJoin, theEdge.src_, theEdge.dst_, m_struct);
    return rtn;
}

vector< vector<int > > SimCalculator::joinEdges_NC(int src, int dst, Meta_Structure &m_struct)
{
    vector<vector<int> > rtn;
    rtn.push_back(vector<int>(m_struct.meta_nodes_.size(), -1));
    rtn[0][0] = src;
    
    queue<Meta_Node> que;
    que.push(m_struct.meta_nodes_[0]);
    while(!que.empty()){
        Meta_Node theNode = que.front();
        que.pop();
        for(vector<Meta_Edge>::iterator iter = m_struct.srt_edges_[theNode.id_].begin(); iter != m_struct.srt_edges_[theNode.id_].end(); ++iter){
            
            que.push(m_struct.meta_nodes_[iter->dst_]);
            
            bool FOUND = false;
            map<int, set<int> > id_next;
            vector< vector<int> > tempRtn;
            
            for(int i = 0; i < rtn.size(); ++i){
                int node_id = rtn[i][theNode.id_];
                if(id_next.find(node_id) != id_next.end())
                    continue;
                if(iter->type_ > 0){
                    for(int j = 0; j < m_struct.hin_graph_.edges_src_[node_id].size(); ++j){
                        if(m_struct.hin_graph_.edges_src_[node_id][j].edge_type_ == iter->type_){
                            FOUND = true;
                            id_next[node_id].insert(m_struct.hin_graph_.edges_src_[node_id][j].dst_);
                        }
                    }
                }else{
                    for(int j = 0; j < m_struct.hin_graph_.edges_dst_[node_id].size(); ++j){
                        if(0 - m_struct.hin_graph_.edges_dst_[node_id][j].edge_type_ == iter->type_){
                            FOUND = true;
                            id_next[node_id].insert(m_struct.hin_graph_.edges_dst_[node_id][j].src_);
                        }
                    }
                }
            }
            // break when we can find no instance for any edge
            if(!FOUND){
                return vector<vector<int> >();
            }
            if(rtn[0][iter->dst_] == -1){
                for(int j = 0; j < rtn.size(); ++j){
                    for(set<int>::iterator k = id_next[rtn[j][theNode.id_]].begin(); k != id_next[rtn[j][theNode.id_]].end(); ++k){
                        tempRtn.push_back(rtn[j]);
                        tempRtn[tempRtn.size() - 1][iter->dst_] = *k;
                    }
                }
                rtn = tempRtn;
            }else{
                for(int j = 0; j < rtn.size(); ++j){
                    if(id_next[rtn[j][theNode.id_]].find(rtn[j][iter->dst_]) != id_next[rtn[j][theNode.id_]].end())
                        tempRtn.push_back(rtn[j]);
                }
                rtn = tempRtn;
            }
        }
    }
    return rtn;
}


vector< vector<int > > SimCalculator::joinEdges_CDFS(int src, int dst, Meta_Structure &m_struct)
{
    vector<map<int,int> > rtn;
    rtn.push_back(map<int,int>());
    rtn[0][0] = src;
    
    queue<Meta_Node> que;
    que.push(m_struct.meta_nodes_[0]);
    
    while(!que.empty()){
        Meta_Node theNode = que.front();
        que.pop();
        for(vector<Meta_Edge>::iterator iter = m_struct.srt_edges_[theNode.id_].begin(); iter != m_struct.srt_edges_[theNode.id_].end(); ++iter){
            
            que.push(m_struct.meta_nodes_[iter->dst_]);
            
            bool FOUND = false;
            map<int, set<int> > id_next;
            vector< map<int, int> > tempRtn;
            
            for(int i = 0; i < rtn.size(); ++i){
                int node_id = rtn[i][theNode.id_];
                if(id_next.find(node_id) != id_next.end())
                    continue;
                if(iter->type_ > 0){
                    for(int j = 0; j < m_struct.hin_graph_.edges_src_[node_id].size(); ++j){
                        if(m_struct.hin_graph_.edges_src_[node_id][j].edge_type_ == iter->type_){
                            FOUND = true;
                            id_next[node_id].insert(m_struct.hin_graph_.edges_src_[node_id][j].dst_);
                        }
                    }
                }else{
                    for(int j = 0; j < m_struct.hin_graph_.edges_dst_[node_id].size(); ++j){
                        if(0 - m_struct.hin_graph_.edges_dst_[node_id][j].edge_type_ == iter->type_){
                            FOUND = true;
                            id_next[node_id].insert(m_struct.hin_graph_.edges_dst_[node_id][j].src_);
                        }
                    }
                }
            }
            // break when we can find no instance for any edge
            if(!FOUND){
                return vector<vector<int> >();
            }
            if(rtn[0].find(iter->dst_) == rtn[0].end()){
                for(int j = 0; j < rtn.size(); ++j){
                    for(set<int>::iterator k = id_next[rtn[j][theNode.id_]].begin(); k != id_next[rtn[j][theNode.id_]].end(); ++k){
                        tempRtn.push_back(rtn[j]);
                        tempRtn[tempRtn.size() - 1][iter->dst_] = *k;
                    }
                }
                rtn = tempRtn;
            }else{
                for(int j = 0; j < rtn.size(); ++j){
                    if(id_next[rtn[j][theNode.id_]].find(rtn[j][iter->dst_]) != id_next[rtn[j][theNode.id_]].end())
                        tempRtn.push_back(rtn[j]);
                }
                rtn = tempRtn;
            }
        }
    }
    vector< vector<int > > RR;
    for(int i = 0; i < rtn.size(); ++i){
        vector<int> v(m_struct.meta_nodes_.size(), 0);
        for(map<int,int>::iterator j = rtn[i].begin(); j != rtn[i].end(); ++j){
            v[j->first] = j->second;
        }
        RR.push_back(v);
    }
    return RR;
}



int SimCalculator::calSim_SCount_NC(int src, int dst, Meta_Structure &m_struct, map<int, int> & id_sim)
{
    vector< vector<int> > join_result = joinEdges_NC(src, dst, m_struct);
    //cout <<"Num of Instance:\t" << join_result.size() << endl;
    for(int i = 0; i < join_result.size(); ++i){
        int tempdst = join_result[i][join_result[i].size() - 1];
        if(id_sim.find(tempdst) == id_sim.end()){
            id_sim[tempdst] = 0;
        }
        id_sim[tempdst]++;
    }
    if(id_sim.find(dst) == id_sim.end())
        return 0;
    return id_sim[dst];
}

int SimCalculator::calSim_SCount_CDFS(int src, int dst, Meta_Structure &m_struct, map<int, int> & id_sim)
{
    vector< vector<int> > join_result = joinEdges_CDFS(src, dst, m_struct);
    //cout <<"Num of Instance:\t" << join_result.size() << endl;
    for(int i = 0; i < join_result.size(); ++i){
        int tempdst = join_result[i][join_result[i].size() - 1];
        if(id_sim.find(tempdst) == id_sim.end()){
            id_sim[tempdst] = 0;
        }
        id_sim[tempdst]++;
    }
    if(id_sim.find(dst) == id_sim.end())
        return 0;
    return id_sim[dst];
}
int SimCalculator::calSim_SCount(int src, int dst, Meta_Structure &m_struct)
{
    
    return 0;
}
/*
void DFS(Meta_Structure &m_struct, vector<int> v, double w, int layer, vector<vector<int> > & rtn, vector<double> & w_r)
{
    if(layer == m_struct.layers_.size() - 1){
        rtn.push_back(v);
        w_r.push_back(w);
        return;
    }
    vector<vector<int> > candidate(m_struct.D_[layer + 1].size());
    for(int k = 0; k < m_struct.D_[layer + 1].size(); ++k){
        int metaID = m_struct.D_[layer + 1][k];
        set<int> c_k;
        
        int edgeType = m_struct.dst_edges_[metaID][0].type_;
        for(int i = 0; i < m_struct.dst_edges_[metaID].size(); ++i){
            fd
            
        }
    }
    
}
*/

double SimCalculator::calSim_DFS(int src, int dst, Meta_Structure &m_struct, map<int, double> &id_sim)
{
    /*
    vector<int> v(m_struct.meta_nodes_.size(), -1);
    v[0] = src;
    vector<vector<int> >  rtn;
    vector<double> d;
    
    DFS(m_struct, v, 1.0, 0, rtn, d);
    for(int i = 0; i < rtn.size(); ++i){
        int tar_id = m_struct.targets_[0];
        if(id_sim.find(rtn[i][tar_id]) == id_sim.end())
            id_sim[rtn[i][tar_id]] = 0;
        id_sim[rtn[i][tar_id]] += d[i];
    }
    if(id_sim.find(dst) == id_sim.end())
        return 0;
    return id_sim[dst];
     */
    return 0;
}

class SCRW_Node_NC
    {
    public:
        int src_id_;
        map<int, double> pro_V;
        double pro_;
    };
double SimCalculator::calSim_SCRW_NC(int src, int dst, Meta_Structure &m_struct, map<int, double> &rtn)
{
    
    queue<SCRW_Node_NC> note_map;
    SCRW_Node_NC tempNode;
    tempNode.src_id_ = src;
    tempNode.pro_V[0] = 1.0;
    tempNode.pro_ = 1.0;
    
    note_map.push(tempNode);
    while(!note_map.empty()){
        SCRW_Node_NC theNode = note_map.front();
        note_map.pop();
        int node_id = theNode.src_id_;
        
        // first remove those points already reach the final position
        if(theNode.pro_V.find(m_struct.meta_nodes_.size() - 1) != theNode.pro_V.end()){
            int len = m_struct.meta_nodes_.size() - 1;
            double metaPro = theNode.pro_V[len];
            if(rtn.find(node_id) == rtn.end()){
                rtn[node_id] = theNode.pro_ * metaPro;
            }else{
                rtn[node_id] += theNode.pro_ * metaPro;
            }
            if(metaPro == 1.0)
                continue;
            theNode.pro_ *= (1- metaPro);
            for(map<int, double>::iterator  i = theNode.pro_V.begin(); i != theNode.pro_V.end(); ++i){
                i->second /= (1- metaPro);
            }
            theNode.pro_V.erase(len);
        }
        
        map<int, map<int, double> > outEdgeType_outNodes;
        map<int, double> outEdgeType_pro;
        
        // get all possible ourEdge type and transition pro
        for(map<int, double>::iterator iterator = theNode.pro_V.begin(); iterator != theNode.pro_V.end(); ++iterator){
            int i = iterator->first;
            double Vpro = iterator->second;
            for(int j = 0; j < m_struct.srt_edges_[i].size(); ++j){
                int tempEdgeType = m_struct.srt_edges_[i][j].type_;
                int tempDst = m_struct.srt_edges_[i][j].dst_;
                if(outEdgeType_outNodes.find(tempEdgeType) == outEdgeType_outNodes.end()){
                    outEdgeType_outNodes[tempEdgeType] = map<int, double>();
                }
                if(outEdgeType_outNodes[tempEdgeType].find(tempDst) == outEdgeType_outNodes[tempEdgeType].end()){
                    outEdgeType_outNodes[tempEdgeType][tempDst] = 0;
                }
                outEdgeType_outNodes[tempEdgeType][tempDst] += Vpro * m_struct.srt_edges_[i][j].weight_ / m_struct.meta_nodes_[i].neighborWeightSum_;
                if(outEdgeType_pro.find(tempEdgeType) == outEdgeType_pro.end())
                    outEdgeType_pro[tempEdgeType] = 0.0;
                outEdgeType_pro[tempEdgeType] += Vpro * m_struct.srt_edges_[i][j].weight_ / m_struct.meta_nodes_[i].neighborWeightSum_;
            }
        }
        // get all possible way out
        map<int, int> realNeiborNum;
        vector<HIN_Edge> EdgesOutToBeAdd, EdgesInToBeAdd;
        for(int i = 0; i < m_struct.hin_graph_.edges_src_[node_id].size(); ++i){
            int tempEdgeType = m_struct.hin_graph_.edges_src_[node_id][i].edge_type_;
            map<int, map<int, double> >::iterator theIter = outEdgeType_outNodes.find(tempEdgeType);
            if(theIter == outEdgeType_outNodes.end()){
                continue;
            }
            if(realNeiborNum.find(m_struct.hin_graph_.edges_src_[node_id][i].edge_type_) == realNeiborNum.end())
                realNeiborNum[m_struct.hin_graph_.edges_src_[node_id][i].edge_type_] = 0;
            realNeiborNum[m_struct.hin_graph_.edges_src_[node_id][i].edge_type_] += 1;
            EdgesOutToBeAdd.push_back(m_struct.hin_graph_.edges_src_[node_id][i]);
        }
        for(int i = 0; i < m_struct.hin_graph_.edges_dst_[node_id].size(); ++i){
            if(outEdgeType_outNodes.find(0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_)
               == outEdgeType_outNodes.end()){
                continue;
            }
            if(realNeiborNum.find(0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_) == realNeiborNum.end())
                realNeiborNum[0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_] = 0;
            realNeiborNum[0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_] += 1;
            EdgesInToBeAdd.push_back(m_struct.hin_graph_.edges_dst_[node_id][i]);
        }
        
        for(int i = 0; i < EdgesOutToBeAdd.size(); ++i){
            int nextID = EdgesOutToBeAdd[i].dst_;
            double nextPro = theNode.pro_ * outEdgeType_pro[EdgesOutToBeAdd[i].edge_type_] / realNeiborNum[EdgesOutToBeAdd[i].edge_type_];
            
            map<int, double> tempNextPro;
            double tempSUM = 0.0;
            for(map<int, double>::iterator j = outEdgeType_outNodes[EdgesOutToBeAdd[i].edge_type_].begin(); j != outEdgeType_outNodes[EdgesOutToBeAdd[i].edge_type_].end(); ++j){
                int dstMetaID = j->first;
                double dstMetaPro = j->second;
                tempSUM += dstMetaPro;
                if(tempNextPro.find(dstMetaID) == tempNextPro.end())
                    tempNextPro[dstMetaID] = 0.0;
                tempNextPro[dstMetaID] += dstMetaPro;
            }
            for(map<int, double>::iterator j = tempNextPro.begin(); j != tempNextPro.end(); ++j){
                j->second /= tempSUM;
            }
            
            SCRW_Node_NC tempNode2;
            tempNode2.src_id_ = nextID;
            tempNode2.pro_ = nextPro;
            tempNode2.pro_V = tempNextPro;
            note_map.push(tempNode2);
        }
        for(int i = 0; i < EdgesInToBeAdd.size(); ++i){
            int nextID = EdgesInToBeAdd[i].src_;
            double nextPro = theNode.pro_ * outEdgeType_pro[-EdgesInToBeAdd[i].edge_type_] / realNeiborNum[-EdgesInToBeAdd[i].edge_type_];
            
            map<int, double> tempNextPro;
            double tempSUM = 0.0;
            for(map<int, double>::iterator j = outEdgeType_outNodes[0 - EdgesInToBeAdd[i].edge_type_].begin(); j != outEdgeType_outNodes[0 - EdgesInToBeAdd[i].edge_type_].end(); ++j){
                int dstMetaID = j->first;
                double dstMetaPro = j->second;
                tempSUM += dstMetaPro;
                if(tempNextPro.find(dstMetaID) == tempNextPro.end())
                    tempNextPro[dstMetaID] = 0.0;
                tempNextPro[dstMetaID] += dstMetaPro;
            }
            for(map<int, double>::iterator j = tempNextPro.begin(); j != tempNextPro.end(); ++j){
                j->second /= tempSUM;
            }
            
            SCRW_Node_NC tempNode2;
            tempNode2.src_id_ = nextID;
            tempNode2.pro_ = nextPro;
            tempNode2.pro_V = tempNextPro;
            note_map.push(tempNode2);
        }
        
    }
    if(rtn.find(dst) == rtn.end())
        return 0.0;

    return rtn[dst];
}

class SCRW_Node_NC2
    {
    public:
        int src_id_;
        vector<int> pro_V;
        
    };
int SimCalculator::calSim_PCount_NC(int src, int dst, Meta_Structure &m_struct, map<int, int> &rtn)
{
    
    queue<SCRW_Node_NC2> note_map;
    SCRW_Node_NC2 tempNode;
    tempNode.src_id_ = src;
    tempNode.pro_V.push_back(0);
    
    
    note_map.push(tempNode);
    while(!note_map.empty()){
        SCRW_Node_NC2 theNode = note_map.front();
        note_map.pop();
        int node_id = theNode.src_id_;
        
        // first remove those points already reach the final position
        int len = m_struct.meta_nodes_.size() - 1;
        for(vector<int>::iterator i = theNode.pro_V.begin(); i != theNode.pro_V.end(); ++i){
            if(*i == len){
                if(rtn.find(node_id) == rtn.end()){
                    rtn[node_id] = 1;
                }else{
                    rtn[node_id] += 1;
                }
                theNode.pro_V.erase(i);
                break;
            }
        }
        if(theNode.pro_V.size() == 0)
            continue;
        
        map<int, vector<int> > outEdgeType_outNodes;
    
        // get all possible ourEdge type and transition pro
        for(vector<int>::iterator iterator = theNode.pro_V.begin(); iterator != theNode.pro_V.end(); ++iterator){
            int i = *iterator;
            for(int j = 0; j < m_struct.srt_edges_[i].size(); ++j){
                int tempEdgeType = m_struct.srt_edges_[i][j].type_;
                int tempDst = m_struct.srt_edges_[i][j].dst_;
                if(outEdgeType_outNodes.find(tempEdgeType) == outEdgeType_outNodes.end()){
                    outEdgeType_outNodes[tempEdgeType] = vector<int>();
                }
                if(find(outEdgeType_outNodes[tempEdgeType].begin(), outEdgeType_outNodes[tempEdgeType].end(), tempDst) == outEdgeType_outNodes[tempEdgeType].end()){
                    outEdgeType_outNodes[tempEdgeType].push_back(tempDst);
                }
            }
        }
        
        // get all possible way out
        for(int i = 0; i < m_struct.hin_graph_.edges_src_[node_id].size(); ++i){
            int tempEdgeType = m_struct.hin_graph_.edges_src_[node_id][i].edge_type_;
            map<int, vector<int> >::iterator theIter = outEdgeType_outNodes.find(tempEdgeType);
            if(theIter == outEdgeType_outNodes.end()){
                continue;
            }
            int nextID = m_struct.hin_graph_.edges_src_[node_id][i].dst_;
            
            vector<int> tempNextPro;
            for(vector<int>::iterator j = outEdgeType_outNodes[tempEdgeType].begin(); j != outEdgeType_outNodes[tempEdgeType].end(); ++j){
                int dstMetaID = *j;
                tempNextPro.push_back(dstMetaID);
            }
            
            SCRW_Node_NC2 tempNode2;
            tempNode2.src_id_ = nextID;
            tempNode2.pro_V = tempNextPro;
            note_map.push(tempNode2);

        }
        for(int i = 0; i < m_struct.hin_graph_.edges_dst_[node_id].size(); ++i){
            if(outEdgeType_outNodes.find(0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_)
               == outEdgeType_outNodes.end()){
                continue;
            }
            int nextID = m_struct.hin_graph_.edges_dst_[node_id][i].src_;
            vector<int> tempNextPro;
            for(vector<int>::iterator j = outEdgeType_outNodes[0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_].begin(); j != outEdgeType_outNodes[0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_].end(); ++j){
                int dstMetaID = *j;
                tempNextPro.push_back(dstMetaID);
            }
            
            SCRW_Node_NC2 tempNode2;
            tempNode2.src_id_ = nextID;
            tempNode2.pro_V = tempNextPro;
            note_map.push(tempNode2);
        }
    }
    if(rtn.find(dst) == rtn.end())
        return 0;
    return rtn[dst];
}


double SimCalculator::calSim_SSim(int src, int dst, Meta_Structure &m_struct)
{
    int sc = calSim_SCount(src, dst, m_struct);
    int total = 0;
    
    total += calSim_SCount_1(0, src, m_struct);
    total += calSim_SCount_1(m_struct.meta_nodes_.size() - 1, dst, m_struct);
    
    return (2.0 *sc)/total;
}

double SimCalculator::calSim_SSim_NC(int src, int dst, Meta_Structure &m_struct)
{
    map<int, int> id_sim1, id_sim2;
    double sc1 = calSim_SCount_NC(src, dst, m_struct, id_sim1);
    if(sc1 == 0.0)
        return 0.0;
    double sc3 = calSim_SCount_NC(dst, dst, m_struct, id_sim2);
    double sc2 = 0.0;
    if(id_sim1.find(src) != id_sim1.end())
        sc2 = (double)id_sim1[src];
    return 2.0 * sc1 / (sc2 + sc3);
}

double SimCalculator::calSim_SSim_NC_all(int src, int dst, Meta_Structure &m_struct, map<int, double> & id_sim)
{
    id_sim.clear();
    map<int, int> id_sim1;
    double sc1 = calSim_SCount_NC(src, dst, m_struct, id_sim1);
    
    int x = 0;
    if(id_sim1.find(src) != id_sim1.end())
        x = id_sim1[src];
    
    for(map<int, int>::iterator i = id_sim1.begin(); i != id_sim1.end(); ++i){
        double sc = (double)i->second;
        int id = i->first;
        
        map<int, int> id_sim2;
        int y = calSim_SCount_NC(id, id, m_struct, id_sim2);
        id_sim[id] = 2.0 * sc / (x + y);
    }
    if(sc1 == 0.0)
        return 0.0;
    if(id_sim.find(dst) != id_sim.end())
        return id_sim[dst];
    return 0.0;
}

class TempNode
    {
    public:
        map<int, int> meta_hin_;
        double weight_;
    };

void SimCalculator::calSim_SCRW_NC_mul(int src, Meta_Structure &m_struct, map<vector<int>, double> &id_sim, double alpha)
{
    id_sim.clear();
    
    vector<TempNode> currentV;
    TempNode temp;
    
    temp.meta_hin_[0] = src;
    temp.weight_ = 1.0;
  

    currentV.push_back(temp);
    
    vector<int> inDegree;
    for(int i = 0; i < m_struct.meta_nodes_.size(); ++i){
        inDegree.push_back(m_struct.dst_edges_[i].size());
    }
    for(int i = 0; i < m_struct.srt_edges_[0].size(); ++i){
        inDegree[m_struct.srt_edges_[0][i].dst_]--;
    }
    inDegree[0] = -1;
    
    int numOfNotVisitedNodes = m_struct.meta_nodes_.size() - 1;
    
    while (numOfNotVisitedNodes > 0) {
        map<int, vector<int> > zeroInMap;
        for (int i = 0; i < inDegree.size(); ++i) {
            if(inDegree[i] == 0){
                zeroInMap[i] = vector<int>();
                inDegree[i] = -1;
            }
        }
        numOfNotVisitedNodes -= zeroInMap.size();
        for(map<int, vector<int> >::iterator iter = zeroInMap.begin(); iter != zeroInMap.end(); ++iter){
            // remove the outedges
            for(int j = 0; j < m_struct.srt_edges_[iter->first].size(); ++j){
                inDegree[m_struct.srt_edges_[iter->first][j].dst_]--;
            }
        }
        vector<TempNode> cumulateV;
        for(int i = 0; i < currentV.size(); ++i){
            map<int, vector<int> > candidatesOnMetaID;
            for(map<int, vector<int> >::iterator iter = zeroInMap.begin(); iter != zeroInMap.end(); ++iter){
                candidatesOnMetaID[iter->first] = vector<int>();
                vector<int> dependencyNodes;
                for(int j = 0; j < m_struct.dst_edges_[iter->first].size(); ++j){
                    dependencyNodes.push_back(m_struct.dst_edges_[iter->first][j].src_);
                }
                int meta_id = dependencyNodes[0];
                int hin_id = currentV[i].meta_hin_[meta_id];
                int edgeType = m_struct.dst_edges_[iter->first][0].type_;
                // candidate id with respect to the first dependency
                set<int> candidates;
                if(edgeType > 0){
                    for(int j = 0; j < m_struct.hin_graph_.edges_src_[hin_id].size(); ++j){
                        if(m_struct.hin_graph_.edges_src_[hin_id][j].edge_type_ == edgeType){
                            candidates.insert(m_struct.hin_graph_.edges_src_[hin_id][j].dst_);
                        }
                    }
                }else{
                    for(int j = 0; j < m_struct.hin_graph_.edges_dst_[hin_id].size(); ++j){
                        if(m_struct.hin_graph_.edges_dst_[hin_id][j].edge_type_ == -edgeType){
                            candidates.insert(m_struct.hin_graph_.edges_dst_[hin_id][j].src_);
                        }
                    }
                }
                // remove those candidates cannot match other dependency
                for(int k = 1; k < dependencyNodes.size(); ++k){
                    set<int> tempcandidates;
                    int meta_id = dependencyNodes[k];
                    int hin_id = currentV[i].meta_hin_[meta_id];
                    int edgeType = m_struct.dst_edges_[iter->first][k].type_;
                    if(edgeType > 0){
                        for(int j = 0; j < m_struct.hin_graph_.edges_src_[hin_id].size(); ++j){
                            if(m_struct.hin_graph_.edges_src_[hin_id][j].edge_type_ == edgeType){
                                tempcandidates.insert(m_struct.hin_graph_.edges_src_[hin_id][j].dst_);
                            }
                        }
                    }else{
                        for(int j = 0; j < m_struct.hin_graph_.edges_dst_[hin_id].size(); ++j){
                            if(m_struct.hin_graph_.edges_dst_[hin_id][j].edge_type_ == -edgeType){
                                tempcandidates.insert(m_struct.hin_graph_.edges_dst_[hin_id][j].src_);
                            }
                        }
                    }
                    set<int>::iterator iter2 = candidates.begin();
                    while(iter2 != candidates.end()){
                        if(tempcandidates.find(*iter2) == tempcandidates.end()){
                            iter2 = candidates.erase(iter2);
                        }else{
                            iter2++;
                        }
                    }
                }
                for(set<int>::iterator iter2 = candidates.begin(); iter2 != candidates.end(); ++iter2){
                    candidatesOnMetaID[iter->first].push_back(*iter2);
                }
            }
            TempNode tempNodeTemplate;
            int totalSize = 1;
            for(map<int, vector<int> >::iterator iterator = candidatesOnMetaID.begin(); iterator != candidatesOnMetaID.end(); ++iterator){
                totalSize *= iterator->second.size();
            }
            // The important equation
            //tempNodeTemplate.weight_ = pow(currentV[i].weight_ / totalSize, alpha);
            tempNodeTemplate.weight_ = currentV[i].weight_  / pow((double)totalSize, alpha);
            
            for(map<int, int>::iterator j = currentV[i].meta_hin_.begin(); j != currentV[i].meta_hin_.end(); ++j){
                if(find(m_struct.targets_.begin(), m_struct.targets_.end(), j->first) != m_struct.targets_.end()){
                    tempNodeTemplate.meta_hin_[j->first] = j->second;
                }
            }
            // for each i
            vector<TempNode> tempCurrentV;
            tempCurrentV.push_back(tempNodeTemplate);
            
            for(map<int, vector<int> >::iterator iterator = candidatesOnMetaID.begin(); iterator != candidatesOnMetaID.end(); ++iterator){
                vector<TempNode> temptempCurrentV;
                for(int k = 0; k < tempCurrentV.size(); ++k){
                    for(int ii = 0; ii < iterator->second.size(); ++ii){
                        int meta = iterator->first;
                        int hin = iterator->second[ii];
                        TempNode temptemp = tempCurrentV[k];
                        temptemp.meta_hin_[meta] = hin;
                        temptempCurrentV.push_back(temptemp);
                    }
                }
                tempCurrentV = temptempCurrentV;
            }
            for(int k = 0; k < tempCurrentV.size(); ++k){
                cumulateV.push_back(tempCurrentV[k]);
            }
        }
        currentV = cumulateV;
    }
    for(int i = 0; i < currentV.size(); ++i){
        vector<int> metaV;
        for(int j = 0; j < m_struct.targets_.size(); ++j){
            metaV.push_back(currentV[i].meta_hin_[m_struct.targets_[j]]);
            if(id_sim.find(metaV) == id_sim.end()){
                id_sim[metaV] = 0;
            }
            id_sim[metaV] += currentV[i].weight_;
        }
    }
    return;
}

int SimCalculator::calSim_SCount_1(int position, int src, Meta_Structure &m_struct)
{
    //   if(find(m_struct.hin_graph_.nodes_[src].types_id_.begin(), m_struct.hin_graph_.nodes_[src].types_id_.end(), m_struct.meta_nodes_[position].type_) == m_struct.hin_graph_.nodes_[src].types_id_.end())
    //     return 0;
    
    // Edges to be join
    vector<Meta_Edge> tobe_Edges;
    for(int i = 0; i < m_struct.meta_nodes_.size(); ++i){
        for(int j = 0; j < m_struct.srt_edges_[i].size(); ++j){
            tobe_Edges.push_back(m_struct.srt_edges_[i][j]);
        }
    }
    
    vector< vector<int> > join_result = joinEdges_1(position, src, tobe_Edges, m_struct);
    return join_result.size();
}

vector< vector<int> > SimCalculator::joinEdges_1(int position, int src, vector<Meta_Edge> tobe_Edges, Meta_Structure &m_struct){
    vector< vector<int> > rtn;
    if(tobe_Edges.size() == 0){
        return rtn;
    }
    Meta_Edge theEdge = tobe_Edges[tobe_Edges.size() - 1];
    tobe_Edges.pop_back();
    vector< vector<int> > subJoin;
    if(tobe_Edges.size() > 0){
        subJoin = joinEdges_1(position, src, tobe_Edges, m_struct);
        if(subJoin.size() == 0)
            return subJoin;
    }
    vector<HIN_Edge> hin_edges;
    
    if(theEdge.src_ == position){
        vector<HIN_Edge> & temp_edges_ = m_struct.hin_graph_.edges_src_[src];
        for(int i = 0; i < temp_edges_.size(); ++i){
            if(temp_edges_[i].edge_type_ == theEdge.type_){
                hin_edges.push_back(temp_edges_[i]);
            }
        }
        
    }else{
        if(theEdge.dst_ == position){
            vector<HIN_Edge> & temp_edges_ = m_struct.hin_graph_.edges_dst_[src];
            for(int i = 0; i < temp_edges_.size(); ++i){
                if(temp_edges_[i].edge_type_ == theEdge.type_){
                    hin_edges.push_back(temp_edges_[i]);
                }
            }
        }else{
            for(map<int, vector<HIN_Edge> >::iterator iter = m_struct.hin_graph_.edges_src_.begin(); iter != m_struct.hin_graph_.edges_src_.end(); ++iter){
                vector<HIN_Edge> & temp_edges_ = iter->second;
                for(int j = 0; j < temp_edges_.size(); ++j){
                    if(temp_edges_[j].edge_type_ == theEdge.type_){
                        hin_edges.push_back(temp_edges_[j]);
                    }
                }
            }
        }
    }
    rtn = mergeSetIntoJoinResult(hin_edges, subJoin, theEdge.src_, theEdge.dst_, m_struct);
    return rtn;
}



double SimCalculator::calSim_SCRW(int src, int dst, Meta_Structure &m_struct, map<int, double> & id_sim)
{
    // Type detection
    if(NODETYPE == 1){
        if(find(m_struct.hin_graph_.nodes_[src].types_id_.begin(), m_struct.hin_graph_.nodes_[src].types_id_.end(), m_struct.meta_nodes_[0].type_) == m_struct.hin_graph_.nodes_[src].types_id_.end())
            return 0;
        if(find(m_struct.hin_graph_.nodes_[dst].types_id_.begin(), m_struct.hin_graph_.nodes_[dst].types_id_.end(), m_struct.meta_nodes_[m_struct.meta_nodes_.size() - 1].type_) == m_struct.hin_graph_.nodes_[dst].types_id_.end())
            return 0;
        vector<double> pro_V(m_struct.meta_nodes_.size(), 0.0);
        pro_V[0] = 1.0;
        
        SCRW_Node tempNode(src, 1.0, pro_V);
        map<int, SCRW_Node> note_map;
        note_map[src] = tempNode;
        
        id_sim = SCRW_Sim_NODETYPE(dst, m_struct, note_map);
        
        if(id_sim.find(dst) == id_sim.end())
            return 0.0;
        return id_sim[dst];
    }
    vector<double> pro_V(m_struct.meta_nodes_.size(), 0.0);
    pro_V[0] = 1.0;
    
    SCRW_Node tempNode(src, 1.0, pro_V);
    map<int, SCRW_Node> note_map;
    note_map[src] = tempNode;
    
    id_sim = SCRW_Sim(dst, m_struct, note_map);
    
    if(id_sim.find(dst) == id_sim.end())
        return 0.0;
    return id_sim[dst];
}

/*
 // To be Changed!!!
 double SimCalculator::calSim_HETES(int src, int dst, Meta_Structure &m_struct, map<int, double> & id_sim)
 {
 // Type detection
 //if(find(m_struct.hin_graph_.nodes_[src].types_id_.begin(), m_struct.hin_graph_.nodes_[src].types_id_.end(), m_struct.meta_nodes_[0].type_) == m_struct.hin_graph_.nodes_[src].types_id_.end())
 //  return 0;
 //if(find(m_struct.hin_graph_.nodes_[dst].types_id_.begin(), m_struct.hin_graph_.nodes_[dst].types_id_.end(), m_struct.meta_nodes_[m_struct.meta_nodes_.size() - 1].type_) == m_struct.hin_graph_.nodes_[dst].types_id_.end())
 //  return 0;
 
 vector<double> pro_V(m_struct.meta_nodes_.size(), 0.0);
 pro_V[0] = 1.0;
 
 SCRW_Node tempNode(src, 1.0, pro_V);
 map<int, SCRW_Node> note_map;
 note_map[src] = tempNode;
 
 id_sim = SCRW_Sim(dst, m_struct, note_map);
 
 if(id_sim.find(dst) == id_sim.end())
 return 0.0;
 return id_sim[dst];
 }
 */

map<int, double>  SimCalculator::SCRW_Sim(int dst, Meta_Structure &m_struct, map<int, SCRW_Node> &note_map)
{
    map<int, double> rtn;
    while(true){
        map<int, SCRW_Node> new_note_map;
        // for each unhandled node
        for(map<int, SCRW_Node>::iterator iter = note_map.begin(); iter != note_map.end(); ++iter){
            int node_id = iter->first;
            SCRW_Node & theNode = iter->second;
            
            // if the pro is too slow, just leave it in the new_note_map
            if(theNode.pro_ < EPSILON){
                if(new_note_map.find(node_id) == new_note_map.end()){
                    new_note_map[node_id] = theNode;
                }else{
                    for(int j = 0; j < theNode.positionV_.size(); ++j){
                        new_note_map[node_id].positionV_[j] = (new_note_map[node_id].positionV_[j] * new_note_map[node_id].pro_ + theNode.positionV_[j] * theNode.pro_) / (theNode.pro_ + new_note_map[node_id].pro_);
                    }
                    new_note_map[node_id].pro_ += theNode.pro_;
                }
                continue;
            }
            // first remove those points already reach the final position
            if(theNode.positionV_[theNode.positionV_.size() - 1] != 0.0){
                if(rtn.find(node_id) == rtn.end()){
                    rtn[node_id] = theNode.pro_ * theNode.positionV_[theNode.positionV_.size() - 1];
                }else{
                    rtn[node_id] += theNode.pro_ * theNode.positionV_[theNode.positionV_.size() - 1];
                }
                double FinalPro = theNode.positionV_[theNode.positionV_.size() - 1];
                if(FinalPro == 1.0)
                    continue;
                theNode.pro_ *= (1- FinalPro);
                for(int  i = 0; i < theNode.positionV_.size(); ++i){
                    theNode.positionV_[i] /= (1- FinalPro);
                }
                theNode.positionV_[theNode.positionV_.size() - 1] = 0.0;
            }
            
            map<int, map<int, double> > outEdgeType_outNodes;
            map<int, double> outEdgeType_pro;
            
            // get all possible ourEdge type and transition pro
            for(int i = 0; i < theNode.positionV_.size(); ++i){
                if(theNode.positionV_[i] == 0)
                    continue;
                //int neiborNum = m_struct.srt_edges_[i].size() + m_struct.dst_edges_[i].size();
                //double addElement = theNode.positionV_[i] / neiborNum;
                
                for(int j = 0; j < m_struct.srt_edges_[i].size(); ++j){
                    int tempEdgeType = m_struct.srt_edges_[i][j].type_;
                    int tempDst = m_struct.srt_edges_[i][j].dst_;
                    if(outEdgeType_outNodes.find(tempEdgeType) == outEdgeType_outNodes.end()){
                        outEdgeType_outNodes[tempEdgeType] = map<int, double>();
                    }
                    if(outEdgeType_outNodes[tempEdgeType].find(tempDst) == outEdgeType_outNodes[tempEdgeType].end()){
                        outEdgeType_outNodes[tempEdgeType][tempDst] = 0;
                    }
                    outEdgeType_outNodes[tempEdgeType][tempDst] += theNode.positionV_[i] * m_struct.srt_edges_[i][j].weight_ / m_struct.meta_nodes_[i].neighborWeightSum_;
                    if(outEdgeType_pro.find(tempEdgeType) == outEdgeType_pro.end())
                        outEdgeType_pro[tempEdgeType] = 0.0;
                    outEdgeType_pro[tempEdgeType] += theNode.positionV_[i] * m_struct.srt_edges_[i][j].weight_ / m_struct.meta_nodes_[i].neighborWeightSum_;
                }
                for(int j = 0; j < m_struct.dst_edges_[i].size(); ++j){
                    int tempEdgeType = 0 - m_struct.dst_edges_[i][j].type_;
                    int tempSrc = m_struct.dst_edges_[i][j].src_;
                    if(outEdgeType_outNodes.find(tempEdgeType) == outEdgeType_outNodes.end()){
                        outEdgeType_outNodes[tempEdgeType] = map<int, double>();
                    }
                    if(outEdgeType_outNodes[tempEdgeType].find(tempSrc) == outEdgeType_outNodes[tempEdgeType].end()){
                        outEdgeType_outNodes[tempEdgeType][tempSrc] = 0;
                    }
                    outEdgeType_outNodes[tempEdgeType][tempSrc] += theNode.positionV_[i] * m_struct.dst_edges_[i][j].weight_ / m_struct.meta_nodes_[i].neighborWeightSum_;
                    if(outEdgeType_pro.find(tempEdgeType) == outEdgeType_pro.end())
                        outEdgeType_pro[tempEdgeType] = 0.0;
                    outEdgeType_pro[tempEdgeType] += theNode.positionV_[i] * m_struct.dst_edges_[i][j].weight_ / m_struct.meta_nodes_[i].neighborWeightSum_;
                    
                }
                
            }
            
            // get all possible way out
            map<int, int> realNeiborNum;
            //int realNeiborNum = 0;
            vector<HIN_Edge> EdgesOutToBeAdd, EdgesInToBeAdd;
            for(int i = 0; i < m_struct.hin_graph_.edges_src_[node_id].size(); ++i){
                int tempEdgeType = m_struct.hin_graph_.edges_src_[node_id][i].edge_type_;
                map<int, map<int, double> >::iterator theIter = outEdgeType_outNodes.find(tempEdgeType);
                if(theIter == outEdgeType_outNodes.end()){
                    continue;
                }
                if(realNeiborNum.find(m_struct.hin_graph_.edges_src_[node_id][i].edge_type_) == realNeiborNum.end())
                    realNeiborNum[m_struct.hin_graph_.edges_src_[node_id][i].edge_type_] = 0;
                realNeiborNum[m_struct.hin_graph_.edges_src_[node_id][i].edge_type_] += 1;
                EdgesOutToBeAdd.push_back(m_struct.hin_graph_.edges_src_[node_id][i]);
            }
            for(int i = 0; i < m_struct.hin_graph_.edges_dst_[node_id].size(); ++i){
                if(outEdgeType_outNodes.find(0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_)
                   == outEdgeType_outNodes.end()){
                    continue;
                }
                if(realNeiborNum.find(0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_) == realNeiborNum.end())
                    realNeiborNum[0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_] = 0;
                realNeiborNum[0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_] += 1;
                EdgesInToBeAdd.push_back(m_struct.hin_graph_.edges_dst_[node_id][i]);
            }
            
            for(int i = 0; i < EdgesOutToBeAdd.size(); ++i){
                int nextID = EdgesOutToBeAdd[i].dst_;
                double nextPro = theNode.pro_ * outEdgeType_pro[EdgesOutToBeAdd[i].edge_type_] / realNeiborNum[EdgesOutToBeAdd[i].edge_type_];
                
                vector<double> tempNextPro(theNode.positionV_.size(), 0.0);
                double tempSUM = 0.0;
                for(map<int, double>::iterator j = outEdgeType_outNodes[EdgesOutToBeAdd[i].edge_type_].begin(); j != outEdgeType_outNodes[EdgesOutToBeAdd[i].edge_type_].end(); ++j){
                    int dstMetaID = j->first;
                    double dstMetaPro = j->second;
                    tempSUM += dstMetaPro;
                    tempNextPro[dstMetaID] += dstMetaPro;
                }
                for(int j = 0; j < tempNextPro.size(); ++j){
                    tempNextPro[j] /= tempSUM;
                }
                
                SCRW_Node tempNode(nextID, nextPro, tempNextPro);
                if(new_note_map.find(nextID) == new_note_map.end()){
                    new_note_map[nextID] = tempNode;
                }else{
                    for(int j = 0; j < tempNextPro.size(); ++j){
                        new_note_map[nextID].positionV_[j] = (new_note_map[nextID].positionV_[j] * new_note_map[nextID].pro_ + tempNode.positionV_[j] * tempNode.pro_) / (tempNode.pro_ + new_note_map[nextID].pro_);
                    }
                    new_note_map[nextID].pro_ += tempNode.pro_;
                }
            }
            for(int i = 0; i < EdgesInToBeAdd.size(); ++i){
                int nextID = EdgesInToBeAdd[i].src_;
                double nextPro = theNode.pro_ * outEdgeType_pro[-EdgesInToBeAdd[i].edge_type_] / realNeiborNum[-EdgesInToBeAdd[i].edge_type_];
                vector<double> tempNextPro(theNode.positionV_.size(), 0.0);
                double tempSUM = 0.0;
                for(map<int, double>::iterator j = outEdgeType_outNodes[0 - EdgesInToBeAdd[i].edge_type_].begin(); j != outEdgeType_outNodes[0 - EdgesInToBeAdd[i].edge_type_].end(); ++j){
                    int dstMetaID = j->first;
                    double dstMetaPro = j->second;
                    tempSUM += dstMetaPro;
                    tempNextPro[dstMetaID] += dstMetaPro;
                }
                for(int j = 0; j < tempNextPro.size(); ++j){
                    tempNextPro[j] /= tempSUM;
                }
                
                SCRW_Node tempNode(nextID, nextPro, tempNextPro);
                if(new_note_map.find(nextID) == new_note_map.end()){
                    new_note_map[nextID] = tempNode;
                }else{
                    for(int j = 0; j < tempNextPro.size(); ++j){
                        new_note_map[nextID].positionV_[j] = (new_note_map[nextID].positionV_[j] * new_note_map[nextID].pro_ + tempNode.positionV_[j] * tempNode.pro_) / (tempNode.pro_ + new_note_map[nextID].pro_);
                    }
                    new_note_map[nextID].pro_ += tempNode.pro_;
                }
            }
            
        }
        double maxChange = 0.0;
        for(map<int, SCRW_Node>::iterator i = new_note_map.begin(); i != new_note_map.end(); ++i){
            int tempID = i->first;
            if(note_map.find(tempID) == note_map.end())
                maxChange = i->second.pro_;
            else if ( abs(i->second.pro_ - note_map[tempID].pro_) > maxChange){
                maxChange = abs(i->second.pro_ - note_map[tempID].pro_);
            }
        }
        
        note_map = new_note_map;
        if(maxChange < EPSILON)
            break;
    }
    return rtn;
}

map<int, double>  SimCalculator::SCRW_Sim_NODETYPE(int dst, Meta_Structure &m_struct, map<int, SCRW_Node> &note_map)
{
    map<int, double> rtn;
    while(true){
        map<int, SCRW_Node> new_note_map;
        // for each unhandled node
        for(map<int, SCRW_Node>::iterator iter = note_map.begin(); iter != note_map.end(); ++iter){
            int node_id = iter->first;
            SCRW_Node & theNode = iter->second;
            
            // if the pro is too slow, just leave it in the new_note_map
            if(theNode.pro_ < EPSILON){
                if(new_note_map.find(node_id) == new_note_map.end()){
                    new_note_map[node_id] = theNode;
                }else{
                    for(int j = 0; j < theNode.positionV_.size(); ++j){
                        new_note_map[node_id].positionV_[j] = (new_note_map[node_id].positionV_[j] * new_note_map[node_id].pro_ + theNode.positionV_[j] * theNode.pro_) / (theNode.pro_ + new_note_map[node_id].pro_);
                    }
                    new_note_map[node_id].pro_ += theNode.pro_;
                }
                continue;
            }
            // first remove those points already reach the final position
            if(theNode.positionV_[theNode.positionV_.size() - 1] != 0.0){
                if(rtn.find(node_id) == rtn.end()){
                    rtn[node_id] = theNode.pro_ * theNode.positionV_[theNode.positionV_.size() - 1];
                }else{
                    rtn[node_id] += theNode.pro_ * theNode.positionV_[theNode.positionV_.size() - 1];
                }
                double FinalPro = theNode.positionV_[theNode.positionV_.size() - 1];
                if(FinalPro == 1.0)
                    continue;
                theNode.pro_ *= (1- FinalPro);
                for(int  i = 0; i < theNode.positionV_.size(); ++i){
                    theNode.positionV_[i] /= (1- FinalPro);
                }
                theNode.positionV_[theNode.positionV_.size() - 1] = 0.0;
            }
            
            map<int, map<int, double> > outEdgeType_outNodes;
            map<int, double> outEdgeType_pro;
            
            // get all possible ourEdge type and transition pro
            for(int i = 0; i < theNode.positionV_.size(); ++i){
                if(theNode.positionV_[i] == 0)
                    continue;
                //int neiborNum = m_struct.srt_edges_[i].size() + m_struct.dst_edges_[i].size();
                //double addElement = theNode.positionV_[i] / neiborNum;
                
                for(int j = 0; j < m_struct.srt_edges_[i].size(); ++j){
                    int tempEdgeType = m_struct.srt_edges_[i][j].type_;
                    int tempDst = m_struct.srt_edges_[i][j].dst_;
                    if(outEdgeType_outNodes.find(tempEdgeType) == outEdgeType_outNodes.end()){
                        outEdgeType_outNodes[tempEdgeType] = map<int, double>();
                    }
                    if(outEdgeType_outNodes[tempEdgeType].find(tempDst) == outEdgeType_outNodes[tempEdgeType].end()){
                        outEdgeType_outNodes[tempEdgeType][tempDst] = 0;
                    }
                    outEdgeType_outNodes[tempEdgeType][tempDst] += theNode.positionV_[i] * m_struct.srt_edges_[i][j].weight_ / m_struct.meta_nodes_[i].neighborWeightSum_;
                    if(outEdgeType_pro.find(tempEdgeType) == outEdgeType_pro.end())
                        outEdgeType_pro[tempEdgeType] = 0.0;
                    outEdgeType_pro[tempEdgeType] += theNode.positionV_[i] * m_struct.srt_edges_[i][j].weight_ / m_struct.meta_nodes_[i].neighborWeightSum_;
                }
                for(int j = 0; j < m_struct.dst_edges_[i].size(); ++j){
                    int tempEdgeType = 0 - m_struct.dst_edges_[i][j].type_;
                    int tempSrc = m_struct.dst_edges_[i][j].src_;
                    if(outEdgeType_outNodes.find(tempEdgeType) == outEdgeType_outNodes.end()){
                        outEdgeType_outNodes[tempEdgeType] = map<int, double>();
                    }
                    if(outEdgeType_outNodes[tempEdgeType].find(tempSrc) == outEdgeType_outNodes[tempEdgeType].end()){
                        outEdgeType_outNodes[tempEdgeType][tempSrc] = 0;
                    }
                    outEdgeType_outNodes[tempEdgeType][tempSrc] += theNode.positionV_[i] * m_struct.dst_edges_[i][j].weight_ / m_struct.meta_nodes_[i].neighborWeightSum_;
                    if(outEdgeType_pro.find(tempEdgeType) == outEdgeType_pro.end())
                        outEdgeType_pro[tempEdgeType] = 0.0;
                    outEdgeType_pro[tempEdgeType] += theNode.positionV_[i] * m_struct.dst_edges_[i][j].weight_ / m_struct.meta_nodes_[i].neighborWeightSum_;
                    
                }
                
            }
            
            // get all possible way out
            map<int, int> realNeiborNum;
            //int realNeiborNum = 0;
            vector<HIN_Edge> EdgesOutToBeAdd, EdgesInToBeAdd;
            for(int i = 0; i < m_struct.hin_graph_.edges_src_[node_id].size(); ++i){
                int tempEdgeType = m_struct.hin_graph_.edges_src_[node_id][i].edge_type_;
                map<int, map<int, double> >::iterator theIter = outEdgeType_outNodes.find(tempEdgeType);
                if(theIter == outEdgeType_outNodes.end()){
                    continue;
                }
                int tempDst = m_struct.hin_graph_.edges_src_[node_id][i].dst_;
                HIN_Node tempDstNode = m_struct.hin_graph_.nodes_[tempDst];
                bool FOUND = false;
                for(map<int, double>::iterator iter = theIter->second.begin(); iter != theIter->second.end(); ++iter){
                    int NodeType = m_struct.meta_nodes_[iter->first].type_;
                    if(find(tempDstNode.types_id_.begin(), tempDstNode.types_id_.end(), NodeType) == tempDstNode.types_id_.end()){
                        continue;
                    }
                    FOUND = true;
                    break;
                }
                if(!FOUND){
                    // NODE TYPE not matched!
                    continue;
                }
                if(realNeiborNum.find(m_struct.hin_graph_.edges_src_[node_id][i].edge_type_) == realNeiborNum.end())
                    realNeiborNum[m_struct.hin_graph_.edges_src_[node_id][i].edge_type_] = 0;
                realNeiborNum[m_struct.hin_graph_.edges_src_[node_id][i].edge_type_] += 1;
                EdgesOutToBeAdd.push_back(m_struct.hin_graph_.edges_src_[node_id][i]);
            }
            for(int i = 0; i < m_struct.hin_graph_.edges_dst_[node_id].size(); ++i){
                int tempEdgeType = m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_;
                map<int, map<int, double> >::iterator theIter = outEdgeType_outNodes.find(0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_);
                if(theIter == outEdgeType_outNodes.end()){
                    continue;
                }
                int tempSrc = m_struct.hin_graph_.edges_dst_[node_id][i].src_;
                HIN_Node tempSrcNode = m_struct.hin_graph_.nodes_[tempSrc];
                
                bool FOUND = false;
                for(map<int, double>::iterator iter = theIter->second.begin(); iter != theIter->second.end(); ++iter){
                    int NodeType = m_struct.meta_nodes_[iter->first].type_;
                    if(find(tempSrcNode.types_id_.begin(), tempSrcNode.types_id_.end(), NodeType) == tempSrcNode.types_id_.end()){
                        continue;
                    }
                    FOUND = true;
                    break;
                }
                if(!FOUND){
                    // NODE TYPE not matched!
                    continue;
                }
                
                
                if(realNeiborNum.find(0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_) == realNeiborNum.end())
                    realNeiborNum[0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_] = 0;
                realNeiborNum[0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_] += 1;
                EdgesInToBeAdd.push_back(m_struct.hin_graph_.edges_dst_[node_id][i]);
            }
            
            for(int i = 0; i < EdgesOutToBeAdd.size(); ++i){
                int nextID = EdgesOutToBeAdd[i].dst_;
                double nextPro = theNode.pro_ * outEdgeType_pro[EdgesOutToBeAdd[i].edge_type_] / realNeiborNum[EdgesOutToBeAdd[i].edge_type_];
                
                vector<double> tempNextPro(theNode.positionV_.size(), 0.0);
                double tempSUM = 0.0;
                for(map<int, double>::iterator j = outEdgeType_outNodes[EdgesOutToBeAdd[i].edge_type_].begin(); j != outEdgeType_outNodes[EdgesOutToBeAdd[i].edge_type_].end(); ++j){
                    int dstMetaID = j->first;
                    double dstMetaPro = j->second;
                    tempSUM += dstMetaPro;
                    tempNextPro[dstMetaID] += dstMetaPro;
                }
                for(int j = 0; j < tempNextPro.size(); ++j){
                    tempNextPro[j] /= tempSUM;
                }
                
                SCRW_Node tempNode(nextID, nextPro, tempNextPro);
                if(new_note_map.find(nextID) == new_note_map.end()){
                    new_note_map[nextID] = tempNode;
                }else{
                    for(int j = 0; j < tempNextPro.size(); ++j){
                        new_note_map[nextID].positionV_[j] = (new_note_map[nextID].positionV_[j] * new_note_map[nextID].pro_ + tempNode.positionV_[j] * tempNode.pro_) / (tempNode.pro_ + new_note_map[nextID].pro_);
                    }
                    new_note_map[nextID].pro_ += tempNode.pro_;
                }
            }
            for(int i = 0; i < EdgesInToBeAdd.size(); ++i){
                int nextID = EdgesInToBeAdd[i].src_;
                double nextPro = theNode.pro_ * outEdgeType_pro[-EdgesInToBeAdd[i].edge_type_] / realNeiborNum[-EdgesInToBeAdd[i].edge_type_];
                vector<double> tempNextPro(theNode.positionV_.size(), 0.0);
                double tempSUM = 0.0;
                for(map<int, double>::iterator j = outEdgeType_outNodes[0 - EdgesInToBeAdd[i].edge_type_].begin(); j != outEdgeType_outNodes[0 - EdgesInToBeAdd[i].edge_type_].end(); ++j){
                    int dstMetaID = j->first;
                    double dstMetaPro = j->second;
                    tempSUM += dstMetaPro;
                    tempNextPro[dstMetaID] += dstMetaPro;
                }
                for(int j = 0; j < tempNextPro.size(); ++j){
                    tempNextPro[j] /= tempSUM;
                }
                
                SCRW_Node tempNode(nextID, nextPro, tempNextPro);
                if(new_note_map.find(nextID) == new_note_map.end()){
                    new_note_map[nextID] = tempNode;
                }else{
                    for(int j = 0; j < tempNextPro.size(); ++j){
                        new_note_map[nextID].positionV_[j] = (new_note_map[nextID].positionV_[j] * new_note_map[nextID].pro_ + tempNode.positionV_[j] * tempNode.pro_) / (tempNode.pro_ + new_note_map[nextID].pro_);
                    }
                    new_note_map[nextID].pro_ += tempNode.pro_;
                }
            }
            
        }
        double maxChange = 0.0;
        for(map<int, SCRW_Node>::iterator i = new_note_map.begin(); i != new_note_map.end(); ++i){
            int tempID = i->first;
            if(note_map.find(tempID) == note_map.end())
                maxChange = i->second.pro_;
            else if ( abs(i->second.pro_ - note_map[tempID].pro_) > maxChange){
                maxChange = abs(i->second.pro_ - note_map[tempID].pro_);
            }
        }
        
        note_map = new_note_map;
        if(maxChange < EPSILON)
            break;
    }
    return rtn;
}




map<int, double>  SimCalculator::SCRW_Sim_Index(int dst, Meta_Structure &m_struct, map<int, SCRW_Node> &note_map)
{
    map<int, double> rtn;
    // index from <node_id, structure_id> to possibility
    map<pair<int, int>, double> index;
    while(true){
        map<int, SCRW_Node> new_note_map;
        // for each unhandled node
        for(map<int, SCRW_Node>::iterator iter = note_map.begin(); iter != note_map.end(); ++iter){
            int node_id = iter->first;
            SCRW_Node & theNode = iter->second;
            
            // if the pro is too slow, just leave it in the new_note_map
            if(theNode.pro_ < EPSILON){
                if(new_note_map.find(node_id) == new_note_map.end()){
                    new_note_map[node_id] = theNode;
                }else{
                    for(int j = 0; j < theNode.positionV_.size(); ++j){
                        new_note_map[node_id].positionV_[j] = (new_note_map[node_id].positionV_[j] * new_note_map[node_id].pro_ + theNode.positionV_[j] * theNode.pro_) / (theNode.pro_ + new_note_map[node_id].pro_);
                    }
                    new_note_map[node_id].pro_ += theNode.pro_;
                }
                continue;
            }
            // first remove those points already reach the final position
            if(theNode.positionV_[theNode.positionV_.size() - 1] != 0.0){
                if(rtn.find(node_id) == rtn.end()){
                    rtn[node_id] = theNode.pro_ * theNode.positionV_[theNode.positionV_.size() - 1];
                }else{
                    rtn[node_id] += theNode.pro_ * theNode.positionV_[theNode.positionV_.size() - 1];
                }
                double FinalPro = theNode.positionV_[theNode.positionV_.size() - 1];
                if(FinalPro == 1.0)
                    continue;
                theNode.pro_ *= (1- FinalPro);
                for(int  i = 0; i < theNode.positionV_.size(); ++i){
                    theNode.positionV_[i] /= (1- FinalPro);
                }
                theNode.positionV_[theNode.positionV_.size() - 1] = 0.0;
            }
            
            map<int, map<int, double> > outEdgeType_outNodes;
            map<int, double> outEdgeType_pro;
            
            // get all possible ourEdge type and transition pro
            for(int i = 0; i < theNode.positionV_.size(); ++i){
                if(theNode.positionV_[i] == 0)
                    continue;
                
                // index hitted!
                if(index.find(make_pair(node_id, i)) != index.end()){
                    rtn[node_id] += index[make_pair(node_id, i)] * theNode.pro_ * theNode.positionV_[i];
                    continue;
                }
                for(int j = 0; j < m_struct.srt_edges_[i].size(); ++j){
                    int tempEdgeType = m_struct.srt_edges_[i][j].type_;
                    int tempDst = m_struct.srt_edges_[i][j].dst_;
                    if(outEdgeType_outNodes.find(tempEdgeType) == outEdgeType_outNodes.end()){
                        outEdgeType_outNodes[tempEdgeType] = map<int, double>();
                    }
                    if(outEdgeType_outNodes[tempEdgeType].find(tempDst) == outEdgeType_outNodes[tempEdgeType].end()){
                        outEdgeType_outNodes[tempEdgeType][tempDst] = 0;
                    }
                    outEdgeType_outNodes[tempEdgeType][tempDst] += theNode.positionV_[i] * m_struct.srt_edges_[i][j].weight_ / m_struct.meta_nodes_[i].neighborWeightSum_;
                    if(outEdgeType_pro.find(tempEdgeType) == outEdgeType_pro.end())
                        outEdgeType_pro[tempEdgeType] = 0.0;
                    outEdgeType_pro[tempEdgeType] += theNode.positionV_[i] * m_struct.srt_edges_[i][j].weight_ / m_struct.meta_nodes_[i].neighborWeightSum_;
                }
                for(int j = 0; j < m_struct.dst_edges_[i].size(); ++j){
                    int tempEdgeType = 0 - m_struct.dst_edges_[i][j].type_;
                    int tempSrc = m_struct.dst_edges_[i][j].src_;
                    if(outEdgeType_outNodes.find(tempEdgeType) == outEdgeType_outNodes.end()){
                        outEdgeType_outNodes[tempEdgeType] = map<int, double>();
                    }
                    if(outEdgeType_outNodes[tempEdgeType].find(tempSrc) == outEdgeType_outNodes[tempEdgeType].end()){
                        outEdgeType_outNodes[tempEdgeType][tempSrc] = 0;
                    }
                    outEdgeType_outNodes[tempEdgeType][tempSrc] += theNode.positionV_[i] * m_struct.dst_edges_[i][j].weight_ / m_struct.meta_nodes_[i].neighborWeightSum_;
                    if(outEdgeType_pro.find(tempEdgeType) == outEdgeType_pro.end())
                        outEdgeType_pro[tempEdgeType] = 0.0;
                    outEdgeType_pro[tempEdgeType] += theNode.positionV_[i] * m_struct.dst_edges_[i][j].weight_ / m_struct.meta_nodes_[i].neighborWeightSum_;
                    
                }
                
            }
            
            // get all possible way out
            map<int, int> realNeiborNum;
            //int realNeiborNum = 0;
            vector<HIN_Edge> EdgesOutToBeAdd, EdgesInToBeAdd;
            for(int i = 0; i < m_struct.hin_graph_.edges_src_[node_id].size(); ++i){
                if(outEdgeType_outNodes.find(m_struct.hin_graph_.edges_src_[node_id][i].edge_type_)
                   == outEdgeType_outNodes.end()){
                    continue;
                }
                if(realNeiborNum.find(m_struct.hin_graph_.edges_src_[node_id][i].edge_type_) == realNeiborNum.end())
                    realNeiborNum[m_struct.hin_graph_.edges_src_[node_id][i].edge_type_] = 0;
                realNeiborNum[m_struct.hin_graph_.edges_src_[node_id][i].edge_type_] += 1;
                EdgesOutToBeAdd.push_back(m_struct.hin_graph_.edges_src_[node_id][i]);
            }
            for(int i = 0; i < m_struct.hin_graph_.edges_dst_[node_id].size(); ++i){
                if(outEdgeType_outNodes.find(0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_)
                   == outEdgeType_outNodes.end()){
                    continue;
                }
                if(realNeiborNum.find(0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_) == realNeiborNum.end())
                    realNeiborNum[0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_] = 0;
                realNeiborNum[0 - m_struct.hin_graph_.edges_dst_[node_id][i].edge_type_] += 1;
                EdgesInToBeAdd.push_back(m_struct.hin_graph_.edges_dst_[node_id][i]);
            }
            
            for(int i = 0; i < EdgesOutToBeAdd.size(); ++i){
                int nextID = EdgesOutToBeAdd[i].dst_;
                double nextPro = theNode.pro_ * outEdgeType_pro[EdgesOutToBeAdd[i].edge_type_] / realNeiborNum[EdgesOutToBeAdd[i].edge_type_];
                
                vector<double> tempNextPro(theNode.positionV_.size(), 0.0);
                double tempSUM = 0.0;
                for(map<int, double>::iterator j = outEdgeType_outNodes[EdgesOutToBeAdd[i].edge_type_].begin(); j != outEdgeType_outNodes[EdgesOutToBeAdd[i].edge_type_].end(); ++j){
                    int dstMetaID = j->first;
                    double dstMetaPro = j->second;
                    tempSUM += dstMetaPro;
                    tempNextPro[dstMetaID] += dstMetaPro;
                }
                for(int j = 0; j < tempNextPro.size(); ++j){
                    tempNextPro[j] /= tempSUM;
                }
                
                SCRW_Node tempNode(nextID, nextPro, tempNextPro);
                if(new_note_map.find(nextID) == new_note_map.end()){
                    new_note_map[nextID] = tempNode;
                }else{
                    for(int j = 0; j < tempNextPro.size(); ++j){
                        new_note_map[nextID].positionV_[j] = (new_note_map[nextID].positionV_[j] * new_note_map[nextID].pro_ + tempNode.positionV_[j] * tempNode.pro_) / (tempNode.pro_ + new_note_map[nextID].pro_);
                    }
                    new_note_map[nextID].pro_ += tempNode.pro_;
                }
            }
            for(int i = 0; i < EdgesInToBeAdd.size(); ++i){
                int nextID = EdgesInToBeAdd[i].src_;
                double nextPro = theNode.pro_ * outEdgeType_pro[-EdgesInToBeAdd[i].edge_type_] / realNeiborNum[-EdgesInToBeAdd[i].edge_type_];
                vector<double> tempNextPro(theNode.positionV_.size(), 0.0);
                double tempSUM = 0.0;
                for(map<int, double>::iterator j = outEdgeType_outNodes[0 - EdgesInToBeAdd[i].edge_type_].begin(); j != outEdgeType_outNodes[0 - EdgesInToBeAdd[i].edge_type_].end(); ++j){
                    int dstMetaID = j->first;
                    double dstMetaPro = j->second;
                    tempSUM += dstMetaPro;
                    tempNextPro[dstMetaID] += dstMetaPro;
                }
                for(int j = 0; j < tempNextPro.size(); ++j){
                    tempNextPro[j] /= tempSUM;
                }
                
                SCRW_Node tempNode(nextID, nextPro, tempNextPro);
                if(new_note_map.find(nextID) == new_note_map.end()){
                    new_note_map[nextID] = tempNode;
                }else{
                    for(int j = 0; j < tempNextPro.size(); ++j){
                        new_note_map[nextID].positionV_[j] = (new_note_map[nextID].positionV_[j] * new_note_map[nextID].pro_ + tempNode.positionV_[j] * tempNode.pro_) / (tempNode.pro_ + new_note_map[nextID].pro_);
                    }
                    new_note_map[nextID].pro_ += tempNode.pro_;
                }
            }
            
        }
        double maxChange = 0.0;
        for(map<int, SCRW_Node>::iterator i = new_note_map.begin(); i != new_note_map.end(); ++i){
            int tempID = i->first;
            if(note_map.find(tempID) == note_map.end())
                maxChange = i->second.pro_;
            else if ( abs(i->second.pro_ - note_map[tempID].pro_) > maxChange){
                maxChange = abs(i->second.pro_ - note_map[tempID].pro_);
            }
        }
        
        note_map = new_note_map;
        if(maxChange < EPSILON)
            break;
    }
    
    
    
    return rtn;
}

void SimCalculator::linkPrediction(string in_path, string out_path, string country_path, Meta_Structure & m_struct)
{
    ifstream countryin(country_path.c_str(), ios::in);
    int country_id;
    set<int> countries;
    while(countryin >> country_id)
        countries.insert(country_id);
    
    ifstream input(in_path.c_str(), ios::in);
    string line;
    ofstream out(out_path, ios::out);
    while(getline(input, line)){
        stringstream in(line);
        int src, dst;
        in >> src >> dst;
        map<int, double> temp_sims;
        
        double sim = SimCalculator::calSim_SCRW(src, dst, m_struct, temp_sims);
        
        out << src << '\t' << dst << '\t' << sim;
        for(map<int, double>::iterator i = temp_sims.begin(); i != temp_sims.end(); ++i){
            if(countries.find(i->first) == countries.end())
                continue;
            out << '\t' << i->first << '\t' << i->second;
        }
        out << endl;
    }
    input.close();
    out.close();
}

void SimCalculator::linkPrediction_PATH(string in_path, string out_path, string country_path, Meta_Paths & metaPaths)
{
    ifstream countryin(country_path.c_str(), ios::in);
    int country_id;
    set<int> countries;
    while(countryin >> country_id)
        countries.insert(country_id);
    
    
    ifstream input(in_path.c_str(), ios::in);
    string line;
    ofstream out(out_path, ios::out);
    while(getline(input, line)){
        stringstream in(line);
        int src, dst;
        in >> src >> dst;
        map<int, double> temp_sims;
        double sim = SimCalculator::calSim_PCRW(src, dst, metaPaths, temp_sims);
        out << src << '\t' << dst << '\t' << sim;
        for(map<int, double>::iterator i = temp_sims.begin(); i != temp_sims.end(); ++i){
            if(countries.find(i->first) == countries.end())
                continue;
            out << '\t' << i->first << '\t' << i->second;
        }
        out << endl;
    }
    input.close();
    out.close();
    
}

void SimCalculator::PCRW(int src, int dst, Meta_Paths &m_path, int p_id, map<int, double> &id_sim, vector< vector<pair<int, double> >> & pathInstances)
{
    pathInstances.clear();
    pathInstances.push_back(vector<pair<int, double> >());
    pathInstances[0].push_back(make_pair(src, 1.0));
    
    clock_t tt;
    for(int i = 0; i < m_path.linkTypes_[p_id].size(); ++i){
        tt = clock();
        vector< vector< pair<int, double> > > extendedVector;
        int edgetype = m_path.linkTypes_[p_id][i];
        for(int ii = 0; ii < pathInstances.size(); ++ii){
            int now = pathInstances[ii][pathInstances[ii].size() - 1].first;
            double nowP = pathInstances[ii][pathInstances[ii].size() - 1].second;
            if(edgetype > 0){
                int edgeCount = 0;
                for(int j = 0; j < m_path.hin_graph_.edges_src_[now].size(); ++j){
                    if(m_path.hin_graph_.edges_src_[now][j].edge_type_ != edgetype)
                        continue;
                    edgeCount++;
                }
                double p_out = nowP / edgeCount;
                for(int j = 0; j < m_path.hin_graph_.edges_src_[now].size(); ++j){
                    if(m_path.hin_graph_.edges_src_[now][j].edge_type_ != edgetype){
                        continue;
                    }
                    extendedVector.push_back(pathInstances[ii]);
                    extendedVector[extendedVector.size() - 1].push_back(make_pair(m_path.hin_graph_.edges_src_[now][j].dst_, p_out));
                }
            } else{
                int tempedgetype = -edgetype;
                int edgeCount = 0;
                for(int j = 0; j < m_path.hin_graph_.edges_dst_[now].size(); ++j){
                    if(m_path.hin_graph_.edges_dst_[now][j].edge_type_ != tempedgetype)
                        continue;
                    edgeCount++;
                }
                double p_out = nowP / edgeCount;
                for(int j = 0; j < m_path.hin_graph_.edges_dst_[now].size(); ++j){
                    if(m_path.hin_graph_.edges_dst_[now][j].edge_type_ != tempedgetype)
                        continue;
                    extendedVector.push_back(pathInstances[ii]);
                    extendedVector[extendedVector.size() - 1].push_back(make_pair(m_path.hin_graph_.edges_dst_[now][j].src_, p_out));
                }
            }
        }
        //cout << i << '\t' << (double)(clock()-tt)/CLOCKS_PER_SEC << endl;
        pathInstances = extendedVector;
    }
    for(int i = 0; i < pathInstances.size(); ++i){
        int dst_id = pathInstances[i][pathInstances[i].size() - 1].first;
        double dst_p = pathInstances[i][pathInstances[i].size() - 1].second;
        if(id_sim.find(dst_id) == id_sim.end()){
            id_sim[dst_id] = 0.0;
        }
        id_sim[dst_id] += dst_p;
    }
}

void SimCalculator::SCRW_NC(int src, int dst, Meta_Structure &m_struct, map<int, double> &id_sim)
{
    
}


double SimCalculator::calSim_PCRW(int src, int dst, Meta_Paths &m_paths, map<int, double> &id_sim)
{
    map<int, double> temp_sim;
    id_sim.clear();
    // for each metapath
    for(int i = 0 ;i < m_paths.weights_.size(); ++i){
        temp_sim.clear();
        vector< vector<pair<int, double> > > pathInstances;
        PCRW(src, dst, m_paths, i, temp_sim, pathInstances);
        for(map<int, double>::iterator j = temp_sim.begin(); j != temp_sim.end(); ++j){
            int dst_id = j->first;
            double dst_p = j->second;
            if(id_sim.find(dst_id) == id_sim.end()){
                id_sim[dst_id] = 0.0;
            }
            id_sim[dst_id] += m_paths.weights_[i] * dst_p;
        }
    }
    if(id_sim.find(dst) == id_sim.end())
        return 0.0;
    return id_sim[dst];
}

double SimCalculator::calSim_PC(int src, int dst, Meta_Paths &m_paths, map<int, double> &id_sim)
{
    map<int, double> temp_sim;
    id_sim.clear();
    int cc = 0;
    // for each metapath
    for(int i = 0 ;i < m_paths.weights_.size(); ++i){
        temp_sim.clear();
        vector< vector<pair<int, double> > > pathInstances;
        PCRW(src, dst, m_paths, i, temp_sim, pathInstances);
        cc += pathInstances.size();
        for(int j = 0; j < pathInstances.size(); ++j){
            int dst_id = pathInstances[j][pathInstances[j].size() - 1].first;
            if(id_sim.find(dst_id) == id_sim.end()){
                id_sim[dst_id] = 0;
            }
            id_sim[dst_id] += m_paths.weights_[i];
        }
    }
    //cout << cc << endl;
    if(id_sim.find(dst) == id_sim.end())
        return 0;
    return id_sim[dst];
}



double SimCalculator::calSim_PS(int src, int dst, Meta_Paths &m_paths)
{
    map<int, double> temp_sim1, temp_sim2;
    
    double pc1 = calSim_PC(src, dst, m_paths, temp_sim1);
    if(pc1 == 0.0)
        return 0.0;
    double pc3 = calSim_PC(dst, dst, m_paths, temp_sim2);
    double pc2 = 0.0;
    if(temp_sim1.find(src) != temp_sim1.end())
        pc2 = temp_sim1[src];
    return 2 * pc1 / (pc2 + pc3);
}

double SimCalculator::calSim_PS_all(int src, int dst, Meta_Paths &m_paths, map<int, double> & id_sim)
{
    map<int, double> temp_sim1;
    id_sim.clear();
    double pc1 = calSim_PC(src, dst, m_paths, temp_sim1);
    for(map<int, double>::iterator i = temp_sim1.begin(); i != temp_sim1.end(); ++i){
        int id = i->first;
        double sim = i->second;
        map<int, double> temp_sim2;
        double sim1 = temp_sim1[src];
        double sim2 = calSim_PC(id, id, m_paths, temp_sim2);
        id_sim[id] = 2.0 * sim / (sim1 + sim2);
    }
    if(pc1 == 0.0)
        return 0.0;
    if(id_sim.find(dst) == id_sim.end())
        return 0.0;
    return id_sim[dst];
}

void SimCalculator::predictNodes_SCRW(int src, Meta_Structure &m_struct)
{
    map<int, double> temp_sims;
    double sim = SimCalculator::calSim_SCRW(src, 0, m_struct, temp_sims);
    
    for(map<int, double>::iterator i = temp_sims.begin(); i != temp_sims.end(); ++i){
        cout << i-> first << '\t' << i-> second << endl;
    }
}

void SimCalculator::predictNodes_CPCRW(int src, Meta_Paths &m_paths)
{
    map<int, double> temp_sim;
    map<int, double> id_sim;

    for(int i = 0 ;i < m_paths.weights_.size(); ++i){
        temp_sim.clear();
        vector< vector<pair<int, double> >> pathInstances;
        PCRW(src, 0, m_paths, i, temp_sim, pathInstances);
        for(map<int, double>::iterator j = temp_sim.begin(); j != temp_sim.end(); ++j){
            int dst_id = j->first;
            double dst_p = j->second;
            if(id_sim.find(dst_id) == id_sim.end()){
                id_sim[dst_id] = 0.0;
            }
            id_sim[dst_id] += m_paths.weights_[i] * dst_p;
        }
    }
    for(map<int, double>::iterator i = id_sim.begin(); i != id_sim.end(); ++i){
        cout << i->first << '\t' << i->second << endl;
    }
    
}

void SimCalculator::calSimForAllEntities_FSPG(string inpath, string outpath, Meta_Paths &mpath)
{
    ifstream in(inpath.c_str(), ios::in);
    vector<int> students;
    
    string line;
    while(getline(in, line)){
        stringstream sin(line);
        int s_id;
        sin >> s_id;
        students.push_back(s_id);
    }
    in.close();
    
    ofstream out(outpath, ios::out);
    for(int i = 0; i < students.size() - 1; ++i){
        map<int, double> id_sim;
        calSim_PC(students[i], students[i + 1], mpath, id_sim);
        //calSim_PCRW(students[i], students[i + 1], mpath, id_sim);
        for(int j = i; j < students.size(); ++j){
            if(id_sim.find(students[j]) == id_sim.end()){
                out << students[i] << '\t' << students[j] << '\t' << 0 << endl;
            }else{
                out << students[i] << '\t' << students[j] << '\t' << id_sim[students[j]] << endl;
            }
        }
    }
    
}


void SimCalculator::calSimForAllEntities(string inpath, string outpath, Meta_Structure &m_struct)
{
    ifstream in(inpath.c_str(), ios::in);
    vector<int> students;
    
    string line;
    while(getline(in, line)){
        stringstream sin(line);
        int s_id;
        sin >> s_id;
        students.push_back(s_id);
    }
    in.close();
    
    ofstream out(outpath, ios::out);
    for(int i = 0; i < students.size() - 1; ++i){
        map<int, double> id_sim;
        double sim = calSim_SCRW(students[i], students[i + 1], m_struct, id_sim);
        for(int j = i; j < students.size(); ++j){
            if(id_sim.find(students[j]) == id_sim.end()){
                out << students[i] << '\t' << students[j] << '\t' << 0 << endl;
            }else{
                out << students[i] << '\t' << students[j] << '\t' << id_sim[students[j]] << endl;
            }
        }
    }
}

void SimCalculator::calSimForAllEntities_SCRW_NC(string inpath, string outpath, Meta_Structure &m_struct)
{
    ifstream in(inpath.c_str(), ios::in);
    vector<int> students;
    
    string line;
    while(getline(in, line)){
        stringstream sin(line);
        int s_id;
        sin >> s_id;
        students.push_back(s_id);
    }
    in.close();
    
    ofstream out(outpath, ios::out);
    for(int i = 0; i < students.size() - 1; ++i){
        map<int, double> id_sim;
        calSim_SCRW_NC(students[i], students[i + 1], m_struct, id_sim);
        for(int j = i; j < students.size(); ++j){
            if(id_sim.find(students[j]) == id_sim.end()){
                out << students[i] << '\t' << students[j] << '\t' << 0 << endl;
            }else{
                out << students[i] << '\t' << students[j] << '\t' << id_sim[students[j]] << endl;
            }
        }
    }
}

void SimCalculator::calSimForAllEntities_SCRW_NC_mul(string inpath, string outpath, Meta_Structure &m_struct)
{
    ifstream in(inpath.c_str(), ios::in);
    vector<int> students;
    
    string line;
    while(getline(in, line)){
        stringstream sin(line);
        int s_id;
        sin >> s_id;
        students.push_back(s_id);
    }
    in.close();
    
    ofstream out(outpath, ios::out);
    for(int i = 0; i < students.size() - 1; ++i){
        map<vector<int>, double> id_sim;
        calSim_SCRW_NC_mul(students[i], m_struct, id_sim, 1);
        map<int, double> tempID_sim;
        for(map<vector<int>, double>::iterator jj = id_sim.begin(); jj != id_sim.end(); ++jj){
            tempID_sim[jj->first[0]] = jj->second;
        }
        for(int j = i; j < students.size(); ++j){
            if(tempID_sim.find(students[j]) == tempID_sim.end()){
                //out << students[i] << '\t' << students[j] << '\t' << 0 << endl;
            }else{
                out << students[i] << '\t' << students[j] << '\t' << tempID_sim[students[j]] << endl;
            }
        }
    }
}

void SimCalculator::calSimForAllEntities_BFS(string inpath, string outpath, Meta_Structure &m_struct)
{
    ifstream in(inpath.c_str(), ios::in);
    vector<int> students;
    
    string line;
    while(getline(in, line)){
        stringstream sin(line);
        int s_id;
        sin >> s_id;
        students.push_back(s_id);
    }
    in.close();
    
    ofstream out(outpath, ios::out);
    for(int i = 0; i < students.size() - 1; ++i){
        map<vector<int>, double> Rtn;
        map<int, int> src;
        src[0] = students[i];
        
        cal_BSCSE_BFS(src, 0, m_struct, Rtn, 0);
        
        map<int, double> tempID_sim;
        for(map<vector<int>, double>::iterator jj = Rtn.begin(); jj != Rtn.end(); ++jj){
            tempID_sim[jj->first[0]] = jj->second;
        }
        for(int j = i; j < students.size(); ++j){
            if(tempID_sim.find(students[j]) == tempID_sim.end()){
                out << students[i] << '\t' << students[j] << '\t' << 0 << endl;
            }else{
                out << students[i] << '\t' << students[j] << '\t' << tempID_sim[students[j]] << endl;
            }
        }
    }
}

void SimCalculator::calSimForAllEntities_StructCount_NC(string inpath, string outpath, Meta_Structure &m_struct)
{
    ifstream in(inpath.c_str(), ios::in);
    vector<int> students;
    
    string line;
    while(getline(in, line)){
        stringstream sin(line);
        int s_id;
        sin >> s_id;
        students.push_back(s_id);
    }
    in.close();
    
    ofstream out(outpath, ios::out);
    for(int i = 0; i < students.size() - 1; ++i){
        map<int, int> id_sim;
        calSim_SCount_NC(students[i], students[i + 1], m_struct, id_sim);
        for(int j = i ; j < students.size(); ++j){
            if(id_sim.find(students[j]) == id_sim.end()){
                out << students[i] << '\t' << students[j] << '\t' << 0 << endl;
            }else{
                out << students[i] << '\t' << students[j] << '\t' << id_sim[students[j]] << endl;
            }
        }
    }
}

void SimCalculator::calSimForAllEntities_PathSim(string inpath, string outpath, Meta_Paths &mpath)
{
    ifstream in(inpath.c_str(), ios::in);
    vector<int> students;
    
    string line;
    while(getline(in, line)){
        stringstream sin(line);
        int s_id;
        sin >> s_id;
        students.push_back(s_id);
    }
    in.close();
    
    ofstream out(outpath, ios::out);
    vector<map<int, double> > id_sim(students.size());
    for(int i = 0; i < students.size() - 1; ++i){
        calSim_PC(students[i], students[i + 1], mpath, id_sim[i]);
    }
    for(int i = 0; i < students.size(); ++i){
        for(int j = i ; j < students.size(); ++j){
            if(id_sim[i].find(students[j]) == id_sim[i].end()){
                out << students[i] << '\t' << students[j] << '\t' << 0 << endl;
            }else{
                int sc = id_sim[i][students[j]];
                int sc1 = 0, sc2 = 0;
                if(id_sim[i].find(students[i]) != id_sim[i].end()){
                    sc1 = id_sim[i][students[i]];
                }
                if(id_sim[j].find(students[j]) != id_sim[j].end()){
                    sc2 = id_sim[j][students[j]];
                }
                out << students[i] << '\t' << students[j] << '\t' << 2.0 * sc / (sc1 + sc2) << endl;
            }
        }
        
    }
}

void SimCalculator::calSimForAllEntities_SSim(string inpath, string outpath, Meta_Structure &m_struct)
{
    ifstream in(inpath.c_str(), ios::in);
    vector<int> students;
    
    string line;
    while(getline(in, line)){
        stringstream sin(line);
        int s_id;
        sin >> s_id;
        students.push_back(s_id);
    }
    in.close();
    
    ofstream out(outpath, ios::out);
    vector<map<int, int> > id_sim(students.size());
    for(int i = 0; i < students.size() - 1; ++i){
        calSim_SCount_NC(students[i], students[i + 1], m_struct, id_sim[i]);
    }
    for(int i = 0; i < students.size(); ++i){
        for(int j = i ; j < students.size(); ++j){
            if(id_sim[i].find(students[j]) == id_sim[i].end()){
                out << students[i] << '\t' << students[j] << '\t' << 0 << endl;
            }else{
                int sc = id_sim[i][students[j]];
                int sc1 = 0, sc2 = 0;
                if(id_sim[i].find(students[i]) != id_sim[i].end()){
                    sc1 = id_sim[i][students[i]];
                }
                if(id_sim[j].find(students[j]) != id_sim[j].end()){
                    sc2 = id_sim[j][students[j]];
                }
                out << students[i] << '\t' << students[j] << '\t' << 2.0 * sc / (sc1 + sc2) << endl;
            }
        }

    }
}

void SimCalculator::calSimForAllEntities_PCount_NC(string inpath, string outpath, Meta_Structure &m_struct)
{
    ifstream in(inpath.c_str(), ios::in);
    vector<int> students;
    
    string line;
    while(getline(in, line)){
        stringstream sin(line);
        int s_id;
        sin >> s_id;
        students.push_back(s_id);
    }
    in.close();
    
    ofstream out(outpath, ios::out);
    for(int i = 0; i < students.size() - 1; ++i){
        map<int, int> id_sim;
        calSim_PCount_NC(students[i], students[i + 1], m_struct, id_sim);
        for(int j = i ; j < students.size(); ++j){
            if(id_sim.find(students[j]) == id_sim.end()){
                out << students[i] << '\t' << students[j] << '\t' << 0 << endl;
            }else{
                out << students[i] << '\t' << students[j] << '\t' << id_sim[students[j]] << endl;
            }
        }
    }
}

void SimCalculator::clustering_Kmeans(string inputPath, string simPath)
{
    map<int, vector<int> > GT;
    vector<int> nodes;
    map<int, int> GTR;
    map< pair<int, int>, double> sims;
    map<int, int> label;
    int count = 0;
    
    ifstream input(inputPath.c_str(), ios::in);
    string line;
    
    
    while(getline(input, line)){
        stringstream sinput(line);
        int s_id, u_id;
        sinput >> s_id >> u_id;
        if(GT.find(u_id) == GT.end()){
            GT[u_id] = vector<int>();
        }
        nodes.push_back(s_id);
        GT[u_id].push_back(s_id);
        GTR[s_id] = u_id;
        label[s_id] = count % KMEANS_NUM;
        count++;
    }
    input.close();
    
    ifstream simInput(simPath.c_str(), ios::in);
    while(getline(simInput, line)){
        stringstream sinput(line);
        int s1, s2;
        double sim;
        sinput >> s1 >> s2 >> sim;
        sims[make_pair(s1, s2)] = sim;
        sims[make_pair(s2, s1)] = sim;
    }
    simInput.close();
    
    vector< vector<int> > templabels;
    for(int i = 0; i < KMEANS_NUM; ++i)
        templabels.push_back(vector<int>());
    for(map<int, int>::iterator iter = label.begin(); iter != label.end(); ++iter){
        int id = iter->first;
        int tl = iter->second;
        templabels[tl].push_back(id);
    }
    
    for(int k = 0; k < KMEANS_ITERATIONS; ++k){
        vector< vector<int> > temlabels2;
        for(int i = 0; i < KMEANS_NUM; ++i){
            temlabels2.push_back(vector<int>());
        }
        for(int i = 0; i< nodes.size(); ++i){
            double maxsim = -1;
            int theCluster = -1;
            for(int j = 0; j < KMEANS_NUM; ++j){
                double sumSim = 0.0;
                int cc = 0;
                for(int ii = 0; ii < templabels[j].size(); ++ii){
                    if(nodes[i] != templabels[j][ii]){
                        sumSim += sims[make_pair(nodes[i], templabels[j][ii])];
                        cc++;
                    }
                }
                sumSim /= cc;
                if(sumSim >= maxsim){
                    maxsim = sumSim;
                    theCluster = j;
                }
            }
            temlabels2[theCluster].push_back(nodes[i]);
        }
        templabels = temlabels2;
    }
    for(int i = 0; i < templabels.size(); ++i){
        for(int j = 0; j < templabels[i].size(); ++j){
            cout << i << '\t' << templabels[i][j] << endl;
        }
    }
}

void SimCalculator::clustering_Hire(string inputPath, string simPath)
{
    map<int, vector<int> > GT;
    vector<int> nodes;
    map<int, int> GTR;
    map< pair<int, int>, double> sims;
    map<int, int> label;
    int count = 0;
    
    ifstream input(inputPath.c_str(), ios::in);
    string line;
    
    
    while(getline(input, line)){
        stringstream sinput(line);
        int s_id, u_id;
        sinput >> s_id >> u_id;
        if(GT.find(u_id) == GT.end()){
            GT[u_id] = vector<int>();
        }
        nodes.push_back(s_id);
        GT[u_id].push_back(s_id);
        GTR[s_id] = u_id;
        label[s_id] = count;
        count++;
    }
    input.close();
    
    ifstream simInput(simPath, ios::in);
    while(getline(simInput, line)){
        stringstream sinput(line);
        int s1, s2;
        double sim;
        sinput >> s1 >> s2 >> sim;
        sims[make_pair(s1, s2)] = sim;
        sims[make_pair(s2, s1)] = sim;
    }
    simInput.close();
    
    vector<pair<double, pair<int, int> >> edges;
    for(map<pair<int, int>, double>::iterator i = sims.begin(); i != sims.end(); ++i){
        edges.push_back(make_pair(i->second, i->first));
    }
    for(int i = 1; i < edges.size(); ++i){
        pair<double, pair<int, int> > p = edges[i];
        double theSim = edges[i].first;
        int j = i - 1;
        while(j >= 0 && edges[j].first < theSim){
            edges[j + 1] = edges[j];
            j--;
        }
        edges[j + 1] = p;
    }
    
    
    count = label.size();
    for(int i = 0; i < edges.size(); ++i){
        if(count <= KMEANS_NUM)
            break;
        double theSim = edges[i].first;
        int src = edges[i].second.first;
        int dst = edges[i].second.second;
        
        if(label[src] == label[dst])
            continue;
        
        int type = label[dst];
        for(map<int, int>::iterator j = label.begin(); j != label.end(); ++j){
            if(j->second == type){
                j->second = label[src];
            }
        }

        count--;
    }
    
    vector<int> labelTypes;
    map<int, int> idx;
    for(map<int, int>::iterator i = label.begin(); i != label.end(); ++i){
        if(find(labelTypes.begin(), labelTypes.end(), i->second) == labelTypes.end()){
            idx[i->second] = labelTypes.size();
            labelTypes.push_back(i->second);
        }
    }
    
    vector<vector<int> > templabels;
    for (map<int, int>::iterator i = label.begin(); i != label.end(); ++i) {
        cout << idx[i->second] << '\t' << i->first << endl;
    }
    
}

void SimCalculator::calTopK_PathCount(int src, int k, Meta_Paths &mpath, map<int, double> &rtn)
{
    map<int, double> id_sim;
    calSim_PC(src, src, mpath, id_sim);
    vector<int> temp;
    vector<double> sims;
    for(map<int, double>::iterator i = id_sim.begin(); i != id_sim.end(); ++i){
        temp.push_back(i->first);
        sims.push_back(i->second);
    }
    for(int i = 1; i < temp.size(); ++i){
        int j = i - 1;
        int theID = temp[i];
        double theSim = sims[i];
        while(j >= 0 && sims[j] < theSim){
            sims[j + 1] = sims[j];
            temp[j + 1] = temp[j];
            j--;
        }
        sims[j + 1] = theSim;
        temp[j + 1] = theID;
    }
    rtn.clear();
    for(int i = 0; i < k; ++i){
        if(i >= temp.size())
            break;
        rtn[temp[i]] = sims[i];
        //cout << temp[i] << '\t'<<  sims[i] << endl;
    }
}

void SimCalculator::calTopK_PathCount_1(int src, int k, Meta_Paths &mpath, vector<int> &rtn)
{
    map<int, double> id_sim;
    calSim_PC(src, src, mpath, id_sim);
    vector<int> temp;
    vector<double> sims;
    for(map<int, double>::iterator i = id_sim.begin(); i != id_sim.end(); ++i){
        temp.push_back(i->first);
        sims.push_back(i->second);
    }
    for(int i = 1; i < temp.size(); ++i){
        int j = i - 1;
        int theID = temp[i];
        double theSim = sims[i];
        while(j >= 0 && sims[j] < theSim){
            sims[j + 1] = sims[j];
            temp[j + 1] = temp[j];
            j--;
        }
        sims[j + 1] = theSim;
        temp[j + 1] = theID;
    }
    rtn.clear();
    for(int i = 0; i < k; ++i){
        if(i >= temp.size())
            break;
        rtn.push_back(temp[i]);
    }
}


void SimCalculator::calTopK_PathSim(int src, int k, Meta_Paths &mpath, map<int, double> &rtn)
{
    map<int, double> id_sim;
    calSim_PS_all(src, src, mpath, id_sim);
    vector<int> temp;
    vector<double> sims;
    for(map<int, double>::iterator i = id_sim.begin(); i != id_sim.end(); ++i){
        temp.push_back(i->first);
        sims.push_back(i->second);
    }
    for(int i = 1; i < temp.size(); ++i){
        int j = i - 1;
        int theID = temp[i];
        double theSim = sims[i];
        while(j >= 0 && sims[j] < theSim){
            sims[j + 1] = sims[j];
            temp[j + 1] = temp[j];
            j--;
        }
        sims[j + 1] = theSim;
        temp[j + 1] = theID;
    }
    rtn.clear();
    for(int i = 0; i < k; ++i){
        if(i >= temp.size())
            break;
        rtn[temp[i]] = sims[i];
    }
}

void SimCalculator::calTopK_PCRW(int src, int k, Meta_Paths &mpath, map<int, double> &rtn)
{
    map<int, double> id_sim;
    calSim_PCRW(src, src, mpath, id_sim);
    vector<int> temp;
    vector<double> sims;
    for(map<int, double>::iterator i = id_sim.begin(); i != id_sim.end(); ++i){
        temp.push_back(i->first);
        sims.push_back(i->second);
    }
    for(int i = 1; i < temp.size(); ++i){
        int j = i - 1;
        int theID = temp[i];
        double theSim = sims[i];
        while(j >= 0 && sims[j] < theSim){
            sims[j + 1] = sims[j];
            temp[j + 1] = temp[j];
            j--;
        }
        sims[j + 1] = theSim;
        temp[j + 1] = theID;
    }
    rtn.clear();
    for(int i = 0; i < k; ++i){
        if(i >= temp.size())
            break;
        rtn[temp[i]] = sims[i];
        //cout << temp[i] << '\t' << sims[i] << endl;
    }
}

void SimCalculator::calTopK_PCRW_1(int src, int k, Meta_Paths &mpath, vector<int> &rtn)
{
    map<int, double> id_sim;
    calSim_PCRW(src, src, mpath, id_sim);
    vector<int> temp;
    vector<double> sims;
    for(map<int, double>::iterator i = id_sim.begin(); i != id_sim.end(); ++i){
        temp.push_back(i->first);
        sims.push_back(i->second);
    }
    for(int i = 1; i < temp.size(); ++i){
        int j = i - 1;
        int theID = temp[i];
        double theSim = sims[i];
        while(j >= 0 && sims[j] < theSim){
            sims[j + 1] = sims[j];
            temp[j + 1] = temp[j];
            j--;
        }
        sims[j + 1] = theSim;
        temp[j + 1] = theID;
    }
    rtn.clear();
    for(int i = 0; i < k; ++i){
        if(i >= temp.size())
            break;
        rtn.push_back(temp[i]);
        //cout << temp[i] << '\t' << sims[i] << endl;
    }
}


void SimCalculator::calTopK_StructCount(int src, int k, Meta_Structure &mStruct, vector<int> &rtn)
{
    map<int, int> id_sim;
    calSim_SCount_NC(src, src, mStruct, id_sim);
    vector<int> temp;
    vector<int> sims;
    for(map<int, int>::iterator i = id_sim.begin(); i != id_sim.end(); ++i){
        temp.push_back(i->first);
        sims.push_back(i->second);
    }
    for(int i = 1; i < temp.size(); ++i){
        int j = i - 1;
        int theID = temp[i];
        int theSim = sims[i];
        while(j >= 0 && sims[j] < theSim){
            sims[j + 1] = sims[j];
            temp[j + 1] = temp[j];
            j--;
        }
        sims[j + 1] = theSim;
        temp[j + 1] = theID;
    }
    rtn.clear();
    for(int i = 0; i < k; ++i){
        if(i >= temp.size())
            break;
        rtn.push_back(temp[i]);
    }
}

void SimCalculator::calTopK_StructSim(int src, int k, Meta_Structure &mStruct, vector<int> &rtn)
{
    map<int, double> id_sim;
    calSim_SSim_NC_all(src, src, mStruct, id_sim);
    vector<int> temp;
    vector<double> sims;
    for(map<int, double>::iterator i = id_sim.begin(); i != id_sim.end(); ++i){
        temp.push_back(i->first);
        sims.push_back(i->second);
    }
    for(int i = 1; i < temp.size(); ++i){
        int j = i - 1;
        int theID = temp[i];
        double theSim = sims[i];
        while(j >= 0 && sims[j] < theSim){
            sims[j + 1] = sims[j];
            temp[j + 1] = temp[j];
            j--;
        }
        sims[j + 1] = theSim;
        temp[j + 1] = theID;
    }
    rtn.clear();
    for(int i = 0; i < k; ++i){
        if(i >= temp.size())
            break;
        rtn.push_back(temp[i]);
    }
}

void SimCalculator::calTopK_BSCSE_DFS(int src, int k, Meta_Structure &mStruct, map<vector<int>, double> &rtn, double alpha)
{
    map<vector<int>, double> id_sim;
    calSim_SCRW_NC_mul(src, mStruct, id_sim, alpha);
    vector<vector<int> > id;
    vector<double> sim;
    for(map<vector<int>, double>::iterator i = id_sim.begin(); i != id_sim.end(); ++i){
        if(i->first.size() == mStruct.targets_.size()){
            id.push_back(i->first);
            sim.push_back(i->second);
        }
    }
    
    for(int i = 1; i < id.size(); ++i){
        int j = i - 1;
        vector<int> theV = id[i];
        double theD = sim[i];
        while(j >=0 && sim[j] < theD){
            id[j+1] = id[j];
            sim[j+1] = sim[j];
            --j;
        }
        id[j+1] = theV;
        sim[j+1] = theD;
    }
    rtn.clear();
    for(int i = 0; i < k; ++i){
        if(i >= id.size())
            break;
        rtn[id[i]] = sim[i];
    }
}

void SimCalculator::calTopK_BSCSE_DFS_1(int src, int k, Meta_Structure &mStruct, vector<int> &rtn, double alpha)
{
    map<vector<int>, double> id_sim;
    calSim_SCRW_NC_mul(src, mStruct, id_sim, alpha);
    vector<vector<int> > id;
    vector<double> sim;
    for(map<vector<int>, double>::iterator i = id_sim.begin(); i != id_sim.end(); ++i){
        if(i->first.size() == mStruct.targets_.size()){
            id.push_back(i->first);
            sim.push_back(i->second);
        }
    }
    
    for(int i = 1; i < id.size(); ++i){
        int j = i - 1;
        vector<int> theV = id[i];
        double theD = sim[i];
        while(j >=0 && sim[j] < theD){
            id[j+1] = id[j];
            sim[j+1] = sim[j];
            --j;
        }
        id[j+1] = theV;
        sim[j+1] = theD;
    }
    rtn.clear();
    for(int i = 0; i < k; ++i){
        if(i >= id.size())
            break;
        rtn.push_back(id[i][0]);
    }
}


class SearchTreeNode
{
public:
    bool Expanded_;
    int layer_;
    //SearchTreeNode & father_;
    map<int, int> meta_hin_;
    double weight_;
    vector<SearchTreeNode> childs_;
    
    SearchTreeNode(map<int, int> & mm, double w, int l)
    {
        layer_ = l;
        meta_hin_ = mm;
        weight_ = w;
        Expanded_ = false;
    }
    SearchTreeNode(const SearchTreeNode & n)
    {
        Expanded_ = n.Expanded_;
        meta_hin_ = n.meta_hin_;
        weight_ = n.weight_;
        childs_ = n.childs_;
        layer_ = n.layer_;
    }
    void init(const SearchTreeNode & n)
    {
        Expanded_ = n.Expanded_;
        weight_ = n.weight_;
        childs_ = n.childs_;
        layer_ = n.layer_;
    }
    SearchTreeNode()
    {
    }
};

struct cmp
{
    bool operator()(SearchTreeNode n1, SearchTreeNode n2)
    {
        return n1.weight_ < n2.weight_;
    }
};

/*

*/

void SimCalculator::calTopK_BSCSE_BFS_index(int src, int k, Meta_Structure &mStruct, map<vector<int>, double> &rtn, double alpha, int layer)
{
    map<int, int> mm;
    mm[0] = src;
    SearchTreeNode root(mm, 1.0, 0);
    
    map<vector<int>, double> id_sim;
    
    rtn.clear();
    // vector<vector<int> > topK;
    //vector<double> topKsim;
    
    //priority_queue<SearchTreeNode, vector<SearchTreeNode>, cmp> queue;
    queue<SearchTreeNode> queue;
    queue.push(root);
    
    while(!queue.empty()){
        //SearchTreeNode node = queue.top();
        SearchTreeNode node = queue.front();
        queue.pop();
        if(node.layer_ == layer){
            // it reaches the index
            vector<int> tempV;
            for(int i = 0; i < mStruct.layers_[layer].size(); ++i){
                tempV.push_back(node.meta_hin_[mStruct.layers_[layer][i]]);
            }
            for(map<vector<int>, double>::iterator iter = mStruct.index_[tempV].begin(); iter != mStruct.index_[tempV].end(); ++iter){
                if(id_sim.find(iter->first) == id_sim.end())
                    id_sim[iter->first] = 0;
                id_sim[iter->first] += node.weight_ * iter->second;
            }
            
        }else{
            // to be expanded
            map<int, vector<int> > candidatesOnMetaID;
            for(int k = 0; k < mStruct.layers_[node.layer_ + 1].size(); ++k){
                int metaID = mStruct.layers_[node.layer_ + 1][k];
                // the node it depends on
                vector<int> dependencyNodes;
                for(int j = 0; j < mStruct.dst_edges_[metaID].size(); ++j){
                    dependencyNodes.push_back(mStruct.dst_edges_[metaID][j].src_);
                }
                int meta_id = dependencyNodes[0];
                int hin_id = node.meta_hin_[meta_id];
                int edgeType = mStruct.dst_edges_[metaID][0].type_;
                // candidate id with respect to the first dependency
                set<int> candidates;
                if(edgeType > 0){
                    for(int j = 0; j < mStruct.hin_graph_.edges_src_[hin_id].size(); ++j){
                        if(mStruct.hin_graph_.edges_src_[hin_id][j].edge_type_ == edgeType){
                            candidates.insert(mStruct.hin_graph_.edges_src_[hin_id][j].dst_);
                        }
                    }
                }else{
                    for(int j = 0; j < mStruct.hin_graph_.edges_dst_[hin_id].size(); ++j){
                        if(mStruct.hin_graph_.edges_dst_[hin_id][j].edge_type_ == -edgeType){
                            candidates.insert(mStruct.hin_graph_.edges_dst_[hin_id][j].src_);
                        }
                    }
                }
                // remove those candidates cannot match other dependency
                for(int k = 1; k < dependencyNodes.size(); ++k){
                    set<int> tempcandidates;
                    int meta_id = dependencyNodes[k];
                    int hin_id = node.meta_hin_[meta_id];
                    int edgeType = mStruct.dst_edges_[metaID][k].type_;
                    if(edgeType > 0){
                        for(int j = 0; j < mStruct.hin_graph_.edges_src_[hin_id].size(); ++j){
                            if(mStruct.hin_graph_.edges_src_[hin_id][j].edge_type_ == edgeType){
                                tempcandidates.insert(mStruct.hin_graph_.edges_src_[hin_id][j].dst_);
                            }
                        }
                    }else{
                        for(int j = 0; j < mStruct.hin_graph_.edges_dst_[hin_id].size(); ++j){
                            if(mStruct.hin_graph_.edges_dst_[hin_id][j].edge_type_ == -edgeType){
                                tempcandidates.insert(mStruct.hin_graph_.edges_dst_[hin_id][j].src_);
                            }
                        }
                    }
                    set<int>::iterator iter2 = candidates.begin();
                    while(iter2 != candidates.end()){
                        if(tempcandidates.find(*iter2) == tempcandidates.end()){
                            iter2 = candidates.erase(iter2);
                        }else{
                            iter2++;
                        }
                    }
                }
                for(set<int>::iterator iter2 = candidates.begin(); iter2 != candidates.end(); ++iter2){
                    candidatesOnMetaID[metaID].push_back(*iter2);
                }
                
            }
            // merge the new nodes
            int totalSize = 1;
            for(map<int, vector<int> >::iterator iterator = candidatesOnMetaID.begin(); iterator != candidatesOnMetaID.end(); ++iterator){
                totalSize *= iterator->second.size();
            }
            SearchTreeNode tempNodeTemplate(node);
            tempNodeTemplate.layer_++;
            tempNodeTemplate.weight_ = pow(node.weight_ / totalSize, alpha);
            
            // for each i
            vector<SearchTreeNode> tempCurrentV;
            tempCurrentV.push_back(tempNodeTemplate);
            
            for(map<int, vector<int> >::iterator iterator = candidatesOnMetaID.begin(); iterator != candidatesOnMetaID.end(); ++iterator){
                vector<SearchTreeNode> temptempCurrentV;
                for(int k = 0; k < tempCurrentV.size(); ++k){
                    for(int ii = 0; ii < iterator->second.size(); ++ii){
                        int meta = iterator->first;
                        int hin = iterator->second[ii];
                        SearchTreeNode temptemp = tempCurrentV[k];
                        temptemp.meta_hin_[meta] = hin;
                        temptempCurrentV.push_back(temptemp);
                    }
                }
                tempCurrentV = temptempCurrentV;
            }
            for(int k = 0; k < tempCurrentV.size(); ++k){
                queue.push(tempCurrentV[k]);
            }
        }
    }
    
    // sort
    
    vector<vector<int> > id;
    vector<double> sim;
    for(map<vector<int>, double>::iterator i = id_sim.begin(); i != id_sim.end(); ++i){
        if(i->first.size() == mStruct.targets_.size()){
            id.push_back(i->first);
            sim.push_back(i->second);
        }
    }
    
    for(int i = 1; i < id.size(); ++i){
        int j = i - 1;
        vector<int> theV = id[i];
        double theD = sim[i];
        while(j >=0 && sim[j] < theD){
            id[j+1] = id[j];
            sim[j+1] = sim[j];
            --j;
        }
        id[j+1] = theV;
        sim[j+1] = theD;
    }
    rtn.clear();
    for(int i = 0; i < k; ++i){
        if(i >= id.size())
            break;
        rtn[id[i]] = sim[i];
    }
    
    
    
    return;

}


void SimCalculator::calTopK_BSCSE_BFS(int src, int k, Meta_Structure &mStruct, map<vector<int>, double> &rtn, double alpha)
{
    map<int, int> mm;
    mm[0] = src;
    SearchTreeNode root(mm, 1.0, 0);
    
    map<vector<int>, double> id_sim;
    
    rtn.clear();

    queue<SearchTreeNode> queue;
    queue.push(root);
    
    while(!queue.empty()){
        //SearchTreeNode node = queue.top();
        SearchTreeNode node = queue.front();
        queue.pop();
        if(node.layer_ == mStruct.layers_.size() - 1){
            // it is an instance
            bool flag = false;
            vector<int> tempV;
            for(int i = 0; i < mStruct.targets_.size(); ++i){
                if(node.meta_hin_.find(mStruct.targets_[i])==node.meta_hin_.end()){
                    flag = true;
                    break;
                }
                if(flag)
                    continue;
                tempV.push_back(node.meta_hin_[mStruct.targets_[i]]);
            }
            if(id_sim.find(tempV) == id_sim.end())
                id_sim[tempV] = 0;
            id_sim[tempV] += node.weight_;
            
        }else{
            // to be expanded
            map<int, vector<int> > candidatesOnMetaID;
            for(int k = 0; k < mStruct.layers_[node.layer_ + 1].size(); ++k){
                int metaID = mStruct.layers_[node.layer_ + 1][k];
                // the node it depends on
                vector<int> dependencyNodes;
                for(int j = 0; j < mStruct.dst_edges_[metaID].size(); ++j){
                    dependencyNodes.push_back(mStruct.dst_edges_[metaID][j].src_);
                }
                int meta_id = dependencyNodes[0];
                int hin_id = node.meta_hin_[meta_id];
                int edgeType = mStruct.dst_edges_[metaID][0].type_;
                // candidate id with respect to the first dependency
                set<int> candidates;
                if(edgeType > 0){
                    for(int j = 0; j < mStruct.hin_graph_.edges_src_[hin_id].size(); ++j){
                        if(mStruct.hin_graph_.edges_src_[hin_id][j].edge_type_ == edgeType){
                            candidates.insert(mStruct.hin_graph_.edges_src_[hin_id][j].dst_);
                        }
                    }
                }else{
                    for(int j = 0; j < mStruct.hin_graph_.edges_dst_[hin_id].size(); ++j){
                        if(mStruct.hin_graph_.edges_dst_[hin_id][j].edge_type_ == -edgeType){
                            candidates.insert(mStruct.hin_graph_.edges_dst_[hin_id][j].src_);
                        }
                    }
                }
                // remove those candidates cannot match other dependency
                for(int k = 1; k < dependencyNodes.size(); ++k){
                    set<int> tempcandidates;
                    int meta_id = dependencyNodes[k];
                    int hin_id = node.meta_hin_[meta_id];
                    int edgeType = mStruct.dst_edges_[metaID][k].type_;
                    if(edgeType > 0){
                        for(int j = 0; j < mStruct.hin_graph_.edges_src_[hin_id].size(); ++j){
                            if(mStruct.hin_graph_.edges_src_[hin_id][j].edge_type_ == edgeType){
                                tempcandidates.insert(mStruct.hin_graph_.edges_src_[hin_id][j].dst_);
                            }
                        }
                    }else{
                        for(int j = 0; j < mStruct.hin_graph_.edges_dst_[hin_id].size(); ++j){
                            if(mStruct.hin_graph_.edges_dst_[hin_id][j].edge_type_ == -edgeType){
                                tempcandidates.insert(mStruct.hin_graph_.edges_dst_[hin_id][j].src_);
                            }
                        }
                    }
                    set<int>::iterator iter2 = candidates.begin();
                    while(iter2 != candidates.end()){
                        if(tempcandidates.find(*iter2) == tempcandidates.end()){
                            iter2 = candidates.erase(iter2);
                        }else{
                            iter2++;
                        }
                    }
                }
                for(set<int>::iterator iter2 = candidates.begin(); iter2 != candidates.end(); ++iter2){
                    candidatesOnMetaID[metaID].push_back(*iter2);
                }
            
            }
            // merge the new nodes
            int totalSize = 1;
            for(map<int, vector<int> >::iterator iterator = candidatesOnMetaID.begin(); iterator != candidatesOnMetaID.end(); ++iterator){
                totalSize *= iterator->second.size();
            }
            SearchTreeNode tempNodeTemplate(node);
            tempNodeTemplate.layer_++;
            tempNodeTemplate.weight_ = pow(node.weight_ / totalSize, alpha);
            
            // for each i
            vector<SearchTreeNode> tempCurrentV;
            tempCurrentV.push_back(tempNodeTemplate);
            
            for(map<int, vector<int> >::iterator iterator = candidatesOnMetaID.begin(); iterator != candidatesOnMetaID.end(); ++iterator){
                vector<SearchTreeNode> temptempCurrentV;
                for(int k = 0; k < tempCurrentV.size(); ++k){
                    for(int ii = 0; ii < iterator->second.size(); ++ii){
                        int meta = iterator->first;
                        int hin = iterator->second[ii];
                        SearchTreeNode temptemp(tempCurrentV[k]);
                        //temptemp.init(tempCurrentV[k]);
                        // KKK
                        
                        temptemp.meta_hin_[meta] = hin;
                        temptempCurrentV.push_back(temptemp);
                    }
                }
                tempCurrentV = temptempCurrentV;
            }
            for(int k = 0; k < tempCurrentV.size(); ++k){
                queue.push(tempCurrentV[k]);
            }
        }
    }
    
    // sort
    
    vector<vector<int> > id;
    vector<double> sim;
    for(map<vector<int>, double>::iterator i = id_sim.begin(); i != id_sim.end(); ++i){
        if(i->first.size() == mStruct.targets_.size()){
            id.push_back(i->first);
            sim.push_back(i->second);
        }
    }
    
    for(int i = 1; i < id.size(); ++i){
        int j = i - 1;
        vector<int> theV = id[i];
        double theD = sim[i];
        while(j >=0 && sim[j] < theD){
            id[j+1] = id[j];
            sim[j+1] = sim[j];
            --j;
        }
        id[j+1] = theV;
        sim[j+1] = theD;
    }
    rtn.clear();
    for(int i = 0; i < k; ++i){
        if(i >= id.size())
            break;
        rtn[id[i]] = sim[i];
    }
    return;
}


void SimCalculator::cal_BSCSE_BFS(map<int, int> src, int k, Meta_Structure &mStruct, map<vector<int>, double> &rtn, double alpha)
{

    SearchTreeNode root(src, 1.0, k);
    
    map<vector<int>, double> id_sim;
    
    rtn.clear();
 
    queue<SearchTreeNode> queue;
    queue.push(root);
    
    
    while(!queue.empty()){
        clock_t t;
        t = clock();
        //SearchTreeNode node = queue.top();
        SearchTreeNode node = queue.front();
        queue.pop();
        if(node.layer_ == mStruct.layers_.size() - 1){
            // it is an instance
            vector<int> tempV;
            for(int i = 0; i < mStruct.targets_.size(); ++i){
                tempV.push_back(node.meta_hin_[mStruct.targets_[i]]);
            }
            if(id_sim.find(tempV) == id_sim.end())
                id_sim[tempV] = 0;
            id_sim[tempV] += node.weight_;
            
        }else{
            // to be expanded
            map<int, vector<int> > candidatesOnMetaID;
            for(int k = 0; k < mStruct.layers_[node.layer_ + 1].size(); ++k){
                int metaID = mStruct.layers_[node.layer_ + 1][k];
                // the node it depends on
                vector<int> dependencyNodes;
                for(int j = 0; j < mStruct.dst_edges_[metaID].size(); ++j){
                    dependencyNodes.push_back(mStruct.dst_edges_[metaID][j].src_);
                }
                int meta_id = dependencyNodes[0];
                int hin_id = node.meta_hin_[meta_id];
                int edgeType = mStruct.dst_edges_[metaID][0].type_;
                // candidate id with respect to the first dependency
                set<int> candidates;
                if(edgeType > 0){
                    for(int j = 0; j < mStruct.hin_graph_.edges_src_[hin_id].size(); ++j){
                        if(mStruct.hin_graph_.edges_src_[hin_id][j].edge_type_ == edgeType){
                            candidates.insert(mStruct.hin_graph_.edges_src_[hin_id][j].dst_);
                        }
                    }
                }else{
                    for(int j = 0; j < mStruct.hin_graph_.edges_dst_[hin_id].size(); ++j){
                        if(mStruct.hin_graph_.edges_dst_[hin_id][j].edge_type_ == -edgeType){
                            candidates.insert(mStruct.hin_graph_.edges_dst_[hin_id][j].src_);
                        }
                    }
                }
                // remove those candidates cannot match other dependency
                for(int k = 1; k < dependencyNodes.size(); ++k){
                    set<int> tempcandidates;
                    int meta_id = dependencyNodes[k];
                    int hin_id = node.meta_hin_[meta_id];
                    int edgeType = mStruct.dst_edges_[metaID][k].type_;
                    if(edgeType > 0){
                        for(int j = 0; j < mStruct.hin_graph_.edges_src_[hin_id].size(); ++j){
                            if(mStruct.hin_graph_.edges_src_[hin_id][j].edge_type_ == edgeType){
                                tempcandidates.insert(mStruct.hin_graph_.edges_src_[hin_id][j].dst_);
                            }
                        }
                    }else{
                        for(int j = 0; j < mStruct.hin_graph_.edges_dst_[hin_id].size(); ++j){
                            if(mStruct.hin_graph_.edges_dst_[hin_id][j].edge_type_ == -edgeType){
                                tempcandidates.insert(mStruct.hin_graph_.edges_dst_[hin_id][j].src_);
                            }
                        }
                    }
                    set<int>::iterator iter2 = candidates.begin();
                    while(iter2 != candidates.end()){
                        if(tempcandidates.find(*iter2) == tempcandidates.end()){
                            iter2 = candidates.erase(iter2);
                        }else{
                            iter2++;
                        }
                    }
                }
                for(set<int>::iterator iter2 = candidates.begin(); iter2 != candidates.end(); ++iter2){
                    candidatesOnMetaID[metaID].push_back(*iter2);
                }
                
            }
            // merge the new nodes
            int totalSize = 1;
            for(map<int, vector<int> >::iterator iterator = candidatesOnMetaID.begin(); iterator != candidatesOnMetaID.end(); ++iterator){
                totalSize *= iterator->second.size();
            }
            SearchTreeNode tempNodeTemplate(node);
            tempNodeTemplate.layer_++;
            tempNodeTemplate.weight_ = pow(node.weight_ / totalSize, alpha);
            
            // for each i
            vector<SearchTreeNode> tempCurrentV;
            tempCurrentV.push_back(tempNodeTemplate);
            
            for(map<int, vector<int> >::iterator iterator = candidatesOnMetaID.begin(); iterator != candidatesOnMetaID.end(); ++iterator){
                vector<SearchTreeNode> temptempCurrentV;
                for(int k = 0; k < tempCurrentV.size(); ++k){
                    for(int ii = 0; ii < iterator->second.size(); ++ii){
                        int meta = iterator->first;
                        int hin = iterator->second[ii];
                        SearchTreeNode temptemp = tempCurrentV[k];
                        temptemp.meta_hin_[meta] = hin;
                        temptempCurrentV.push_back(temptemp);
                    }
                }
                tempCurrentV = temptempCurrentV;
            }
            for(int k = 0; k < tempCurrentV.size(); ++k){
                queue.push(tempCurrentV[k]);
            }
        }
       // cout << node.layer_ << '\t' << (double)(clock() - t)/CLOCKS_PER_SEC << endl;
    }
    
    rtn = id_sim;
}


void SimCalculator::buildIndex(string outpath, Meta_Structure & mStruct)
{
    int type1 = 1;
    //int type2 = 2;
    
    ofstream out(outpath, ios::out);
    /*
    for(map<int, HIN_Node>::iterator i = mStruct.hin_graph_.nodes_.begin(); i != mStruct.hin_graph_.nodes_.end(); ++i){
        
        if(i->second.types_id_.size() == 0 || i->second.types_id_[0] != type1){
            continue;
        }
        for(map<int, HIN_Node>::iterator j = mStruct.hin_graph_.nodes_.begin(); j != mStruct.hin_graph_.nodes_.end(); ++j){
            if(mStruct.hin_graph_.edges_dst_[j->first].size() == 0 || mStruct.hin_graph_.edges_dst_[j->first][0].edge_type_ != type2)
                continue;
            int id1 = i->first;
            int id2 = j->first;
            map<int, int> mm;
            mm[2] = id1;
            mm[3] = id2;
            map<vector<int>, double> rtn;
            SimCalculator::cal_BSCSE_BFS(mm, 2, mStruct, rtn, 1);
            for(map<vector<int>, double>::iterator iter = rtn.begin(); iter != rtn.end(); ++iter){
                out << id1 << '\t' << id2 << '\t';
                for(int k = 0; k < mStruct.targets_.size(); ++k){
                    out << iter->first[k] << '\t';
                }
                out << iter->second << endl;
            }
        }
    }
     */
    
    for(map<int, HIN_Node>::iterator i = mStruct.hin_graph_.nodes_.begin(); i != mStruct.hin_graph_.nodes_.end(); ++i){
        
        if(i->second.types_id_.size() == 0 || i->second.types_id_[0] != type1){
            continue;
        }
            int id1 = i->first;
            map<int, int> mm;
            mm[0] = id1;
            map<vector<int>, double> rtn;
            SimCalculator::cal_BSCSE_BFS(mm, 0, mStruct, rtn, 1);
            for(map<vector<int>, double>::iterator iter = rtn.begin(); iter != rtn.end(); ++iter){
                out << id1 << '\t' ;
                for(int k = 0; k < mStruct.targets_.size(); ++k){
                    out << iter->first[k] << '\t';
                }
                out << iter->second << endl;
            }
        }
    
}