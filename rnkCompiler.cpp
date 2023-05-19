#include <algorithm>


#include "./src/Tokenizer.cpp"
#include "./src/ExpressionNodes.cpp"
#include "./src/assembly_gen.cpp"
#include "./src/parser.cpp"

using namespace std;



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
