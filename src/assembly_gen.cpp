
#include "./ExpressionNodes.cpp"
#include <cstdlib>
#include <iostream>
#include <memory>


using namespace std;


//TODO
//functions
//ops
//
//idk which to do first prob print



//prints assembly code to stdout
void generate_assemply( unique_ptr<ProgramNode> program){
    cout << ".global _start"<<endl;
    cout << ".text"<<endl;

    //loop all functions in program
    //find the main and then use that to generate stuff to start
    //expressions of main function
    //
    //all my functions will have _ at the start for some reason now
    

    //loop in the future

    if (program->program->name=="main") {
        cout<<"_"<<"start:"<<endl; //this only when it is main functin
    }else{
        cout<<"_"<<program->program->name<<":"<<endl; //this normally
    }

    for(int expressionIndex=0;expressionIndex< program->program->body.size();expressionIndex++){
        //what expression is this
        StatementNode* statement =program->program->body[expressionIndex].get();
        //check what statement this is (+-*/= return, function call etc)
        RetNode* ret= dynamic_cast<RetNode*>(statement);
        //if this expression is a return
        if(ret){
            //if we are main function
            //when we put this entire thing into a loop this needs to change aswell
            if(program->program->name=="main"){
                cout <<"\tmov $60 , %rax"<<endl;
                if (dynamic_cast<ILiterealNode*>(ret->value.get())) {
                    cout <<"\tmov $"<<dynamic_cast<ILiterealNode*>(ret->value.get())->value<< ", %rdi"<<endl;
                    cout<<"\tsyscall"<<endl;
                }else{
                    cout <<"cannot support non iliteral return values from main"<<endl;
                    exit(1);
                }

            }
        }


    }


}



