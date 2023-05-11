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
    vector<ExprNode> arguments;
}; 

//function decleration
class FuncDeclNode : public ExprNode{
    public:
    string name;
    vector<ExprNode> params;
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

class ProgramNode: public ExprNode{
    public:
    unique_ptr< FuncDeclNode> program;
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
        cout<<"FUNCTION DEBUG PRINT IS UNIMPLEMENTED\n";
    }


}


//will crash if you have a recursive function
void PrintEntireAST(ProgramNode* program){
    //TODO loop all function decleration nodes

    cout<<":----Function----: "<<endl;
    cout<<program->program->name<<endl;

    cout<<":----Params----: "<<endl;
    for (int i=0 ; i< program->program->params.size();i++) {
        PrintExprNode(&program->program->params[i]);
        cout<<endl;
    }

    cout<<":----Body----: "<<endl;
    for (int i=0 ; i< program->program->body.size();i++) {
        PrintStatementNode(program->program->body.at(i).get());
        cout<<endl;
    }

    cout<<":----Returns----: "<<endl;
    cout<<program->program->returnType;
    cout<<endl;
    //this needs formatting
    
}





