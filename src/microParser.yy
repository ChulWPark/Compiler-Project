%{
	#include <iostream>
	#include <cstdio>
	#include <string>
	#include "../generated/parser.hpp"
	#include "../src/symbols.hpp"

	int counter = 1;
	bool declaring_args = false;
	const char* list_type;
	std::string lbl;

	std::vector <ASTNode*> node_vector = {};
	std::vector <int> num_args = {};
	char op;
	Var* var;
	char glob[] = "GLOBAL";
	Symbol_Table* symbol_table;
	extern int yylex();
	extern int yylineno;
	extern char * yytext;
	void yyerror(const char * s) {
		//printf("Not ");
	}
%}

/*Union*/
%union{
	const char * s;
	char operation;
	class ASTNode* node;
}

/*Type*/
%type <s> id str var_type compop any_type;
%type <node> call_expr primary expr expr_list expr_prefix factor factor_prefix postfix_expr assign_expr id_tail id_list while_stmt if_stmt cond expr_list_tail;
%type <operation> addop mulop;

/*Tokens*/
%token	<s> INTLITERAL
%token	<s> FLOATLITERAL
%token	<s> STRINGLITERAL
%token	_PROGRAM
%token	_BEGIN
%token	_END
%token	_FUNCTION
%token	_READ
%token	_WRITE
%token	_IF
%token	_ELSE
%token	_ENDIF
%token	_WHILE
%token	_ENDWHILE
%token	_RETURN
%token	<s> _INT
%token	_VOID
%token	<s> _STRING
%token	<s> _FLOAT
%token	_TRUE
%token	_FALSE
%token	_FOR
%token	_ENDFOR
%token	_CONTINUE
%token	_BREAK
%token	_COLONEQ
%token	_ADD
%token	_SUB
%token	_MULT
%token	_DIV
%token	_EQ
%token	_NOTEQ
%token	_LT
%token	_GT
%token	_LEFTPAR
%token	_RIGHTPAR
%token	_SEMICOL
%token	_COMMA
%token	_LTE
%token	_GTE
%token	<s> IDENTIFIER

%%
/* Program */
program:		_PROGRAM	{
					symbol_table = new Symbol_Table("GLOBAL");
					node_vector.push_back(new ASTNode(symbol_table -> scope, ";PUSH"));
					node_vector.push_back(new ASTNode(symbol_table -> scope, ";PUSH"));
					node_vector.push_back(new ASTNode(symbol_table -> scope, ";PUSH"));
					node_vector.push_back(new ASTNode(symbol_table -> scope, ";PUSH"));
					node_vector.push_back(new ASTNode(symbol_table -> scope, ";PUSH"));
					node_vector.push_back(new ASTNode(symbol_table -> scope, ";JSR", "main", 2));
					node_vector.push_back(new ASTNode(symbol_table -> scope, ";HALT"));
				}
				id _BEGIN pgm_body _END;

id:				IDENTIFIER	{
					$$ = $1;
				};

pgm_body:		decl func_declarations;

decl:			string_decl decl | var_decl decl | ;


/* Global String Declaration */
string_decl:	_STRING id _COLONEQ str _SEMICOL	{
					var = new Var("STRING", $2, $4);
					symbol_table -> add_var(var);
				};

str:           	STRINGLITERAL	{
					$$ = $1;
				};


/* Variable Declaration */
var_decl:		var_type id_list _SEMICOL	{
					list_type = $1;
				};

var_type:	    _FLOAT	{
					$$ = "FLOAT";
					list_type = "FLOAT";
				}
				| _INT	{
					list_type = "INT";
					$$ = "INT";
				};

any_type:       var_type {
					$$ = $1;
				} | _VOID {
					$$ = "";
				};

id_list:        id id_tail	{
					if(list_type != NULL)	{
						var = new Var(list_type, $1);
						symbol_table -> add_var(var);
						$$ = NULL;
					}
					else	{
						$$ = new ASTNode(op, symbol_table -> scope, $1);
						if($2 != NULL)	{
							$$ -> add_right($2);
						}
						if(get_back_op(node_vector) == op)	{
							node_vector.back() -> add_right($$);
						}
						else	{
							node_vector.push_back($$);
						}
					}
				};

id_tail:		_COMMA id id_tail	{
					if(list_type != NULL)	{
						var = new Var(list_type, $2);
						symbol_table -> add_var(var);
						$$ = NULL;
					}
					else	{
						$$ = new ASTNode(op, symbol_table -> scope, $2);
						if($3 != NULL)	{
							$$ -> add_right($3);
						}
					}
				}
				|	{
					$$ = NULL;
				};


/* Function Paramater List */
param_decl_list: 	param_decl param_decl_tail | ;

param_decl:         var_type id	{
						if(declaring_args) {
							symbol_table -> inc_args();
						}
						var = new Var(list_type, $2);
						symbol_table -> add_var(var);
					};

param_decl_tail:	_COMMA param_decl param_decl_tail |  ;


/* Function Declarations */
func_declarations:	func_decl func_declarations | ;

func_decl:			_FUNCTION any_type id	{
						symbol_table = symbol_table -> global;
						symbol_table = symbol_table -> make_child($3, std::string($2));
						node_vector.push_back(new ASTNode(symbol_table -> scope, ";LABEL", $3, 2));
						declaring_args = true;
					}
					_LEFTPAR param_decl_list _RIGHTPAR _BEGIN	{
						declaring_args = false;
						node_vector.push_back(new ASTNode('L', symbol_table -> scope, ";LINK"));
					}
					func_body _END	{
						symbol_table = symbol_table -> global;
						if(node_vector.back() -> id.compare(";RET")) {
							node_vector.push_back(new ASTNode(symbol_table -> scope, ";UNLINK"));
							node_vector.push_back(new ASTNode(symbol_table -> scope, ";RET"));
						}
					};

func_body:			decl stmt_list ;


/* Statement List */
stmt_list:			stmt stmt_list | ;

stmt:             	base_stmt | if_stmt | loop_stmt ;

base_stmt:          assign_stmt | read_stmt | write_stmt | control_stmt;


/* Basic Statements */
assign_stmt:        assign_expr _SEMICOL ;

assign_expr:        id _COLONEQ expr	{
						$$ = new ASTNode(':', symbol_table -> scope);
						$$ -> add_right($3);
						$$ -> add_left(new ASTNode(symbol_table -> scope, $1));
						node_vector.push_back($$);
					};

read_stmt:          _READ _LEFTPAR	{
						list_type = NULL;
						op = 'r';
					}
					id_list _RIGHTPAR _SEMICOL ;

write_stmt:         _WRITE _LEFTPAR {
						list_type = NULL;
						op = 'w';
					}
					id_list _RIGHTPAR _SEMICOL ;

return_stmt:        _RETURN expr	{
						ASTNode* return_node = new ASTNode('R', symbol_table -> scope, ";RET");
						return_node -> add_right($2);
						node_vector.push_back(return_node);
					}
					_SEMICOL ;


/* Expressions */
expr:             	expr_prefix factor	{
						if($1 != NULL)	{
							$$ = $1;
							$$ -> add_right($2);
						}
						else	{
							$$ = $2;
						}
					};

expr_prefix:        expr_prefix factor addop	{
						if($1 != NULL)	{
							$1 -> add_right($2);
							$$ = new ASTNode($3, symbol_table -> scope);
							$$ -> add_left($1);
						}
						else	{
							$$ = new ASTNode($3, symbol_table -> scope);
							$$ -> add_left($2);
						}
					}
					|	{
						$$ = NULL;
					};

factor:             factor_prefix postfix_expr	{
						if($1 != NULL)	{
							$$ = $1;
							$$ -> add_right($2);
						}
						else	{
							$$ = $2;
						}
					};

factor_prefix:      factor_prefix postfix_expr mulop	{
						if($1 != NULL)	{
							$1 -> add_right($2);
							$$ = new ASTNode($3, symbol_table -> scope);
							$$ -> add_left($1);
						}
						else	{
							$$ = new ASTNode($3, symbol_table -> scope);
							$$ -> add_left($2);
						}
					}
					|	{
						$$ = NULL;
					};

postfix_expr:       primary	{
						$$ = $1;
					}
					| call_expr	{
						$$ = $1;
					};

call_expr:          id _LEFTPAR expr_list {
						node_vector.push_back(new ASTNode(symbol_table -> scope, ";PUSH")); //For return value
						if($3 != NULL) {
							node_vector.push_back($3); //Push each arg
						}
						node_vector.push_back(new ASTNode(symbol_table -> scope, ";PUSHREGS")); //For the sake of the regs themselves
						node_vector.push_back(new ASTNode(symbol_table -> scope, ";JSR", $1, 2)); //actually jumping
					}
					_RIGHTPAR {
						node_vector.push_back(new ASTNode(symbol_table -> scope, ";POPREGS")); //For all regs
						node_vector.push_back(new ASTNode('O', $1, ";POP_ARGS")); //args has to go back and check how many pushes for args there are
						node_vector.push_back(new ASTNode(symbol_table -> scope, ";POPR", 0, $1)); //For return value
						$<node>$ = new ASTNode(symbol_table -> scope, ";POPR", 0, $1);
					} ;

expr_list:          expr expr_list_tail {
						$$ = new ASTNode('s', symbol_table -> scope, ";PUSH");
						$$ -> add_left($1);
						$$ -> add_right($2);
					}
					| {
						$$ = NULL;
					};

expr_list_tail:     _COMMA expr expr_list_tail {
						$$ = new ASTNode('s', symbol_table -> scope, ";PUSH");
						$$ -> add_left($2);
						$$ -> add_right($3);
					}
					| {
						$$ = NULL;
					};

primary:            _LEFTPAR expr _RIGHTPAR	{
						$$ = $2;
					}
					| id	{
						$$ = new ASTNode(symbol_table -> scope, $1);
					}
					| INTLITERAL	{
						$$ = new ASTNode(symbol_table -> scope, $1);
					}
					| FLOATLITERAL	{
						$$ = new ASTNode(symbol_table -> scope, $1);
					};

addop:              _ADD	{
						$$ = '+';
					}
					| _SUB	{
						$$ = '-';
					};

mulop:              _MULT	{
						$$ = '*';
					}
					| _DIV	{
						$$ = '/';
					};


/* Complex Statements and Condition */
if_stmt:            _IF _LEFTPAR cond _RIGHTPAR	{
						counter += 1;
						$<node>$ = new ASTNode('f', $3, symbol_table -> scope);
						if($<node>$ -> right_child != NULL) {
							$<node>$ -> right_child -> ctrl = 'f';
						}
						node_vector.push_back($<node>$);
					}
					decl stmt_list	{
						//symbol_table = symbol_table -> parent;
					}
					else_part _ENDIF	{
						lbl = "END_IF_ELSE";
						lbl += (std::to_string((long long int) std::stoi(symbol_table -> scope) + 1));
						node_vector.push_back(new ASTNode(symbol_table -> scope, ";LABEL ", lbl.c_str(), 1));
						symbol_table = symbol_table -> parent;
					};

else_part:        	_ELSE	{
						//symbol_table = symbol_table -> make_child(std::to_string((long long int) counter++).c_str());
						//counter -= 1;
						lbl = "END_IF_ELSE";
						lbl += (std::to_string((long long int) std::stoi(symbol_table -> scope) + 1));
						node_vector.push_back(new ASTNode(symbol_table -> scope, ";JUMP ", lbl.c_str(), 1));
						lbl = "ELSE_";
						lbl += symbol_table -> scope;
						node_vector.push_back(new ASTNode(symbol_table -> scope, ";LABEL ", lbl.c_str(), 1));
					}
					decl stmt_list	{
						//symbol_table = symbol_table -> parent;
					}
					|	{
						lbl = "END_IF_ELSE";
						lbl += (std::to_string((long long int) std::stoi(symbol_table -> scope) + 1));
						node_vector.push_back(new ASTNode(symbol_table -> scope, ";JUMP ", lbl.c_str(), 1));
						lbl = "ELSE_";
						lbl += symbol_table -> scope;
						node_vector.push_back(new ASTNode(symbol_table -> scope, ";LABEL ", lbl.c_str(), 1));
					};

cond:               expr compop expr	{
						symbol_table = symbol_table -> make_child(std::to_string((long long int) counter++).c_str());
						$$ = new ASTNode($2, symbol_table -> scope);
						$$ -> add_left($1);
						$$ -> add_right($3);
					}
					| _TRUE		{
						symbol_table = symbol_table -> make_child(std::to_string((long long int) counter++).c_str());
						$$ = new ASTNode("=", symbol_table -> scope);
						$$ -> add_left(new ASTNode(symbol_table -> scope, "1"));
						$$ -> add_right(new ASTNode(symbol_table -> scope, "1"));
					}
					| _FALSE	{
						symbol_table = symbol_table -> make_child(std::to_string((long long int) counter++).c_str());
						$$ = new ASTNode("=", symbol_table -> scope);
						$$ -> add_left(new ASTNode(symbol_table -> scope, "1"));
						$$ -> add_right(new ASTNode(symbol_table -> scope, "0"));
					};

compop:             _LT		{
						$$ = "<";
					}
					| _GT 		{
						$$ = ">";
					}
					| _EQ		{
						$$ = "=";
					}
					| _NOTEQ	{
						$$ = "!=";
					}
					| _LTE		{
						$$ = "<=";
					}
					| _GTE		{
						$$ = ">=";
					};

while_stmt:         _WHILE	_LEFTPAR cond _RIGHTPAR	{
						counter += 1;
						lbl = "WHILE_START_";
						lbl += symbol_table -> scope;
						node_vector.push_back(new ASTNode(symbol_table -> scope, ";LABEL ", lbl.c_str(), 1));
						$<node>$ = new ASTNode('l', $3, symbol_table -> scope);
						$<node>$ -> right_child -> ctrl = 'l';
						node_vector.push_back($<node>$);
					}
					decl stmt_list _ENDWHILE	{
						lbl = "WHILE_START_";
						lbl += symbol_table -> scope;
						node_vector.push_back(new ASTNode(symbol_table -> scope, ";JUMP ", lbl.c_str(), 1));
						lbl = "WHILE_END_";
						lbl += (std::to_string((long long int) std::stoi(symbol_table -> scope) + 1));
						node_vector.push_back(new ASTNode(symbol_table -> scope, ";LABEL ", lbl.c_str(), 1));
						symbol_table = symbol_table -> parent;
					};


/*ECE468 ONLY*/
control_stmt:         	return_stmt {
							node_vector.push_back(new ASTNode(symbol_table -> scope, ";UNLINK"));
							node_vector.push_back(new ASTNode(symbol_table -> scope, ";RET"));
						};
loop_stmt:            	while_stmt;
%%
