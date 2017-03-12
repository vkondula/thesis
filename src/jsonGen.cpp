#include "jsonGen.hpp"

std::string JsonGen::getName(){
    return "TODO";
}


void JsonGen::setStream(FILE *){

}


int JsonGen::build(llvm::Module * module){
    std::cout << "got_it" << "\n";
    for (llvm::Module::iterator a = module->begin(), b = module->end(); a!=b; a++){
        std::cout << "fml\n\n";
        for (llvm::Function::iterator block = a->begin(), block_end = a->end(); block!=block_end; block++){
            std::cout << "block\n\n";
            for (llvm::BasicBlock::iterator inst = block->begin(), inst_end = block->end(); inst!=inst_end; inst++){
                std::cout << "instruction\n\n";
                inst->dump();
                llvm::DebugLoc deb = inst->getDebugLoc();
                if (!deb) continue;
                std::cout << "line:" << deb.getLine() << "\n";
            }
        }
    }
    return 0;
}


int JsonGen::output(){
    return 0;
}


std::string JsonGen::getOutput(){
    return "TODO";
}