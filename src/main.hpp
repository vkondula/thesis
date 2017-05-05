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
#include "clang/Driver/Options.h"
#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "formater.hpp"
#include "cfg_gen.hpp"



class EmitMetadataAction : public  clang::EmitLLVMAction {
    using clang::EmitLLVMAction::EmitLLVMAction;
    void EndSourceFileAction() override;
};

#endif //PROJECT_CFGGEN_H
