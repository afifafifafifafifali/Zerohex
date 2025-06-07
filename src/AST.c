#include "include/AST.h"
#include <stdlib.h>

AST_LIST_T* init_ast_list()
{
    AST_LIST_T* list = calloc(1, sizeof(AST_LIST_T));
    list->items = NULL;
    list->size = 0;
    return list;
}

void ast_list_add_element(AST_LIST_T* list, AST_T* element)
{
    if (!list) return;

    list->size++;
    list->items = realloc(list->items, sizeof(AST_T*) * list->size);
    list->items[list->size - 1] = element;
}

AST_T* init_ast(int type)
{
    AST_T* ast = calloc(1, sizeof(struct AST_STRUCT));
    ast->type = type;
    ast->scope = NULL;

    // Variable Definition
    ast->variable_definition_variable_name = NULL;
    ast->variable_definition_value = NULL;

    // Function Definition
    ast->function_definition_body = NULL;
    ast->function_definition_name = NULL;
    ast->function_definition_args = NULL;
    ast->function_definition_args_size = 0;

    // Variable
    ast->variable_name = NULL;

    // Function Call
    ast->function_call_name = NULL;
    ast->function_call_arguments = NULL;
    ast->function_call_arguments_size = 0;

    // String / Int / Decimal / Char
    ast->string_value = NULL;
    ast->int_value = 0;
    ast->decimal_value = 0.0f;
    ast->char_value = '\0';

    // List / Tuple
    ast->list_elements = NULL;
    ast->list_elements_size = 0;
    ast->tuple_elements = NULL;
    ast->tuple_elements_size = 0;

    // Dictionary entries (as AST_KEYVALUE)
    ast->dict_entries = NULL;
    ast->dict_entries_size = 0;

    // KeyValue
    ast->key = NULL;
    ast->value = NULL;

    // Compound
    ast->compound_value = NULL;
    ast->compound_size = 0;

    return ast;
}
