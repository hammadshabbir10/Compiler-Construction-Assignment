%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tables.h"


typedef struct ASTNode ASTNode;
extern int yylineno;
extern FILE *yyin;
int yylex(void);
void yyerror(const char *s);
extern char *yytext;

ASTNode *yyparse_result = NULL;

// Forward declare the AST functions
ASTNode *create_object_node(ASTNode *pairs);
ASTNode *create_array_node(ASTNode *elements);
ASTNode *create_pair_node(char *key, ASTNode *value);
ASTNode *create_string_node(char *value);
ASTNode *create_number_node(int value);
ASTNode *create_float_node(double value);
ASTNode *create_boolean_node(int value);
ASTNode *create_null_node();
ASTNode *create_list_node(ASTNode *item);
ASTNode *append_list_node(ASTNode *item, ASTNode *list);
void print_ast_node(ASTNode *node, int indent);
int process_ast(ASTNode *node, const char *out_dir);
void free_ast(ASTNode *node);

%}

%{
// Add these at the top of your parser.y
void yyerror(const char *s) {
    fprintf(stderr, "Parser error at line %d: %s\n", yylineno, s);
    fprintf(stderr, "Near: %s\n", yytext);
}

// Add this to help with error recovery
int yyerror_silent = 0;
%}

// Add these token definitions if not already present
%token LBRACE '{'
%token RBRACE '}'
%token LBRACKET '['
%token RBRACKET ']'
%token COLON ':'
%token COMMA ','

%union {
    int num;
    double dbl;
    char *str;
    int boolean;
    struct ASTNode *node;  // Use struct keyword here
}

%token <str> STRING
%token <num> NUMBER
%token <dbl> FLOAT
%token <boolean> BOOLEAN
%token NULL_TOKEN

%type <node> json value object array pair pairs elements

%start json

%%

json: value { 
        $$ = $1; 
        if (yyparse_result == NULL) {
            yyparse_result = $$;
        }
    };

value: STRING { $$ = create_string_node($1); }
    | NUMBER { $$ = create_number_node($1); }
    | FLOAT { $$ = create_float_node($1); }
    | BOOLEAN { $$ = create_boolean_node($1); }
    | NULL_TOKEN { $$ = create_null_node(); }
    | object { $$ = $1; }
    | array { $$ = $1; }
    ;


elements: value { $$ = create_list_node($1); }
    | value ',' elements { $$ = append_list_node($1, $3); }
    ;

object: '{' pairs '}' { $$ = create_object_node($2); }
    | '{' '}' { $$ = create_object_node(NULL); }
    | '{' error '}' { 
          yyerror("Invalid object syntax");
          $$ = create_object_node(NULL); 
      }
    ;

array: '[' elements ']' { $$ = create_array_node($2); }
    | '[' ']' { $$ = create_array_node(NULL); }
    | '[' error ']' {
          yyerror("Invalid array syntax");
          $$ = create_array_node(NULL);
      }
    ;

pair: STRING ':' value { $$ = create_pair_node($1, $3); }
    | STRING error {
          yyerror("Expected ':' after key in object");
          $$ = NULL;
      }
    ;
    
pairs: pair { $$ = create_list_node($1); }
    | pair ',' pairs { $$ = append_list_node($1, $3); }
    ;
     
%%
/*
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

    yyin = stdin;
    yyparse();

    if (print_ast && yyparse_result) {
        print_ast_node(yyparse_result, 0);
    }

    if (yyparse_result) {
        process_ast(yyparse_result, out_dir);
        free_ast(yyparse_result);
    }

    return 0;
}*/
