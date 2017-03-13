#ifndef PROJECT_CFGGEN_H
#define PROJECT_CFGGEN_H

#include <iostream>
#include <stdio.h>
#include <memory>
#include <clang/Basic/LangOptions.h>
#include "clang/AST/Decl.h"
#include "clang/Analysis/CFG.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/BasicBlock.h"
#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

class FormatGen {
public:
    virtual void set_stream(FILE *) = 0;
    virtual int build(llvm::Module *) = 0;
    virtual int output() = 0;
    virtual std::string get_output() = 0;
};

class FormatFactory {
public:
    FormatFactory(){};
    FormatGen * getFormatGen(std::string format);
};

class EmitMetadataAction : public  clang::EmitLLVMAction {
    using clang::EmitLLVMAction::EmitLLVMAction;
    void EndSourceFileAction() override;
    FILE * fp = stdout;
    std::string format = "json";
public:
    void setOutputFile(FILE * fp){
        this->fp = fp;
    }
    void setFormat(std::string format){
        this->format = format;
    }
};

#endif //PROJECT_CFGGEN_H
