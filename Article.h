//
//  Article.h
//  DBLP_Reader
//
//  Created by 黄智鹏 on 15/10/7.
//  Copyright (c) 2015年 黄智鹏. All rights reserved.
//

#ifndef __DBLP_Reader__Article__
#define __DBLP_Reader__Article__

#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

class Article
{
private:
    void addAttribute(string xml);
    
public:
    string title_;
    string mdate_;
    string key_;
    string year_;
    string volume_;
    string journal_;
    
    vector<string> authors_;
    vector<string> cites_;
    
    Article(string xml);
    Article();
};

#endif /* defined(__DBLP_Reader__Article__) */
