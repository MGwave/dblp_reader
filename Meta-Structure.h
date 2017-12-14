//
//  Meta-Structure.h
//  DBLP_Reader
//
//  Created by 黄智鹏 on 15/10/8.
//  Copyright (c) 2015年 黄智鹏. All rights reserved.
//

#ifndef __DBLP_Reader__Meta_Structure__
#define __DBLP_Reader__Meta_Structure__

#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>

#include "HIN_Graph.h"

using namespace std;

class Meta_Node
{
public:
    int id_;
    int type_;
    double neighborWeightSum_;
};

class Meta_Edge
{
public:
    int src_, dst_;
    int type_;
    double weight_;
};

class Meta_Paths
{
public:
    HIN_Graph & hin_graph_;
    vector< vector<int> > nodeTypes_;
    vector< vector<int> > linkTypes_;
    vector<double> weights_;
    
    Meta_Paths & operator = (const Meta_Paths & p);
    
    //Meta_Paths();
    Meta_Paths(HIN_Graph & hg);
    Meta_Paths(const Meta_Paths & p);
};

class Meta_Structure
{
public:
    HIN_Graph & hin_graph_;
    
    vector<vector<int> > layers_;
    vector<Meta_Node> meta_nodes_;
    map<int, vector<Meta_Edge> > srt_edges_;
    map<int, vector<Meta_Edge> > dst_edges_;
    
    vector< map<int, vector<int> > > id_type_ids_;
    
    vector<vector<int> > D_;
    
    vector<int> targets_;
    
    map<vector<int>, map<vector<int>, double> > index_;
    
    Meta_Structure(HIN_Graph & hin);
    void initialFromMetaPath(Meta_Paths & m_paths);
    void initialFromFile(string path);
    void initialFromFile_mul(string path);
    void showStruct();
    void readIndex(string path);
    void readD(string path);
};



#endif /* defined(__DBLP_Reader__Meta_Structure__) */
