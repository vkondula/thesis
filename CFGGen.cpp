// Declares clang::SyntaxOnlyAction.
#include <iostream>
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
// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"


using namespace clang::tooling;
using namespace llvm;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static cl::OptionCategory MyToolCategory("my-tool options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...");

//class FunctionCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
//public:
//    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &Result){
//        using namespace clang::ast_matchers;
//        const clang::FunctionDecl * fn = Result.Nodes.getNodeAs<clang::FunctionDecl>("functionDef");
//        const clang::SourceManager * src_mng = Result.SourceManager;
//        const clang::ASTContext * ast_con = Result.Context;
//        const clang::LangOptions & lang_opt = ast_con->getLangOpts();
//        std::string name = fn->getNameInfo().getAsString();
//        std::cout << "It's something!" << name << "\n";
//        std::unique_ptr<clang::CFG> src_cfg = clang::CFG::buildCFG(
//                fn, fn->getBody(), Result.Context, clang::CFG::BuildOptions()
//        );
//        for (clang::CFG::iterator a = src_cfg->begin(), b = src_cfg->end(); a!=b; a++){
//            (*a)->dump();
//        }
//
//        clang::CFGBlock & front = src_cfg->getExit();
//        std::cout << src_cfg->size() << "<<size \n";
//        std::cout << "defuq" << "\n";
//    }
//};

class EmitMetadataAction : public  clang::EmitLLVMAction {
    using clang::EmitLLVMAction::EmitLLVMAction;
    void EndSourceFileAction() override{
        std::unique_ptr<llvm::Module> module = this->takeModule();
        if (!module) return;
        std::cout << "got_it" << "\n";
        module->dump();
    }

};


int main(int argc, const char **argv) {
    using namespace clang::tooling;
    CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
    ClangTool Tool(
		OptionsParser.getCompilations(),
        OptionsParser.getSourcePathList()
	);
    std::cout << "It's nothing!\n";
    return Tool.run(newFrontendActionFactory<EmitMetadataAction>().get());
}
