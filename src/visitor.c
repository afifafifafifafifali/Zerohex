#include "include/visitor.h"
#include "include/scope.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static AST_T* builtin_function_print(visitor_T* visitor, AST_T** args, int args_size)
{
    for (int i = 0; i < args_size; i++)
    {
        AST_T* visited_ast = visitor_visit(visitor, args[i]);

        switch (visited_ast->type)
        {
            case AST_STRING:
                printf("%s\n", visited_ast->string_value);
                break;

            case AST_INT:
                printf("%d\n", visited_ast->int_value);
                break;

            case AST_DECIMAL:
                printf("%f\n", visited_ast->decimal_value);
                break;

            case AST_CHARACTER:
                printf("%c\n", visited_ast->char_value);
                break;

            default:
                printf("%p\n", (void*)visited_ast);
                break;
        }
    }

    return init_ast(AST_NOOP);
}

static AST_T* builtin_function_exit(visitor_T* visitor, AST_T** args, int args_size)
{
    for (int i = 0; i < args_size; i++)
    {
        AST_T* visited_ast = visitor_visit(visitor, args[i]);

        switch (visited_ast->type)
        {
            case AST_NOOP:
                printf("You exited\n");
                exit(0);
                break;

            default:
                printf("%p\n", (void*)visited_ast);
                break;
        }
    }

    return init_ast(AST_NOOP);
}

static AST_T* builtin_function_clear(visitor_T* visitor, AST_T** args, int args_size)
{
    for (int i = 0; i < args_size; i++)
    {
        AST_T* visited_ast = visitor_visit(visitor, args[i]);

        switch (visited_ast->type)
        {
            case AST_NOOP:
                system("clear");
                break;

            default:
                printf("%p\n", (void*)visited_ast);
                break;
        }
    }

    return init_ast(AST_NOOP);
}

visitor_T* init_visitor()
{
    visitor_T* visitor = calloc(1, sizeof(struct VISITOR_STRUCT));
    return visitor;
}

AST_T* visitor_visit(visitor_T* visitor, AST_T* node)
{
    switch (node->type)
    {
        case AST_VARIABLE_DEFINITION: return visitor_visit_variable_definition(visitor, node);
        case AST_FUNCTION_DEFINITION: return visitor_visit_function_definition(visitor, node);
        case AST_VARIABLE: return visitor_visit_variable(visitor, node);
        case AST_FUNCTION_CALL: return visitor_visit_function_call(visitor, node);
        case AST_STRING: return visitor_visit_string(visitor, node);
        case AST_COMPOUND: return visitor_visit_compound(visitor, node);

        // Literals
        case AST_INT: return visitor_visit_integer(visitor, node);
        case AST_DECIMAL: return visitor_visit_decimal(visitor, node);
        case AST_CHARACTER: return visitor_visit_character(visitor, node);
        case AST_LIST: return visitor_visit_list(visitor, node);
        case AST_TUPLE: return visitor_visit_tuple(visitor, node);
        case AST_DICTIONARY: return visitor_visit_dictionary(visitor, node);

        case AST_NOOP: return node;
    }

    printf("Uncaught statement of type `%d`\n", node->type);
    exit(1);
    return init_ast(AST_NOOP);
}

AST_T* visitor_visit_variable_definition(visitor_T* visitor, AST_T* node)
{
    scope_add_variable_definition(
        node->scope,
        node        
    ); 

    return node;
}

AST_T* visitor_visit_function_definition(visitor_T* visitor, AST_T* node)
{
    scope_add_function_definition(
        node->scope,
        node        
    );

    return node;
}

AST_T* visitor_visit_variable(visitor_T* visitor, AST_T* node)
{
    AST_T* vdef = scope_get_variable_definition(
        node->scope,
        node->variable_name
    );
    
    if (vdef != NULL)
        return visitor_visit(visitor, vdef->variable_definition_value);

    printf("Undefined variable `%s`\n", node->variable_name);
    exit(1);
    return NULL;
}

AST_T* visitor_visit_function_call(visitor_T* visitor, AST_T* node)
{
    if (strcmp(node->function_call_name, "print") == 0)
    {
        return builtin_function_print(visitor, node->function_call_arguments, node->function_call_arguments_size);
    }

    if (strcmp(node->function_call_name, "exit") == 0)
    {
        return builtin_function_exit(visitor, node->function_call_arguments, node->function_call_arguments_size);
    }

    if (strcmp(node->function_call_name, "clear") == 0)
    {
        return builtin_function_clear(visitor, node->function_call_arguments, node->function_call_arguments_size);
    }

    AST_T* fdef = scope_get_function_definition(
        node->scope,
        node->function_call_name
    );

    if (fdef == NULL)
    {
        printf("Undefined method `%s`\n", node->function_call_name);
        exit(1);
    }

    for (int i = 0; i < (int) node->function_call_arguments_size; i++)
    {
        AST_T* ast_var = (AST_T*) fdef->function_definition_args[i];
        AST_T* ast_value = (AST_T*) node->function_call_arguments[i];

        AST_T* ast_vardef = init_ast(AST_VARIABLE_DEFINITION);
        ast_vardef->variable_definition_value = visitor_visit(visitor, ast_value);

        ast_vardef->variable_definition_variable_name = (char*) calloc(strlen(ast_var->variable_name) + 1, sizeof(char));
        strcpy(ast_vardef->variable_definition_variable_name, ast_var->variable_name);

        scope_add_variable_definition(fdef->function_definition_body->scope, ast_vardef);
    }
    
    return visitor_visit(visitor, fdef->function_definition_body);
}

AST_T* visitor_visit_string(visitor_T* visitor, AST_T* node)
{
    return node;
}

AST_T* visitor_visit_compound(visitor_T* visitor, AST_T* node)
{
    for (int i = 0; i < node->compound_size; i++)
    {
        visitor_visit(visitor, node->compound_value[i]);
    }

    return init_ast(AST_NOOP);
}

// Literal visitors:

AST_T* visitor_visit_integer(visitor_T* visitor, AST_T* node)
{
    return node;
}

AST_T* visitor_visit_decimal(visitor_T* visitor, AST_T* node)
{
    return node;
}

AST_T* visitor_visit_character(visitor_T* visitor, AST_T* node)
{
    return node;
}

AST_T* visitor_visit_list(visitor_T* visitor, AST_T* node)
{
    for (int i = 0; i < node->list_elements_size; i++)
    {
        node->list_elements[i] = visitor_visit(visitor, node->list_elements[i]);
    }
    return node;
}

AST_T* visitor_visit_tuple(visitor_T* visitor, AST_T* node)
{
    for (int i = 0; i < node->tuple_elements_size; i++)
    {
        node->tuple_elements[i] = visitor_visit(visitor, node->tuple_elements[i]);
    }
    return node;
}

AST_T* visitor_visit_dictionary(visitor_T* visitor, AST_T* node)
{
    for (int i = 0; i < node->dict_entries_size; i++)
    {
        node->dict_entries[i]->key = visitor_visit(visitor, node->dict_entries[i]->key);
        node->dict_entries[i]->value = visitor_visit(visitor, node->dict_entries[i]->value);
    }
    return node;
}
