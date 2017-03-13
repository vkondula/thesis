#include "jsonGen.hpp"

using json = nlohmann::json;

void JsonGen::set_stream(FILE *){
    // TODO
}


int JsonGen::build(llvm::Module * module){
    std::cerr << "got_it" << "\n";
    if (!module) return 0;
    ModuleJson mod(module);
    if (int retval = mod.build()) return retval;
    json output = mod.get_json();
    // TODO redirect to file stream
    std::cout << output << "\n";
    return 0;
}


int JsonGen::output(){
    // TODO
    return 0;
}


std::string JsonGen::get_output(){
    // TODO
    return "TODO";
}

json JsonWrapper::get_json(){
    if (!this->built){
        if(!this->build()){
            return NULL;
        }
    }
    return this->scope;
}

std::string JsonWrapper::get_string() {
    if (!this->built){
        if(!this->build()){
            return NULL;
        }
    }
    return this->scope.dump();
}

int ModuleJson::build() {
    this->built = true;
    this->scope["id"] = "MODULE_TODO_GEN_UUID";
    json cfg_json = json::array();
    for (llvm::Module::iterator fce = this->module->begin(), end = this->module->end(); fce!=end; fce++){
        if (!&*fce) continue;
        CFGJson cfg(&*fce);
        if (int retval = cfg.build()){
            // ERROR occurred
            return retval;
        }
        cfg_json.push_back(cfg.get_json());
    }
    this->scope["cfg"] = cfg_json;
    return 0;
}

int CFGJson::build() {
    this->built = true;
    this->scope["id"] = "CFG_TODO_GEN_UUID";
    this->scope["file"] = {"TODO", "ADD_FILES"};
    json bb_json = json::array();
    for (llvm::Function::iterator bb = this->fce->begin(), end = this->fce->end(); bb!=end; bb++){
        if (!&*bb) continue;
        BasicBlockJson basic_block(&*bb);
        if (int retval = basic_block.build()){
            // ERROR occurred
            return retval;
        }
        bb_json.push_back(basic_block.get_json());
    }
    // TODO add entry points and exit points
    this->scope["basic_blocks"] = bb_json;
    return 0;
}


int BasicBlockJson::build() {
    this->built = true;
    this->scope["id"] = "BB_TODO_GEN_UUID";
    json ins_json = json::array();
    for (llvm::BasicBlock::iterator inst = this->bb->begin(), end = this->bb->end(); inst!=end; inst++){
        if (!&*inst) continue;
        InstructionJson instruction(&*inst);
        if (int retval = instruction.build()){
            // ERROR occurred
            return retval;
        }
        ins_json.push_back(instruction.get_json());
    }
    // TODO add entry points and exit points
    this->scope["instructions"] = ins_json;
    return 0;
}


int InstructionJson::build() {
    this->built = true;
    llvm::DebugLoc deb = this->inst->getDebugLoc();
    ProgramLocJson ploc(this->inst->getDebugLoc());
    if (ploc.build()) return 1;
    this->scope["ploc"] = ploc.get_json();
    this->scope["raw_llvm"] = this->inst->getName();
    return 0;
}

int ProgramLocJson::build() {
    this->built = true;
    if (!this->loc){
        // MISSING DEBUG INFO
        this->scope = NULL;
        return 0;
    }
    this->scope["line_begin"] = this->loc.getLine();
    this->scope["line_end"] = this->loc.getLine();
    this->scope["col_begin"] = this->loc.getCol();
    this->scope["col_end"] = this->loc.getCol();
    // TODO FILENAME
    return 0;
}