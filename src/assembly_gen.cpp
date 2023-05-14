
#include "./ExpressionNodes.cpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include "./Tokenizer.cpp"


using namespace std;


//TODO
//functions
//ops
//
//idk which to do first prob print


bool generateCorefnAdd(string name){
    
            //rn can only print a single nuymber 
            //fix this later
    if (name=="log") {
        cout<<"\tmov 8(%rsp), %rbx "   <<endl  ;
        cout<<"\tadd $48, %rbx "       <<endl  ;
        cout<<"\tmov %rbx, 8(%rsp)"    <<endl  ;
        cout<<"\tmov $1, %rax "        <<endl  ;
        cout<<"\tmov $1, %rdi "        <<endl  ;
        cout<<"\tlea 8(%rsp), %rsi "   <<endl  ;
        cout<<"\tmov 16(%rsp), %rdx"   <<endl  ;
        cout<<"\tsyscall"              <<endl  ;
        //cout<<"\tret"                  <<endl  ;
        return true;
    }

    return false;
}


void generateFunctionCall(FuncCallNode* call){

    //call the function based on params
    for (int i=call->arguments.size()-1; i>=0 ;i--){

        //what type is the argument
        //if int push it direclty on to the stack
        if(dynamic_cast<ILiterealNode*>(call->arguments[i].get())){
            ILiterealNode* n=dynamic_cast<ILiterealNode*>(call->arguments[i].get());
            cout<<"\tpush $"<<n->value<<endl;
            continue;
        }

        //if the parameter is a parameter of the caller. then push the val again on to the stack
        //this duplicates the value
        if(dynamic_cast<ParamVarNode*>(call->arguments[i].get())){
            ParamVarNode* n=dynamic_cast<ParamVarNode*>(call->arguments[i].get());
            //we know how manyeth param this is
            //so we need to do like 8*(nth+2)(%rsp)//we are adding 2 since one of these is the instruction pointer that is gotten when this fn is called

            cout<<"\tpush "<<8*(n->nth+2)<<"(%rsp)"<<endl ;
            continue;
        }


        cout<<"unsupported datatype pushed";
    }

    //call the actual function
    cout<<"\tcall "<<"_"<<call->name<<endl; 

    //after call we must pop what we pushed
    for (int i=call->arguments.size()-1; i>=0 ;i--){
        cout<<"\tpop "<<"%rax"<<endl;
    }

}



void generateFunctionBody(unique_ptr<FuncDeclNode> function ){

    //loop all statements in the function body
    for(int expressionIndex=0;expressionIndex< function->body.size();expressionIndex++){
        //what expression is this

        StatementNode* statement =function->body[expressionIndex].get();
        //check what statement this is (+-*/= return, function call etc)

        RetNode* ret= dynamic_cast<RetNode*>(statement);
        
        //if a function call 
        if(dynamic_cast<FuncCallNode*>(statement)){
            generateFunctionCall(dynamic_cast<FuncCallNode*>(statement));
            continue;
        }


        //TODO do proper returns
        //if this expression is a return
        if(ret){
            //if we are main function
            //when we put this entire thing into a loop this needs to change aswell
            if(function->name=="main"){
                cout <<"\tmov $60 , %rax"<<endl;
                if (dynamic_cast<ILiterealNode*>(ret->value.get())) {
                    cout <<"\tmov $"<<dynamic_cast<ILiterealNode*>(ret->value.get())->value<< ", %rdi"<<endl;
                    cout<<"\tsyscall"<<endl;
                }else{
                    cout <<"cannot support non iliteral return values from main"<<endl;
                    exit(1);
                }

            }
        }//ret if end
    }
}




//prints assembly code to stdout
void generate_assemply( unique_ptr<ProgramNode> program){
    cout << ".global _start"<<endl;
    cout << ".text"<<endl;

    //loop all functions in program
    //find the main and then use that to generate stuff to start
    //expressions of main function
    //
    //all my functions will have _ at the start for some reason now
    

    
    //this loop should be broken down into multiple functions
    for (int i =0; i< program->program.size(); i++) {

        string fnName=program->program[i]->name;
        if (program->program[i]->name=="main") {
            //cout<<"_"<<"start:"<<endl; //this only when it is main functin
            fnName="start";
        }
        //all should start with
        //jmp _asdf_end
        //_asdf
        //{...}
        //_asdf_end

        cout<<"jmp _" <<fnName<<"_end\n"; //jump over function incase we accidentally walk into it
        cout<<"_"<<fnName<<":\n";

        if(!generateCorefnAdd(program->program[i]->name)){ //if this function is not a core function
            generateFunctionBody(std::move(program->program[i]));
        }
        cout<<"\tret"<<endl;
        cout<<"_"<<fnName<<"_end:"<<endl;
    }


}





