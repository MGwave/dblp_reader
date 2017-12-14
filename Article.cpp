//
//  Article.cpp
//  DBLP_Reader
//
//  Created by 黄智鹏 on 15/10/7.
//  Copyright (c) 2015年 黄智鹏. All rights reserved.
//

#include "Article.h"
#include <iostream>

using namespace std;

string getContentBetween(string s, string s1, string s2)
{
    if(s.find(s1) == string::npos || s.find(s2) == string::npos)
        return "";
    int begin = s.find(s1);
    int end = s.find(s2);
    if(begin >= end)
        return "";
    string rtn = s.substr(begin + s1.size(), end - (begin + s1.size()));
    return rtn;
}

Article::Article()
{}


Article::Article(string xml)
{
    addAttribute(xml);
}

void Article::addAttribute(string xml)
{
    title_ = getContentBetween(xml, "<title>", "</title>");
    year_ = getContentBetween(xml, "<year>", "</year>");
    volume_ = getContentBetween(xml, "<volume>", "</volume>");
    journal_ = getContentBetween(xml, "<journal>", "</journal>");
    
    string head = getContentBetween(xml, "<article", ">");
    
    mdate_ = getContentBetween(head, "mdate=\"", " key");
    mdate_ = mdate_.substr(0, mdate_.size() - 1);
    
    int temp = head.find("key=");
    key_ = head.substr(temp + 5, head.size() - (temp + 6));
    
    temp = xml.find("<author>");
    while(temp != string::npos){
        authors_.push_back(getContentBetween(xml.substr(temp), "<author>", "</author>"));
        temp = xml.find("<author>", temp + 1);
    }
    temp = xml.find("<cite>");
    while(temp != string::npos){
        cites_.push_back(getContentBetween(xml.substr(temp), "<cite>", "</cite>"));
        temp = xml.find("<cite>", temp + 1);
    }
    
}





