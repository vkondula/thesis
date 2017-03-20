#include "jsonGen.hpp"

using json = nlohmann::json;

void JsonGen::set_stream(FILE *){
    // TODO
}

/*
 * Shared counter for all object inheriting from JsonWrapper
 * Allows referencing between objects and adds unique identifier
 */
int JsonWrapper::id_counter = 1;
int JsonWrapper::gen_id() {
    return id_counter++;
}

int JsonGen::build(llvm::Module * module){
    std::cerr << "got_it" << "\n";
    if (!module) return 0;
    ModuleJson mod(module);
    if (int retval = mod.build()) return retval;
    json output = mod.get_json();
    // TODO redirect to file stream
    std::cout << output.dump(4) << "\n";
    return 0;
}


int JsonGen::output(){
    // TODO
    return 0;
}


std::string JsonGen::get_output(){
    // TODO
    return "TODO";
}

json JsonWrapper::get_json(){
    if (!this->built){
        if(!this->build()){
            return nullptr;
        }
    }
    return this->scope;
}

std::string JsonWrapper::get_string() {
    if (!this->built){
        if(!this->build()){
            return nullptr;
        }
    }
    return this->scope.dump();
}

int ModuleJson::build() {
    this->built = true;
    this->id = this->scope["id"] = JsonWrapper::gen_id();
    json cfg_json = json::array();
    for (llvm::Module::iterator fce = this->module->begin(), end = this->module->end(); fce!=end; fce++){
        if (!&*fce) continue;
        CFGJson cfg(&*fce);
        if (int retval = cfg.build()){
            // ERROR occurred
            return retval;
        }
        cfg_json.push_back(cfg.get_json());
    }
    this->scope["cfg"] = cfg_json;
    return 0;
}

int CFGJson::build() {
    this->built = true;
    this->id = this->scope["id"] = JsonWrapper::gen_id();
    this->scope["file"] = {"TODO", "ADD_FILES"};
    ProgramLocJson ploc(nullptr); // program location of cfg
    std::vector<BasicBlockJson *> bb_objs;
    // 1st iteration... Create BB(generate id) and Instruction
    for (llvm::Function::iterator bb = this->fce->begin(), end = this->fce->end(); bb!=end; bb++){
        if (!&*bb) continue;
        BasicBlockJson * basic_block = new BasicBlockJson(&*bb);
        if (int retval = basic_block->build()){
            // ERROR occurred
            return retval;
        }
        // For next iterations
        bb_objs.push_back(basic_block);
        // Reversed map: (BB *)->(int)id
        this->add_bb(&*bb, basic_block->get_id());
    }
    json bb_json = json::array();
    json init_bb = json::array();
    json fini_bb = json::array();
    // 2nd iteration... Get successors and predecessors, get program location, entry and exit points
    for(std::vector<BasicBlockJson *>::size_type i = 0; i != bb_objs.size(); i++) {
        // Predecessors and Successors
        bb_objs[i]->set_predecessors(this);
        bb_objs[i]->set_successors(this);
        // Entry and Exit points
        if (bb_objs[i]->is_entry()) init_bb.push_back(bb_objs[i]->get_id());
        if (bb_objs[i]->is_exit()) fini_bb.push_back(bb_objs[i]->get_id());
        // Program location
        json curr_bb_json = bb_objs[i]->get_json();
        ploc.merge(curr_bb_json["ploc"]);
        bb_json.push_back(curr_bb_json);
    }
    // 3rd iteration... Dealocate BB objects
    for(std::vector<BasicBlockJson *>::size_type i = 0; i != bb_objs.size(); i++) {
        delete bb_objs[i];
    }
    this->scope["initBB"] = init_bb;
    this->scope["finiBB"] = fini_bb;
    this->scope["ploc"] = ploc.get_json();
    this->scope["basic_blocks"] = bb_json;
    return 0;
}

int CFGJson::find_bb(llvm::BasicBlock * bb){
    std::map<llvm::BasicBlock *, int>::iterator it;
    it = this->bb_ids.find(bb);
    if (it == this->bb_ids.end())
        // Not found!
        return 0;
    return it->second;
}

void CFGJson::add_bb(llvm::BasicBlock * bb, int id){
    this->bb_ids[bb] = id;
}

int BasicBlockJson::build() {
    this->built = true;
    this->id = this->scope["id"] = JsonWrapper::gen_id();
    ProgramLocJson ploc(nullptr); // program location of bb
    json ins_json = json::array(); // buffer for instruction json
    for (llvm::BasicBlock::iterator inst = this->bb->begin(), end = this->bb->end(); inst!=end; inst++){
        if (!&*inst) continue;
        InstructionJson instruction(&*inst);
        if (int retval = instruction.build()){
            // ERROR occurred
            return retval;
        }
        json curr_inst_json = instruction.get_json();
        ins_json.push_back(curr_inst_json);
        // Get BasicBlock program location
        ploc.merge(curr_inst_json["ploc"]);
    }
    this->scope["instructions"] = ins_json;
    this->scope["ploc"] = ploc.get_json();
    return 0;
}

void BasicBlockJson::set_predecessors(CFGJson * cfg){
    this->entry = true;
    json predecessors = json::array();
    for (llvm::pred_iterator pred = llvm::pred_begin(this->bb), end = llvm::pred_end(this->bb); pred != end; ++pred) {
        this->entry = false;
        int id = cfg->find_bb(*pred);
        if (id) predecessors.push_back(id);
    }
    this->scope["pred"] = predecessors;
}

void BasicBlockJson::set_successors(CFGJson * cfg){
    this->exit = true;
    json successors = json::array();
    for (llvm::succ_iterator succ = llvm::succ_begin(this->bb), end = llvm::succ_end(this->bb); succ != end; ++succ) {
        this->exit = false;
        int id = cfg->find_bb(*succ);
        if (id) successors.push_back(id);
    }
    this->scope["succ"] = successors;
}

int InstructionJson::build() {
    this->built = true;
    this->id = this->scope["id"] = JsonWrapper::gen_id();
    // Handle debug location
    ProgramLocJson ploc(this->inst->getDebugLoc());
    if (ploc.build()) return 1;
    this->scope["ploc"] = ploc.get_json();
    // Transfer llvm::Instruction to human readable form
    this->scope["raw_llvm"] = this->get_inst_string();
    return 0;
}

std::string InstructionJson::get_inst_string(){
    // create raw_stream to print instruction into
    std::string raw_instruction;
    llvm::raw_string_ostream debug_stream(raw_instruction);
    this->inst->print(debug_stream, false);
    // flush stream to string
    debug_stream.flush();
    // instruction contains leading whitespaces, remove it
    raw_instruction.erase(0, raw_instruction.find_first_not_of(" \t\n\r\f\v"));
    return raw_instruction;
}

int ProgramLocJson::build() {
    this->built = true;
    if (!this->loc){
        // MISSING DEBUG INFO
        this->scope = nullptr;
        return 0;
    }
    // More advanced debug information
    llvm::DILocation * di_loc = this->loc.get();

    this->scope["file"] = di_loc ? di_loc->getFilename() : nullptr;
    this->scope["line_begin"] = this->loc.getLine();
    this->scope["line_end"] = this->loc.getLine();
    this->scope["col_begin"] = this->loc.getCol();
    this->scope["col_end"] = this->loc.getCol();
    return 0;
}

void ProgramLocJson::merge(json new_loc) {
    if(this->scope.is_null()) {
        this->built = true;
        this->scope = new_loc;
        return;
    }
    if(new_loc["line_begin"] < this->scope["line_begin"] ||
            (new_loc["line_begin"] == this->scope["line_begin"] &&
             new_loc["col_begin"] < this->scope["col_begin"])){
        this->scope["line_begin"] = new_loc["line_begin"];
        this->scope["col_begin"] = new_loc["col_begin"];
    }
    if(new_loc["line_end"] > this->scope["line_end"] ||
            (new_loc["line_end"] == this->scope["line_end"] &&
             new_loc["col_end"] > this->scope["col_end"])){
        this->scope["line_end"] = new_loc["line_end"];
        this->scope["col_end"] = new_loc["col_end"];
    }
}