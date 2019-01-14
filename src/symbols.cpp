#include "symbols.hpp"
//See symbols.hpp for function descriptions


//Variable Class
Var::Var(const char* var_type, const char* var_id, const char* var_value) {
	if(var_type == NULL) {
		type = std::string("");
	} else {
		type = std::string(var_type);
	}
	if(var_id == NULL) {
		id = std::string("");
	} else {
		id = std::string(var_id);
	}
	if(var_value == NULL) {
		value = std::string("");
	} else {
		value = std::string(var_value);
	}
}

Var::Var(const char* var_type, const char* var_id) {
	if(var_type == NULL) {
		type = std::string("");
	} else {
		type = std::string(var_type);
	}
	if(var_id == NULL) {
		id = std::string("");
	} else {
		id = std::string(var_id);
	}
	value = std::string("");
}

Var::Var(std::string var_type, std::string var_id) {
	type = var_type;
	id = var_id;
	value = std::string("");
}

Var::Var(std::string var_type, std::string var_id, std::string var_value) {
	type = var_type;
	id = var_id;
	value = var_value;
}

Var::Var(Var* other_var) {
	if(other_var != NULL) {
		type = std::string(other_var -> type);
		id = std::string(other_var -> id);
		value = std::string(other_var -> value);
	}
}

Var::~Var() {}

void Var::print_var() {
	if(value.empty() == true) {
		std::cout << ";name " << id << " type " << type << std::endl;
	} else {
		std::cout << ";name " << id << " type " << type << " value " << value << std::endl;
	}
}

void Var::print_tiny() {
	if(!type.compare("STRING")) {
		std::cout << "str " << id << " " << value << std::endl;
	} else {
		std::cout << "var " << id << std::endl;
	}

}

//Symbol Table

std::string Symbol_Table::error = std::string("");
Symbol_Table* Symbol_Table::global = NULL;

Symbol_Table::Symbol_Table(const char* curr_scope, Symbol_Table* curr_parent) {
	scope = std::string(curr_scope);
	parent = curr_parent;
	args = 0;
}

Symbol_Table::Symbol_Table(const char* curr_scope, Symbol_Table* curr_parent, std::string type) {
	scope = std::string(curr_scope);
	parent = curr_parent;
	args = 0;
	return_type = type;
}

Symbol_Table::Symbol_Table(const char* curr_scope) { //Only for global declaration
	scope = std::string(curr_scope);
	parent = NULL;
	global = this;
}

Symbol_Table::~Symbol_Table() {}

std::string Symbol_Table::find_func_type(std::string func_name) {
	if(scope == func_name) {
		return return_type;
	}
	for(int i = 0; i < children.size(); i++) {
		std::string result = children[i] -> find_func_type(func_name);
		if(result.compare("")) {
			return result;
		}
	}
	return "";
}

void Symbol_Table::add_var(Var* new_var) {
	if(this -> search_ids((new_var -> id).c_str())) { //Check to see if var already declared here
		if(error.empty()) { //Only modifiy error if no error
			error = new_var -> id;
		}
	} else {
		vars.push_back(new_var);
	}
}

bool Symbol_Table::search_ids(const char* id) {
	for(int i = 0; i < vars.size(); i++) {
		if(!(vars[i] -> id).compare(id)) {
			return true;
		}
	}

	return false;
}

bool Symbol_Table::print_error() {
	if(!error.empty()) { //Only prints if error
		std::cout << "DECLARATION ERROR " << error << std::endl;
		return true;
	}
	return false;
}

void Symbol_Table::print_table() {
	if(!error.empty()) {
		std::cout << ";DECLARATION ERROR " << error << std::endl;
		return;
	}
	std::cout << ";Symbol table " << scope << std::endl;
	std::cout << ";Num args: " << args << std::endl;
	for(int i = 0; i < vars.size(); i++) {
		vars[i] -> print_var();
	}

	for(int i = 0; i < children.size(); i++) {
		std::cout << std::endl;
		children[i] -> print_table();
	}
}

void Symbol_Table::print_4_tiny_global() {
	for(int i = 0; i < vars.size(); i++) {
		vars[i] -> print_tiny();
	}
	for(int i = 0; i < children.size(); i++) {
		if(children[i] != NULL) {
			children[i] -> print_4_tiny();
		}
	}
}

void Symbol_Table::print_4_tiny() {
	for(int i = 0; i < vars.size(); i++) {
		if(vars[i] -> type == "STRING") {
			vars[i] -> print_tiny();
		}
	}
	for(int i = 0; i < children.size(); i++) {
		if(children[i] != NULL) {
			children[i] -> print_4_tiny();
		}
	}
}

Symbol_Table* Symbol_Table::make_child(const char* scope) {
	Symbol_Table* new_table = new Symbol_Table(scope, this);
	children.push_back(new_table);
	return new_table;
}

Symbol_Table* Symbol_Table::make_child(const char* scope, std::string type) {
	Symbol_Table* new_table = new Symbol_Table(scope, this, type);
	children.push_back(new_table);
	return new_table;
}

Symbol_Table* Symbol_Table::search_scope(std::string search_s) {
	if(!scope.compare(search_s)) {
		return this;
	} else {
		for(int i = 0; i < children.size(); i++) {
			Symbol_Table* addr = children[i] -> search_scope(search_s);
			if(addr != NULL) {
				return addr;
			}
		}
	}
	return NULL;
}

Var* Symbol_Table::find_var(std::string var_id) {
	//std::cout << ";Fetching var " << var_id << " from " << scope << std::endl; 
	for(int i = 0; i < vars.size(); i++) { //Begins searching in own scope, then goes up
		if(!var_id.compare(vars[i] -> id)) {
			return vars[i];
		}
	}

	if(parent != NULL) {
		return parent -> find_var(var_id);
	} else {
		return NULL;
	}
}

Var* Symbol_Table::find_var(std::string var_id, bool allow_global) {
	//std::cout << ";Fetching var " << var_id << " from " << scope << std::endl; 
	if(!allow_global && (!scope.compare("global"))) {
		return NULL;
	}
	for(int i = 0; i < vars.size(); i++) { //Begins searching in own scope, then goes up
		if(!var_id.compare(vars[i] -> id)) {
			return vars[i];
		}
	}

	if(parent != NULL) {
		return parent -> find_var(var_id);
	} else {
		return NULL;
	}
}

int Symbol_Table::get_local_var_length() {
	int length = 0;
	
	for(unsigned int i = 0; i < vars.size(); i++) {
		if(!vars[i] -> type.compare("STRING")) {
			length = vars[i] -> value.size() + 1;
		} else {
		 	length++;
		}
	}

	for(unsigned int i = 0; i < children.size(); i++) {
		length += children[i] -> get_local_var_length();
	}
	return length;
}

void Symbol_Table::inc_args() {
	args++;
}

int Symbol_Table::get_link_num() { 
	return this -> get_scope_size() - args + 1;
}

int Symbol_Table::get_scope_size() {
	int curr_size = (int) vars.size();
	for(unsigned int i = 0; i < children.size(); i++) {
		curr_size += children[i] -> get_scope_size();
	}
	return curr_size;
}

std::string Symbol_Table::get_ref_id(std::string var_id, std::string type) {
	if(type == "STRING") {
		return var_id;
	}
	
	//std::cout << "Var ID: " << var_id << std::endl;
	if(var_id[0] >= '0' && var_id[0] <= '9') {
		return var_id;
	} else if (var_id[0] == 'r' && var_id.length() >= 2) {
		bool is_register = true;
		for(unsigned int i = 1; i < var_id.length(); i++) {
			if(!(var_id[i] >= '0' && var_id[i] <= '9')) {
				is_register = false;
			}
		}
		if(is_register) {
			return var_id;
		}
		
	}
	
	for(unsigned int i = 0; i < global -> vars.size(); i++) {
		if(!var_id.compare(global -> vars[i] -> id)) {
			if(this -> find_var(var_id, false) == NULL) {
				return var_id;
			}
		}
	}
	
	int crawling_index = 0;
	int curr_index = this -> get_ref_id(var_id, type, &crawling_index);
		
	if(curr_index < 0) {
		return var_id;
	} else if (curr_index < args) {
		curr_index = -1 * (curr_index - args - 4 - 1);	
	} else {
		curr_index = -1 * (curr_index - args + 2);
	}
	
	return std::string("$") + std::to_string((long long int) curr_index);
}

int Symbol_Table::get_ref_id(std::string var_id, std::string type, int* curr_index) {
	for(unsigned int i = 0; i < vars.size(); i++) {
		if(!var_id.compare(vars[i] -> id)) {
			return *curr_index + i;
		}
	}
	
	*curr_index = *curr_index + (int) vars.size();
	
	for(unsigned int i = 0; i < children.size(); i++) {
		int new_index = children[i] -> get_ref_id(var_id, type, curr_index);
		if(new_index >= 0) {
			return new_index;
		}
	}
	
	return -1;
}


//ASTNode
//Why are there so many declarations of ASTNode you ask, Look a bird --->
//In all seriousness, different situations meant I needed different things filled into the ASTNode,
//can it be reduced? YES, Do I want to do that? Not really
ASTNode::ASTNode(char op_in, std::string scope_in) {
	op = op_in;
	scope = scope_in;
	right_child = NULL;
	left_child = NULL;
	id = "";
	compop = NULL;
	label = "null";
	ctrl = '\0';
}

ASTNode::ASTNode(const char * op_in, std::string scope_in) { // cond Node
	op = 'c';
	scope = scope_in;
	right_child = NULL;
	left_child = NULL;
	id = "";
	compop = (char*) op_in;
	label = "null";
	ctrl = '\0';
}

ASTNode::ASTNode(char op_in, ASTNode* right, std::string scope_in) {
	op = op_in;
	scope = scope_in;
	right_child = right;
	left_child = NULL;
	id = "";
	compop = NULL;
	label = "null";
	ctrl = '\0';
}

ASTNode::ASTNode(char op_in, std::string scope_in, const char* id_in) {
	op = op_in;
	scope = scope_in;
	right_child = NULL;
	left_child = NULL;
	id = std::string(id_in);
	compop = NULL;
	label = "null";
	ctrl = '\0';
}

ASTNode::ASTNode(std::string scope_in, const char* id_in) {
	op = '\0';
	scope = scope_in;
	right_child = NULL;
	left_child = NULL;
	id = std::string(id_in);
	compop = NULL;
	label = "null";
	ctrl = '\0';
}

ASTNode::ASTNode(std::string scope_in, const char* id_in, int i, const char* func_id) {
	op = '\0';
	scope = scope_in;
	right_child = NULL;
	left_child = NULL;
	id = std::string(id_in);
	compop = NULL;
	label = std::string(func_id);
	ctrl = '\0';
}

ASTNode::ASTNode(std::string scope_in, std::string id_in) {
	op = '\0';
	scope = scope_in;
	right_child = NULL;
	left_child = NULL;
	id = id_in;
	compop = NULL;
	label = "null";
	ctrl = '\0';
}

ASTNode::ASTNode(std::string scope_in, const char* id_in, const char* id_2) {
	op = '\0';
	scope = scope_in;
	right_child = NULL;
	left_child = NULL;
	id = std::string(id_in) + std::string(id_2);
	compop = NULL;
	label = "null";
	ctrl = '\0';
}

ASTNode::ASTNode(std::string scope_in, const char* id_in, const char* id_2, int i) {
	op = 'l';
	scope = scope_in;
	right_child = NULL;
	left_child = NULL;
	id = std::string(id_in);
	compop = NULL;
	if (i == 1) {
		label = std::string(id_2);
	} else if (i == 2) {
		label = std::string("FUNC_") + std::string(id_2);
	}
	ctrl = '\0';
}

ASTNode::~ASTNode() {}

void ASTNode::add_right(ASTNode* new_node) {
	if(right_child != NULL) {
		right_child -> add_right(new_node); //Keeps adding right until NULL because of write
	} else {
		right_child = new_node;
	}
}

void ASTNode::add_left(ASTNode* new_node) {
	if(left_child != NULL) {
		left_child -> add_left(new_node); //Same on left but not used
	} else {
		left_child = new_node;
	}
}

void ASTNode::add_id(const char* new_id) {
	id = std::string(new_id);
}

void ASTNode::print_tree() {
	std::cout << "( ";
	if(left_child != NULL) {
		left_child -> print_tree();
	}
	if((op == 'w') || (op == 'r')) {
		std::cout << op << ": " << id;
	} else if (!id.empty()) {
		std::cout << id;
	} else {
		std::cout << op;
	}
	if(right_child != NULL) {
		right_child -> print_tree();
	}
	std::cout << " )";
}

void ASTNode::generate_code(TempRegs& regs, std::vector<ThreeAC*>& instructions, Symbol_Table* global_table, std::vector<Var*>& return_FIFO) {
	if(right_child == NULL && left_child == NULL && (op == '\0' || op == 'l')) {
		if(op == '\0' && !id.compare(";POPR")) {
			Var* temp_reg = regs.generate_temp_reg(global_table -> find_func_type(label), id);
			return_FIFO.push_back(temp_reg);
			instructions.push_back(new ThreeAC(scope, ";POP", '\0', temp_reg));
			return;
		} else if(op == '\0') {
			instructions.push_back(new ThreeAC(scope,id));
			return;
		}
		else if(op == 'l') {
			instructions.push_back(new ThreeAC(scope,id, label));
			return;
		}
	} else {
		this -> generate_code_priv(regs, instructions, global_table, return_FIFO); //Call to normal recursion
	}
}

Var* ASTNode::generate_code_priv(TempRegs& regs, std::vector<ThreeAC*>& instructions, Symbol_Table* global_table, std::vector<Var*>& return_FIFO) {
	//All the fun is in this function
	if(op == '\0') { //for all non functions, will either return its id to be an arg, or store the value then return the temp reg it stored it in		
		std::string str_type = is_number(id);
		char type = make_type_char(str_type);
		if(id == ";POPR") {
			Var* temp = return_FIFO.front();
			return_FIFO.erase(return_FIFO.begin());
			return temp;
		} else if(type != '\0') {
			Var* base_arg = new Var(str_type, id, id);
			Var* temp_reg = regs.generate_temp_reg(str_type, id);
			instructions.push_back(new ThreeAC(scope,";STORE", type, temp_reg, base_arg));
			return temp_reg;
		} else {
			return global_table -> search_scope(scope) -> find_var(id);
		}
	}

	Var* arg1 = NULL;
	Var* arg2 = NULL;

	if(op == 'r' || op == 'w') { //Read or write (contains both the op 'w' or 'r' and the id to be stored, right child is another read/write)
		Var* id_var = global_table -> search_scope(scope) -> find_var(id);
		instructions.push_back(new ThreeAC(scope,this -> get_str_op(), make_type_char(id_var -> type), id_var));
		if(right_child != NULL) {
			right_child -> generate_code_priv(regs, instructions, global_table, return_FIFO);
		}
		return NULL;
	}
	else if (op == 'l') {
		if(right_child != NULL) {
			right_child -> generate_code_priv(regs, instructions, global_table, return_FIFO);
		}
		return NULL;
	}
	else if (op == 'f') { //The crash happens in here!!!
		if(right_child != NULL) {
			right_child -> generate_code_priv(regs, instructions, global_table, return_FIFO); //In this call, so need to keep looking
		}
		return NULL;
	}
	else if (op == 'c') { //The crash leads here
		if(left_child != NULL) {
			arg1 = left_child -> generate_code_priv(regs, instructions, global_table, return_FIFO);
			if (arg1 -> id[0] != '!')  {
				Var* temp_reg = regs.generate_temp_reg(arg1 -> type, id);
				instructions.push_back(new ThreeAC(scope,";STORE", make_type_char(arg1 -> type), temp_reg, arg1));
				arg1 = temp_reg;
			}
		}
		if(right_child != NULL) {
			arg2 = right_child -> generate_code_priv(regs, instructions, global_table, return_FIFO);
			if (arg2 -> id[0] != '!')  {
				Var* temp_reg = regs.generate_temp_reg(arg2 -> type, id);
				instructions.push_back(new ThreeAC(scope,";STORE", make_type_char(arg2 -> type), temp_reg, arg2));
				arg2 = temp_reg;
			}
		}
		if (ctrl == 'l') { // cond node is the child of WHILE node
			std::string lbl = "WHILE_END_";
			lbl += (std::to_string((long long int) std::stoi(scope) + 1));
			if (strcmp(compop, "!=") == 0) {
				instructions.push_back(new ThreeAC(scope,";EQ", make_type_char(arg1 -> type), arg1, arg2, lbl));
			}
			else if (strcmp(compop, "=") == 0) {
				instructions.push_back(new ThreeAC(scope,";NE", make_type_char(arg1 -> type), arg1, arg2, lbl));
			}
			else if (strcmp(compop, "<") == 0) {
				instructions.push_back(new ThreeAC(scope,";GE", make_type_char(arg1 -> type), arg1, arg2, lbl));
			}
			else if (strcmp(compop, ">") == 0) {
				instructions.push_back(new ThreeAC(scope,";LE", make_type_char(arg1 -> type), arg1, arg2, lbl));
			}
			else if (strcmp(compop, "<=") == 0) {
				instructions.push_back(new ThreeAC(scope,";GT", make_type_char(arg1 -> type), arg1, arg2, lbl));
			}
			else if (strcmp(compop, ">=") == 0) {
				instructions.push_back(new ThreeAC(scope,";LT", make_type_char(arg1 -> type), arg1, arg2, lbl));
			}
		}
		else if (ctrl == 'f') { // cond node is the child of IF node
			std::string lbl = "ELSE_";
			lbl += scope;
			if (strcmp(compop, "!=") == 0) {
				instructions.push_back(new ThreeAC(scope,";EQ", make_type_char(arg1 -> type), arg1, arg2, lbl));
			}
			else if (strcmp(compop, "=") == 0) {
				instructions.push_back(new ThreeAC(scope,";NE", make_type_char(arg1 -> type), arg1, arg2, lbl));
			}
			else if (strcmp(compop, "<") == 0) {
				instructions.push_back(new ThreeAC(scope,";GE", make_type_char(arg1 -> type), arg1, arg2, lbl));
			}
			else if (strcmp(compop, ">") == 0) {
				//std::cout << "Before Instruction" << std::endl;
				instructions.push_back(new ThreeAC(scope,";LE", make_type_char(arg1 -> type), arg1, arg2, lbl)); //Crash search step 3, happens in the push?
				//std::cout << "After Instruction" << std::endl;
			}
			else if (strcmp(compop, "<=") == 0) {
				instructions.push_back(new ThreeAC(scope,";GT", make_type_char(arg1 -> type), arg1, arg2, lbl));
			}
			else if (strcmp(compop, ">=") == 0) {
				instructions.push_back(new ThreeAC(scope,";LT", make_type_char(arg1 -> type), arg1, arg2, lbl));
			}
		}
		return NULL;
	}
	else if (op == 's') {//Triggers on the tree for pushing args onto stack in function call
		if(left_child != NULL) {
			arg1 = left_child -> generate_code_priv(regs, instructions, global_table, return_FIFO);
		}
		instructions.push_back(new ThreeAC(scope,";PUSH", '\0', arg1));
		if(right_child != NULL) {
			right_child -> generate_code_priv(regs, instructions, global_table, return_FIFO);
		}
		return NULL;
	}
	else if (op == 'L') { //Triggered by the link command,
		Var* link_constant = new Var("INT", std::to_string((long long int) global_table -> search_scope(scope) -> get_link_num()), std::to_string((long long int) global_table -> search_scope(scope) -> get_link_num()));
		instructions.push_back(new ThreeAC(scope,";LINK", '\0', link_constant));
		return NULL;
	}
	else if (op == 'O') {
		int num_args = global_table -> search_scope(scope) -> args;
		std::cout << ";Scope: " << scope << std::endl;
		std::cout << ";Num Args: " << num_args << std::endl;
		for(int i = 0; i < num_args; i++) {
			instructions.push_back(new ThreeAC(scope,";POP"));
		}
		return NULL;
	}
	else if (op == 'R') {
		if(right_child != NULL) {
			arg2 = right_child -> generate_code_priv(regs, instructions, global_table, return_FIFO);
		}
		if(arg2 != NULL) {
			if (arg2 -> id[0] != '!')  {
				Var* temp_reg = regs.generate_temp_reg(arg2 -> type, id);
				instructions.push_back(new ThreeAC(scope,";STORE", make_type_char(arg2 -> type), temp_reg, arg2));
				arg2 = temp_reg;
			}
			Symbol_Table* func_table = global_table -> search_scope(scope);
			while((func_table -> scope[0] >= 48) && (func_table -> scope[0] <= 57)) {
				func_table = func_table -> parent;
			}
			int return_location = func_table -> args + 2 + 4; //2(no idea) + 4 regs + # of args for the function is the location of the return address
			if(scope == "main") {
				return_location  = 6;
			}
			std::string return_loc_id = std::string("$") + std::to_string((long long int) return_location);
			Var* return_target = new Var(arg2 -> type, return_loc_id);
			instructions.push_back(new ThreeAC(scope,";STORE", make_type_char(arg2 -> type), return_target, arg2));
		}
		return NULL;
	}

	if(left_child != NULL) { //Calling children for args
		arg1 = left_child -> generate_code_priv(regs, instructions, global_table, return_FIFO);
	}
	if(right_child != NULL) {
		arg2 = right_child -> generate_code_priv(regs, instructions, global_table, return_FIFO);
	}

	std::string operation = this -> get_str_op();

	if (op == ':') { //':' is for :=
		if(arg1 != NULL && arg2 == NULL) { //If this assign is a return address
			Var* temp_reg = regs.generate_temp_reg(arg1 -> type, arg1 -> id);
			instructions.back() -> result = temp_reg;
			instructions.push_back(new ThreeAC(scope,";STORE", make_type_char(arg1 -> type), arg1, temp_reg));
			return NULL;
		}
		if(arg1 == NULL || arg2 == NULL) {
			instructions.push_back(new ThreeAC(scope,"ERROR: An arg is NULL"));
			return NULL;
		}
		if(!arg1 -> type.compare(arg2 -> type) || (arg2 -> type.empty())) {
			if(arg2 -> id[0] != '!') {
				Var* temp_reg = regs.generate_temp_reg(arg1 -> type, arg2 -> id);
				instructions.push_back(new ThreeAC(scope,";STORE", make_type_char(arg2 -> type), temp_reg, arg2)); //This is getting called twice, this if needs to be refined not to catch normal stores
				arg2 = temp_reg;
			}
			instructions.push_back(new ThreeAC(scope,operation, make_type_char(arg1 -> type), arg1, arg2));
			return NULL;
		} else {
			instructions.push_back(new ThreeAC(scope,"ERROR: Assignment types don't match"));
			return NULL;
		}
	} else if (op == '+' || op == '-' || op == '*' || op == '/') {
		if(arg1 == NULL || arg2 == NULL) {
			instructions.push_back(new ThreeAC(scope,"ERROR: Expression arg is NULL"));
			return NULL;
		}
		if(!arg1 -> type.compare(arg2 -> type)) {
			Var* target = regs.generate_temp_reg(arg1->type);
			instructions.push_back(new ThreeAC(scope,operation, make_type_char(arg1 -> type), target, arg1, arg2));
			return target;
		} else {
			instructions.push_back(new ThreeAC(scope,"ERROR: Explosion types don't match"));
			return NULL;
		}
	}

	return NULL;
}

std::string ASTNode::get_str_op() {
	if(op == '\0') {
		return "";
	} else if(op == 'w') {
		return ";WRITE";
	} else if(op == 'r') {
		return ";READ";
	} else if(op == '+') {
		return ";ADD";
	} else if(op == '-') {
		return ";SUB";
	} else if(op == '*') {
		return ";MUL";
	} else if(op == '/') {
		return ";DIV";
	} else if(op == ':') {
		return ";STORE";
	}
	return 0;
}


//Temp Reg
TempRegs::TempRegs() {}
TempRegs::~TempRegs() {}

Var* TempRegs::generate_temp_reg(std::string type) {
	std::string new_id = "!T" + std::to_string((long long int) ids.size());
	Var* new_reg = new Var(type, new_id);
	ids.push_back(new_reg);
	return new_reg;
}

Var* TempRegs::generate_temp_reg(std::string type, std::string value) {
	std::string new_id = "!T" + std::to_string((long long int) ids.size());
	Var* new_reg = new Var(type, new_id, value);
	ids.push_back(new_reg);
	return new_reg;
}

Var* TempRegs::find_entry(std::string search_id) {
	for(int i = 0; i < ids.size(); i++) {
		if(!search_id.compare(ids[i] -> id)) {
			return ids[i];
		}
	}
	return NULL;
}

//ThreeAC

ThreeAC::ThreeAC(std::string scope_in, std::string com) {
	scope = scope_in;
	command = std::string(com);
	out_type = '\0';
	arg1 = NULL;
	arg2 = NULL;
	result = NULL;
	label = "null";
	living_vars = {};
}
ThreeAC::ThreeAC(std::string scope_in, std::string com, std::string lbl) {
	scope = scope_in;
	command = std::string(com);
	out_type = '\0';
	arg1 = NULL;
	arg2 = NULL;
	result = NULL;
	label = lbl;
	living_vars = {};
}
ThreeAC::ThreeAC(std::string scope_in, std::string com, char ty, Var* tar) {
	scope = scope_in;
	command = std::string(com);
	out_type = ty;
	arg1 = NULL;
	arg2 = NULL;
	result = new Var(tar);
	label = "null";
	living_vars = {};
}
ThreeAC::ThreeAC(std::string scope_in, std::string com, char ty, Var* tar, Var* a1) {
	scope = scope_in;
	command = std::string(com);
	out_type = ty;
	arg1 = new Var(a1);
	arg2 = NULL;
	result = new Var(tar);
	label = "null";
	living_vars = {};
}
ThreeAC::ThreeAC(std::string scope_in, std::string com, char ty, Var* tar, Var* a1, Var* a2){
	scope = scope_in;
	command = std::string(com);
	out_type = ty;
	arg1 = new Var(a1);
	arg2 = new Var(a2);
	result = new Var(tar);
	label = "null";
	living_vars = {};
}
ThreeAC::ThreeAC(std::string scope_in, std::string com, char ty, Var* a1, Var* a2, std::string tar){
	scope = scope_in;
	command = std::string(com);
	out_type = ty;
	if(a1 != NULL) {
		arg1 = new Var(a1);
	} else {
		arg1 = NULL;
	}
	if(a2 != NULL) {
		arg2 = new Var(a2);
	} else {
		arg2 = NULL;
	}
	result = NULL;
	label = tar;
	living_vars = {};
}
ThreeAC::~ThreeAC() {}

void ThreeAC::print_command() {//Doesn't error handle, if there is an error, 3AC object only has an ID of "ERROR"
	std::string nc ("null");
	std::cout << command;
	if(out_type != '\0') {
		std::cout << out_type;
	}
	if(arg1 != NULL) {
		std::cout << " " << arg1 -> id;
	}
	if(arg2 != NULL) {
		std::cout << " " << arg2 -> id;
	}
	if(result != NULL) {
		std::cout << " " << result -> id;
	}
	if(label.compare(nc) != 0) {
		std::cout << " " << label;
	}
	std::cout << std::endl;
}

void ThreeAC::print_living_vars() {
	std::cout << ';';
	if(living_vars.empty()) {
		std::cout << "No living vars!" << std::endl;
		return;
	}
	for(std::set<std::string>::iterator it = living_vars.begin(); it != living_vars.end(); ++it) {
		std::cout << " " << (*it);
	}
	std::cout << std::endl;
}

void ThreeAC::generate_living_vars(ThreeAC* prev) {
	if(prev != NULL) {
		living_vars = std::set<std::string> (prev -> living_vars);	
		//If previous is a read, delete target
		//if prev is a write, add the target
		if(!prev -> command.compare(";WRITE") || !prev -> command.compare(";READ")) {
			if(!prev -> command.compare(";WRITE") && !is_number(prev -> result -> id).compare("")) {
				living_vars.insert(prev -> result -> id);
			} else {
				living_vars.erase(prev -> result -> id);
			}
		} else if (prev -> command.compare("") ) {
			//Remove prev's target
			if(prev -> result != NULL) {
				living_vars.erase(prev -> result -> id);	
			}
			
			//Add prev's arguemnts
			if(prev -> arg1 != NULL && !is_number(prev -> arg1 -> id).compare("")) {
				living_vars.insert(prev -> arg1 -> id);
			}

			if(prev -> arg2 != NULL && !is_number(prev -> arg2 -> id).compare("")) {
				living_vars.insert(prev -> arg2 -> id);
			}
		}
	}	
}

//Regs
Regs::Regs(int limit) {
	num_regs = limit;
	reg_vector = std::vector<Var*>(limit);
	str_vector = std::vector<std::string>(limit);
	curr_reg = 0;
}
Regs::~Regs() {}

Var* Regs::allocate_reg(Var* write_reg) {
	Var* found_reg = find_prev_reg(write_reg);
	if(found_reg != NULL) {
		return found_reg;
	}
	Var* new_reg = new Var(write_reg);
	new_reg -> id = "r" + std::to_string((long long int) curr_reg);
	reg_vector[curr_reg] = new_reg; //Be sure to remember that str_vector exists (or fix it to be a vector of a 4-field var)
	str_vector[curr_reg] = write_reg -> id;
	curr_reg = (curr_reg + 1) % num_regs;
	return new_reg;
}

Var* Regs::find_prev_reg(Var* search_var) {
	for(int i = 0; i < num_regs; i++) {
		if(!str_vector[i].compare(search_var -> id)) {
			return reg_vector[i];
		}
	}
	return NULL;
}

//Tiny
Tiny::Tiny(ThreeAC* ac_instruct, Regs& regs) {
	//convert command
	scope = ac_instruct -> scope;
	label = ac_instruct -> label;
	command = convert_command(ac_instruct -> command);

	out_type = ac_instruct -> out_type + 32;
	if(ac_instruct -> out_type == 'F') {
		out_type = 'r';
	}
	if(ac_instruct -> result == NULL || !(ac_instruct -> result -> id.compare(""))) {
		result = NULL;
	} else {
		if(ac_instruct -> result -> id[0] == '!') { //is a temp reg
			result = regs.allocate_reg(ac_instruct -> result);
		} else { //Not a temp reg
			result = new Var(ac_instruct -> result);
		}
	}
	if(ac_instruct -> arg1 == NULL || !(ac_instruct -> arg1 -> id.compare(""))) {
		arg1 = NULL;
	} else {
		if(ac_instruct -> arg1 -> id[0] == '!') { //is a temp reg
			arg1 = regs.allocate_reg(ac_instruct -> arg1); //Arg 1 was acting dumb here, it fixed itself but I still have no idea what was happening
		} else { //Not a temp reg
			arg1 = new Var(ac_instruct -> arg1);
		}
	}
	if(ac_instruct -> arg2 == NULL || !(ac_instruct -> arg2 -> id.compare(""))) {
		arg2 = NULL;
	} else {
		if(ac_instruct -> arg2 -> id[0] == '!') { //is a temp reg
			arg2 = regs.allocate_reg(ac_instruct -> arg2);
		} else { //Not a temp reg
			arg2 = new Var(ac_instruct -> arg2);
		}
	}

}

void Tiny::print_tiny_code(Symbol_Table* global_table) {
	Symbol_Table* func_table = global_table -> search_scope(scope);
	while(func_table != NULL && (func_table -> scope[0] >= 48) && (func_table -> scope[0] <= 57)) {
		func_table = func_table -> parent;
	}
	
	if(!command.compare("")){
		return;
	}
	if(!command.compare("add") || !command.compare("sub") || !command.compare("mul") || !command.compare("div")) { //Because these commands take 2 steps
		std::cout << "move " << func_table -> get_ref_id(arg1 -> id, arg1 -> type) << " " << func_table -> get_ref_id(result -> id, result -> type) << std::endl;
		std::cout << command << out_type << " " << func_table -> get_ref_id(arg2 -> id, arg2 -> type) << " " << func_table -> get_ref_id(result -> id, result -> type) << std::endl;
	}
	else if(!command.compare("jeq") || !command.compare("jne") || !command.compare("jge") || !command.compare("jle") || !command.compare("jgt") || !command.compare("jlt")) {
		std::cout << "cmp" << out_type << " " << func_table -> get_ref_id(arg1 -> id, arg1 -> type) << " " << func_table -> get_ref_id(arg2 -> id, arg2 -> type) << std::endl;
		std::cout << command << " " << label << std::endl;
	}
	else if( !command.compare("jsr") ) {
		std::cout << command << " " << label << std::endl;
	}
	else if( !command.compare("label") ) {
		std::cout << command << " " << label << std::endl;
	}
	else if( !command.compare("jmp") ) {
		std::cout << command << " " << label << std::endl;
	}
	else if( !command.compare("popregs")) {
		std::cout << "pop r3" << std::endl;
		std::cout << "pop r2" << std::endl;
		std::cout << "pop r1" << std::endl;
		std::cout << "pop r0" << std::endl;
	}
	else if( !command.compare("pushregs")) {
		std::cout << "push r0" << std::endl;
		std::cout << "push r1" << std::endl;
		std::cout << "push r2" << std::endl;
		std::cout << "push r3" << std::endl;
	}
	else {
		std::cout << command;
		if(out_type != '\0' && command.compare("move")) {
			std::cout << out_type;
		}
		if(arg1 != NULL) {
			std::cout << " " << func_table -> get_ref_id(arg1 -> id, arg1 -> type);
		}
		if(arg2 != NULL) {
			std::cout << " " << func_table -> get_ref_id(arg2 -> id, arg2 -> type);
		}
		if(result != NULL) {
			std::cout << " " << func_table -> get_ref_id(result -> id, result -> type);
		}
		std::cout << std::endl;
	}
}

Tiny::~Tiny() {}

//Other Functions
ASTNode* real_pop(std::vector<ASTNode*>& node_vector) {
	ASTNode* node = node_vector.back();
	node_vector.pop_back();
	return node;
}

char get_back_op(std::vector<ASTNode*> node_vector) {
	if(node_vector.empty()) {
		return '\0';
	} else {
		return node_vector.back() -> op;
	}
}

std::string is_number(const std::string& s) {
	int dot = 0;
	for(int i = 0; i < s.length(); i++) {
		if(s[i] == '.') {
			dot++;
			if(dot > 1) {
				return "";
			}
		} else if(!std::isdigit(s[i])) {
			return "";
		}
	}
	if(dot == 1) {
		return "FLOAT";
	} else {
		return "INT";
	}
}

void append_string(std::string& out_str, std::string add_str) {
	if(out_str.substr(0,5).compare("ERROR")) {
		out_str = out_str + add_str;
	}
}

char make_type_char(std::string& type_str) {
	if(!type_str.compare("")) {
		return '\0';
	}
	char type = type_str.c_str()[0];
	return type;
}

std::string convert_command(std::string ThreeAC_com) {
	if(!ThreeAC_com.compare(";WRITE")) {
		return "sys write";
	} else if(!ThreeAC_com.compare(";READ")) {
		return "sys read";
	} else if(!ThreeAC_com.compare(";ADD")) {
		return "add";
	} else if(!ThreeAC_com.compare(";SUB")) {
		return "sub";
	} else if(!ThreeAC_com.compare(";MUL")) {
		return "mul";
	} else if(!ThreeAC_com.compare(";DIV")) {
		return "div";
	} else if(!ThreeAC_com.compare(";STORE")) {
		return "move";
	} else if(!ThreeAC_com.compare(";PUSH")) {
		return "push";
	} else if(!ThreeAC_com.compare(";POP")) {
		return "pop";
	} else if(!ThreeAC_com.compare(";HALT")) {
		return "sys halt";
	} else if(!ThreeAC_com.compare(";RET")) {
		return "unlnk\nret";
	} else if(!ThreeAC_com.substr(0,4).compare(";JSR")) {
		return "jsr";
	} else if(!ThreeAC_com.substr(0,6).compare(";LABEL")) {
		return "label";
	} else if(!ThreeAC_com.substr(0,5).compare(";LINK")) {
		return "link";
	} else if(!ThreeAC_com.substr(0,5).compare(";JUMP")) {
		return "jmp";
	} else if(!ThreeAC_com.compare(";EQ")) {
		return "jeq";
	} else if(!ThreeAC_com.compare(";NE")) {
		return "jne";
	} else if(!ThreeAC_com.compare(";GE")) {
		return "jge";
	} else if(!ThreeAC_com.compare(";LE")) {
		return "jle";
	} else if(!ThreeAC_com.compare(";GT")) {
		return "jgt";
	} else if(!ThreeAC_com.compare(";LT")) {
		return "jlt";
	} else if(!ThreeAC_com.compare(";POPREGS")) {
		return "popregs";
	} else if(!ThreeAC_com.compare(";PUSHREGS")) {
		return "pushregs";
	} else {
		return "";
	}
}
