#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ASTNode *create_object_node(ASTNode *pairs) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_OBJECT;
    node->pairs = pairs;
    return node;
}

ASTNode *create_array_node(ASTNode *elements) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_ARRAY;
    node->elements = elements;
    return node;
}

ASTNode *create_pair_node(char *key, ASTNode *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_PAIR;
    node->key = strdup(key);
    node->value = value;
    return node;
}

ASTNode *create_string_node(char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_STRING;
    node->string_value = strdup(value);
    return node;
}

ASTNode *create_number_node(int value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_NUMBER;
    node->number_value = value;
    return node;
}

ASTNode *create_float_node(double value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_FLOAT;
    node->float_value = value;
    return node;
}

ASTNode *create_boolean_node(int value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_BOOLEAN;
    node->boolean_value = value;
    return node;
}

ASTNode *create_null_node() {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_NULL;
    return node;
}

ASTNode *create_list_node(ASTNode *item) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_LIST;
    node->item = item;
    node->next = NULL;
    return node;
}

ASTNode *append_list_node(ASTNode *item, ASTNode *list) {
    ASTNode *new_node = create_list_node(item);
    new_node->next = list;
    return new_node;
}

void print_ast_node(ASTNode *node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch (node->type) {
        case NODE_OBJECT:
            printf("OBJECT\n");
            ASTNode *pair = node->pairs;
            while (pair) {
                print_ast_node(pair->item, indent + 1);
                pair = pair->next;
            }
            break;
        case NODE_ARRAY:
            printf("ARRAY\n");
            ASTNode *elem = node->elements;
            while (elem) {
                print_ast_node(elem->item, indent + 1);
                elem = elem->next;
            }
            break;
        case NODE_PAIR:
            printf("PAIR: %s\n", node->key);
            print_ast_node(node->value, indent + 1);
            break;
        case NODE_STRING:
            printf("STRING: %s\n", node->string_value);
            break;
        case NODE_NUMBER:
            printf("NUMBER: %d\n", node->number_value);
            break;
        case NODE_FLOAT:
            printf("FLOAT: %f\n", node->float_value);
            break;
        case NODE_BOOLEAN:
            printf("BOOLEAN: %s\n", node->boolean_value ? "true" : "false");
            break;
        case NODE_NULL:
            printf("NULL\n");
            break;
        case NODE_LIST:
            print_ast_node(node->item, indent);
            if (node->next) {
                print_ast_node(node->next, indent);
            }
            break;
        default:
            break;
    }
}

void free_ast(ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_OBJECT:
            free_ast(node->pairs);
            break;
        case NODE_ARRAY:
            free_ast(node->elements);
            break;
        case NODE_PAIR:
            free(node->key);
            free_ast(node->value);
            break;
        case NODE_STRING:
            free(node->string_value);
            break;
        case NODE_LIST:
            free_ast(node->item);
            free_ast(node->next);
            break;
        default:
            break;
    }
    
    free(node);
}
