
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tables.h"
#include "parser.h"

extern int yyparse(void);
extern FILE* yyin;
extern ASTNode *yyparse_result;
extern int yylineno;
extern char* yytext;

void print_lexeme(const char* type, const char* value) {
    printf("LEXEME: %-10s at line %d: %s\n", type, yylineno, value);
}

int main(int argc, char **argv) {
    int print_ast = 0;
    int print_tokens = 0;
    char *out_dir = ".";
    char *input_file = NULL;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--print-ast") == 0) {
            print_ast = 1;
        } else if (strcmp(argv[i], "--print-tokens") == 0) {
            print_tokens = 1;
        } else if (strcmp(argv[i], "--out-dir") == 0 && i+1 < argc) {
            out_dir = argv[++i];
        } else if (argv[i][0] != '-') {
            input_file = argv[i];
        }
    }

    if (!input_file) {
        fprintf(stderr, "Error: No input file specified\n");
        return 1;
    }

    yyin = fopen(input_file, "r");
    if (!yyin) {
        perror("Error opening input file");
        return 1;
    }

    if (print_tokens) {
        // Print tokens as they're lexed
        int token;
        while ((token = yylex()) != 0) {
            switch (token) {
                case STRING: print_lexeme("STRING", yytext); break;
                case NUMBER: print_lexeme("NUMBER", yytext); break;
                case FLOAT: print_lexeme("FLOAT", yytext); break;
                case BOOLEAN: print_lexeme("BOOLEAN", yytext); break;
                case NULL_TOKEN: print_lexeme("NULL", "null"); break;
                case '{': print_lexeme("LBRACE", "{"); break;
                case '}': print_lexeme("RBRACE", "}"); break;
                case '[': print_lexeme("LBRACKET", "["); break;
                case ']': print_lexeme("RBRACKET", "]"); break;
                case ':': print_lexeme("COLON", ":"); break;
                case ',': print_lexeme("COMMA", ","); break;
                default: print_lexeme("UNKNOWN", yytext); break;
            }
        }
        fclose(yyin);
        return 0;
    }

    // Parse the input
    int parse_result = yyparse();
    fclose(yyin);

    if (parse_result != 0 || !yyparse_result) {
        fprintf(stderr, "Error: Failed to parse JSON input\n");
        return 1;
    }

    if (print_ast) {
        printf("Abstract Syntax Tree:\n");
        print_ast_node(yyparse_result, 0);
    }

    int process_result = process_ast(yyparse_result, out_dir);
    if (process_result != 0) {
        fprintf(stderr, "Error processing JSON\n");
    }

    if (yyparse_result) {
        free_ast(yyparse_result);
    }

    return process_result;
}

/*#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "tables.h"
#include "parser.h"  // Add this at the top
extern int yyparse(void);
extern FILE* yyin;
void yyerror(const char *s);
//extern int yyparse();
extern ASTNode *yyparse_result;

    void yyerror(const char *s) {
    	fprintf(stderr, "Parser error: %s\n", s);
	}
	
int main(int argc, char **argv) {
    int print_ast = 0;
    char *out_dir = ".";
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--print-ast") == 0) {
            print_ast = 1;
        } else if (strcmp(argv[i], "--out-dir") == 0 && i+1 < argc) {
            out_dir = argv[++i];
        }
    }

    yyparse();

    if (print_ast && yyparse_result) {
        print_ast_node(yyparse_result, 0);
    }

    process_ast(yyparse_result, out_dir);

    if (yyparse_result) {
        free_ast(yyparse_result);
    }


    return 0;
}*/
