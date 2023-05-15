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








class Parser{

    public:


    //we should do like a standard lib thing that auto defines certain functions
    //like log
    //so we can get used to calling actual functions in ass
    //the fn should return a list of function definitions


    //parse expression
    //takes list of tokens returns ast
    //this is the start of the parser
    unique_ptr<ProgramNode> ParseExpression(vector<Token>& tokens){
        unique_ptr<ProgramNode> progstart= make_unique<ProgramNode>();

        //we need to start finding function declerations from tokens
        unique_ptr<FuncDeclNode> mainfn= ParseFuncExpression( tokens);
        tokens.erase(tokens.begin());
        //loop this ^^ untill there are no more tokens

        unique_ptr<FuncDeclNode> anotfn= ParseFuncExpression( tokens);


        

        vector<unique_ptr<FuncDeclNode>> standardFunctions=IncludeCoreFunctions();
        for (int i=0; i<standardFunctions.size();i++) {
            progstart->program.push_back(std::move(standardFunctions[i]));
        }

        progstart->program.push_back(  std::move(mainfn));
        progstart->program.push_back(  std::move(anotfn));
        

        return progstart;
    }



    vector<unique_ptr<FuncDeclNode>> IncludeCoreFunctions(){//these should be in a .rnk file later not constructed in a compiler
        //fns have
        //name
        //body
        //params
        //return type
        vector<unique_ptr<FuncDeclNode>> corefns;

        //------------------------------LOG------------------------------//
        //log does not have a body it is generated in asm
        unique_ptr<FuncDeclNode> logfn= make_unique<FuncDeclNode>();
        logfn->name="log";
        logfn->returnType=Void;
        vector<Token> logparams;
        logparams.push_back(Token(i32,"printable"));
        logparams.push_back(Token(i32,"size"));
        logfn->params=logparams;

        corefns.push_back(std::move(logfn));


        return corefns;
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


    //whenever we declare a value we put it on the stack
    //every variable decleration has a "nth" value
    //this value is used to see where on the stack the variable is
    //this is called when a new var is declared. so it increments the nth of the older variables
    void IncrementVarDecls(vector<unique_ptr<StatementNode>>* statements){
        for(int i=0; i<statements->size();i++){
            if (dynamic_cast<VarDeclNode*>( statements->at(i).get())){
                VarDeclNode* node= dynamic_cast<VarDeclNode*>( statements->at(i).get());
                node->nth=node->nth+1;
            }
        }
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

            if (token.type==Identifier && tokens[1].type==Paren){
                unique_ptr<FuncCallNode> call=ParseFunctionCall(tokens);
                statements.push_back(std::move(call));
                tokens.erase(tokens.begin());
            }

            //variable decleration id, semicolon, type

            if(token.type==Identifier && tokens[1].type==Colon ){
                unique_ptr<VarDeclNode> vardecl= make_unique<VarDeclNode>();
                vardecl->name=token.val;
                vardecl->nth=0;
                //loop all vardecls in statements and increment their nth
                
                IncrementVarDecls(&statements);
                statements.push_back(std::move(vardecl));
                tokens.erase(tokens.begin());
            }


            if(token.type==Identifier && tokens[1].type==Equals ){
                unique_ptr<VarAssigmentNode> Assign=make_unique<VarAssigmentNode>();
                Assign->toassign_name=token.val;

                if (tokens[2].type==IntLiteral) {
                    unique_ptr<ILiterealNode> a=make_unique<ILiterealNode>();
                    a->value=stoi(tokens[2].val);
                    Assign->value=std::move(a);
                    //cout<<a.value;
                }else{
                    cout<<"INT CAN ONLY BE ASSIGNED RN"<<endl;
                }

                statements.push_back(std::move(Assign));
                tokens.erase(tokens.begin());
            }



            tokens.erase(tokens.begin());
        }


        return statements;
    }


    //parse function decleration
    unique_ptr<FuncDeclNode> ParseFuncExpression(vector<Token>& tokens){
        // assumes the firt token is the function name
        // inits name,body, params, ret type
        unique_ptr<FuncDeclNode> mainfn= make_unique<FuncDeclNode>();
        Token token=tokens[0];
        tokens.erase(tokens.begin());
        mainfn->name=token.val;

        vector<Token> params;
        while(tokens[0].val!=")"){

            //figure out params
            //if token is int identifier
            //this prob needs to run the lexer again to see what the tokens should be idk
            //of just untill its an id
            //there are 2 ids
            //name and integer thing
            //prob just split the i token into a int asibment thing
            if (tokens[0].type==i32){
                params.push_back(Token(tokens[0].type,tokens[0].val));
            }
            
            tokens.erase(tokens.begin());
        }
        
        mainfn->params= params;
        tokens.erase(tokens.begin());

        //next token should be :
        
        tokens.erase(tokens.begin());//remove
                                     
        //then { or literal value
        if(tokens[0].type!=Paren){
            if(tokens[0].val=="i"){
                mainfn->returnType=IntLiteral; 
            }else{
                mainfn->returnType=Void; 
            }
        }

                                       

        mainfn->body=ParseFuncBody(tokens);


        return mainfn;
    }

    unique_ptr<FuncCallNode> ParseFunctionCall(vector<Token>& tokens){
        unique_ptr<FuncCallNode> call=make_unique<FuncCallNode>();
        Token token=tokens[0];
        tokens.erase(tokens.begin());
        call->name=token.val;
        
        //parse arguments
        //arguments are a list of ExprNode's in this case they are only iliterals
        //arguments are inside () seperated by ,
        call->arguments=ParseArguments(tokens);

        return call;
    }

    vector<unique_ptr<ExprNode>> ParseArguments(vector<Token>& tokens){
        vector<unique_ptr<ExprNode>> arguments;
        
        while (tokens[0].val!="(") {
            tokens.erase(tokens.begin());
        }
        tokens.erase(tokens.begin()); //index 0 should be"("

        int paramcounter=0;
        while (tokens[0].val!=")") {
            //make literals from tokens that are not ,
            if (tokens[0].type==Comma){
                tokens.erase(tokens.begin());
                continue;
            }

            if (tokens[0].type==IntLiteral) {
                unique_ptr<ILiterealNode> nod=make_unique<ILiterealNode>();
                nod->value=stoi(tokens[0].val); //val is string this makes it number
                arguments.push_back(std::move(nod));
                paramcounter++;
            }else {
                //cout<<"CANNOT MAKE AN ARGUMENT FROM NON ILITERAL\n";
                //trying to make an arg from x witch is a shorthand for the param that is given
                //how do we represent it in here so that we know how to make asm from it
                //should we just have it as some type that represents the first param
                //we should be able to make asm from that then
                //arguments are a list of expressions
                //we should then just make an expression that is param value or somehitng like that 
                //even though it prob is not an expression really
            }
            if (tokens[0].type==Identifier) {
                //trying to make an arg from x witch is a shorthand for the param that is given
                //how do we represent it in here so that we know how to make asm from it
                //should we just have it as some type that represents the first param
                //we should be able to make asm from that then
                //arguments are a list of expressions
                //we should then just make an expression that is param value or somehitng like that 
                //even though it prob is not an expression really
                unique_ptr<ParamVarNode> nod=make_unique<ParamVarNode>();
                nod->name=tokens[0].val;
                nod->type=tokens[0].type;
                nod->nth=paramcounter;
                arguments.push_back(std::move(nod));

                paramcounter++;
            }
            tokens.erase(tokens.begin());
        }
        return arguments;

    }

    


};














int main(int argc, char* argv[]){



    if (argc<=1) {
        cout<<"use arguments "<<endl;
        cout<<" -d and -b "<<endl;
    }else{
        Lexer *lex= new Lexer();
        lex->lex();
        lex->RecheckTokens();

        Parser * parsr= new Parser();
        if(strcmp(argv[1], "-d")){
            lex->PrintLex();
            unique_ptr<ProgramNode> nod= parsr->ParseExpression(lex->tokens);
            PrintEntireAST(nod.get());
        }
        if(strcmp(argv[1], "-b")){
            unique_ptr<ProgramNode> nod= parsr->ParseExpression(lex->tokens);
            generate_assemply(std::move(nod));
        }

    }






    //make assembly from this

}
