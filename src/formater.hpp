#ifndef PROJECT_FORMATER_HPP
#define PROJECT_FORMATER_HPP

#include "external_lib/json.hpp"
#include "cfg_gen.hpp"
#include <vector>

using json = nlohmann::json;

class ModuleMeta;
class CFGMeta;
class BasicBlockMeta;
class InstructionMeta;
class ProgramLoc;

class Formater {
public:
    void set_stream(FILE *);
    virtual int build(ModuleMeta *) = 0;
    virtual std::string get_output() = 0;
};

class FormatFactory {
public:
    FormatFactory(){};
    Formater * getFormatGen(std::string format);
};

class JsonGen : public Formater {
public:
    JsonGen(){};
    int build(ModuleMeta * module);
    std::string get_output();
    json get_module_json(ModuleMeta * module, bool recursive);
    json get_cfg_json(CFGMeta * cfg, bool recursive);
    json get_basic_block_json(BasicBlockMeta * bb, bool recursive);
    json get_instruction_json(InstructionMeta * inst);
    json get_ploc_json(ProgramLoc * ploc);
private:
    json scope = nullptr;
};

#endif //PROJECT_FORMATER_HPP
