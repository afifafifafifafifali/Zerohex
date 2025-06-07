#define print printf
#include <stdio.h>
#include <string.h>
#include <stdlib.h>  // for exit()
#include "include/lexer.h"
#include "include/parser.h"
#include "include/visitor.h"
#include "include/io.h"

#define MAX_LIMIT 146

void print_help()
{
    printf("Usage:\nZerohex.exe <filename>\n");
    exit(1);
}

int main(int argc, char* argv[])
{
    if (argc >= 2){
        for(int i = 1; i < argc; i++){
            int len = strlen(argv[i]);
            if (len >= 4) {  // Ensure length enough for extension check
                const char* ext = &argv[i][len - 4];  // last 4 chars
                if (strcmp(ext, ".zhs") == 0) {
                    char* file_contents = get_file_contents(argv[i]);
                    if (!file_contents) {
                        fprintf(stderr, "Error: Could not read file '%s'\n", argv[i]);
                        continue;  // or exit(1);
                    }
                    lexer_T* lexer = init_lexer(file_contents);
                    parser_T* parser = init_parser(lexer);
                    AST_T* root = parser_parse(parser, parser->scope);
                    visitor_T* visitor = init_visitor();
                    visitor_visit(visitor, root);
                    // free resources if needed, depending on your implementations
                } else {
                    print_help();
                    return 1;  // stop on invalid file extension
                }
            } else {
                print_help();
                return 1;  // stop on invalid file name length
            }
        }
    }
    else {
        char input[MAX_LIMIT];
        printf("Welcome to the ZeroHex language Interpreter version 1! :D\nCreated by Afif Ali Saadman \n Type exit(); to quit. Note that the variables dont get saved here .");
        while(1){
            print("\n>>>");
            if (!fgets(input, MAX_LIMIT, stdin)) break; // handle EOF or error
            lexer_T* lexer = init_lexer(input);
            parser_T* parser = init_parser(lexer);
            AST_T* root = parser_parse(parser, parser->scope);
            visitor_T* visitor = init_visitor();
            visitor_visit(visitor, root);
            // cleanup if necessary
        }
    }
    return 0;
}
