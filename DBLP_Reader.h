//
//  DBLP_Reader.h
//  DBLP_Reader
//
//  Created by 黄智鹏 on 15/10/7.
//  Copyright (c) 2015年 黄智鹏. All rights reserved.
//

#ifndef __DBLP_Reader__DBLP_Reader__
#define __DBLP_Reader__DBLP_Reader__

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "Article.h"


using namespace std;

class DBLP_Reader
{
public:
    //vector<Article> articles_;
    string DBLP_path_;
    
    DBLP_Reader(string xml_path);
    DBLP_Reader();
    void read(vector<Article> & rtn);
    void writeToFile(string path, vector<Article> & article);
    void ReadFromFile(string path, vector<Article> & article);
    
    void writeConfToFile(string path, vector<Article> & article);
};

#endif /* defined(__DBLP_Reader__DBLP_Reader__) */
