
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


void generateCorefnAdd(string name){
    
            //rn can only print a single nuymber 
            //fix this later
            if (name=="log") {
                
            cout<<"\tmov 8(%rsp), %rbx " <<endl  ;
            cout<<"\tadd $48, %rbx "       <<endl  ;
            cout<<"\tmov %rbx, 8(%rsp)   " <<endl  ;
            cout<<"\tmov $1, %rax "        <<endl  ;
            cout<<"\tmov $1, %rdi "        <<endl  ;
            cout<<"\tlea 8(%rsp), %rsi "   <<endl  ;
            cout<<"\tmov 16(%rsp), %rdx"   <<endl  ;
            cout<<"\tsyscall"              <<endl  ;
            cout<<"\tret"                  <<endl  ;
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
    

    //loop in the future
    
    for (int i =0; i< program->program.size(); i++) {



        if (program->program[i]->name=="main") {
            cout<<"_"<<"start:"<<endl; //this only when it is main functin
        }else{
            //something with params xd
            //that comes in calling not here
            string fnName=program->program[i]->name;
            //all should start with
            //jmp _asdf_end
            //_asdf
            //{...}
            //_asdf_end
            cout<<"jmp _" <<fnName<<"_end\n";
            cout<<"_"<<fnName<<":\n";
            //do if statement here if it infact is core then make this otherwize generate assem from statements
            generateCorefnAdd(program->program[i]->name);

            cout<<"_"<<fnName<<"_end:"<<endl;

        }





        for(int expressionIndex=0;expressionIndex< program->program[i]->body.size();expressionIndex++){
            //what expression is this

            StatementNode* statement =program->program[i]->body[expressionIndex].get();
            //check what statement this is (+-*/= return, function call etc)

            RetNode* ret= dynamic_cast<RetNode*>(statement);
            
            if(dynamic_cast<FuncCallNode*>(statement)){
                FuncCallNode* call=dynamic_cast<FuncCallNode*>(statement);
                //call the function based on params
                for (int i=call->arguments.size()-1; i>=0 ;i--){
                    //what type is the argument
                    if(dynamic_cast<ILiterealNode*>(call->arguments[i].get())){
                        ILiterealNode* n=dynamic_cast<ILiterealNode*>(call->arguments[i].get());
                        cout<<"\tpush $"<<n->value<<endl;
                    }else{
                        cout<<"INTIGERS ARE CURRENTLY ONLY SUPPORTED IN ASSEMBLY GENERATION\n";
                    }
                }
                cout<<"\tcall "<<"_"<<call->name<<endl;

            }


            //if this expression is a return
            if(ret){
                //if we are main function
                //when we put this entire thing into a loop this needs to change aswell
                if(program->program[i]->name=="main"){
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

}



