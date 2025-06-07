#ifndef AST_H
#define AST_H
#include <stdlib.h>

typedef struct AST_STRUCT
{
    enum {
        AST_VARIABLE_DEFINITION,
        AST_FUNCTION_DEFINITION,
        AST_VARIABLE,
        AST_FUNCTION_CALL,
        AST_STRING,
        AST_COMPOUND,
        AST_NOOP,
        AST_INT,
        AST_DECIMAL,
        AST_CHARACTER,
        AST_LIST,
        AST_TUPLE,
        AST_DICTIONARY,
        AST_KEYVALUE
    } type;

    struct SCOPE_STRUCT* scope;

    // Variable Definition
    char* variable_definition_variable_name;
    struct AST_STRUCT* variable_definition_value;

    // Function Definition
    struct AST_STRUCT* function_definition_body;
    char* function_definition_name;
    struct AST_STRUCT** function_definition_args;
    size_t function_definition_args_size;

    // Variable
    char* variable_name;

    // Function Call
    char* function_call_name;
    struct AST_STRUCT** function_call_arguments;
    size_t function_call_arguments_size;

    // String, Int, Decimal, Char
    char* string_value;
    int int_value;
    float decimal_value;
    char char_value;

    // List / Tuple elements
    struct AST_STRUCT** list_elements;
    size_t list_elements_size;

    struct AST_STRUCT** tuple_elements;
    size_t tuple_elements_size;

    // Dict entries (as AST_KEYVALUE nodes)
    struct AST_STRUCT** dict_entries;
    size_t dict_entries_size;

    // AST_KEYVALUE pair
    struct AST_STRUCT* key;
    struct AST_STRUCT* value;

    // Compound statement body
    struct AST_STRUCT** compound_value;
    size_t compound_size;

} AST_T;

// Dynamic AST node list
typedef struct {
    AST_T** items;
    size_t size;
} AST_LIST_T;

AST_T* init_ast(int type);
AST_LIST_T* init_ast_list();
void ast_list_add_element(AST_LIST_T* list, AST_T* element);

#endif
