%{
	#include <iostream>
	#include <string>
	#include "../generated/parser.hpp"
	extern char* yytext;
	#define register
%}
%option yylineno
%option noyywrap

INTLITERAL	[0-9]+
FLOATLITERAL	[0-9]+\.[0-9]*|\.[0-9]+
STRINGLITERAL	\"[^"]+\"
COMMENT		[-]{2}.*$
_PROGRAM	PROGRAM
_BEGIN		BEGIN
_END		END
_FUNCTION	FUNCTION
_READ		READ
_WRITE		WRITE
_IF			IF
_ELSE		ELSE
_ENDIF		ENDIF
_WHILE		WHILE
_ENDWHILE	ENDWHILE
_RETURN		RETURN
_INT		INT
_VOID		VOID
_STRING		STRING
_FLOAT		FLOAT
_TRUE		TRUE
_FALSE		FALSE
_FOR		FOR
_ENDFOR		ENDFOR
_CONTINUE	CONTINUE
_BREAK		BREAK
_COLONEQ	\:\=
_ADD		\+
_SUB		\-
_MULT		\*
_DIV		\/
_EQ			\=
_NOTEQ		\!\=
_LT			\<
_GT			\>
_LEFTPAR	\(
_RIGHTPAR	\)
_SEMICOL	\;
_COMMA		\,
_LTE		\<\=
_GTE		\>\=
IDENTIFIER	[a-zA-Z]{1}[a-zA-z0-9]*

%%
{INTLITERAL} 		yylval.s = strdup(yytext); return INTLITERAL;
{FLOATLITERAL}		yylval.s = strdup(yytext); return FLOATLITERAL;
{STRINGLITERAL}		yylval.s = strdup(yytext); return STRINGLITERAL;
{COMMENT}		/* ignore comment */
{_PROGRAM}		return _PROGRAM;
{_BEGIN}		return _BEGIN;
{_END}		    	return _END;
{_FUNCTION}	    	return _FUNCTION;
{_READ}		    	return _READ;
{_WRITE}	    	return _WRITE;
{_IF}		    	return _IF;
{_ELSE}		    	return _ELSE;
{_ENDIF}	    	return _ENDIF;
{_WHILE}	    	return _WHILE;
{_ENDWHILE}	    	return _ENDWHILE;
{_RETURN}	    	return _RETURN;
{_INT}		    	yylval.s = strdup(yytext); return _INT;
{_VOID}		    	return _VOID;
{_STRING}	    	yylval.s = strdup(yytext); return _STRING;
{_FLOAT}	    	yylval.s = strdup(yytext); return _FLOAT;
{_TRUE}		    	return _TRUE;
{_FALSE}	    	return _FALSE;
{_FOR}		    	return _FOR;
{_ENDFOR}	    	return _ENDFOR;
{_CONTINUE}	    	return _CONTINUE;
{_BREAK}	    	return _BREAK;
{_COLONEQ}	    	return _COLONEQ;
{_ADD}		    	return _ADD;
{_SUB}		    	return _SUB;
{_MULT}		    	return _MULT;
{_DIV}		    	return _DIV;
{_EQ}		    	return _EQ;
{_NOTEQ}	    	return _NOTEQ;
{_LT}		    	return _LT;
{_GT}		    	return _GT;
{_LEFTPAR}	    	return _LEFTPAR;
{_RIGHTPAR}	    	return _RIGHTPAR;
{_SEMICOL}	    	return _SEMICOL;
{_COMMA}	    	return _COMMA;
{_LTE}		    	return _LTE;
{_GTE}		    	return _GTE;
{IDENTIFIER}		yylval.s = strdup(yytext); return IDENTIFIER;

[ \t\r\n]+        /*do nothing*/

%%
