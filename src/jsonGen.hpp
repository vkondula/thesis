#ifndef PROJECT_JSONGEN_H
#define PROJECT_JSONGEN_H

#include <llvm/IR/Module.h>
#include "external_lib/json.hpp"
#include "CFGGen.hpp"

class JsonGen : public FormatGen {
public:
    JsonGen(){};
    std::string getName();
    void setStream(FILE *);
    int build(llvm::Module *);
    int output();
    std::string getOutput();

private:
    FILE * fp = NULL;
    llvm::Module * module = NULL;
};


#endif //PROJECT_JSONGEN_H
