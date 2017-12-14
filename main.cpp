
//
//  main.cpp
//  DBLP_Reader
//
//  Created by 黄智鹏 on 15/10/7.
//  Copyright (c) 2015年 黄智鹏. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <time.h>

#include "HIN_Graph.h"
#include "DBLP_Reader.h"
#include "Article.h"
#include "Meta-Structure.h"
#include "SimCalculator.h"
#include "yagoReader.h"

#define DBLP_PATH "/Users/huangzhipeng/research/DBLP/dblp.xml"


#define DATA 1
// 0 for DBLP
// 1 for YAGO
// 2 for DBLP_four_area
// 3 for movie
// 4 for expariment, linkPrediction
// 5 for experiment, efficiency time
// 6 for experiment, case study (ACT)
// 7 for experiment, case study (ADP)
// 8 for experiment, case study (P2P_ACT)
// 9 for exp, Disambiguation
vector<Article> articles_;


using namespace std;

void Exp_find_Disambiguation()
{
    map<int, vector<Edge>> adj;
    map<int,string> node_name;
    map<int,string> node_type_name;
    map<int,int> node_type_num;
    map<int,vector<int>> node_id_to_type;
    map<int,string> edge_name;
    
    YagoReader::readADJ("/Users/huangzhipeng/research/Yago/yagoadj.txt", adj);
    YagoReader::readNodeName("/Users/huangzhipeng/XcodeWorkPlace/Join/Data/yagoTaxID.txt",node_name,node_type_name);
    YagoReader::readNodeTypeNum("/Users/huangzhipeng/XcodeWorkPlace/Join/Data/yagoTypeNum.txt", node_type_num);
    YagoReader::readNodeIdToType("/Users/huangzhipeng/XcodeWorkPlace/Join/Data/totalType.txt", node_id_to_type);
    YagoReader::readEdgeName("/Users/huangzhipeng/XcodeWorkPlace/Join/Data/yagoType.txt",edge_name);
    
    HIN_Graph Yago_Graph;
    Yago_Graph.buildYAGOGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
    
    
    Meta_Structure meta_structure(Yago_Graph);
    meta_structure.initialFromFile_mul("/Users/huangzhipeng/research/Yago/struct_new/Affliated_Marry.struct");
    meta_structure.showStruct();

    
    Meta_Paths metaPaths1(Yago_Graph), metaPaths2(Yago_Graph), metaPaths3(Yago_Graph);
    YagoReader::readMetaPath("/Users/huangzhipeng/research/Yago/path_new/affliated.metapath", metaPaths1.linkTypes_, metaPaths1.nodeTypes_, metaPaths1.weights_);
    //YagoReader::readMetaPath("/Users/huangzhipeng/research/Yago/path_new/born.metapath", metaPaths2.linkTypes_, metaPaths2.nodeTypes_, metaPaths2.weights_);
    YagoReader::readMetaPath("/Users/huangzhipeng/research/Yago/path_new/marry.metapath", metaPaths3.linkTypes_, metaPaths3.nodeTypes_, metaPaths3.weights_);


    //ofstream out1("/Users/huangzhipeng/research/Yago/disambiguation/result88/Affliated_Marry_SC.txt", ios::out);
    //ofstream out2("/Users/huangzhipeng/research/Yago/disambiguation/result88/Affliated_Marry_SCSE.txt", ios::out);
    /*
    ofstream outpc1("/Users/huangzhipeng/research/Yago/disambiguation/result88/affliated_PC.txt", ios::out);
     ofstream outpc2("/Users/huangzhipeng/research/Yago/disambiguation/result88/born_PC.txt", ios::out);
     ofstream outpc3("/Users/huangzhipeng/research/Yago/disambiguation/result88/marry_PC.txt", ios::out);
     ofstream outpcrw1("/Users/huangzhipeng/research/Yago/disambiguation/result88/affliated_PCRW.txt", ios::out);
     ofstream outpcrw2("/Users/huangzhipeng/research/Yago/disambiguation/result88/born_PCRW.txt", ios::out);
     ofstream outpcrw3("/Users/huangzhipeng/research/Yago/disambiguation/result88/marry_PCRW.txt", ios::out);
    
    ofstream outpathsim1("/Users/huangzhipeng/research/Yago/disambiguation/result88/affliated_PS.txt", ios::out);
    ofstream outpathsim2("/Users/huangzhipeng/research/Yago/disambiguation/result88/born_PS.txt", ios::out);
    ofstream outpathsim3("/Users/huangzhipeng/research/Yago/disambiguation/result88/marry_PS.txt", ios::out);
    */
    ofstream outpathsim1("/Users/huangzhipeng/research/Yago/disambiguation/result88/affliated_PS.txt", ios::out);
    /*
    ofstream out_2("/Users/huangzhipeng/research/Yago/disambiguation/result88/Affliated_Marry_SCSE_0.2.txt", ios::out);
    ofstream out_4("/Users/huangzhipeng/research/Yago/disambiguation/result88/Affliated_Marry_SCSE_0.4.txt", ios::out);
    ofstream out_6("/Users/huangzhipeng/research/Yago/disambiguation/result88/Affliated_Marry_SCSE_0.6.txt", ios::out);
    ofstream out_8("/Users/huangzhipeng/research/Yago/disambiguation/result88/Affliated_Marry_SCSE_0.8.txt", ios::out);
    */
    ifstream input("/Users/huangzhipeng/research/Yago/disambiguation/testData.txt", ios::in);
    int id;
    while(input >> id){
        //map<int, double> pc1, pc2, pc3, pcrw1, pcrw2, pcrw3, pathsim1, pathsim2, pathsim3;
        map<int, double> pathsim1;
        //map<vector<int>, double> rtn, rtn2;
        /*
        map<vector<int>, double> rtn_2, rtn_4, rtn_6, rtn_8;
        //SimCalculator::calTopK_BSCSE_BFS(id, 1000000, meta_structure, rtn, 0);
        SimCalculator::calTopK_BSCSE_BFS(id, 1000000, meta_structure, rtn_2, 0.2);
        SimCalculator::calTopK_BSCSE_BFS(id, 1000000, meta_structure, rtn_4, 0.4);
        SimCalculator::calTopK_BSCSE_BFS(id, 1000000, meta_structure, rtn_6, 0.6);
        SimCalculator::calTopK_BSCSE_BFS(id, 1000000, meta_structure, rtn_8, 0.8);
        */
         /*
        for(map<vector<int>, double>::iterator j = rtn.begin(); j != rtn.end(); ++j){
            if(id != j->first[0]){
                out1 << id << '\t' << j->first[0] << '\t' << j->second << endl;
            }
        }
         */
        /*
        for(map<vector<int>, double>::iterator j = rtn_2.begin(); j != rtn_2.end(); ++j){
            if(id != j->first[0]){
                out_2 << id << '\t' << j->first[0] << '\t' << j->second << endl;
            }
        }
        
        for(map<vector<int>, double>::iterator j = rtn_4.begin(); j != rtn_4.end(); ++j){
            if(id != j->first[0]){
                out_4 << id << '\t' << j->first[0] << '\t' << j->second << endl;
            }
        }
        for(map<vector<int>, double>::iterator j = rtn_6.begin(); j != rtn_6.end(); ++j){
            if(id != j->first[0]){
                out_6 << id << '\t' << j->first[0] << '\t' << j->second << endl;
            }
        }
        for(map<vector<int>, double>::iterator j = rtn_8.begin(); j != rtn_8.end(); ++j){
            if(id != j->first[0]){
                out_8 << id << '\t' << j->first[0] << '\t' << j->second << endl;
            }
        }
        */
        /*
        SimCalculator::calTopK_PathCount(id, 1000000, metaPaths1, pc1);
        SimCalculator::calTopK_PathCount(id, 1000000, metaPaths2, pc2);
        SimCalculator::calTopK_PathCount(id, 1000000, metaPaths3, pc3);
        
        SimCalculator::calTopK_PCRW(id, 1000000, metaPaths1, pcrw1);
        SimCalculator::calTopK_PCRW(id, 1000000, metaPaths2, pcrw2);
        SimCalculator::calTopK_PCRW(id, 1000000, metaPaths3, pcrw3);
        */
        
        SimCalculator::calTopK_PathSim(id, 1000000, metaPaths1, pathsim1);
         //SimCalculator::calTopK_PathSim(id, 1000000, metaPaths2, pathsim2);
        // SimCalculator::calTopK_PathSim(id, 1000000, metaPaths3, pathsim3);
        /*
        for(map<int,double>::iterator j = pc1.begin(); j != pc1.end(); ++j){
            if(id != j->first)
            outpc1 << id << '\t' << j->first << '\t' << j->second << endl;
        }
        
        for(map<int,double>::iterator j = pc2.begin(); j != pc2.end(); ++j){
            if(id != j->first)
            outpc2 << id << '\t' << j->first << '\t' << j->second << endl;
        }
         
        for(map<int,double>::iterator j = pc3.begin(); j != pc3.end(); ++j){
            if(id != j->first)
            outpc3 << id << '\t' << j->first << '\t' << j->second << endl;
        }
        
        for(map<int,double>::iterator j = pcrw1.begin(); j != pcrw1.end(); ++j){
            if(id != j->first)
            outpcrw1 << id << '\t' << j->first << '\t' << j->second << endl;
        }
        for(map<int,double>::iterator j = pcrw2.begin(); j != pcrw2.end(); ++j){
            if(id != j->first)
            outpcrw2 << id << '\t' << j->first << '\t' << j->second << endl;
        }
        for(map<int,double>::iterator j = pcrw3.begin(); j != pcrw3.end(); ++j){
            if(id != j->first)
            outpcrw3 << id << '\t' << j->first << '\t' << j->second << endl;
        }
         */
        
        for(map<int,double>::iterator j = pathsim1.begin(); j != pathsim1.end(); ++j){
            if(id != j->first)
                outpathsim1 << id << '\t' << j->first << '\t' << j->second << endl;
        }
        
        /*
        for(map<int,double>::iterator j = pathsim2.begin(); j != pathsim2.end(); ++j){
            if(id != j->first)
                outpathsim2 << id << '\t' << j->first << '\t' << j->second << endl;
        }
        
        for(map<int,double>::iterator j = pathsim3.begin(); j != pathsim3.end(); ++j){
            if(id != j->first)
                outpathsim3 << id << '\t' << j->first << '\t' << j->second << endl;
        }
        */
    }
    
    return;
}

void Exp_CaseStudy_P2P()
{
    map<int, vector<Edge>> adj;
    map<int,string> node_name;
    map<int,string> node_type_name;
    map<int,int> node_type_num;
    map<int,vector<int>> node_id_to_type;
    map<int,string> edge_name;
    
    YagoReader::readADJ("/Users/huangzhipeng/research/DBLP_four_area/dblpAdj.txt", adj);
    
    
    YagoReader::readNodeIdToType("/Users/huangzhipeng/research/DBLP_four_area/dblpTotalType.txt", node_id_to_type);
    YagoReader::readEdgeName("/Users/huangzhipeng/research/DBLP_four_area/dblpType.txt",edge_name);
    
    cerr <<"read dblp_four_area" << endl;
    
    HIN_Graph DBLP_Graph,DBLP_Graph9,DBLP_Graph8,DBLP_Graph7,DBLP_Graph6,DBLP_Graph5;
    DBLP_Graph.buildYAGOGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
    
    cerr << " change dblp_four_area" << endl;
    
    Meta_Structure meta_structure(DBLP_Graph);
    meta_structure.initialFromFile_mul("/Users/huangzhipeng/research/DBLP_four_area/struct_mul/P2P_NC_struct_mul_C.txt");
    meta_structure.showStruct();
    
    
    
    ifstream in("/Users/huangzhipeng/research/DBLP_four_area/name_id/paperN.txt", ios::in);
    string line;
    int c = 0;
    int count = 0;
    int total = 0;
    while(getline(in, line)){
        c++;
        if(c % 10 != 0)
            continue;
        map<vector<int>, double> rtn;
        int src;
        
        stringstream input(line);
        input >> src;
        SimCalculator::calTopK_BSCSE_BFS(src, 1000000, meta_structure, rtn, 0);
        count++;
        total += rtn.size();
    }
    double aver = (double)total / count;
    cout << aver << endl;
    
    
    return;
}

void Exp_CaseStudy()
{
    map<int, vector<Edge>> adj;
    map<int,string> node_name;
    map<int,string> node_type_name;
    map<int,int> node_type_num;
    map<int,vector<int>> node_id_to_type;
    map<int,string> edge_name;
    
    YagoReader::readADJ("/Users/huangzhipeng/research/DBLP_four_area/dblpAdj.txt", adj);
    
    
    YagoReader::readNodeIdToType("/Users/huangzhipeng/research/DBLP_four_area/dblpTotalType.txt", node_id_to_type);
    YagoReader::readEdgeName("/Users/huangzhipeng/research/DBLP_four_area/dblpType.txt",edge_name);
    
    cerr <<"read dblp_four_area" << endl;
    
    HIN_Graph DBLP_Graph,DBLP_Graph9,DBLP_Graph8,DBLP_Graph7,DBLP_Graph6,DBLP_Graph5;
    DBLP_Graph.buildYAGOGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
    
    cerr << " change dblp_four_area" << endl;
    
    Meta_Structure meta_structure(DBLP_Graph);
    meta_structure.initialFromFile_mul("/Users/huangzhipeng/research/DBLP_four_area/struct_mul/ACT_NC_struct_mul.txt");
    meta_structure.showStruct();
    
    
    Meta_Paths metaPaths1(DBLP_Graph);
    YagoReader::readMetaPath("/Users/huangzhipeng/research/DBLP_four_area/metapath/APC.metapath", metaPaths1.linkTypes_, metaPaths1.nodeTypes_, metaPaths1.weights_);
    
    Meta_Paths metaPaths2(DBLP_Graph);
    YagoReader::readMetaPath("/Users/huangzhipeng/research/DBLP_four_area/metapath/APT.metapath", metaPaths2.linkTypes_, metaPaths2.nodeTypes_, metaPaths2.weights_);
    
    int src = 1820;
    map<vector<int>, double> rtn_10, rtn_2000, rtn_scse;
    
    SimCalculator::calTopK_BSCSE_BFS(src, 10, meta_structure, rtn_10, 0);
    SimCalculator::calTopK_BSCSE_BFS(src, 2000, meta_structure, rtn_2000, 0);
    
    SimCalculator::calTopK_BSCSE_BFS(src, 10, meta_structure, rtn_scse, 1);
    vector<pair<double, vector<int>>> SCSE_V;
    for(map<vector<int>, double>::iterator i = rtn_scse.begin(); i != rtn_scse.end(); ++i){
        SCSE_V.push_back(make_pair(i->second, i->first));
    }
    for(int i = 1; i < SCSE_V.size(); ++i){
        int j = i - 1;
        double thesim = SCSE_V[i].first;
        vector<int> theV = SCSE_V[i].second;
        while(j >= 0 && SCSE_V[j].first < thesim){
            SCSE_V[j + 1] = SCSE_V[j];
            --j;
        }
        SCSE_V[j + 1] =make_pair(thesim, theV);
    }
    
    
    
    // print result
    cout << "SC" << endl;
    for( map<vector<int>, double>::iterator i = rtn_10.begin(); i != rtn_10.end(); ++i){
        cout << i->first[0] << '\t' << i->first[1] << '\t' << i->second << endl;
    }
    
    cout << "SCSE" << endl;
    for(int i = 0 ; i < 10 ; ++i){
        cout << SCSE_V[i].second[0] << '\t' << SCSE_V[i].second[1] << '\t' << rtn_2000[SCSE_V[i].second] << endl;
    }
    
    
    //////--------------- PC -------------------------
    map<int, double> result_pc1, result_pc2;
    
    SimCalculator::calTopK_PathCount(src, 10, metaPaths1, result_pc1);
    SimCalculator::calTopK_PathCount(src, 10, metaPaths2, result_pc2);
    
    vector<pair<double, vector<int>>> PC_V;
    for(map<int, double>::iterator i = result_pc1.begin(); i != result_pc1.end(); ++i){
        for(map<int, double>::iterator j = result_pc2.begin(); j != result_pc2.end(); ++j){
            vector<int> tuple;
            tuple.push_back(i->first);
            tuple.push_back(j->first);
            double sim = i->second + j->second;
            PC_V.push_back(make_pair(sim, tuple));
        }
    }
    for(int i = 1; i < PC_V.size(); ++i){
        int j = i - 1;
        double thesim = PC_V[i].first;
        vector<int> theV = PC_V[i].second;
        while(j >= 0 && PC_V[j].first < thesim){
            PC_V[j + 1] = PC_V[j];
            --j;
        }
        PC_V[j + 1] =make_pair(thesim, theV);
    }
    // print result
    cout << "PC:" << endl;
    for(int i = 0 ; i < 10 ; ++i){
        cout << PC_V[i].second[0] << '\t' << PC_V[i].second[1] << '\t' << rtn_2000[PC_V[i].second] << endl;
    }
    
    
    //////--------------- PCRW -------------------------
    map<int, double> result_pcrw1, result_pcrw2;
    SimCalculator::calTopK_PCRW(src, 10, metaPaths1, result_pcrw1);
    SimCalculator::calTopK_PCRW(src, 10, metaPaths2, result_pcrw2);
    
    vector<pair<double, vector<int>>> PCRW_V;
    for(map<int, double>::iterator i = result_pcrw1.begin(); i != result_pcrw1.end(); ++i){
        for(map<int, double>::iterator j = result_pcrw2.begin(); j != result_pcrw2.end(); ++j){
            vector<int> tuple;
            tuple.push_back(i->first);
            tuple.push_back(j->first);
            double sim = i->second + j->second;
            PCRW_V.push_back(make_pair(sim, tuple));
        }
    }
    for(int i = 1; i < PCRW_V.size(); ++i){
        int j = i - 1;
        double thesim = PCRW_V[i].first;
        vector<int> theV = PCRW_V[i].second;
        while(j >= 0 && PCRW_V[j].first < thesim){
            PCRW_V[j + 1] = PCRW_V[j];
            --j;
        }
        PCRW_V[j + 1] =make_pair(thesim, theV);
    }
    // print result
    cout << "PCRW:" << endl;
    for(int i = 0 ; i < 10 ; ++i){
        cout << PCRW_V[i].second[0] << '\t' << PCRW_V[i].second[1] << '\t' << rtn_2000[PCRW_V[i].second] << endl;
    }
    
    
    return;
}

void Exp_CaseStudy_Actor()
{
    map<int, vector<Edge>> adj;
    map<int,string> node_name;
    map<int,string> node_type_name;
    map<int,int> node_type_num;
    map<int,vector<int>> node_id_to_type;
    map<int,string> edge_name;
    
    YagoReader::readADJ("/Users/huangzhipeng/research/Yago_movies/newmovieAdj.txt", adj);
    YagoReader::readNodeName("/Users/huangzhipeng/research/Yago_movies/movieTax.txt",node_name,node_type_name);
    YagoReader::readNodeIdToType("/Users/huangzhipeng/research/Yago_movies/movieTotalType.txt", node_id_to_type);
    YagoReader::readEdgeName("/Users/huangzhipeng/research/Yago_movies/linkType.txt",edge_name);
    
    HIN_Graph movie_Graph;
    movie_Graph.buildYAGOGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
  
    
    Meta_Structure meta_structure(movie_Graph);
    meta_structure.initialFromFile_mul("/Users/huangzhipeng/research/Yago_movies/struct/ADP_struct.txt");
    meta_structure.showStruct();
    
    
    Meta_Paths metaPaths1(movie_Graph);
    YagoReader::readMetaPath("/Users/huangzhipeng/research/Yago_movies/metapath/ADP1.metapath", metaPaths1.linkTypes_, metaPaths1.nodeTypes_, metaPaths1.weights_);
    
    Meta_Paths metaPaths2(movie_Graph);
    YagoReader::readMetaPath("/Users/huangzhipeng/research/Yago_movies/metapath/ADP2.metapath", metaPaths2.linkTypes_, metaPaths2.nodeTypes_, metaPaths2.weights_);
    
    int src = 4156;
    map<vector<int>, double> rtn_10, rtn_2000, rtn_scse;
    
    SimCalculator::calTopK_BSCSE_BFS(src, 10, meta_structure, rtn_10, 0);
    SimCalculator::calTopK_BSCSE_BFS(src, 2000, meta_structure, rtn_2000, 0);
    
    SimCalculator::calTopK_BSCSE_BFS(src, 10, meta_structure, rtn_scse, 1);
    vector<pair<double, vector<int>>> SCSE_V;
    for(map<vector<int>, double>::iterator i = rtn_scse.begin(); i != rtn_scse.end(); ++i){
        SCSE_V.push_back(make_pair(i->second, i->first));
    }
    for(int i = 1; i < SCSE_V.size(); ++i){
        int j = i - 1;
        double thesim = SCSE_V[i].first;
        vector<int> theV = SCSE_V[i].second;
        while(j >= 0 && SCSE_V[j].first < thesim){
            SCSE_V[j + 1] = SCSE_V[j];
            --j;
        }
        SCSE_V[j + 1] =make_pair(thesim, theV);
    }
    
    
    
    // print result
    cout << "SC" << endl;
    for( map<vector<int>, double>::iterator i = rtn_10.begin(); i != rtn_10.end(); ++i){
        cout << node_name[i->first[0]] << '\t' << node_name[i->first[1]] << '\t' << i->second << endl;
    }
    
    cout << "SCSE" << endl;
    for(int i = 0 ; i < 10 && i < SCSE_V.size() ; ++i){
        cout << SCSE_V[i].second[0] << '\t' << SCSE_V[i].second[1] << '\t' << rtn_2000[SCSE_V[i].second] << endl;
    }
    
    
       //////--------------- PC -------------------------
    map<int, double> result_pc1, result_pc2;

    SimCalculator::calTopK_PathCount(src, 10, metaPaths1, result_pc1);
    SimCalculator::calTopK_PathCount(src, 10, metaPaths2, result_pc2);
    
    vector<pair<double, vector<int>>> PC_V;
    for(map<int, double>::iterator i = result_pc1.begin(); i != result_pc1.end(); ++i){
        for(map<int, double>::iterator j = result_pc2.begin(); j != result_pc2.end(); ++j){
            vector<int> tuple;
            tuple.push_back(i->first);
            tuple.push_back(j->first);
            double sim = i->second + j->second;
            PC_V.push_back(make_pair(sim, tuple));
        }
    }
    for(int i = 1; i < PC_V.size(); ++i){
        int j = i - 1;
        double thesim = PC_V[i].first;
        vector<int> theV = PC_V[i].second;
        while(j >= 0 && PC_V[j].first < thesim){
            PC_V[j + 1] = PC_V[j];
            --j;
        }
        PC_V[j + 1] =make_pair(thesim, theV);
    }
    // print result
    cout << "PC:" << endl;
    for(int i = 0 ; i < 10 ; ++i){
        cout << PC_V[i].second[0] << '\t' << PC_V[i].second[1] << '\t' << rtn_2000[PC_V[i].second] << endl;
    }
    
    
    //////--------------- PCRW -------------------------
    map<int, double> result_pcrw1, result_pcrw2;
    SimCalculator::calTopK_PCRW(src, 10, metaPaths1, result_pcrw1);
    SimCalculator::calTopK_PCRW(src, 10, metaPaths2, result_pcrw2);
    
    vector<pair<double, vector<int>>> PCRW_V;
    for(map<int, double>::iterator i = result_pcrw1.begin(); i != result_pcrw1.end(); ++i){
        for(map<int, double>::iterator j = result_pcrw2.begin(); j != result_pcrw2.end(); ++j){
            vector<int> tuple;
            tuple.push_back(i->first);
            tuple.push_back(j->first);
            double sim = i->second + j->second;
            PCRW_V.push_back(make_pair(sim, tuple));
        }
    }
    for(int i = 1; i < PCRW_V.size(); ++i){
        int j = i - 1;
        double thesim = PCRW_V[i].first;
        vector<int> theV = PCRW_V[i].second;
        while(j >= 0 && PCRW_V[j].first < thesim){
            PCRW_V[j + 1] = PCRW_V[j];
            --j;
        }
        PCRW_V[j + 1] =make_pair(thesim, theV);
    }
    // print result
    cout << "PCRW:" << endl;
    for(int i = 0 ; i < 10 ; ++i){
        cout << PCRW_V[i].second[0] << '\t' << PCRW_V[i].second[1] << '\t' << rtn_2000[PCRW_V[i].second] << endl;
    }

    
    return;
}

void Exp_Runtime()
{
    
        clock_t t;
    
    map<int, vector<Edge>> adj,adj_9, adj_8,adj_7,adj_6,adj_5;
    map<int,string> node_name;
    map<int,string> node_type_name;
    map<int,int> node_type_num;
    map<int,vector<int>> node_id_to_type;
    map<int,string> edge_name;
    
    YagoReader::readADJ("/Users/huangzhipeng/research/DBLP_four_area/dblpAdj.txt", adj);
    
    YagoReader::readADJ("/Users/huangzhipeng/research/DBLP_four_area/SomeLinks/dblpAdj_0.9.txt", adj_9);
    YagoReader::readADJ("/Users/huangzhipeng/research/DBLP_four_area/SomeLinks/dblpAdj_0.8.txt", adj_8);
    YagoReader::readADJ("/Users/huangzhipeng/research/DBLP_four_area/SomeLinks/dblpAdj_0.7.txt", adj_7);
    YagoReader::readADJ("/Users/huangzhipeng/research/DBLP_four_area/SomeLinks/dblpAdj_0.6.txt", adj_6);
    YagoReader::readADJ("/Users/huangzhipeng/research/DBLP_four_area/SomeLinks/dblpAdj_0.5.txt", adj_5);
    
    
    YagoReader::readNodeIdToType("/Users/huangzhipeng/research/DBLP_four_area/dblpTotalType.txt", node_id_to_type);
    YagoReader::readEdgeName("/Users/huangzhipeng/research/DBLP_four_area/dblpType.txt",edge_name);
    
    cerr <<"read dblp_four_area" << endl;
    
    HIN_Graph DBLP_Graph,DBLP_Graph9,DBLP_Graph8,DBLP_Graph7,DBLP_Graph6,DBLP_Graph5;
    DBLP_Graph.buildYAGOGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
   
    DBLP_Graph9.buildYAGOGraph(node_name, adj_9, node_type_name, node_type_num, node_id_to_type, edge_name);
    DBLP_Graph8.buildYAGOGraph(node_name, adj_8, node_type_name, node_type_num, node_id_to_type, edge_name);
    DBLP_Graph7.buildYAGOGraph(node_name, adj_7, node_type_name, node_type_num, node_id_to_type, edge_name);
    DBLP_Graph6.buildYAGOGraph(node_name, adj_6, node_type_name, node_type_num, node_id_to_type, edge_name);
    DBLP_Graph5.buildYAGOGraph(node_name, adj_5, node_type_name, node_type_num, node_id_to_type, edge_name);

    cerr << " change dblp_four_area" << endl;
    
    Meta_Structure meta_structure(DBLP_Graph);
    Meta_Structure meta_structure9(DBLP_Graph9);
    Meta_Structure meta_structure8(DBLP_Graph8);
    Meta_Structure meta_structure7(DBLP_Graph7);
    Meta_Structure meta_structure6(DBLP_Graph6);
    Meta_Structure meta_structure5(DBLP_Graph5);
    
    meta_structure.initialFromFile_mul("/Users/huangzhipeng/research/DBLP_four_area/struct_mul/C2C_NC_struct_mul.txt");

    meta_structure9.initialFromFile_mul("/Users/huangzhipeng/research/DBLP_four_area/struct_mul/C2C_NC_struct_mul.txt");
    meta_structure8.initialFromFile_mul("/Users/huangzhipeng/research/DBLP_four_area/struct_mul/C2C_NC_struct_mul.txt");
    meta_structure7.initialFromFile_mul("/Users/huangzhipeng/research/DBLP_four_area/struct_mul/C2C_NC_struct_mul.txt");
    meta_structure6.initialFromFile_mul("/Users/huangzhipeng/research/DBLP_four_area/struct_mul/C2C_NC_struct_mul.txt");
    meta_structure5.initialFromFile_mul("/Users/huangzhipeng/research/DBLP_four_area/struct_mul/C2C_NC_struct_mul.txt");
    

    meta_structure.showStruct();

    

    
    
     t = clock();
    map<int, int> src;
    map<vector<int>, double> rtn;
    SimCalculator::calSimForAllEntities_BFS("/Users/huangzhipeng/research/DBLP_four_area/name_id/confN.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/NoUse.txt", meta_structure);
    cout << "CDFS\t" << (double)(clock() - t) / CLOCKS_PER_SEC << endl;
    
    t = clock();
    SimCalculator::calSimForAllEntities_BFS("/Users/huangzhipeng/research/DBLP_four_area/name_id/confN.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/NoUse.txt", meta_structure9);
    cout << "CDFS\t" << (double)(clock() - t) / CLOCKS_PER_SEC << endl;
    
    t = clock();
    SimCalculator::calSimForAllEntities_BFS("/Users/huangzhipeng/research/DBLP_four_area/name_id/confN.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/NoUse.txt", meta_structure8);
    cout << "CDFS\t" << (double)(clock() - t) / CLOCKS_PER_SEC << endl;
    
    t = clock();
    SimCalculator::calSimForAllEntities_BFS("/Users/huangzhipeng/research/DBLP_four_area/name_id/confN.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/NoUse.txt", meta_structure7);
    cout << "CDFS\t" << (double)(clock() - t) / CLOCKS_PER_SEC << endl;
    
    t = clock();
    SimCalculator::calSimForAllEntities_BFS("/Users/huangzhipeng/research/DBLP_four_area/name_id/confN.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/NoUse.txt", meta_structure6);
    cout << "CDFS\t" << (double)(clock() - t) / CLOCKS_PER_SEC << endl;
    
    t = clock();
    SimCalculator::calSimForAllEntities_BFS("/Users/huangzhipeng/research/DBLP_four_area/name_id/confN.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/NoUse.txt", meta_structure5);
    cout << "CDFS\t" << (double)(clock() - t) / CLOCKS_PER_SEC << endl;
    
    /*
    t = clock();
    SimCalculator::calSimForAllEntities_StructCount_NC("/Users/huangzhipeng/research/DBLP_four_area/author100.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/SCRW_NC_mul.txt", meta_structure);
    cout << "BSCSE\t" << (double)(clock() - t) / CLOCKS_PER_SEC << endl;
    */
    /*
    Meta_Paths metaPaths1(DBLP_Graph);
    YagoReader::readMetaPath("/Users/huangzhipeng/research/DBLP_four_area/metapath/C2C1.metapath", metaPaths1.linkTypes_, metaPaths1.nodeTypes_, metaPaths1.weights_);
    
    Meta_Paths metaPaths2(DBLP_Graph);
    YagoReader::readMetaPath("/Users/huangzhipeng/research/DBLP_four_area/metapath/C2C2.metapath", metaPaths2.linkTypes_, metaPaths2.nodeTypes_, metaPaths2.weights_);
    
    Meta_Paths metaPaths(DBLP_Graph);
    YagoReader::readMetaPath("/Users/huangzhipeng/research/DBLP_four_area/metapath/C2C.metapath", metaPaths.linkTypes_, metaPaths.nodeTypes_, metaPaths.weights_);
*/
    
    
    /*
    Meta_Paths metaPaths1(DBLP_Graph);
    YagoReader::readMetaPath("/Users/huangzhipeng/research/DBLP_four_area/metapath/A2A1.metapath", metaPaths1.linkTypes_, metaPaths1.nodeTypes_, metaPaths1.weights_);
    
    Meta_Paths metaPaths2(DBLP_Graph);
    YagoReader::readMetaPath("/Users/huangzhipeng/research/DBLP_four_area/metapath/A2A2.metapath", metaPaths2.linkTypes_, metaPaths2.nodeTypes_, metaPaths2.weights_);
    
    Meta_Paths metaPaths(DBLP_Graph);
    YagoReader::readMetaPath("/Users/huangzhipeng/research/DBLP_four_area/metapath/A2A.metapath", metaPaths.linkTypes_, metaPaths.nodeTypes_, metaPaths.weights_);

    */
    //SimCalculator::calSimForAllEntities_SCRW_NC_mul("/Users/huangzhipeng/research/DBLP_four_area/name_id/confN.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/SCRW_NC_mul.txt", meta_structure);

    /*
    t = clock();
    SimCalculator::calSimForAllEntities_FSPG("/Users/huangzhipeng/research/DBLP_four_area/author100.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/NoUse.txt", metaPaths1);
    cout << "PCRW1\t" << (double)(clock() - t) / CLOCKS_PER_SEC << endl;
    
    t = clock();
    SimCalculator::calSimForAllEntities_FSPG("/Users/huangzhipeng/research/DBLP_four_area/author100.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/NoUse.txt", metaPaths2);
    cout << "PCRW2\t" << (double)(clock() - t) / CLOCKS_PER_SEC << endl;
    
    t = clock();
    SimCalculator::calSimForAllEntities_FSPG("/Users/huangzhipeng/research/DBLP_four_area/author100.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/NoUse.txt", metaPaths);
    cout << "PCRW\t" << (double)(clock() - t) / CLOCKS_PER_SEC << endl;
    
    
    t = clock();
    SimCalculator::calSimForAllEntities_PathSim("/Users/huangzhipeng/research/DBLP_four_area/author100.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/NoUse.txt", metaPaths1);
    cout << "PS1\t" << (double)(clock() - t) / CLOCKS_PER_SEC << endl;
    
    t = clock();
    SimCalculator::calSimForAllEntities_PathSim("/Users/huangzhipeng/research/DBLP_four_area/author100.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/NoUse.txt", metaPaths2);
    cout << "PS2\t" << (double)(clock() - t) / CLOCKS_PER_SEC << endl;
    
    t = clock();
    SimCalculator::calSimForAllEntities_PathSim("/Users/huangzhipeng/research/DBLP_four_area/author100.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/NoUse.txt", metaPaths);
    cout << "PS\t" << (double)(clock() - t) / CLOCKS_PER_SEC << endl;
    */
    
    
    
    
}

void Exp_LinkPrediction()
{
    map<int, vector<Edge>> adj;
    map<int,string> node_name;
    map<int,string> node_type_name;
    map<int,int> node_type_num;
    map<int,vector<int>> node_id_to_type;
    map<int,string> edge_name;
    
    YagoReader::readADJ("/Users/huangzhipeng/research/DBLP_four_area/dblpAdj.txt", adj);
    YagoReader::readNodeIdToType("/Users/huangzhipeng/research/DBLP_four_area/dblpTotalType.txt", node_id_to_type);
    YagoReader::readEdgeName("/Users/huangzhipeng/research/DBLP_four_area/dblpType.txt",edge_name);

    cerr <<"read dblp_four_area" << endl;
    
    HIN_Graph DBLP_Graph;
    DBLP_Graph.buildYAGOGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
    DBLP_Graph.removeLink("/Users/huangzhipeng/research/DBLP_four_area/publishInSample.txt");
    
    cerr << " change dblp_four_area" << endl;
    
    Meta_Structure meta_structure(DBLP_Graph);
    meta_structure.initialFromFile_mul("/Users/huangzhipeng/research/DBLP_four_area/struct_mul/P2C_NC_struct_mul.txt");
    meta_structure.showStruct();
    
    Meta_Paths metaPaths(DBLP_Graph);
    YagoReader::readMetaPath("/Users/huangzhipeng/research/DBLP_four_area/metapath/P2C.metapath", metaPaths.linkTypes_, metaPaths.nodeTypes_, metaPaths.weights_);

    
   // ifstream in("/Users/huangzhipeng/research/DBLP_four_area/isAuthorSample.txt", ios::in);
    ifstream in("/Users/huangzhipeng/research/DBLP_four_area/publishInSample.txt", ios::in);
    vector<int> K = {1,2,4,8,16};
    int src, dst, type;
    vector<int> rank_total;
    vector<int> hits;
    
    for(int i = 0; i < K.size(); ++i){
        rank_total.push_back(0);
        hits.push_back(0);
    }
    
    int count = 0;
    int zcount = 0;
    while(in >> src >> dst >> type){
        count ++;
        vector<int> ranks;
        
        //SimCalculator::calTopK_PCRW_1(src, 10000, metaPaths, ranks);
        //SimCalculator::calTopK_PathCount_1(dst, 10000, metaPaths, ranks);
        SimCalculator::calTopK_BSCSE_DFS_1(src, 10000, meta_structure, ranks, 1);
        //SimCalculator::calTopK_StructCount(dst, 10000, meta_structure, ranks);
        //if(find(ranks.begin(), ranks.end(), src) != ranks.end())
            //cout << "found!" << endl;
        /*
        int k = 0;
        while(k < ranks.size()){
            bool found = false;
            for(int j = 0; j < DBLP_Graph.edges_dst_[dst].size(); ++j){
                if(DBLP_Graph.edges_dst_[dst][j].src_ == ranks[k]){
                    found = true;
                    break;
                }
            }
            if(found){
                ranks.erase(ranks.begin() + k);
            }else{
                k++;
            }
        }
        */
        if(ranks.size() == 0)
            zcount ++;
        for(int i = 0; i < K.size(); ++i){
            int rank_i;
            vector<int>::iterator iter = find(ranks.begin(), ranks.end(), dst);
            if(iter != ranks.end() && iter - ranks.begin() <= K[i]){
                rank_i = iter - ranks.begin();
                hits[i]++;
            }else{
                rank_i = K[i] + 1;
            }
            rank_total[i] += rank_i;
            //cerr << "rank_i " << rank_i << endl;
        }
    }
    for(int i = 0; i < rank_total.size(); ++i){
        double averRank = (double)rank_total[i] / count;
        double hitR = (double)hits[i] / count;
        cout << K[i] << '\t' << averRank << '\t' << hitR << '\t' <<  hits[i] << endl;
    }
    cout << "zeroCount = " << zcount;
}


int main(int argc, const char * argv[]) {
    //-----------------------------------------------------------
    // DBLPPART
    //
    //
    //-----------------------------------------------------------
    if(DATA == 0){
//        clock_t t1, t2, t3, t4;
//        t1 = clock();
//        DBLP_Reader reader(DBLP_PATH);
//        reader.read(articles_);
//        t2 = clock();
//        cerr << "Take " << (t2 - t1 + 0.0) / CLOCKS_PER_SEC << "s to load DBLP" << endl;
//        
//        
//        // BUGS !!!!!!
//        //reader.writeToFile("/Users/huangzhipeng/research/DBLP/sample.file", articles_);
//        //reader.ReadFromFile("/Users/huangzhipeng/research/DBLP/sample.file", articles_);
//        
//        HIN_Graph DBLP_Graph;
//        DBLP_Graph.buildDBLPGraph(articles_);
//        
//        Meta_Structure meta_structure(DBLP_Graph);
//        meta_structure.initialFromFile("/Users/huangzhipeng/research/DBLP/venue2venue.struct");
//        meta_structure.showStruct();
//        cerr << "Take " << (t3 - t2 + 0.0) / CLOCKS_PER_SEC << "s to build graph" << endl;
//        t3 = clock();
//        
//        reader.writeConfToFile("/Users/huangzhipeng/research/DBLP/allConf.txt", articles_);
        
        /*
        for(int i = 0; i < 10; ++i){
            int author_id = DBLP_Graph.author_ids_[i];
            cerr << "The SCount is: " << SimCalculator::calSim_SCount(author_id, author_id, meta_structure) << endl;
            cerr << "The SSim is: " << SimCalculator::calSim_SSim(author_id, author_id, meta_structure) << endl;
            cerr << "the num of paper is " << DBLP_Graph.edges_src_[author_id].size() << endl;
        }
        t4 = clock();
        cerr << "Take " << (t4 - t3 + 0.0) / CLOCKS_PER_SEC << "s to cal" << endl;
        cerr << "end" << endl;
         */
        
        
        
    }
    // --------------------------------------------------------------
    // YAGO part
    //
    //
    // --------------------------------------------------------------
    else if(DATA == 1){
        
        //SimCalculator::clustering("/Users/huangzhipeng/research/Yago/clustering/5U_250S.txt", "/Users/huangzhipeng/research/Yago/clustering/5U_sim_SCRW.txt");
        //SimCalculator::clustering("/Users/huangzhipeng/research/Yago/clustering/5U_250S.txt", "/Users/huangzhipeng/research/Yago/clustering/5U_sim_FSPG.txt");
        
        clock_t t1, t2, t3, t4, t5, t6;
        
        // ------------- BEGIN loading Yago ------------
        t1 = clock();
        map<int, vector<Edge>> adj;
        map<int,string> node_name;
        map<int,string> node_type_name;
        map<int,int> node_type_num;
        map<int,vector<int>> node_id_to_type;
        map<int,string> edge_name;
        
        YagoReader::readADJ("/Users/huangzhipeng/research/Yago/yagoadj.txt", adj);
        YagoReader::readNodeName("/Users/huangzhipeng/XcodeWorkPlace/Join/Data/yagoTaxID.txt",node_name,node_type_name);
        YagoReader::readNodeTypeNum("/Users/huangzhipeng/XcodeWorkPlace/Join/Data/yagoTypeNum.txt", node_type_num);
        YagoReader::readNodeIdToType("/Users/huangzhipeng/XcodeWorkPlace/Join/Data/totalType.txt", node_id_to_type);
        YagoReader::readEdgeName("/Users/huangzhipeng/XcodeWorkPlace/Join/Data/yagoType.txt",edge_name);
        t2 = clock();
        
        cerr << "Take " << (0.0 + t2 - t1)/CLOCKS_PER_SEC << "s to read Yago" << endl;
        
        HIN_Graph Yago_Graph;
        Yago_Graph.buildYAGOGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
        t3 = clock();
        cerr << "Take " << (0.0 + t3 - t2)/CLOCKS_PER_SEC << "s to change Yago" << endl;
        
        
        
        // meta_structure part
        
        Meta_Structure meta_structure(Yago_Graph);
        meta_structure.initialFromFile_mul("/Users/huangzhipeng/research/Yago/struct_new/actorCase/(AD)A_new");
        meta_structure.showStruct();
        
        map<vector<int>, double> rtn;
        //map<int, double> id_sim;
        
        SimCalculator::calTopK_BSCSE_BFS(107821, 5, meta_structure, rtn, 1.0);
        for(map<vector<int>, double>::iterator i = rtn.begin(); i != rtn.end(); ++i){
            cout << node_name[i->first[0]] << '\t' << i->second << endl;
        }
        
        /*
        SimCalculator::calSim_SCount_NC(669876, 669876, meta_structure, id_count);
        
        for(map<int, int>::iterator i = id_count.begin(); i != id_count.end(); ++i){
            cout << i->first <<'\t' << Yago_Graph.nodes_[i->first].key_ << '\t' << i->second << endl;
        }
         */
        /*
        SimCalculator::calSim_SSim_NC_all(669876, 669876, meta_structure, id_sim);
        for(map<int, double>::iterator i = id_sim.begin(); i != id_sim.end(); ++i){
            cout << i->first <<'\t' << Yago_Graph.nodes_[i->first].key_ << '\t' << i->second << endl;
        }
        */
      /*
        Meta_Paths metaPaths(Yago_Graph);
        YagoReader::readMetaPath("/Users/huangzhipeng/research/Yago/metapath/Award_University.metapath", metaPaths.linkTypes_, metaPaths.nodeTypes_, metaPaths.weights_);
     
        cout << "PathCount" << endl;
       */
        /*
        map<int, double> id_sim;
        SimCalculator::calSim_PC(669876, 669876, metaPaths, id_sim);
        for(map<int, double>::iterator i = id_sim.begin(); i != id_sim.end(); ++i){
            if(i->second > 1){
                cout << i->first <<'\t' << Yago_Graph.nodes_[i->first].key_ << '\t' << i->second << endl;
            }
        }
         */
      
    }
    //---------------------- DBLP_four_area ------------------------------------
    else if(DATA == 2){
        //SimCalculator::clustering_Kmeans("/Users/huangzhipeng/research/DBLP_four_area/author_label_new.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Author/sim/SCSE.txt");
        //return 0;
        
        clock_t t1, t2, t3, t4, t5, t6;
        
        map<int, vector<Edge>> adj;
        map<int,string> node_name;
        map<int,string> node_type_name;
        map<int,int> node_type_num;
        map<int,vector<int>> node_id_to_type;
        map<int,string> edge_name;
        
        t1 = clock();
        YagoReader::readADJ("/Users/huangzhipeng/research/DBLP_four_area/dblpAdj.txt", adj);
        YagoReader::readNodeIdToType("/Users/huangzhipeng/research/DBLP_four_area/dblpTotalType.txt", node_id_to_type);
        YagoReader::readEdgeName("/Users/huangzhipeng/research/DBLP_four_area/dblpType.txt",edge_name);
        t2 = clock();
        
        cerr << "Take " << (0.0 + t2 - t1)/CLOCKS_PER_SEC << "s to read dblp_four_area" << endl;
        
        HIN_Graph DBLP_Graph;
        DBLP_Graph.buildYAGOGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
        t3 = clock();
        cerr << "Take " << (0.0 + t3 - t2)/CLOCKS_PER_SEC << "s to change dblp_four_area" << endl;
        
        Meta_Structure meta_structure(DBLP_Graph);
        meta_structure.initialFromFile_mul("/Users/huangzhipeng/research/DBLP_four_area/struct_mul/A2A_NC_struct_mul.txt");
        meta_structure.showStruct();
        
        
        //SimCalculator::clustering_Kmeans("/Users/huangzhipeng/research/DBLP_four_area/name_id/confN.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/SCSE_0.8.txt");
        /*
        t1 = clock();
        SimCalculator::calSimForAllEntities_SCRW_NC_mul("/Users/huangzhipeng/research/DBLP_four_area/author_label_new.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Author/sim/SC.txt", meta_structure);
        t2 = clock();
        
        cerr << "Take " << (0.0 + t2 - t1)/CLOCKS_PER_SEC << "s to cal_sim" << endl;
        return 0;
        */
         //meta_structure.readIndex("/Users/huangzhipeng/research/DBLP_four_area/struct_mul/A2A_index.txt");
        
        
        map<vector<int>, double> id_sim;
        /*
        t1 = clock();
        SimCalculator::buildIndex("/Users/huangzhipeng/research/DBLP_four_area/struct_mul/A2A_index_1.txt", meta_structure);
        t2 = clock();
        cout << (double)(t2 - t1)/CLOCKS_PER_SEC << endl;
        
        return 0;
        */
        //meta_structure.readIndex("/Users/huangzhipeng/research/DBLP_four_area/struct_mul/A2A_index_1.txt");
        /*
        t1 = clock();
        SimCalculator::calTopK_BSCSE_BFS(1623, 10, meta_structure, id_sim, 0);
        t2 = clock();
        cout << (double)(t2 - t1)/CLOCKS_PER_SEC << endl;

        */
        /*
        map<vector<int>, double> rttn;
        
        t1 = clock();
        SimCalculator::calTopK_BSCSE_BFS(1623, 10, meta_structure, rttn, 0);
        for(map<vector<int>, double>::iterator i = rttn.begin(); i != rttn.end(); ++i){
            cout << i->first[0] << '\t' << i->second << endl;
        }
        t2 = clock();
        cout << (double)(t2 - t1)/CLOCKS_PER_SEC << endl;

        
        //SimCalculator::calTopK_BSCSE_BFS(1623, 1000, meta_structure, id_sim, 0);
        
        return 0;
         */
        /*
        id_sim.clear();
        SimCalculator::calTopK_BSCSE_DFS(1623, 5, meta_structure, id_sim, 1);
        for(map<vector<int>, double>::iterator i = id_sim.begin(); i != id_sim.end(); ++i){
            cout << i->first[0] << '\t' << i->first[1] << '\t' << i->second << endl;
        }
         
        cout << endl;
        t3 = clock();
        cout << t2 - t1 << '\t' << t3 - t2 << endl;
        */
        //SimCalculator::calSimForAllEntities_SCRW_NC_mul("/Users/huangzhipeng/research/DBLP_four_area/name_id/confN.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/SCRW_NC_mul.txt", meta_structure);
        double SCRW_time = (double)(clock() - t1)/CLOCKS_PER_SEC;
        
        /*
        t1 = clock();
        SimCalculator::calSimForAllEntities_StructCount_NC("/Users/huangzhipeng/research/DBLP_four_area/name_id/confN.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/SC_NC.txt", meta_structure);
        double SC_time = (double)(clock() - t1)/CLOCKS_PER_SEC;
        
        t1 = clock();
        SimCalculator::calSimForAllEntities_SSim("/Users/huangzhipeng/research/DBLP_four_area/name_id/confN.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/SSim_NC.txt", meta_structure);
        double SSim_time = (double)(clock() - t1)/CLOCKS_PER_SEC;
        
        map<int, int> id_count;
*/
        Meta_Paths metaPaths(DBLP_Graph);
        YagoReader::readMetaPath("/Users/huangzhipeng/research/DBLP_four_area/metapath/APT.metapath", metaPaths.linkTypes_, metaPaths.nodeTypes_, metaPaths.weights_);
 
        t1 = clock();
        //SimCalculator::calSimForAllEntities_PathSim("/Users/huangzhipeng/research/DBLP_four_area/author_label_new.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Author/sim/PathSim1.txt", metaPaths);
        //SimCalculator::calSimForAllEntities_FSPG("/Users/huangzhipeng/research/DBLP_four_area/author_label_new.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Author/sim/PCRW1.txt", metaPaths);
        map<int, double> rtn;
        //SimCalculator::calTopK_PCRW(1623, 5, metaPaths, rtn);
       
        Meta_Paths metaPaths2(DBLP_Graph);
        YagoReader::readMetaPath("/Users/huangzhipeng/research/DBLP_four_area/metapath/A2A1.metapath", metaPaths2.linkTypes_, metaPaths2.nodeTypes_, metaPaths2.weights_);
        SimCalculator::calTopK_PCRW(1623, 10, metaPaths2, rtn);
        rtn.clear();
        SimCalculator::calTopK_PathCount(1623, 10, metaPaths2, rtn);
        /*
        for(map<int,double>::iterator i = rtn.begin(); i != rtn.end(); ++i){
            cout << i->first << '\t' << i->second<< endl;
        }
         */
        return 0;
        
        t1 = clock();
        //SimCalculator::calSimForAllEntities_PathSim("/Users/huangzhipeng/research/DBLP_four_area/author_label_new.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Author/sim/PathSim2.txt", metaPaths);
        //SimCalculator::calSimForAllEntities_FSPG("/Users/huangzhipeng/research/DBLP_four_area/author_label_new.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Author/sim/PCRW2.txt", metaPaths);
        
        
        /*
        t1 = clock();
        SimCalculator::calSimForAllEntities_PathSim("/Users/huangzhipeng/research/DBLP_four_area/name_id/confN.txt", "/Users/huangzhipeng/research/DBLP_four_area/Case_Conference/PathSim.txt", metaPaths);
        double PathSim_time = (double)(clock() - t1) / CLOCKS_PER_SEC;
        */
        /*
        cout << "PC:\t" << PC_time << endl;
        cout << "SC:\t" << SC_time << endl;
        cout << "PathSim:\t" << PathSim_time << endl;
        cout << "SSim:\t" << SSim_time << endl;
        cout << "SCRW:\t" << SCRW_time << endl;
*/
      //  cout << "SCRW:\t" << SCRW_time << endl;
    

    }
    
    // -------------------------- MOVIE --------------------------------
    else if(DATA == 3){
        clock_t t1, t2, t3, t4, t5, t6;
        
        map<int, vector<Edge>> adj;
        map<int,string> node_name;
        map<int,string> node_type_name;
        map<int,int> node_type_num;
        map<int,vector<int>> node_id_to_type;
        map<int,string> edge_name;
        
        t1 = clock();
        YagoReader::readADJ("/Users/huangzhipeng/research/Yago_movies/newmovieAdj.txt", adj);
        YagoReader::readNodeName("/Users/huangzhipeng/research/Yago_movies/movieTax.txt",node_name,node_type_name);
        YagoReader::readNodeIdToType("/Users/huangzhipeng/research/Yago_movies/movieTotalType.txt", node_id_to_type);
        YagoReader::readEdgeName("/Users/huangzhipeng/research/Yago_movies/linkType.txt",edge_name);
        t2 = clock();
        cerr << "Take " << (0.0 + t2 - t1)/CLOCKS_PER_SEC << "s to read movieData" << endl;
        
        HIN_Graph movie_Graph;
        movie_Graph.buildYAGOGraph(node_name, adj, node_type_name, node_type_num, node_id_to_type, edge_name);
        t3 = clock();
        cerr << "Take " << (0.0 + t3 - t2)/CLOCKS_PER_SEC << "s to change movieData" << endl;
        
        Meta_Structure meta_structure(movie_Graph);
        meta_structure.initialFromFile_mul("/Users/huangzhipeng/research/Yago_movies/struct/M2M_struct.txt");
        meta_structure.showStruct();
        
        map<vector<int>, double> id_sim;
        
        id_sim.clear();
        
        /*
        SimCalculator::calTopK_BSCSE_DFS(1391, 10, meta_structure, id_sim, 0);
        for(map<vector<int>, double>::iterator i = id_sim.begin(); i != id_sim.end(); ++i){
            cout << node_name[i->first[0]] << '\t' << node_name[i->first[1]] << '\t' << i->second << endl;
        }
        return 0;
        */
        /*
        t1 = clock();
        SimCalculator::calSimForAllEntities_SCRW_NC_mul("/Users/huangzhipeng/research/Yago_movies/movies.txt", "/Users/huangzhipeng/research/Yago_movies/M2M/nouse.txt", meta_structure);
        cout << "BSCSE\t" << (double)(clock() - t1) / CLOCKS_PER_SEC << endl;
        return 0;
         */
        map<int, int> id_count;
        
        Meta_Paths metaPaths(movie_Graph);
        YagoReader::readMetaPath("/Users/huangzhipeng/research/Yago_movies/metapath/M2M1.metapath", metaPaths.linkTypes_, metaPaths.nodeTypes_, metaPaths.weights_);
        
        t1 = clock();
        SimCalculator::calSimForAllEntities_PathSim("/Users/huangzhipeng/research/Yago_movies/movies.txt", "/Users/huangzhipeng/research/Yago_movies/M2M/nouse.txt", metaPaths);
        cout << "BSCSE\t" << (double)(clock() - t1) / CLOCKS_PER_SEC << endl;

        
        map<int, double> rtn;
        //SimCalculator::calTopK_PCRW(1391, 10, metaPaths, rtn);
        //SimCalculator::calTopK_PathCount(1391, 10, metaPaths, rtn);
        //SimCalculator::calTopK_PathSim(4609, 10, metaPaths, rtn);
        /*
        for(map<int, double>::iterator i = rtn.begin(); i != rtn.end(); ++i){
            cout << i->first << '\t' << i->second << endl;
        }*/
        
        // ------------------ check M2M ----------
        ifstream in("/Users/huangzhipeng/research/Yago_movies/topMovies.txt", ios::in);
        string line;
        int k = 5;
        int total_PC = 0;
        int total_PS = 0;
        int wrong_PC = 0;
        int wrong_PS = 0;
        int total_PCRW = 0;
        int wrong_PCRW = 0;
        while (getline(in, line)) {
            int src;
            stringstream input(line);
            input >> src;
            cout << src << endl;
            vector<int> PCs, SCs, PSs, SSs, PCRWs;
            //SimCalculator::calTopK_PathCount(src, k, metaPaths, PCs);
            //SimCalculator::calTopK_PathSim(src, k, metaPaths, PSs);
            //SimCalculator::calTopK_PCRW(src, k, metaPaths, PCRWs);

            SimCalculator::calTopK_StructCount(src, k, meta_structure, SCs);
            SimCalculator::calTopK_StructSim(src, k, meta_structure, SSs);
            
            cout << "SC and SS " << SCs.size() << '\t' << SSs.size() << endl;
            int count = 0;
            for(int i = 0; i < PCs.size(); ++i){
                if(find(SCs.begin(), SCs.end(), PCs[i]) != SCs.end())
                    count++;
            }
            cout << "PC all " << PCs.size() << " correct: " << count << endl;
            total_PC += PCs.size();
            wrong_PC += PCs.size() - count;
            
            count = 0;
            for(int i = 0; i < PSs.size(); ++i){
                if(find(SCs.begin(), SCs.end(), PSs[i]) != SCs.end())
                    count++;
            }
            cout << "PS all " << PSs.size() << " correct: " << count << endl;
            
            total_PS += PSs.size();
            wrong_PS += PSs.size() - count;
            
            count = 0;
            for(int i = 0; i < PCRWs.size(); ++i){
                if(find(SCs.begin(), SCs.end(), PCRWs[i]) != SCs.end())
                    count++;
            }
            cout << "PCRW all " << PCRWs.size() << " correct: " << count << endl;
            total_PCRW += PCRWs.size();
            wrong_PCRW += PCRWs.size() - count;
            cout << endl;

            
        }
        cout << "PC's FPR: " << (double)wrong_PC / total_PC << endl;
        cout << "PS's FPR: " << (double)wrong_PS / total_PS << endl;
        cout << "PCRW's FPR: " << (double)wrong_PCRW / total_PCRW << endl;
    }
    // ------------- Link Prediction--------------------------
    else if(DATA == 4){
        Exp_LinkPrediction();
    }
    // ------------- Efficiency--------------------------
    else if(DATA == 5){
        Exp_Runtime();
    }
    // -------------Case study (ACT)------------------
    else if(DATA == 6){
        Exp_CaseStudy();
    }else if(DATA == 7){
        Exp_CaseStudy_Actor();
    }else if(DATA == 8){
        Exp_CaseStudy_P2P();
    }else if(DATA == 9){
        Exp_find_Disambiguation();
    }
    
    return 0;
}
