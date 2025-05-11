#ifndef TABLES_H
#define TABLES_H

#include "ast.h"

typedef struct Column {
    char *name;
    int is_foreign_key;
    struct Column *next;
} Column;

typedef struct RowValue {
    char *value;
    struct RowValue *next;
} RowValue;

typedef struct Row {
    int id;
    int parent_id;
    RowValue *values;
    struct Row *next;
} Row;

typedef struct Table {
    char *name;
    Column *columns;
    Row *rows;
    int row_count;
    struct Table *next;
} Table;

typedef struct TableContext {
    Table *tables;
    int current_id;
} TableContext;

typedef struct {
    int line;
    int column;
    char *message;
} JSONError;

void report_json_error(int line, int column, const char *message);

// Main interface functions
int process_ast(ASTNode *node, const char *out_dir);
void generate_csv_files(TableContext *context, const char *out_dir);

// Helper functions (needed for the implementation)
void process_object(TableContext *context, ASTNode *object, const char *parent_name, int parent_id);

#endif
