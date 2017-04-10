#include "formater.hpp"

// Factory for output generator in specific format
// For now only JSON is available
Formater * FormatFactory::getFormatGen(std::string format){
    if (format.compare("json") == 0){
        return new JsonGen();
    } else {
        return nullptr;
    }
}

void Formater::set_stream(FILE *) {
    // TODO
}

std::string JsonGen::get_output() {
    return scope.dump(4);
}

int JsonGen::build(ModuleMeta * module){
    if (!module) {
        scope = nullptr;
        return 0;
    }
    try{
        scope = get_module_json(module, true);
    }
    catch (...) {
        std::cout << "Build failed";
        return 1;
    }
    return 0;
}

json JsonGen::get_module_json(ModuleMeta * module, bool recursive){
    json retval;
    retval["id"] = std::to_string(module->get_id());
    retval["cfgs"] = nullptr;
    if (recursive){
        // Iter over functions in module
        json cfgs_json = json::array();
        std::vector<CFGMeta *> cfgs = module->get_cfgs();
        for (std::vector<CFGMeta *>::iterator it = cfgs.begin(); it != cfgs.end(); ++it){
            json cfg = get_cfg_json(*it, true);
            cfgs_json.push_back(cfg);
        }
        retval["cfgs"] = cfgs_json;
    }
    json variables_json = json::array();
    std::vector<InstructionMeta *> vars = module->get_global_vars();
    // Iter over global variables
    for (std::vector<InstructionMeta *>::iterator it = vars.begin(); it != vars.end(); ++it){
        json cfg = get_instruction_json(*it);
        variables_json.push_back(cfg);
    }
    retval["global_vars"] = variables_json;
    return retval;
}

json JsonGen::get_cfg_json(CFGMeta * cfg, bool recursive){
    json retval;
    retval["id"] = std::to_string(cfg->get_id());
    retval["basic_blocks"] = nullptr;
    if (recursive) {
        json bbs_json = json::array();
        std::vector<BasicBlockMeta *> bbs = cfg->get_bbs();
        for (std::vector<BasicBlockMeta *>::iterator it = bbs.begin(); it != bbs.end(); ++it) {
            json bb = get_basic_block_json(*it, true);
            bbs_json.push_back(bb);
        }
        retval["basic_blocks"] = bbs_json;
    }
    json init_bb = json::array();
    json fini_bb = json::array();
    std::vector<BasicBlockMeta *> bbs = cfg->get_init_bbs();
    for (std::vector<BasicBlockMeta *>::iterator it = bbs.begin(); it != bbs.end(); ++it){
        init_bb.push_back(std::to_string((*it)->get_id()));
    }
    bbs = cfg->get_fini_bbs();
    for (std::vector<BasicBlockMeta *>::iterator it = bbs.begin(); it != bbs.end(); ++it){
        fini_bb.push_back(std::to_string((*it)->get_id()));
    }
    std::string name = cfg->get_name();
    if (!name.empty()){
        json metadata;
        metadata["name"] = name;
        retval["meta"] = metadata;
    }
    retval["initBB"] = init_bb;
    retval["finiBB"] = fini_bb;
    return retval;
}

json JsonGen::get_basic_block_json(BasicBlockMeta * bb, bool recursive){
    json retval;
    retval["id"] = std::to_string(bb->get_id());
    retval["instructions"] = nullptr;
    if (recursive) {
        json insts_json = json::array();
        json label;
        label["llvm_raw"] = "; <label>:" + bb->get_label_string();
        label["ploc"] = nullptr;
        insts_json.push_back(label);
        std::vector<InstructionMeta *> insts = bb->get_insts();
        for (std::vector<InstructionMeta *>::iterator it = insts.begin(); it != insts.end(); ++it) {
            json inst = get_instruction_json(*it);
            insts_json.push_back(inst);
        }
        retval["instructions"] = insts_json;
    }
    json pred_bb = json::array();
    json succ_bb = json::array();
    std::vector<BasicBlockMeta *> bbs = bb->get_pred_bbs();
    for (std::vector<BasicBlockMeta *>::iterator it = bbs.begin(); it != bbs.end(); ++it){
        pred_bb.push_back(std::to_string((*it)->get_id()));
    }
    bbs = bb->get_succ_bbs();
    for (std::vector<BasicBlockMeta *>::iterator it = bbs.begin(); it != bbs.end(); ++it){
        succ_bb.push_back(std::to_string((*it)->get_id()));
    }
    retval["def"] = bb->get_def_vars();
    retval["use"] = bb->get_used_vars();
    retval["pred"] = pred_bb;
    retval["succ"] = succ_bb;
    json metadata;
    metadata["called_functions"] = bb->get_used_funcs();
    metadata["label_name"] = bb->get_label_string();
    retval["meta"] = metadata;
    retval["ploc"] = get_ploc_json(bb->get_ploc());
    return retval;
}

json JsonGen::get_instruction_json(InstructionMeta * inst){
    json retval;
    retval["raw_llvm"] = inst->get_raw_llvm();
    retval["ploc"] = get_ploc_json(inst->get_ploc());
    return retval;
}

json JsonGen::get_ploc_json(ProgramLoc * ploc){
    if (!ploc) return nullptr;
    json retval;
    int line_min = retval["line_min"] = ploc->get_line_min();
    int line_max = ploc->get_line_max();
    if (line_max && line_max != line_min){
        retval["line_max"] = line_max;
    }
    if (int col_begin = ploc->get_col_begin()){
        retval["col_begin"] = col_begin;
        int col_end = ploc->get_col_end();
        if (col_end && (col_end != col_begin || line_min != line_max)){
            retval["col_end"] = col_end;
        }
    }
    retval["source_file"] = ploc->get_source_file();
    return retval;
}
