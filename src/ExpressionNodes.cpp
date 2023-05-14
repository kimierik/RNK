#pragma  once


#include <cctype>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string.h>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include "./Tokenizer.cpp"




using namespace std;

//ast nodes
//
//parent. every node inherrits this for polymorphism
class ExprNode{
    public:
        virtual ~ExprNode(){};
};


class StatementNode{  // :  public ExprNode{ 
    //this is statements  like +=- return, func call stuff like that
    //stuff that does things and is inside a function body
    public:
        virtual ~StatementNode(){};
};


//function. node probably return is here aswell
class FuncCallNode : public StatementNode{
    public:
    string name;
    vector<unique_ptr<ExprNode>> arguments;
}; 

//function decleration
class FuncDeclNode : public ExprNode{
    public:
    string name;
    vector<Token> params;
    vector< unique_ptr<StatementNode>> body;
    TokenType returnType;
}; 



//return statement
class RetNode: public StatementNode{
    public:
    unique_ptr<ExprNode> value;
};



//int literal node
class ILiterealNode: public ExprNode{
    public:
    int value;
};

class ParamVarNode: public ExprNode{
    public:
    int nth; //how manyeth parameter this one is
    string name;
    TokenType type;
};

class ProgramNode: public ExprNode{
    public:
    vector<unique_ptr< FuncDeclNode>> program;
};





//logging



void PrintILiteral(ILiterealNode* node){
    cout<<node->value<<endl;
}

//see what is the child of this node then print it aproprietly
//or make function for each node seperately
void PrintExprNode(ExprNode* node){
    //iliteral and function declatarion
    if(dynamic_cast<ILiterealNode*>(node)){
        PrintILiteral(dynamic_cast<ILiterealNode*>(node));
    }
}


void PrintFuncCallNode(FuncCallNode*node){
    cout<<"calls function "<<node->name<<endl;
    cout<<"\twith "<< node->arguments.size() <<" arguments\n";
    
    for ( int i=0; i<node->arguments.size();i++) {
        ExprNode* arg=node->arguments[i].get();
        if(dynamic_cast<ILiterealNode*>(arg)){
            cout<<"\t int "<<dynamic_cast<ILiterealNode*>(arg)->value<<endl;
            continue;
        }

        if(dynamic_cast<ParamVarNode*>(arg)){
            cout<<"\t varialbe "<<dynamic_cast<ParamVarNode*>(arg)->name<<" type: "<< dynamic_cast<ParamVarNode*>(arg)->type<<endl;
            continue;
        }


        cout<<"NON SUPPORTED DATATYPE";
    }
}


void PrintStatementNode(StatementNode* node){
    //statment nodes are
    //func call
    //retnode

    //if staement is return
    if(dynamic_cast<RetNode*>(node)){
        cout<<"Return: ";
        PrintExprNode(dynamic_cast<RetNode*>(node)->value.get());
    }

    if(dynamic_cast<FuncCallNode*>(node)){
        //cout<<"FUNCTION DEBUG PRINT IS UNIMPLEMENTED\n";
        PrintFuncCallNode(dynamic_cast<FuncCallNode*>(node));
        //c
    }

}


//this should be cdhanged to whatever changes enum names to strings
void PrintParams(Token tok){
    if(tok.type==i32){
        cout<<"\tint :"<<tok.val <<"\n";
    }else{
        cout<<"NO OTHER TYPES SUPPORTED\n";
    }
}





//will crash if you have a recursive function
void PrintEntireAST(ProgramNode* program){
    //TODO loop all function decleration nodes
    //
    for (int i =0; i< program->program.size(); i++) {
    
        cout<<":----Function----: "<<endl;
        cout<<program->program[i]->name<<endl;

        cout<<":----Params----: ";
        cout<<program->program[i]->params.size() <<endl;
        for (int j=0 ; j< program->program[i]->params.size();j++) {
            PrintParams(program->program[i]->params[j]);
        }

        cout<<":----Body----: "<<endl;
        //cout<<program->program[i]->body.size()<<endl;
        for (int j=0 ; j< program->program[i]->body.size();j++) {
            PrintStatementNode(program->program[i]->body.at(j).get());
            cout<<endl;
        }

        cout<<":----Returns----: "<<endl;
        cout<<program->program[i]->returnType;
        cout<<endl;
        cout<<endl;
        cout<<endl;
        //this needs formatting
    }
    
}





