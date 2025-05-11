#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    NODE_OBJECT,
    NODE_ARRAY,
    NODE_PAIR,
    NODE_STRING,
    NODE_NUMBER,
    NODE_FLOAT,
    NODE_BOOLEAN,
    NODE_NULL,
    NODE_LIST
} NodeType;

typedef struct ASTNode ASTNode;

struct ASTNode {
    NodeType type;
    union {
        struct { // OBJECT
            ASTNode *pairs;
        };
        struct { // ARRAY
            ASTNode *elements;
        };
        struct { // PAIR
            char *key;
            ASTNode *value;
        };
        struct { // LIST
            ASTNode *item;
            ASTNode *next;
        };
        char *string_value;
        int number_value;
        double float_value;
        int boolean_value;
    };
};

// Creation functions
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

// Utility functions
void print_ast_node(ASTNode *node, int indent);
void free_ast(ASTNode *node);

#endif
