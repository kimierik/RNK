#include <algorithm>
#include <cctype>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string.h>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <type_traits>
#include <vector>
#include <regex>

#include "./src/Tokenizer.cpp"
#include "./src/ExpressionNodes.cpp"
#include "./src/assembly_gen.cpp"

using namespace std;




//i assume this program leaks memory
//
//swap to smartpointers with make_unique
//
//smart pointers since idk what the fuck is happening anymore
//i think something might get dereffed maybe




class Parser{

    public:


    //parse expression
    //takes list of tokens returns ast
    //this is the start of the parser
    unique_ptr<ProgramNode> ParseExpression(vector<Token>& tokens){
        //this should return 
        //ProgramNode that has the main function node
        //then in the main function body there should be everythin else
        //
        //
        unique_ptr<ProgramNode> progstart= make_unique<ProgramNode>();


        unique_ptr<FuncDeclNode> mainfn= ParseFuncExpression( tokens);
        //main body somewhere somehow

        vector<ExprNode> emnt;
        mainfn->params= emnt;// no params on main, will implement later

        mainfn->returnType=IntLiteral; // rn main is always int literal 
                                       // we can make a function for this later

        mainfn->body=ParseFuncBody(tokens);
        //this does not get somethign 
        //something gets lost in pointers
        //we delete somehting we were refrencing with erace 
        //or idk how this works

        //cout<<"prograsdr\n";


        //main body is a list of expressions
        //the list starts at { and ends at }
        //for now we only do this later we can nest expressions
        //parse body fn? vector of expressions
        //

        progstart->program = std::move(mainfn);
        

        return progstart;
    }



    //parsing int literal
    ExprNode* ParseILiteralExression(vector<Token>& tokens){
        ILiterealNode *node = new ILiterealNode();
        string str_num =tokens[0].val;
        tokens.erase(tokens.begin());
        //convert string to numvber 
        int n =stoi(str_num);
        node->value=n;
        return node;
    }



    //for parsing return
    unique_ptr<RetNode> ParseRetExpression(vector<Token>& tokens){
        unique_ptr< RetNode>ret= make_unique<RetNode>();
        //if we get here the return will be the next token
        //TODO
        //everything before semicolon is what is returned

        if (tokens[1].type==IntLiteral) {
            unique_ptr<ILiterealNode> retval= make_unique<ILiterealNode>() ; 
            retval->value=stoi( tokens[1].val);

            //cout<< "retvalue is assigned as"<<"\n";
            //cout<< retval->value<<"\n";

            ret->value = std::move(retval);
            tokens.erase(tokens.begin());

        }else{
            cout<<"ERROR in parse ret expression\n";
        }


        return ret;
    }

    vector<unique_ptr<StatementNode>> ParseFuncBody(vector<Token>& tokens){
        // goes through tokens untill it meets '}' makes expressions out of them
        vector<unique_ptr<StatementNode>> statements;
        while (tokens[0].val!="{") {
            tokens.erase(tokens.begin());
        }

        while (tokens[0].val!="}") {
            Token token=tokens[0];
            if (token.type==Identifier&&token.val=="return"){
                //maybe check ret value here
                unique_ptr<RetNode> r=ParseRetExpression(tokens);

                //cout<< dynamic_cast<ILiterealNode*>(r->value.get())->value;

                statements.push_back(std::move(r));
                tokens.erase(tokens.begin());
            }
            //if token.type is something else then make push something else to statements

            tokens.erase(tokens.begin());
        }


        return statements;
    }


    //parse function
    unique_ptr<FuncDeclNode> ParseFuncExpression(vector<Token>& tokens){
        unique_ptr<FuncDeclNode> mainfn= make_unique<FuncDeclNode>();
        Token token=tokens[0];
        tokens.erase(tokens.begin());
        mainfn->name=token.val;
        return mainfn;
    }


};














int main(){

    Lexer *lex= new Lexer();
    lex->lex();
    
    //lex->PrintLex();


    Parser * parsr= new Parser();
    unique_ptr<ProgramNode> nod= parsr->ParseExpression(lex->tokens);


    //walk the node
    //cout<<"parser ran\n";



    //PrintEntireAST(nod.get());

    //make assembly from this

    generate_assemply(std::move(nod));
    


}
