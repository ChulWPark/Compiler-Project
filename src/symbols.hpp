#ifndef SYMBOLS_H_
#define SYMBOLS_H_

#include <cstring>
#include <vector>
#include <iostream>
#include <string>
#include <set>

class Symbol_Table;

class Var {
//Type created by parser each time and identifier is found, containers value, identifier, and type
public:
	Var(const char*, const char*, const char*); //with value
	Var(const char*, const char*); //without value
	Var(std::string, std::string); //without value
	Var(std::string, std::string, std::string); //without value
	Var(Var*); //Used for creating copies for 3AC and Tiny variables
	virtual ~Var();

	std::string type;
	std::string id;
	std::string value;

	//Methods
	void print_var();  //For debugging
	void print_tiny(); //Prints the VAR info in the correct form for tiny code
};

class TempRegs {
//Stores which temp register have been allocated and what type they contain (and value if applicable)
public:
	TempRegs();
	virtual ~TempRegs();
	
	std::vector<Var*> ids;

	Var* generate_temp_reg(std::string); //Creates a new temp reg, storing the ID number and the information about it
	Var* generate_temp_reg(std::string, std::string); //Also stores value (which can be used for optimization, only used for literals right now)
	Var* find_entry(std::string); //compares returns the full var when given an ID
};

class Regs {
//Stores the contents of the register in a class
public:
	Regs(int);
	~Regs();
	
	int num_regs;
	std::vector <Var*> reg_vector; //Stores the var where ID is of the form "r0" or the like
	std::vector<std::string> str_vector; //Stores string to remember what was stored in the register, used when using the same register a second time
	int curr_reg;
	
	Var* allocate_reg(Var*); //Assigns the register for use and returns it
	Var* find_prev_reg(Var*); //Checks to see if the value of the Var given matches something in the regs
};

class ThreeAC {
//Stores all the information for each of the 3AC commands
public:
	ThreeAC(std::string, std::string);
	ThreeAC(std::string, std::string, std::string);
	ThreeAC(std::string, std::string, char, Var*);
	ThreeAC(std::string, std::string, char, Var*, Var*);
	ThreeAC(std::string, std::string, char, Var*, Var*, Var*);
	ThreeAC(std::string, std::string, char, Var*, Var*, std::string);
	virtual ~ThreeAC();
	
	std::string scope;
	std::string command;
	std::set <std::string> living_vars;
	char out_type;
	Var* arg1;
	Var* arg2;
	Var* result;
	std::string label;
	
	void print_command(); //Prints the commands for 3AC
	void generate_living_vars(ThreeAC*);
	void print_living_vars();
};

class Tiny {
//Stores all the information for Tiny code, very similar to 3AC, but prints differently and uses real regs
public:
	Tiny(ThreeAC*, Regs&);
	virtual ~Tiny();
	
	std::string command;
	char out_type;
	Var* arg1;
	Var* arg2;
	Var* result;
	std::string label;
	std::string scope;
	
	void print_tiny_code(Symbol_Table*);
};

class Symbol_Table {
//Stores the symbol table, the scopes are formulated as a tree, so a while loop's symbol table will be in the children vector of the global symbol table
//The variables are stored within each scope's symbol table as a vector of Var* (see above), the address of the global symbol table is stored as a static
//Error is also stored as a static, with only the first error variable being stored
public:
	Symbol_Table(const char*, Symbol_Table*, std::string);
	Symbol_Table(const char*, Symbol_Table*);
	Symbol_Table(const char*);
	virtual ~Symbol_Table();

	static Symbol_Table* global;
	Symbol_Table* parent;
	std::vector<Symbol_Table*> children;
	std::string scope;
	int args;
	std::string return_type;

	static std::string error;

	std::vector<Var*> vars;

	//Methods
	std::string find_func_type(std::string func_name);
	void print_table(); //Prints the table like in part 3
	void add_var(Var*); //Adds a variable to the current scope
	bool search_ids(const char*); //Checks to see if the variable is in the current scope (checking for declaration error)
	bool print_error(); //Prints the error variable if it isn't "", otherwise prints nothing
	Var* find_var(std::string); //Finds and returns the variable starting from the called scope, will call the parent scope if not found (if A is not in main, checks global to see if A is declared)
	Var* find_var(std::string, bool);
	Symbol_Table* search_scope(std::string); //Finds the given scope, recommend only using this on global
	Symbol_Table* make_child(const char*, std::string);
	Symbol_Table* make_child(const char*); //Creates a child scope symbol table under the current symbol table, sets the parent of the symbol table as the callee table
	void print_4_tiny(); //Prints the current scope for tiny code (the declarations are out of order for list declarations, didn't seem to matter)
	void print_4_tiny_global();
	int get_local_var_length(); //Gets the length of the local variables (str are the length+1 and floats and ints are 1
	void inc_args(); //Increments the args field by one, should be called whenever the args of a function are defined
	int get_link_num(); //Returns the number to link, which seems to be the number of local variable (not args) + 1
	int get_scope_size();
	std::string get_ref_id(std::string, std::string);
	int get_ref_id(std::string, std::string, int*);
};

class ASTNode {
//Stores expressions, see specifications of part 4 for a good example
public:
	ASTNode(char, std::string);
	ASTNode(const char *, std::string);
	ASTNode(char,ASTNode*, std::string);
	ASTNode(char, std::string, const char*);
	ASTNode(std::string, const char*);
	ASTNode(std::string, std::string);
	ASTNode(std::string, const char*, const char*);
	ASTNode(std::string, const char*, const char*, int);
	ASTNode(std::string, const char*, int , const char*);
	virtual ~ASTNode();

	void add_left(ASTNode*); //Adds passed ASTNode as right_child, if right_child != NULL, adds passed node as the right child's right child (used for write commands)
	void add_right(ASTNode*); //Adds passed ASTNode as left child,
	void add_id(const char*); //adds an id (string) to the node
	void print_tree(); //Prints the tree, used in debugging
	void generate_code(TempRegs&, std::vector<ThreeAC*>&, Symbol_Table*, std::vector<Var*>&); //Converts the tree into 3AC, stores the output in std::string& arg, there are two of these functions because this one checks to see if the tree is an identifier with no children, if it is, the identifier is printed directly
	std::string get_str_op(); //Converts the op character into the corresponding 3AC command, for example '+' -> "ADD" 
private:
	Var* generate_code_priv(TempRegs&, std::vector<ThreeAC*>&, Symbol_Table*, std::vector<Var*>&); //The program that actually generates the 3AC recursively
	
public:
	char op;
	char * compop;
	ASTNode* left_child;
	ASTNode* right_child;
	std::string id;
	std::string scope;
	std::string label;
	char ctrl;
};

ASTNode* real_pop(std::vector<ASTNode*>&); //vector.pop does not return what is popped, this function does
char get_back_op(std::vector<ASTNode*>); //returns the opartaion of the last ASTNode, used for write/read expressions
std::string is_number(const std::string&); //checks if the string given is a number (needs to be renovated for floats), 0 is not a number, 1 for int, 2 for float
void append_string(std::string&, std::string); //used for updating the 3AC string, will not append if the string starts with "ERROR"
char make_type_char(std::string&); //Converts a type string into an uppercase character ("FLOAT" -> 'F')
std::string convert_command(std::string); //converts 3AC commands into tiny commands ("ADD" -> "add")


#endif
