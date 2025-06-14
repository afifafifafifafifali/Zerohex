#include "include/parser.h"
#include "include/scope.h"
#include <stdio.h>
#include <string.h>


parser_T* init_parser(lexer_T* lexer)
{
    parser_T* parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(lexer);
    parser->prev_token = parser->current_token;

    parser->scope = init_scope();

    return parser;
}

void parser_eat(parser_T* parser, int token_type)
{
    if (parser->current_token->type == token_type)
    {
        parser->prev_token = parser->current_token;
        parser->current_token = lexer_get_next_token(parser->lexer);
    }
    else
    {
        printf(
            "Unexpected token `%s`, with type %d",
            parser->current_token->value,
            parser->current_token->type
        );
        exit(1);
    }
}

AST_T* parser_parse(parser_T* parser, scope_T* scope)
{
    return parser_parse_statements(parser, scope);
}

AST_T* parser_parse_statement(parser_T* parser, scope_T* scope)
{
    switch (parser->current_token->type)
    {
        case TOKEN_ID: return parser_parse_id(parser, scope);
    }

    return init_ast(AST_NOOP);
}

AST_T* parser_parse_statements(parser_T* parser, scope_T* scope)
{
    AST_T* compound = init_ast(AST_COMPOUND);
    compound->scope = scope;
    compound->compound_value = calloc(1, sizeof(struct AST_STRUCT*));

    AST_T* ast_statement = parser_parse_statement(parser, scope);
    ast_statement->scope = scope;
    compound->compound_value[0] = ast_statement;
    compound->compound_size += 1;

    while (parser->current_token->type == TOKEN_SEMI)
    {
        parser_eat(parser, TOKEN_SEMI);

        AST_T* ast_statement = parser_parse_statement(parser, scope);

        if (ast_statement)
        {
            compound->compound_size += 1;
            compound->compound_value = realloc(
                compound->compound_value,
                compound->compound_size * sizeof(struct AST_STRUCT*)
            );
            compound->compound_value[compound->compound_size-1] = ast_statement;
        }
    }

    return compound;
}

AST_T* parser_parse_expr(parser_T* parser, scope_T* scope)
{
    switch (parser->current_token->type)
    {
        case TOKEN_STRING:
            return parser_parse_string(parser, scope);

        case TOKEN_INT:
            return parser_parse_integer(parser, scope);

        case TOKEN_DECIMAL:
            return parser_parse_decimal(parser, scope);

        case TOKEN_CHARACTER:
            return parser_parse_character(parser, scope);

        case TOKEN_LBRACKET:  // '[' for list
            return parser_parse_list(parser, scope);

        case TOKEN_LPAREN:    // '(' for tuple
            return parser_parse_tuple(parser, scope);

        case TOKEN_LBRACE:    // '{' for dictionary, assuming you use braces
            return parser_parse_dictionary(parser, scope);

        case TOKEN_ID:
            return parser_parse_id(parser, scope);

        default:
            // Unexpected token, maybe error or return noop
            return init_ast(AST_NOOP);
    }
}


AST_T* parser_parse_factor(parser_T* parser, scope_T* scope)
{
}

AST_T* parser_parse_term(parser_T* parser, scope_T* scope)
{
}

AST_T* parser_parse_function_call(parser_T* parser, scope_T* scope)
{
    AST_T* function_call = init_ast(AST_FUNCTION_CALL);

    function_call->function_call_name = parser->prev_token->value;
    parser_eat(parser, TOKEN_LPAREN); 

    function_call->function_call_arguments = calloc(1, sizeof(struct AST_STRUCT*));

    AST_T* ast_expr = parser_parse_expr(parser, scope);
    function_call->function_call_arguments[0] = ast_expr;
    function_call->function_call_arguments_size += 1;

    while (parser->current_token->type == TOKEN_COMMA)
    {
        parser_eat(parser, TOKEN_COMMA);

        AST_T* ast_expr = parser_parse_expr(parser, scope);
        function_call->function_call_arguments_size += 1;
        function_call->function_call_arguments = realloc(
            function_call->function_call_arguments,
            function_call->function_call_arguments_size * sizeof(struct AST_STRUCT*)
        );
        function_call->function_call_arguments[function_call->function_call_arguments_size-1] = ast_expr;
    }
    parser_eat(parser, TOKEN_RPAREN);

    function_call->scope = scope;

    return function_call;
}

AST_T* parser_parse_variable_definition(parser_T* parser, scope_T* scope)
{
    parser_eat(parser, TOKEN_ID); // var
    char* variable_definition_variable_name = parser->current_token->value;
    parser_eat(parser, TOKEN_ID); // var name
    parser_eat(parser, TOKEN_EQUALS);
    AST_T* variable_definition_value = parser_parse_expr(parser, scope);

    AST_T* variable_definition = init_ast(AST_VARIABLE_DEFINITION);
    variable_definition->variable_definition_variable_name = variable_definition_variable_name;
    variable_definition->variable_definition_value = variable_definition_value;

    variable_definition->scope = scope;

    return variable_definition;
}

AST_T* parser_parse_function_definition(parser_T* parser, scope_T* scope)
{
    AST_T* ast = init_ast(AST_FUNCTION_DEFINITION);
    parser_eat(parser, TOKEN_ID); // function

    char* function_name = parser->current_token->value;
    ast->function_definition_name = calloc(
            strlen(function_name) + 1, sizeof(char)
    );
    strcpy(ast->function_definition_name, function_name);

    parser_eat(parser, TOKEN_ID); // function name

    parser_eat(parser, TOKEN_LPAREN);

    ast->function_definition_args =
        calloc(1, sizeof(struct AST_STRUCT*));

    AST_T* arg = parser_parse_variable(parser, scope);
    ast->function_definition_args_size += 1;
    ast->function_definition_args[ast->function_definition_args_size-1] = arg;

    while (parser->current_token->type == TOKEN_COMMA)
    {
        parser_eat(parser, TOKEN_COMMA);

        ast->function_definition_args_size += 1;

        ast->function_definition_args =
            realloc(
                    ast->function_definition_args,
                    ast->function_definition_args_size * sizeof(struct AST_STRUCT*)
                   );

        AST_T* arg = parser_parse_variable(parser, scope);
        ast->function_definition_args[ast->function_definition_args_size-1] = arg;
    }

    parser_eat(parser, TOKEN_RPAREN);
    
    parser_eat(parser, TOKEN_LBRACE);
    
    ast->function_definition_body = parser_parse_statements(parser, scope);

    parser_eat(parser, TOKEN_RBRACE);

    ast->scope = scope;

    return ast;
}

AST_T* parser_parse_variable(parser_T* parser, scope_T* scope)
{
    char* token_value = parser->current_token->value;
    parser_eat(parser, TOKEN_ID); // var name or function call name

    if (parser->current_token->type == TOKEN_LPAREN)
        return parser_parse_function_call(parser, scope);

    AST_T* ast_variable = init_ast(AST_VARIABLE);
    ast_variable->variable_name = token_value;

    ast_variable->scope = scope;

    return ast_variable;
}

AST_T* parser_parse_string(parser_T* parser, scope_T* scope)
{
    AST_T* ast_string = init_ast(AST_STRING);
    ast_string->string_value = parser->current_token->value;

    parser_eat(parser, TOKEN_STRING);

    ast_string->scope = scope;

    return ast_string;
}


AST_T* parser_parse_integer(parser_T* parser, scope_T* scope)
{
    AST_T* ast = init_ast(AST_INT);
    ast->int_value = atoi(parser->current_token->value); // convert string to int
    parser_eat(parser, TOKEN_INT); // advance to next token
    return ast;
}


AST_T* parser_parse_decimal(parser_T* parser, scope_T* scope)
{
    AST_T* ast_dec = init_ast(AST_DECIMAL);
    ast_dec->decimal_value = atof(parser->current_token->value);

    parser_eat(parser, TOKEN_DECIMAL);
    ast_dec->scope = scope;

    return ast_dec;
}

AST_T* parser_parse_character(parser_T* parser, scope_T* scope)
{
    AST_T* ast_char = init_ast(AST_CHARACTER);
    ast_char->char_value = parser->current_token->value[1]; // between single quotes: 'A'

    parser_eat(parser, TOKEN_CHARACTER);
    ast_char->scope = scope;

    return ast_char;
}

AST_T* parser_parse_list(parser_T* parser, scope_T* scope)
{
    AST_T* ast_list = init_ast(AST_LIST);
    ast_list->list_elements = init_ast_list();

    parser_eat(parser, TOKEN_LBRACKET);

    while (parser->current_token->type != TOKEN_RBRACKET)
    {
        AST_T* element = parser_parse_expression(parser, scope);
        ast_list_add_element(ast_list, element);

        if (parser->current_token->type == TOKEN_COMMA)
            parser_eat(parser, TOKEN_COMMA);
    }

    parser_eat(parser, TOKEN_RBRACKET);
    ast_list->scope = scope;

    return ast_list;
}

AST_T* parser_parse_tuple(parser_T* parser, scope_T* scope)
{
    AST_T* ast_tuple = init_ast(AST_TUPLE);
    ast_tuple->tuple_elements = init_ast_list();

    parser_eat(parser, TOKEN_LPAREN);

    while (parser->current_token->type != TOKEN_RPAREN)
    {
        AST_T* element = parser_parse_expression(parser, scope);
        ast_list_add_element(ast_tuple, element);

        if (parser->current_token->type == TOKEN_COMMA)
            parser_eat(parser, TOKEN_COMMA);
    }

    parser_eat(parser, TOKEN_RPAREN);
    ast_tuple->scope = scope;

    return ast_tuple;
}

AST_T* parser_parse_dictionary(parser_T* parser, scope_T* scope)
{
    AST_T* ast_dict = init_ast(AST_DICTIONARY);
    ast_dict->dict_entries = init_ast_list();

    parser_eat(parser, TOKEN_LBRACKET);

    while (parser->current_token->type != TOKEN_RBRACKET)
    {
        AST_T* key = parser_parse_expression(parser, scope);
        parser_eat(parser, TOKEN_COLON);
        AST_T* value = parser_parse_expression(parser, scope);

        AST_T* pair = init_ast(AST_KEYVALUE);
        pair->key = key;
        pair->value = value;

        ast_list_add_element(ast_dict, pair);

        if (parser->current_token->type == TOKEN_COMMA)
            parser_eat(parser, TOKEN_COMMA);
    }

    parser_eat(parser, TOKEN_RBRACKET);
    ast_dict->scope = scope;

    return ast_dict;
}



AST_T* parser_parse_id(parser_T* parser, scope_T* scope)
{
    if (strcmp(parser->current_token->value, "var") == 0)
    {
        return parser_parse_variable_definition(parser, scope);
    }
    else
    if (strcmp(parser->current_token->value, "function") == 0)
    {
        return parser_parse_function_definition(parser, scope);
    }
    else
    {
        return parser_parse_variable(parser, scope);
    }
}
