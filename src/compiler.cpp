#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "../generated/parser.hpp"
#include "../src/symbols.hpp"

extern char* yytext;
extern int yylex();
extern int yyparse();
extern FILE* yyin;
extern Symbol_Table* symbol_table;
extern std::vector<ASTNode*> node_vector;
using namespace std;

int main(int argc, char* argv[]) {
	yyin = fopen(argv[1], "r");
	yyparse();

	//Error Checking: if there is a declaration error, print it and return
	symbol_table = symbol_table -> global;
	if(symbol_table -> print_error()) {
		return 1;
	}
	
	symbol_table -> print_table();
	
	//Parse ASTNode, creating the vector of 3AC commands
	TempRegs regs = TempRegs();  //Define your class of temp registers (acts as a vector of vars)
	std::vector<ThreeAC*> instructions;
	std::vector<Var*> return_FIFO;
	for(int i = 0; i < node_vector.size(); i++) { //For each node in the ASTNode vector (vector stores the top node of the tree), create the needed lines of code
		//std::cout << node_vector[i] -> id << std::endl;
		//std::cout << ";" << std::endl;
		//node_vector[i] -> print_tree();
		//std::cout << std::endl;

		node_vector[i] -> generate_code(regs, instructions, symbol_table, return_FIFO); //Passes regs and string as reference
	}


	/*ThreeAC* prev = NULL;
	for(int i = instructions.size() - 1; i >= 0 ; i--) {
		instructions[i] -> generate_living_vars(prev);
		prev = instructions[i];
	}*/

	//Print 3AC code
	std::cout << std::endl << ";IR code" << std::endl;
	for(int i = 0; i < instructions.size(); i++) {
		
		instructions[i] -> print_command();
		//instructions[i] -> print_living_vars();
	}

	//Print Tiny Code
	std::cout << std::endl;
	symbol_table -> print_4_tiny_global(); //Have to print global vars
	Regs final_regs = Regs(200);
	std::vector<Tiny*> tiny_instructs;
	for(int i = 0; i < instructions.size(); i++) {
		Tiny* new_tiny = new Tiny(instructions[i], final_regs);
		if(new_tiny -> command.compare("")) {
			tiny_instructs.push_back(new_tiny);
		}
	}


	for(int i = 0; i < tiny_instructs.size(); i++) {
		tiny_instructs[i] -> print_tiny_code(symbol_table);
	}

	return 0;
}


