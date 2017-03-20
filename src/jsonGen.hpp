#ifndef PROJECT_JSONGEN_H
#define PROJECT_JSONGEN_H

#include <llvm/IR/Module.h>
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/Support/raw_ostream.h>
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
    FILE * fp = nullptr;
    llvm::Module * module = nullptr;
};

class ProgramLocJson;

class JsonWrapper {
public:
    json get_json();
    std::string get_string();
    ProgramLocJson * get_min() {return nullptr;}; //TODO
    ProgramLocJson * get_max() {return nullptr;}; //TODO
    virtual int build() = 0;
    static int get_id();


protected:
    json scope = nullptr;
    bool built = false;
    ProgramLocJson * min;
    ProgramLocJson * max;
    static int id;
};

class ProgramLocJson : public JsonWrapper {
public:
    ProgramLocJson(llvm::DebugLoc loc){
        this->loc = loc;
    }
    void merge(json);
    int build();
private:
    llvm::DebugLoc loc = nullptr;
};

class InstructionJson : public JsonWrapper {
public:
    InstructionJson(llvm::Instruction * inst){
        this->inst = inst;
    }
    int build();
    llvm::DebugLoc deb;

private:
    llvm::Instruction * inst;
    std::string get_inst_string();
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
