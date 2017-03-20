#ifndef PROJECT_JSONGEN_H
#define PROJECT_JSONGEN_H

#include <map>
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
class CFGJson;

class JsonWrapper {
public:
    json get_json();
    std::string get_string();
    int get_id(){ return this->id; }
    virtual int build() = 0;
    static int gen_id();


protected:
    json scope = nullptr;
    bool built = false;
    static int id_counter;
    int id = 0;
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
    void set_predecessors(CFGJson *);
    void set_successors(CFGJson *);
    bool is_entry() {return this->entry;}
    bool is_exit() {return this->exit;}

private:
    llvm::BasicBlock * bb;
    bool entry = false;
    bool exit = false;
};

class CFGJson : public JsonWrapper {
public:
    CFGJson(llvm::Function * fce){
        this->fce = fce;
    }
    int build();
    int find_bb(llvm::BasicBlock *);
    void add_bb(llvm::BasicBlock *, int);

private:
    llvm::Function * fce;
    std::map<llvm::BasicBlock *, int> bb_ids;
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
