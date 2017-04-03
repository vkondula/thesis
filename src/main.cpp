#include "main.hpp"

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

// Callback at the end of Generating LLVM IR code
// Allows to access llvm::Module
void EmitMetadataAction::EndSourceFileAction(){
    clang::EmitLLVMAction::EndSourceFileAction();
    std::unique_ptr<llvm::Module> module = this->takeModule();
    if (!module) return;
    // Build Metadata structure for module
    ModuleMeta mod(module.get());
    if (mod.build()) return;
    // Print to set output stream (stdout) in set format (json)
    FormatFactory factory;
    Formater * formater = factory.getFormatGen(format);
    std::cerr << "It's something!" << format << "\n";
    formater->build(&mod);
    std::cout << formater->get_output();
}

int main(int argc, const char **argv) {
    using namespace clang::tooling;
    CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
    ClangTool Tool(
		OptionsParser.getCompilations(),
        OptionsParser.getSourcePathList()
	);
    Tool.appendArgumentsAdjuster(getInsertArgumentAdjuster("-g", ArgumentInsertPosition::END));
    std::cout << "It's nothing!\n";
    return Tool.run(newFrontendActionFactory<EmitMetadataAction>().get());
}
