#ifndef PROJECT_JSONGEN_H
#define PROJECT_JSONGEN_H

#include <llvm/IR/Module.h>
#include "external_lib/json.hpp"
#include "CFGGen.hpp"

using json = nlohmann::json;

class JsonGen : public FormatGen {
public:
    JsonGen(){};
    void set_stream(FILE *);
    int build(llvm::Module *);
    int output();
    std::string get_output();

private:
    FILE * fp = NULL;
    llvm::Module * module = NULL;
};

class ProgramLocJson;

class JsonWrapper {
public:
    json get_json();
    std::string get_string();
    ProgramLocJson * get_min() {return NULL;}; //TODO
    ProgramLocJson * get_max() {return NULL;}; //TODO
    virtual int build() = 0;


protected:
    json scope;
    bool built = false;
    ProgramLocJson * min;
    ProgramLocJson * max;
};

class ProgramLocJson : public JsonWrapper {
public:
    ProgramLocJson(llvm::DebugLoc loc){
        this->loc = loc;
    }
    int build();

private:
    llvm::DebugLoc loc;
};

class InstructionJson : public JsonWrapper {
public:
    InstructionJson(llvm::Instruction * inst){
        this->inst = inst;
    }
    int build();

private:
    llvm::Instruction * inst;
};

class BasicBlockJson : public JsonWrapper {
public:
    BasicBlockJson(llvm::BasicBlock * bb){
        this->bb = bb;
    }
    int build();

private:
    llvm::BasicBlock * bb;
};

class CFGJson : public JsonWrapper {
public:
    CFGJson(llvm::Function * fce){
        this->fce = fce;
    }
    int build();

private:
    llvm::Function * fce;
};

class ModuleJson : public JsonWrapper {
public:
    ModuleJson(llvm::Module * module){
        this->module = module;
    }
    int build();

private:
    llvm::Module * module;
};

#endif //PROJECT_JSONGEN_H
