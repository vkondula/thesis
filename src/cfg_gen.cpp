#include "cfg_gen.hpp"

/*
 * Shared counter for all object inheriting from MetadataWrapper
 * Allows referencing between objects and adds unique identifier
 */
int MetadataWrapper::id_counter = 1;
int MetadataWrapper::gen_id() {
    return id_counter++;
}

int ModuleMeta::build() {
    built = true;
    id = MetadataWrapper::gen_id();
    // build funcions (cfgs) in module
    for (llvm::Module::iterator fce = module->begin(), end = module->end(); fce!=end; fce++){
        if (!&*fce) continue;
        CFGMeta * cfg = new CFGMeta(&*fce);
        if (int retval = cfg->build()){
            // ERROR occurred
            return retval;
        }
        cfgs.push_back(cfg);
    }
    // get global variables
    for (llvm::Module::global_iterator var = module->global_begin(), end = module->global_end(); var!=end; var++){
        if (!&*var) continue;
        std::cerr << "========\n";
        (&*var)->dump();
    }
    // get dependant modules
    return 0;
}


int CFGMeta::build() {
    built = true;
    id = MetadataWrapper::gen_id();
    if(fce->hasName()){
        name = fce->getName().str();
    }

    // 1st iteration... Create BB(generate id) and Instruction
    for (llvm::Function::iterator bb = fce->begin(), end = fce->end(); bb!=end; bb++){
        if (!&*bb) continue;
        BasicBlockMeta * basic_block = new BasicBlockMeta(&*bb);
        if (int retval = basic_block->build()){
            // ERROR occurred
            return retval;
        }
        // For next iterations
        bb_objs.push_back(basic_block);
        // Reversed map: (BB *)->(int)id
        add_bb(&*bb, basic_block);
    }
    // 2nd iteration... Get successors and predecessors, get program location, entry and exit points
    for(std::vector<BasicBlockMeta *>::size_type i = 0; i != bb_objs.size(); i++) {
        // Predecessors and Successors
        bb_objs[i]->set_predecessors(this);
        bb_objs[i]->set_successors(this);
        // Entry and Exit points
        if (bb_objs[i]->is_entry()) init_bbs.push_back(bb_objs[i]);
        if (bb_objs[i]->is_exit()) fini_bbs.push_back(bb_objs[i]);
    }
    return 0;
}

BasicBlockMeta * CFGMeta::find_bb(llvm::BasicBlock * bb){
    std::map<llvm::BasicBlock *, BasicBlockMeta *>::iterator it;
    it = bbs_meta.find(bb);
    if (it == bbs_meta.end())
        // Not found!
        return nullptr;
    return it->second;
}

void CFGMeta::add_bb(llvm::BasicBlock * bb, BasicBlockMeta * meta){
    bbs_meta[bb] = meta;
}

int BasicBlockMeta::build() {
    built = true;
    id = MetadataWrapper::gen_id();
    // Get label for basic block
    label = create_label_string();
    // Get instructions in a basic block
    for (llvm::BasicBlock::iterator inst = bb->begin(), end = bb->end(); inst!=end; inst++){
        if (!&*inst) continue;
        InstructionMeta * instruction = new InstructionMeta(&*inst);
        if (int retval = instruction->build()){
            // ERROR occurred
            return retval;
        }
        insts.push_back(instruction);
        // Get BasicBlock program location
        if (ProgramLoc * inst_ploc = instruction->get_ploc())
            ploc.merge(inst_ploc);
        set_def_use_values();
    }
    return 0;
}

void BasicBlockMeta::set_predecessors(CFGMeta * cfg){
    entry = true;
    for (llvm::pred_iterator pred = llvm::pred_begin(bb), end = llvm::pred_end(bb); pred != end; ++pred) {
        entry = false;
        BasicBlockMeta * bb_metadata = cfg->find_bb(*pred);
        if (bb_metadata) pred_bbs.push_back(bb_metadata);
    }
}

void BasicBlockMeta::set_successors(CFGMeta * cfg){
    exit = true;;
    for (llvm::succ_iterator succ = llvm::succ_begin(bb), end = llvm::succ_end(bb); succ != end; ++succ) {
        exit = false;
        BasicBlockMeta * bb_metadata = cfg->find_bb(*succ);
        if (bb_metadata) succ_bbs.push_back(bb_metadata);
    }
}

void BasicBlockMeta::set_def_use_values(){
    for (std::vector<InstructionMeta *>::iterator it = insts.begin(); it != insts.end(); ++it) {
        std::vector<llvm::Value *>operands = (*it)->get_operands();
        // remove debug information
        if ((*it)->get_raw_llvm().find("llvm.dbg") != std::string::npos) continue;
        // If instruction == call, save function name and remove it from operands
        if(llvm::CallInst * call_inst = dynamic_cast<llvm::CallInst *>((*it)->get_inst())){
            llvm::Function *fce = call_inst->getCalledFunction();
            if(fce->hasName()){
                std::string name = fce->getName().str();
                if (std::find(used_funcs.begin(), used_funcs.end(), name) == used_funcs.end())
                    used_funcs.push_back(fce->getName().str());
            }

        }
        // Iter over operands
        for (std::vector<llvm::Value *>::iterator val = operands.begin(); val != operands.end(); ++val){
            if ((*val)->hasName()){
                std::string name = (*val)->getName().str();
                // remove function names
                if (std::find(used_funcs.begin(), used_funcs.end(), name) != used_funcs.end()) continue;
                if (std::find(used_vars.begin(), used_vars.end(), name) == used_vars.end())
                    used_vars.push_back(name);
            }
        }
        std::string defined =(*it)->get_defined_variable();
        if (!defined.empty()){
            if (std::find(def_vars.begin(), def_vars.end(), defined) == def_vars.end())
                def_vars.push_back(defined);
        }
    }
}

std::string BasicBlockMeta::create_label_string(){
    // create raw_stream to print instruction into
    std::string raw_instruction;
    llvm::raw_string_ostream debug_stream(raw_instruction);
    bb->printAsOperand(debug_stream, false);
    // flush stream to string
    debug_stream.flush();
    // instruction contains leading whitespaces, remove it
    raw_instruction.erase(0, raw_instruction.find_first_not_of(" \t\n\r\f\v"));
    return raw_instruction;
}

int InstructionMeta::build() {
    built = true;
    id = MetadataWrapper::gen_id();
    // Transfer llvm::Instruction to human readable form
    raw_llvm = get_inst_string();
    if(llvm::Instruction * instruction = dynamic_cast<llvm::Instruction *>(value)){
        // Handle debug location
        if (llvm::DebugLoc debud_loc = instruction->getDebugLoc()){
            ploc = new ProgramLoc(instruction->getDebugLoc());
            if (ploc->build()) return 1;
        }
        // Get defined variable, if any
        unsigned int ops_count = instruction->getNumOperands();
        if(instruction->getOpcode() == llvm::Instruction::Store){
            if (!instruction->getOperand(1)->hasName()) return 0;
            char c = instruction->getOperand(1)->getName().str().at(0);
            if (isdigit(c)) return 0; // Temporary variables start with digit
            defined_variable = instruction->getOperand(1)->getName().str();
            ops_count--;
        }
        // Get operands
        for(unsigned int i = 0; i < ops_count; i++){
            llvm::Value * op = instruction->getOperand(i);
            operands.push_back(op);
        }
    }
    return 0;
}

std::string InstructionMeta::get_inst_string(){
    // create raw_stream to print instruction into
    std::string raw_instruction;
    llvm::raw_string_ostream debug_stream(raw_instruction);
    value->print(debug_stream, false);
    // flush stream to string
    debug_stream.flush();
    // instruction contains leading whitespaces, remove it
    raw_instruction.erase(0, raw_instruction.find_first_not_of(" \t\n\r\f\v"));
    return raw_instruction;
}

int ProgramLoc::build() {
    built = true;
    id = MetadataWrapper::gen_id();
    if (!loc){
        // MISSING DEBUG INFO
        return 0;
    }
    // More advanced debug information
    llvm::DILocation * di_loc = loc.get();

    source_file = di_loc ? di_loc->getFilename() : nullptr;
    line_min = loc.getLine();
    line_max = loc.getLine();
    col_begin = loc.getCol();
    col_end = loc.getCol();
    return 0;
}

void ProgramLoc::merge(ProgramLoc * new_loc) {
    // copy one to another (initialize)
    if(!built) {
        built = true;
        id = MetadataWrapper::gen_id();
        source_file = new_loc->get_source_file();
        line_min = new_loc->get_line_min();
        line_max = new_loc->get_line_max();
        col_begin = new_loc->get_col_begin();
        col_end = new_loc->get_col_end();
        return;
    }
    // edit the beginning
    if(new_loc->get_line_min() < line_min ||
            (new_loc->get_line_min() == line_min &&
             new_loc->get_col_begin() < col_begin)){
        line_min = new_loc->get_line_min();
        col_begin = new_loc->get_col_begin();
    }
    // edit the end
    if(new_loc->get_line_max() > line_max ||
            (new_loc->get_line_max() == line_max &&
             new_loc->get_col_end() > col_end)){
        line_max = new_loc->get_line_max();
        col_end = new_loc->get_col_end();
    }
}