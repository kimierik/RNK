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
class FuncCallNode : public StatementNode, public ExprNode{
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

//this prob needs to change when we start to do actual variables
class ParamVarNode: public ExprNode{
    public:
    int nth; //how manyeth parameter this one is
    string name;
    TokenType type;
};

class VarUseageNode: public ExprNode{//this is a small hack
                                     //mainly used in seperationg paramn var and  var useage
    public:
    int nth; 
    string name;
    TokenType type;
};

class VarDeclNode: public StatementNode{
    public:
    string name;
    int nth;
};

class VarAssigmentNode: public StatementNode{
    public:
    VarDeclNode* variable;
    unique_ptr<ExprNode> value;
};



class ProgramNode: public ExprNode{
    public:
    vector<unique_ptr< FuncDeclNode >> program;
};


class AddOperand:public ExprNode{
    public:
    unique_ptr<ExprNode>left;      //pointer or uniq pointer  idk if we can do uniq but idk why not
    unique_ptr<ExprNode> right;
};




//logging


void PrintOps(AddOperand* node){

    if (dynamic_cast<ILiterealNode*>(node->left.get())){
        ILiterealNode* ilit =dynamic_cast<ILiterealNode*>(node->left.get());
        cout<<ilit->value;
    }
    cout<<" + ";

    if (dynamic_cast<ILiterealNode*>(node->right.get())){
        ILiterealNode* ilit =dynamic_cast<ILiterealNode*>(node->right.get());
        cout<<ilit->value<<endl;
    }

}



void PrintILiteral(ILiterealNode* node){
    cout<<node->value<<endl;
}

//see what is the child of this node then print it aproprietly
//or make function for each node seperately
void PrintExprNode(ExprNode* node){
    //iliteral and function declatarion
    if (!node) {
        cout<<"attempting to print null prt";
    }
    if(dynamic_cast<ILiterealNode*>(node)){
        PrintILiteral(dynamic_cast<ILiterealNode*>(node));
    }
    if(dynamic_cast<AddOperand*>(node)){
        PrintOps(dynamic_cast<AddOperand*>(node));
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
            cout<<"\t variable (param)"<<dynamic_cast<ParamVarNode*>(arg)->name<<" type: "<< dynamic_cast<ParamVarNode*>(arg)->type<<endl;
            continue;
        }

        if(dynamic_cast<VarUseageNode*>(arg)){
            cout<<"\t variable "<<dynamic_cast<VarUseageNode*>(arg)->name<<" type: "<< dynamic_cast<VarUseageNode*>(arg)->type<<endl;
            continue;
        }


        cout<<"NON SUPPORTED DATATYPE";
    }
}

void PrintVarDeclNode(VarDeclNode* node){
    cout<<"Declares "<<node->nth <<"'th variable: "<<node->name<<endl;
}

void PrintVarAssignNode(VarAssigmentNode* node){
    cout<<"Assigns var: " <<node->variable->name<<" with: ";
    PrintExprNode( node->value.get()) ;
}


void PrintStatementNode(StatementNode* node){
    //statment nodes are
    //func call
    //retnode
    //var decl 
    //var assigned

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
    if(dynamic_cast<VarDeclNode*>(node)){
        PrintVarDeclNode(dynamic_cast<VarDeclNode*>(node));
    }

    if(dynamic_cast<VarAssigmentNode*>(node)){
        PrintVarAssignNode(dynamic_cast<VarAssigmentNode*>(node));
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





