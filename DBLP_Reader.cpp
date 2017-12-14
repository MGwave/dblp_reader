//
//  DBLP_Reader.cpp
//  DBLP_Reader
//
//  Created by 黄智鹏 on 15/10/7.
//  Copyright (c) 2015年 黄智鹏. All rights reserved.
//

#include "DBLP_Reader.h"

#include <fstream>
#include <iostream>
#include <sstream>


using namespace std;




DBLP_Reader::DBLP_Reader(string path):DBLP_path_(path)
{

}

void DBLP_Reader::read(vector<Article> &rtn)
{
    ifstream input;
    input.open(DBLP_path_, ios::in);
    string line, rest_buffer;
    while(getline(input, line)){
        line = rest_buffer + line;
        string buffer;
        if(line.find("<article") != string::npos){
            buffer += line.substr(line.find("<article"));
            while(getline(input, line)){
                if(line.find("</article>") == string::npos){
                    buffer += line;
                }else{
                    buffer += line.substr(0, line.find("</article>") + 10);
                    rest_buffer = line.substr(line.find("</article>" + 10));
                    break;
                }
            }
            Article tempArticle(buffer);
            //cerr << tempArticle.title_ << endl;
            rtn.push_back(tempArticle);
        }
        
    }
}

void DBLP_Reader::writeConfToFile(string path, vector<Article> & articles)
{
    ofstream out(path, ios::out);
    for(int i = 0; i < articles.size(); ++i){
        if(articles[i].key_.find("conf") == string::npos)
            continue;
        string line;
        line += articles[i].key_;
        line += "\t";
        line += articles[i].mdate_;
        line += "\t";
        line += articles[i].title_;
        line += "\t";
        line += articles[i].volume_;
        line += "\t";
        line += articles[i].year_;
        line += "\t";
        line += articles[i].journal_;
        line += "\t";
        
        string s_authors = "[";
        for(int j = 0; j < articles[i].authors_.size(); ++j){
            s_authors += articles[i].authors_[j];
            if(j < articles[i].authors_.size() - 1)
                s_authors += '\t';
        }
        line += s_authors;
        line += "]\t";
        string c_authors = "{";
        for(int j = 0; j < articles[i].cites_.size(); ++j){
            c_authors += articles[i].cites_[j];
            if(j < articles[i].cites_.size() - 1)
                c_authors += '\t';
        }
        line += c_authors;
        line += "}";
        
        out << line << endl;
    }
    out.close();
}

void DBLP_Reader::writeToFile(string path, vector<Article> & articles)
{
    ofstream out(path, ios::out);
    for(int i = 0; i < articles.size(); ++i){
        string line;
        line += articles[i].key_;
        line += "\t";
        line += articles[i].mdate_;
        line += "\t";
        line += articles[i].title_;
        line += "\t";
        line += articles[i].volume_;
        line += "\t";
        line += articles[i].year_;
        line += "\t";
        line += articles[i].journal_;
        line += "\t";
        
        string s_authors = "[";
        for(int j = 0; j < articles[i].authors_.size(); ++j){
            s_authors += articles[i].authors_[j];
            if(j < articles[i].authors_.size() - 1)
                s_authors += '\t';
        }
        line += s_authors;
        line += "]\t";
        string c_authors = "{";
        for(int j = 0; j < articles[i].cites_.size(); ++j){
            c_authors += articles[i].cites_[j];
            if(j < articles[i].cites_.size() - 1)
                c_authors += '\t';
        }
        line += c_authors;
        line += "}";
        
        out << line << endl;
    }
    out.close();
}

void DBLP_Reader::ReadFromFile(string path, vector<Article> &article)
{
    ifstream input(path, ios::in);
    string line;
    while(getline(input, line)){
        stringstream sinput(line);
        Article tempArticle;
        sinput >> tempArticle.key_ >> tempArticle.mdate_ >> tempArticle.title_ >> tempArticle.volume_ >> tempArticle.year_ >> tempArticle.journal_;
        int a_b = line.find("[") + 1;
        int a_e = line.find("]");
        string s_author = line.substr(a_b, a_e - a_b);
        
        stringstream au_input(s_author);
        string s_author0;
        while(au_input >> s_author0){
            tempArticle.authors_.push_back(s_author0);
        }
        int c_b = line.find("{") + 1;
        int c_e = line.find("}");
        string c_author = line.substr(c_b, c_e - c_b);
        
        stringstream cite_input(c_author);
        string s_cite0;
        while(cite_input >> s_cite0){
            tempArticle.cites_.push_back(s_cite0);
        }
        article.push_back(tempArticle);
    }
    input.close();
}