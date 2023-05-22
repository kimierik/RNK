#pragma once

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <fstream>
#include <string>
#include <vector>
#include <regex>


using namespace std;

enum TokenType{
    Void,
    Brace,
    Paren,
    Semicolon,
    i32,
    Return,
    Identifier,//name
    IntLiteral,
    Colon,
    Comma,
    Equals,
    Parameter,
    Operator,
};


//also should re check the tokens to see if identifiers should be other keywords
//cannot get semicolon rn that needs to be fixed

struct Token{
    TokenType type;
    string val;
    Token(TokenType t, string v){
        type=t;
        val=v;
    }
};

//lexer. returns a list of tokens tokens have a type and a value
//it reads one line at a time (as string) and lexes it
class Lexer{

    public:
    int row;
    int cursor;
    bool inClosure=false;// if we are inside (...)
    
    vector<Token> tokens;



    //starts the lexing operaiton
    void lex(){
        
        ifstream file("./main.rnk");
        string line;

        while (getline(file,line)){
            lexLine(line);
        }

    }


    void addToken(TokenType type,string value){
        tokens.push_back(Token(type,value));
    }
    void addToken(TokenType type,char value){
        string s(1,value);//!?!
        tokens.push_back(Token(type,s));
    }



    //goes through line and makes tokens from that
    void lexLine(string line){

        for (int cIndex=0 ; cIndex<line.length();) {
            char c=line[cIndex];

            //there has to be a better way
            if (c=='{') {
                addToken(Brace, c);
                cIndex++;
                continue;
            }
            if (c=='}') {
                addToken(Brace, c);
                cIndex++;
                continue;
            }

            if (c=='=') {
                addToken(Equals, c);
                cIndex++;
                continue;
            }

            if (c==')') {
                addToken(Paren, c);
                cIndex++;
                continue;
            }
            if (c=='(') {
                addToken(Paren, c);
                cIndex++;
                continue;
            }
            if (c==':') {
                addToken(Colon, c);
                cIndex++;
                continue;
            }
            if (c==',') {
                addToken(Comma, c);
                cIndex++;
                continue;
            }
            if (c==';') {
                addToken(Semicolon, c);
                cIndex++;
                continue;
            }
            if (c=='+') {
                addToken(Operator, c);
                cIndex++;
                continue;
            }

            //identifiers like function names and variable names
            if (isalpha(c)) {
                string str="";
                str+=c;
                char nchar=line[++cIndex];
                while (isalpha(nchar)){
                    str+=nchar;
                    nchar=line[++cIndex];
                }
                //this does not work
                //ofc in closure THE FUNCTION CALL CLOSURE 
                addToken(Identifier, str);
                continue;
            }


            //int literals
            if(isalnum(c)){
                string str="";
                str+=c;
                char nchar=line[++cIndex];
                while (isalnum(nchar)){
                    str+=nchar;
                    nchar=line[++cIndex];
                }
                addToken(IntLiteral, str);
                continue;
                
            }
        

            if(c==' '||'\n'){
                cIndex++;
                continue;
            }
            cout<<"SYNTAX ERROR: "<<c<<" is not represented \n";
            exit(1);
        }

    }


    //a horrible horrible function that mainly exists because i dont know how to lex properly
    void RecheckTokens(){
        for(int i=0; i< tokens.size();i++){
            if(tokens[i].val=="i"){
                tokens[i].type=i32;
            }
        }
        //another loop lol
        vector<string> ids;
        bool inparens=false;
        bool inbrakets=false;
        for(int i=0; i< tokens.size();i++){
            Token token=tokens[i];
            
            //check if we are in a function body or defenition or call etc
            if(token.val=="}"){
                inbrakets=false;
                //if we leave brakets we should remove the ids
                ids.clear();
            }
            if(token.val=="{"){
                inbrakets=true;
            }

            if(token.val==")"){
                inparens=false;
            }
            if(token.val=="("){
                inparens=true;
            }

            //func definition
            if(token.type==Identifier && !inbrakets && inparens){
                ids.push_back(token.val);
            }

            //function call
            if(token.type==Identifier && inbrakets && inparens){
                //cout<<"thing"<<endl;
                //if ids contains token.val; then change token.type to Parameter type
                for (int j=0; j<ids.size(); j++) {
                    //cout<<ids[j]<<endl;
                    if(ids[j]==token.val){
                        //cout<<"----------changed------------\n\n\n\n"; //this does not run at all
                        tokens[i].type=Parameter;
                    }
                } 
            }


            

        }

    }


    void PrintLex(){
        for (int i =0 ; i< tokens.size();i++) {
            cout<<""<<tokens[i].type<<" has value: "<< tokens[i].val  <<"\n";
        }
    }

};
