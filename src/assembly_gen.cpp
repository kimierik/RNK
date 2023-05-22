
#include "./ExpressionNodes.cpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
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


void generateFunctionCall(FuncCallNode* call, int variableDeclCount){

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

            //this needs to know that we have pushed a param&variable on to the stack
            cout<<"\tpush "<<8*(n->nth+2+variableDeclCount)<<"(%rsp)#param"<<endl ;


            continue;
        }
        
        if(dynamic_cast<VarUseageNode*>(call->arguments[i].get())){
            //push a variable onto to top of the stack
            //this is used when we are using a variable as a parameter
            VarUseageNode* n=dynamic_cast<VarUseageNode*>(call->arguments[i].get());

            cout<<"\tpush "<<8*(n->nth+1)<<"(%rsp)#var"<<endl ; //+1 to offset indexing starting at 0?
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


void generateVarDecl(VarDeclNode* node){
    //var decl
    //push something on to the stack
    cout<< "\tpush $0"<<endl;
}



string GetExprValue(ExprNode*node){

    if (dynamic_cast<ILiterealNode*>(node)){
        ILiterealNode* ilit =dynamic_cast<ILiterealNode*>(node);
        return "$"+to_string(ilit->value);
    }

    if (dynamic_cast<VarUseageNode*>(node)){
        VarUseageNode* ilit =dynamic_cast<VarUseageNode*>(node);
        //get offset from var useage
        int off = 8*(ilit->nth);
        return to_string(off)+"(%rsp)";
    }



    cout <<" unable to get valur from expression "<<endl;
    return "ERROR VALUE";
}

void generateOpp( AddOperand* opp){
    //what get the expresisons of the thingsd and add them together
    string l;
    string r;

    //this is kinda bad design
    //manually check left and right node for their type by checking if casting it to the type does not return null


    
    l=GetExprValue(opp->left.get());

    r=GetExprValue(opp->right.get());


    cout<<"\tmov "<<l<<", %rax"<<endl;
    cout<<"\tadd "<<r<<", %rax"<<endl;

    
}


void generateVarAssigment(VarAssigmentNode* node, int variableDeclCount){
    //we need to find variable what is the nth value of it
    //unless we save it to assigment node
    //then  something like : cout<<"\tmov "<<" value we want to put here ,"<<8*(n->nth+?)<<"(%rsp)"<<endl ;
    ILiterealNode* a= dynamic_cast<ILiterealNode*>(node->value.get());
    if(a){
        cout<<"\tmov $"<< a->value <<", " << 8*(node->variable->nth)  <<"(%rsp)" <<endl; //TODO remove the -1 from nth
    }

    if(dynamic_cast<AddOperand*>(node->value.get())){
        generateOpp(dynamic_cast<AddOperand*>(node->value.get()));
        cout<<"\tmov %rax"<<", " <<8*(node->variable->nth)  <<"(%rsp)"<<endl;
    }

    if(dynamic_cast<FuncCallNode*>(node->value.get())){
        FuncCallNode* call =dynamic_cast<FuncCallNode*>(node->value.get());
        generateFunctionCall(call, variableDeclCount);
        cout<<"\tmov %rbx"<<", " <<8*(node->variable->nth)  <<"(%rsp)"<<endl;
    }


}





void generateFunctionBody(unique_ptr<FuncDeclNode> function ){
    int variableCount=0;

    //loop all statements in the function body
    for(int expressionIndex=0;expressionIndex< function->body.size();expressionIndex++){
        //what expression is this

        StatementNode* statement =function->body[expressionIndex].get();
        //check what statement this is (+-*/= return, function call etc)


        //if a function call 
        if(dynamic_cast<FuncCallNode*>(statement)){
            generateFunctionCall(dynamic_cast<FuncCallNode*>(statement),variableCount);
            continue;
        }
        
        //if variable decleration
        if(dynamic_cast<VarDeclNode*>(statement)){
            variableCount++;
            generateVarDecl(dynamic_cast<VarDeclNode*>(statement));
            continue;
        }
        if(dynamic_cast<VarAssigmentNode*>(statement)){
            generateVarAssigment(dynamic_cast<VarAssigmentNode*>(statement), variableCount);
            continue;
        }


        //if this expression is a return
        RetNode* ret= dynamic_cast<RetNode*>(statement);
        if(ret){

            //if we are returning int literal
            if (dynamic_cast<ILiterealNode*>(ret->value.get())){
                cout << "\tmov $" << dynamic_cast<ILiterealNode*>(ret->value.get())->value<< ", %rbx" <<endl;
                for (int i=0; i<variableCount; i++) {
                    cout<<"\tpop %rax"<<endl;
                }
                cout <<"\tret"<<endl;
            }





        }//ret if end
    }



    //for every variable declared we must now pop them
    //if we do not the return adress is not at the top and we cannot get to it with ret
    for (int i=0; i<variableCount; i++) {
        cout<<"\tpop %rax"<<endl;
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

    cout << "_start:"<<endl;
    cout << "\tcall _main"<<endl;

    cout <<"\tmov $60 , %rax"<<endl;
    cout <<"\tmov "<<"%rbx"<< ", %rdi"<<endl;
    cout <<"\tsyscall"<<endl;


    
    for (int i =0; i< program->program.size(); i++) {

        string fnName=program->program[i]->name;


        //all functions have
        //jmp _[fn name]_end
        //_[fn name]
        //{...}
        //ret
        //_[fn name]_end
        cout<<"jmp _" <<fnName<<"_end\n"; //jump over function incase we accidentally walk into it
        cout<<"_"<<fnName<<":\n";
        if(!generateCorefnAdd(program->program[i]->name)){ //if this function is not a core function
            generateFunctionBody(std::move(program->program[i]));
        }
        cout<<"\tret"<<endl;
        cout<<"_"<<fnName<<"_end:"<<endl;
    }


}





