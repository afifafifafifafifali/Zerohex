#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "AST.h"
#include "scope.h"


#define parser_parse_expression parser_parse_expr

typedef struct PARSER_STRUCT
{
    lexer_T* lexer;
    token_T* current_token;
    token_T* prev_token;
    scope_T* scope;
} parser_T;

parser_T* init_parser(lexer_T* lexer);

void parser_eat(parser_T* parser, int token_type);

AST_T* parser_parse(parser_T* parser, scope_T* scope);

AST_T* parser_parse_statement(parser_T* parser, scope_T* scope);

AST_T* parser_parse_statements(parser_T* parser, scope_T* scope);

AST_T* parser_parse_expr(parser_T* parser, scope_T* scope);

AST_T* parser_parse_factor(parser_T* parser, scope_T* scope);

AST_T* parser_parse_term(parser_T* parser, scope_T* scope);

AST_T* parser_parse_function_call(parser_T* parser, scope_T* scope);
        
AST_T* parser_parse_variable_definition(parser_T* parser, scope_T* scope);

AST_T* parser_parse_function_definition(parser_T* parser, scope_T* scope);

AST_T* parser_parse_variable(parser_T* parser, scope_T* scope);

AST_T* parser_parse_string(parser_T* parser, scope_T* scope);

AST_T* parser_parse_integer(parser_T* parser, scope_T* scope);

AST_T* parser_parse_decimal(parser_T* parser, scope_T* scope);

AST_T* parser_parse_tuple(parser_T* parser, scope_T* scope);

AST_T* parser_parse_list(parser_T* parser, scope_T* scope);

AST_T* parser_parse_character(parser_T* parser, scope_T* scope);

AST_T* parser_parse_dictionary(parser_T* parser, scope_T* scope);


AST_T* parser_parse_id(parser_T* parser, scope_T* scope);
#endif
