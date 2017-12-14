//
//  SimCalculator.h
//  DBLP_Reader
//
//  Created by 黄智鹏 on 15/10/8.
//  Copyright (c) 2015年 黄智鹏. All rights reserved.
//

#ifndef __DBLP_Reader__SimCalculator__
#define __DBLP_Reader__SimCalculator__

#include <stdio.h>
#include <map>
#include "Meta-Structure.h"
#include "HIN_Graph.h"

//#define EPSILON 0.000001
  #define EPSILON 0.00000001


#define KMEANS_NUM 4 // 7  5
#define KMEANS_ITERATIONS 50
//#define KMEANS_ITERATIONS 30
#define NODETYPE 0
// 1 for considering nodetype
// 0 for not considering

using namespace std;


class SCRW_Node
{
public:
    int src_id_;
    double pro_;
    vector<double> positionV_;
    
    SCRW_Node(int ide, double pro, vector<double> v);
    SCRW_Node(const SCRW_Node & n);
    SCRW_Node();
    
};



class SimCalculator
{
private:
    static vector< vector<int> > joinEdges(int src, int dst, vector<Meta_Edge> tobe_Edges, Meta_Structure & m_struct);
    static vector< vector<int> > joinEdges_NC(int src, int dst, Meta_Structure & m_struct);
    
    static vector< vector<int> > joinEdges_CDFS(int src, int dst, Meta_Structure & m_struct);
    
    static vector< vector<int> > joinEdges_1(int position, int src, vector<Meta_Edge> tobe_Edges, Meta_Structure & m_struct);
    
    static vector< vector<int> > mergeSetIntoJoinResult(vector<HIN_Edge> & theSet, vector< vector<int> > & tempJoinResult, int s_position, int d_position, Meta_Structure & m_struct);
    
    static int calSim_SCount_1(int position, int src, Meta_Structure & m_struct);
    
    static map<int, double> SCRW_Sim(int dst, Meta_Structure & m_struct, map<int, SCRW_Node> &note_map);
    static map<int, double> SCRW_Sim_NODETYPE(int dst, Meta_Structure & m_struct, map<int, SCRW_Node> &note_map);
    
    static map<int, double> SCRW_Sim_Index(int dst, Meta_Structure & m_struct, map<int, SCRW_Node> &note_map);
    
    
    
    
    static void SCRW_NC(int src, int dst, Meta_Structure & m_struct, map<int, double> & id_sim);
    
public:
    static void PCRW(int src, int dst, Meta_Paths & m_path, int p_id, map<int, double> & id_sim, vector< vector<pair<int, double> > > & pathInstances);
    
    // Cyclic version, not useful
    static int calSim_SCount(int src, int dst, Meta_Structure & m_struct);
    static double calSim_SSim(int src, int dst, Meta_Structure & m_struct);
    static double calSim_SCRW(int src, int dst, Meta_Structure & m_struct, map<int, double> & id_sim);

    // meta path version
    static double calSim_PCRW(int src, int dst, Meta_Paths & m_paths, map<int, double> & id_sim);
    static double calSim_PC(int src, int dst, Meta_Paths & m_paths, map<int, double> & id_sim);
    static double calSim_PS(int src, int dst, Meta_Paths & m_paths);
    static double calSim_PS_all(int src, int dst, Meta_Paths & m_paths, map<int, double> & id_sim);
    // not yet
    static double calSim_HeteSim(int src, int dst, Meta_Paths & m_paths);
    
    // no cyclic version
    static int calSim_SCount_NC(int src, int dst, Meta_Structure & m_struct, map<int, int>& id_count);
    static int calSim_SCount_CDFS(int src, int dst, Meta_Structure & m_struct, map<int, int>& id_count);
    
    static double calSim_DFS(int src, int dst, Meta_Structure & m_struct, map<int, double> & id_sim);
    
    static double calSim_SCRW_NC(int src, int dst, Meta_Structure & m_struct, map<int, double> & id_sim);
    static int calSim_PCount_NC(int src, int dst, Meta_Structure & m_struct, map<int, int>& id_count);
    static double calSim_SSim_NC(int src, int dst, Meta_Structure & m_struct);
    static double calSim_SSim_NC_all(int src, int dst, Meta_Structure & m_struct, map<int, double> & id_sim);
   
    // the BSCSE, the most important function. BSCSE for multiple targets.
    static void calSim_SCRW_NC_mul(int src, Meta_Structure & m_struct, map<vector<int>, double> & id_sim, double alpha);

    static void linkPrediction(string in_path, string out_path, string country_path, Meta_Structure & m_struct);
    static void linkPrediction_PATH(string in_path, string out_path, string country_path, Meta_Paths & metaPaths);
    
    static void predictNodes_SCRW(int src, Meta_Structure & m_struct);
    static void predictNodes_CPCRW(int src, Meta_Paths & mpath);
    
    static void calSimForAllEntities(string inpath, string outpath, Meta_Structure & m_struct);
    
    static void calSimForAllEntities_SCRW_NC(string inpath, string outpath, Meta_Structure & m_struct);
    
    static void calSimForAllEntities_SCRW_NC_mul(string inpath, string outpath, Meta_Structure & m_struct);
    static void calSimForAllEntities_BFS(string inpath, string outpath, Meta_Structure & m_struct);

    static void calSimForAllEntities_FSPG(string inpath, string outpath,  Meta_Paths & mpath);
    static void calSimForAllEntities_PathSim(string inpath, string outpath,  Meta_Paths & mpath);
    
    static void calSimForAllEntities_StructCount_NC(string inpath, string outpath, Meta_Structure & m_struct);
    static void calSimForAllEntities_PCount_NC(string inpath, string outpath, Meta_Structure & m_struct);
    
    static void calSimForAllEntities_SSim(string inpath, string outpath, Meta_Structure & m_struct);
    
    
    static void calTopK_PathCount(int src, int k, Meta_Paths & mpath, map<int, double> & rtn);
    static void calTopK_PathCount_1(int src, int k, Meta_Paths & mpath, vector<int> & rtn);
    
    static void calTopK_PathSim(int src, int k, Meta_Paths & mpath, map<int, double> & rtn);
    static void calTopK_PCRW(int src, int k, Meta_Paths & mpath, map<int, double> & rtn);
    static void calTopK_PCRW_1(int src, int k, Meta_Paths & mpath, vector<int> & rtn);

    
    static void calTopK_StructCount(int src, int k, Meta_Structure & mStruct, vector<int> & rtn);
    static void calTopK_StructSim(int src, int k, Meta_Structure & mStruct, vector<int> & rtn);
    
    // proposed algorithm
    static void calTopK_BSCSE_DFS(int src, int k, Meta_Structure & mStruct, map<vector<int>, double> & rtn, double alpha);
    static void calTopK_BSCSE_CDFS(int src, int k, Meta_Structure & mStruct, map<vector<int>, double> & rtn, double alpha);
    static void calTopK_BSCSE_DFS_1(int src, int k, Meta_Structure & mStruct, vector<int> & rtn, double alpha);
    static void calTopK_BSCSE_BFS(int src, int k, Meta_Structure & mStruct, map<vector<int>, double> & rtn, double alpha);
    
    static void calTopK_BSCSE_BFS_index(int src, int k, Meta_Structure & mStruct, map<vector<int>, double> & rtn, double alpha, int layer);
    static void cal_BSCSE_BFS(map<int, int> src, int k, Meta_Structure & mStruct, map<vector<int>, double> & rtn, double alpha);
    
    // cluster
    static void clustering_Kmeans(string inputPath, string simPath);
    static void clustering_Hire(string inputPath, string simPath);
    
    static void buildIndex(string outpath, Meta_Structure & mStruct);
    
};


#endif /* defined(__DBLP_Reader__SimCalculator__) */
