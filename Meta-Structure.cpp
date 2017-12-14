//
//  Meta-Structure.cpp
//  DBLP_Reader
//
//  Created by 黄智鹏 on 15/10/8.
//  Copyright (c) 2015年 黄智鹏. All rights reserved.
//

#include <sstream>
#include <fstream>
#include <iostream>
#include "Meta-Structure.h"

using namespace  std;

Meta_Structure::Meta_Structure(HIN_Graph & hin):hin_graph_(hin)
{

}


Meta_Paths::Meta_Paths(HIN_Graph & hg):hin_graph_(hg)
{

}

Meta_Paths & Meta_Paths::operator=(const Meta_Paths &p)
{
    hin_graph_ = p.hin_graph_;
    linkTypes_ = p.linkTypes_;
    nodeTypes_ = p.nodeTypes_;
    weights_ = p.weights_;
    return *this;
}
Meta_Paths::Meta_Paths(const Meta_Paths & p): hin_graph_(p.hin_graph_)
{
    linkTypes_ = p.linkTypes_;
    nodeTypes_ = p.nodeTypes_;
    weights_ = p.weights_;
}

void Meta_Structure::initialFromFile(string path)
{
    ifstream input(path.c_str(), ios::in);
    string line;
    while(getline(input, line)){
        if(line == "EDGE")
            break;
        stringstream s_input(line);
        Meta_Node tempNode;
        tempNode.neighborWeightSum_ = 0.0;
        s_input >> tempNode.id_ >> tempNode.type_;
        meta_nodes_.push_back(tempNode);
    }
    while(getline(input, line)){
        Meta_Edge tempEdge;
        stringstream s_input(line);
        s_input >> tempEdge.src_ >> tempEdge.dst_ >> tempEdge.type_ >> tempEdge.weight_;
        meta_nodes_[tempEdge.src_].neighborWeightSum_ += tempEdge.weight_;
        meta_nodes_[tempEdge.dst_].neighborWeightSum_ += tempEdge.weight_;
        srt_edges_[tempEdge.src_].push_back(tempEdge);
        dst_edges_[tempEdge.dst_].push_back(tempEdge);
    }
    
    for(int i = 0; i < meta_nodes_.size(); ++i){
        id_type_ids_.push_back(map<int, vector<int> >());
        for(int j = 0; j < srt_edges_[i].size(); ++j){
            int edgeType = srt_edges_[i][j].type_;
            int dstIC = srt_edges_[i][j].dst_;
            if(id_type_ids_[i].find(edgeType) == id_type_ids_[i].end())
                id_type_ids_[i][edgeType] = vector<int>();
            id_type_ids_[i][edgeType].push_back(dstIC);
        }
    }
    
    
    return;
}

void Meta_Structure::initialFromFile_mul(string path)
{
    ifstream input(path.c_str(), ios::in);
    string line;
    
    getline(input, line);
    stringstream s_input(line);
    int t;
    while(s_input >> t){
        targets_.push_back(t);
    }
    
    while(getline(input, line)){
        if(line == "EDGE")
            break;
        stringstream s_input(line);
        Meta_Node tempNode;
        tempNode.neighborWeightSum_ = 0.0;
        s_input >> tempNode.id_ >> tempNode.type_;
        meta_nodes_.push_back(tempNode);
    }
    while(getline(input, line)){
        Meta_Edge tempEdge;
        stringstream s_input(line);
        s_input >> tempEdge.src_ >> tempEdge.dst_ >> tempEdge.type_ >> tempEdge.weight_;
        meta_nodes_[tempEdge.src_].neighborWeightSum_ += tempEdge.weight_;
        meta_nodes_[tempEdge.dst_].neighborWeightSum_ += tempEdge.weight_;
        srt_edges_[tempEdge.src_].push_back(tempEdge);
        dst_edges_[tempEdge.dst_].push_back(tempEdge);
    }
    
    for(int i = 0; i < meta_nodes_.size(); ++i){
        id_type_ids_.push_back(map<int, vector<int> >());
        for(int j = 0; j < srt_edges_[i].size(); ++j){
            int edgeType = srt_edges_[i][j].type_;
            int dstIC = srt_edges_[i][j].dst_;
            if(id_type_ids_[i].find(edgeType) == id_type_ids_[i].end())
                id_type_ids_[i][edgeType] = vector<int>();
            id_type_ids_[i][edgeType].push_back(dstIC);
        }
    }
    
    vector<int> degrees;
    for(int i = 0; i < meta_nodes_.size(); ++i){
        degrees.push_back(dst_edges_[i].size());
    }
    int count = 0;
    
    while(count < meta_nodes_.size()){
        vector<int> temp;
        for(int i = 0; i < degrees.size(); ++i){
            if(degrees[i] == 0){
                temp.push_back(i);
                degrees[i]  = -1;
            }
            
        }
        count += temp.size();
        layers_.push_back(temp);
        for(int i = 0; i < temp.size(); ++i){
            for(int j = 0; j < srt_edges_[temp[i]].size(); ++j)
            {
                degrees[srt_edges_[temp[i]][j].dst_]--;
            }
        }
    }
    return;
}



void Meta_Structure::showStruct()
{
    cerr << "print Meta_structure:" << endl;
    for(map<int, vector<Meta_Edge> >::iterator i = srt_edges_.begin(); i != srt_edges_.end(); ++i){
        for(int j = 0; j < i->second.size(); ++j){
            int src = i->second[j].src_;
            int dst = i->second[j].dst_;
            int type = i->second[j].type_;
            if(type > 0){
                cout << hin_graph_.node_types_[meta_nodes_[src].type_] << '\t' << hin_graph_.edge_types_[type] << '\t' << hin_graph_.node_types_[meta_nodes_[dst].type_] << endl;
            }else{
                cout << hin_graph_.node_types_[meta_nodes_[src].type_] << "\t-" <<  hin_graph_.edge_types_[-type] << '\t' << hin_graph_.node_types_[meta_nodes_[dst].type_] << endl;
            }
        }
    }
}

void Meta_Structure::initialFromMetaPath(Meta_Paths &m_paths)
{
    vector<set<int> > nodes;
    vector<map<int, int> > edges;
    
    
    for(int j = 0; j < m_paths.nodeTypes_[0].size(); ++j){
        set<int> temp;
        temp.insert(m_paths.nodeTypes_[0][j]);
        nodes.push_back(temp);
        if(j > 0){
            edges.push_back(map<int, int>());
            edges[edges.size() - 1][m_paths.linkTypes_[0][j - 1]] = nodes.size() - 1;
        }
    }
    for(int i = 1; i < m_paths.linkTypes_.size(); ++i){
        nodes[0].insert(m_paths.nodeTypes_[i][0]);
        int now = 0;
        for(int j = 1; j < m_paths.nodeTypes_[i].size(); ++j){
            int found = -1;
            int nodeType = m_paths.nodeTypes_[i][j];
            // bfs, not for
            for(int k = now; k < nodes.size(); ++k){
                if(nodes[k].find(nodeType) != nodes[k].end()){
                    found = k;
                    break;
                }
            }
            if(found == -1){
                
            }
        }
    }

}


// To be continue
void Meta_Structure::readIndex(string path)
{
    ifstream in(path.c_str(), ios::in);
    int id1, id2, idA;
    double w;
    while(in>> id1 >> id2 >> idA >> w)
    {
        vector<int> v;
        v.push_back(id1);
        v.push_back(id2);
        if(index_.find(v) == index_.end())
            index_[v] = map<vector<int>, double>();
        vector<int> vv;
        vv.push_back(idA);
        index_[v][vv] = w;
    }
    in.close();
}


void Meta_Structure::readD(string path)
{
    ifstream in(path.c_str(), ios::in);
    string line;

    int l;
    in >> l;
    for(int i = 0; i < l; ++i)
    {
        getline(in, line);
        stringstream input(line);
        int c;
        
        input >> c;
        vector<int> v;
        while(input >> c){
            v.push_back(c);
        }
        
        D_.push_back(v);
    }
    in.close();
}
