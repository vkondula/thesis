#ifndef PROJECT_JSONGEN_H
#define PROJECT_JSONGEN_H

#include <map>
#include <llvm/IR/Module.h>
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/Support/raw_ostream.h>
#include "main.hpp"


class ProgramLoc;
class CFGMeta;

class MetadataWrapper {
public:
    int get_id(){ return id; }
    static int gen_id();
    virtual int build() = 0;
protected:
    bool built = false;
    static int id_counter;
    int id = 0;
};

class ProgramLoc : public MetadataWrapper {
public:
    ProgramLoc(llvm::DebugLoc loc){
        this->loc = loc;
    }
    void merge(ProgramLoc *);
    std::string get_source_file(){ return source_file; }
    int get_line_min(){ return line_min; }
    int get_line_max(){ return line_max; }
    int get_col_begin(){ return col_begin; }
    int get_col_end(){ return col_end; }
    int build();
private:
    llvm::DebugLoc loc = nullptr;
    std::string source_file;
    int line_min = 0;
    int line_max = 0;
    int col_begin = 0;
    int col_end = 0;
};

class InstructionMeta : public MetadataWrapper {
public:
    InstructionMeta(llvm::Instruction * inst){
        this->inst = inst;
    }
    ~InstructionMeta(){
        if (ploc) delete ploc;
    }
    int build();
    ProgramLoc * get_ploc(){ return ploc; }
    std::string get_raw_llvm(){ return raw_llvm; }
    std::vector<llvm::Value *> get_operands() { return operands; }
    std::string get_defined_variable() { return defined_variable; }

private:
    std::string get_inst_string();
    llvm::Instruction * inst;
    ProgramLoc * ploc = nullptr;
    std::string raw_llvm;
    std::vector<llvm::Value *> operands;
    std::string defined_variable = "";
};

class BasicBlockMeta : public MetadataWrapper {
public:
    BasicBlockMeta(llvm::BasicBlock * bb){
        this->bb = bb;
    }
    ~BasicBlockMeta(){
        for(std::vector<InstructionMeta *>::size_type i = 0; i != insts.size(); i++) {
            delete insts[i];
        }
    }
    int build();
    bool is_entry() { return entry; }
    bool is_exit() { return exit; }
    ProgramLoc * get_ploc(){ return ploc.get_line_min() ? &ploc : nullptr; }
    std::vector<InstructionMeta *> get_insts() { return insts; }
    std::vector<BasicBlockMeta *> get_pred_bbs() { return pred_bbs; }
    std::vector<BasicBlockMeta *> get_succ_bbs() { return succ_bbs; }
    std::vector<std::string> get_def_vars() { return def_vars; }
    std::vector<std::string> get_used_vars() { return used_vars; }
    std::vector<std::string> get_used_funcs() { return used_funcs; }
    void set_predecessors(CFGMeta *);
    void set_successors(CFGMeta *);
private:
    void set_def_use_values();
    llvm::BasicBlock * bb;
    bool entry = false;
    bool exit = false;
    ProgramLoc ploc = ProgramLoc(nullptr);
    std::vector<InstructionMeta *> insts;
    std::vector<BasicBlockMeta *> pred_bbs;
    std::vector<BasicBlockMeta *> succ_bbs;
    std::vector<std::string> def_vars;
    std::vector<std::string> used_vars;
    std::vector<std::string> used_funcs;
};

class CFGMeta : public MetadataWrapper {
public:
    CFGMeta(llvm::Function * fce){
        this->fce = fce;
    }
    ~CFGMeta(){
        for(std::vector<BasicBlockMeta *>::size_type i = 0; i != bb_objs.size(); i++) {
            delete bb_objs[i];
        }
    }
    int build();
    BasicBlockMeta * find_bb(llvm::BasicBlock *);
    void add_bb(llvm::BasicBlock *, BasicBlockMeta *);
    std::vector<BasicBlockMeta *> get_bbs() { return bb_objs; }
    std::vector<BasicBlockMeta *> get_init_bbs() { return init_bbs; }
    std::vector<BasicBlockMeta *> get_fini_bbs() { return fini_bbs; }

private:
    llvm::Function * fce;
    std::map<llvm::BasicBlock *, BasicBlockMeta *> bbs_meta;
    std::vector<BasicBlockMeta *> bb_objs;
    std::vector<BasicBlockMeta *> init_bbs;
    std::vector<BasicBlockMeta *> fini_bbs;
};

class ModuleMeta : public MetadataWrapper {
public:
    ModuleMeta(llvm::Module * module){
        this->module = module;
    }
    ~ModuleMeta(){
        for (std::vector<CFGMeta *>::iterator it = cfgs.begin(); it != cfgs.end(); ++it){
            delete *it;
        }
    }
    int build();
    std::vector<CFGMeta *> get_cfgs() { return cfgs; }

private:
    llvm::Module * module;
    std::vector<CFGMeta *> cfgs;
};

#endif //PROJECT_JSONGEN_H
