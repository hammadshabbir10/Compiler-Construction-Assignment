#include "tables.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

// Helper function declarations
static ASTNode *find_pair_by_key(ASTNode *pairs, const char *key);
static int is_scalar_array(ASTNode *array);
static void format_scalar_array(char *buffer, size_t size, ASTNode *array);
static void process_array(TableContext *context, const char *array_name, 
                         ASTNode *array, const char *parent_table, int parent_id);


JSONError current_error = {0, 0, NULL};

void report_json_error(int line, int column, const char *message) {
    if (current_error.message) free(current_error.message);
    current_error.line = line;
    current_error.column = column;
    current_error.message = strdup(message);
}

int has_json_error() {
    return current_error.message != NULL;
}

void clear_json_error() {
    if (current_error.message) {
        free(current_error.message);
        current_error.message = NULL;
    }
    current_error.line = 0;
    current_error.column = 0;
}

TableContext *create_table_context() {
    TableContext *context = malloc(sizeof(TableContext));
    context->tables = NULL;
    context->current_id = 1;
    return context;
}

void free_table_context(TableContext *context) {
    Table *table = context->tables;
    while (table) {
        Table *next_table = table->next;
        
        // Free columns
        Column *col = table->columns;
        while (col) {
            Column *next_col = col->next;
            free(col->name);
            free(col);
            col = next_col;
        }
        
        // Free rows
        Row *row = table->rows;
        while (row) {
            Row *next_row = row->next;
            
            // Free row values
            RowValue *rv = row->values;
            while (rv) {
                RowValue *next_rv = rv->next;
                free(rv->value);
                free(rv);
                rv = next_rv;
            }
            
            free(row);
            row = next_row;
        }
        
        free(table->name);
        free(table);
        table = next_table;
    }
    free(context);
}

Table *find_or_create_table(TableContext *context, const char *name, Column *columns) {
    Table *table = context->tables;
    while (table) {
        if (strcmp(table->name, name) == 0) {
            // Free the passed columns since we're using existing table
            Column *col = columns;
            while (col) {
                Column *next = col->next;
                free(col->name);
                free(col);
                col = next;
            }
            return table;
        }
        table = table->next;
    }
    
    // Create new table
    table = malloc(sizeof(Table));
    table->name = strdup(name);
    table->columns = columns;
    table->rows = NULL;
    table->row_count = 0;
    table->next = context->tables;
    context->tables = table;
    return table;
}

Column *create_column(const char *name, int is_foreign_key) {
    Column *col = malloc(sizeof(Column));
    col->name = strdup(name);
    col->is_foreign_key = is_foreign_key;
    col->next = NULL;
    return col;
}

Row *create_row(int id, int parent_id) {
    Row *row = malloc(sizeof(Row));
    row->id = id;
    row->parent_id = parent_id;
    row->values = NULL;
    row->next = NULL;
    return row;
}

void add_row_value(Row *row, const char *value) {
    RowValue *rv = malloc(sizeof(RowValue));
    rv->value = value ? strdup(value) : strdup("");
    rv->next = NULL;
    
    if (!row->values) {
        row->values = rv;
    } else {
        RowValue *last = row->values;
        while (last->next) last = last->next;
        last->next = rv;
    }
}

static ASTNode *find_pair_by_key(ASTNode *pairs, const char *key) {
    while (pairs) {
        ASTNode *pair = pairs->item;
        if (pair && pair->type == NODE_PAIR && strcmp(pair->key, key) == 0) {
            return pair;
        }
        pairs = pairs->next;
    }
    return NULL;
}

static int is_scalar_array(ASTNode *array) {
    if (!array || array->type != NODE_ARRAY) return 0;
    
    ASTNode *elem = array->elements;
    while (elem) {
        if (elem->item->type == NODE_OBJECT || elem->item->type == NODE_ARRAY) {
            return 0;
        }
        elem = elem->next;
    }
    return 1;
}

static void format_scalar_array(char *buffer, size_t size, ASTNode *array) {
    if (!array || array->type != NODE_ARRAY) {
        buffer[0] = '\0';
        return;
    }
    
    strncpy(buffer, "[", size);
    size_t pos = 1;
    
    ASTNode *elem = array->elements;
    while (elem && pos < size - 1) {
        if (elem != array->elements) {
            strncat(buffer, ",", size - pos);
            pos++;
        }
        
        switch (elem->item->type) {
            case NODE_STRING:
                snprintf(buffer + pos, size - pos, "\"%s\"", elem->item->string_value);
                break;
            case NODE_NUMBER:
                snprintf(buffer + pos, size - pos, "%d", elem->item->number_value);
                break;
            case NODE_FLOAT:
                snprintf(buffer + pos, size - pos, "%f", elem->item->float_value);
                break;
            case NODE_BOOLEAN:
                snprintf(buffer + pos, size - pos, "%s", elem->item->boolean_value ? "true" : "false");
                break;
            case NODE_NULL:
                snprintf(buffer + pos, size - pos, "null");
                break;
            default:
                break;
        }
        pos = strlen(buffer);
        elem = elem->next;
    }
    
    if (pos < size - 1) {
        strncat(buffer, "]", size - pos);
    }
}

static void process_array(TableContext *context, const char *array_name, 
                         ASTNode *array, const char *parent_table, int parent_id) {
    if (!array || array->type != NODE_ARRAY) return;
    
    // Create columns for array items table
    Column *columns = create_column("id", 0);
    char fk_name[256];
    snprintf(fk_name, sizeof(fk_name), "%s_id", parent_table);
    columns->next = create_column(fk_name, 1);
    
    // Find all unique keys in array objects
    ASTNode *elem = array->elements;
    while (elem) {
        if (elem->item->type == NODE_OBJECT) {
            ASTNode *item_pairs = elem->item->pairs;
            while (item_pairs) {
                ASTNode *pair = item_pairs->item;
                if (pair->type == NODE_PAIR) {
                    // Check if column already exists
                    Column *col = columns->next;
                    while (col) {
                        if (strcmp(col->name, pair->key) == 0) break;
                        col = col->next;
                    }
                    if (!col) {
                        Column *last = columns;
                        while (last->next) last = last->next;
                        last->next = create_column(pair->key, 0);
                    }
                }
                item_pairs = item_pairs->next;
            }
        }
        elem = elem->next;
    }
    
    // Create the table
    char table_name[256];
    snprintf(table_name, sizeof(table_name), "%s_%s", parent_table, array_name);
    Table *table = find_or_create_table(context, table_name, columns);
    
    // Process array elements
    elem = array->elements;
    while (elem) {
        if (elem->item->type == NODE_OBJECT) {
            process_object(context, elem->item, table_name, parent_id);
        }
        elem = elem->next;
    }
}

void process_object(TableContext *context, ASTNode *object, const char *parent_name, int parent_id) {
    if (!object || object->type != NODE_OBJECT) return;
    
    // Create columns for this object (without duplicate id)
    Column *columns = NULL;
    Column *last_col = NULL;
    
    // Only add id column for main table
    if (!parent_name) {
        columns = create_column("id", 0);
        last_col = columns;
    }
    
    // Add parent_id if this is a child object
    if (parent_name) {
        char fk_name[256];
        snprintf(fk_name, sizeof(fk_name), "%s_id", parent_name);
        Column *fk_col = create_column(fk_name, 1);
        if (!columns) columns = fk_col;
        else last_col->next = fk_col;
        last_col = fk_col;
    }
    
    // Process all pairs in the object
    ASTNode *pair_list = object->pairs;
    while (pair_list) {
        ASTNode *pair = pair_list->item;
        if (pair && pair->type == NODE_PAIR) {
            // Skip if this is the "id" field (we already have it)
            if (strcmp(pair->key, "id") != 0) {
                Column *new_col = create_column(pair->key, 0);
                if (!columns) columns = new_col;
                else last_col->next = new_col;
                last_col = new_col;
            }
        }
        pair_list = pair_list->next;
    }
    
    // Find or create table with these columns
    const char *table_name = parent_name ? "child" : "main";
    Table *table = find_or_create_table(context, table_name, columns);
    
    // Create a new row
    int current_id = context->current_id++;
    Row *row = create_row(current_id, parent_id);
    
    // Add values in proper order
    Column *current_col = table->columns;
    while (current_col) {
        char value_buffer[256] = {0};
        
        if (strcmp(current_col->name, "id") == 0) {
            snprintf(value_buffer, sizeof(value_buffer), "%d", current_id);
        }
        else if (current_col->is_foreign_key) {
            snprintf(value_buffer, sizeof(value_buffer), "%d", parent_id);
        }
        else {
            // Find matching pair
            ASTNode *pair = find_pair_by_key(object->pairs, current_col->name);
            if (pair && pair->value) {
                switch (pair->value->type) {
                    case NODE_STRING:
                        snprintf(value_buffer, sizeof(value_buffer), "\"%s\"", pair->value->string_value);
                        break;
                    case NODE_NUMBER:
                        snprintf(value_buffer, sizeof(value_buffer), "%d", pair->value->number_value);
                        break;
                    case NODE_BOOLEAN:
                        snprintf(value_buffer, sizeof(value_buffer), "%s", pair->value->boolean_value ? "true" : "false");
                        break;
                    case NODE_OBJECT:
                        process_object(context, pair->value, table_name, current_id);
                        snprintf(value_buffer, sizeof(value_buffer), "%d", context->current_id - 1);
                        break;
                    case NODE_ARRAY:
                        if (is_scalar_array(pair->value)) {
                            format_scalar_array(value_buffer, sizeof(value_buffer), pair->value);
                        } else {
                            process_array(context, pair->key, pair->value, table_name, current_id);
                            snprintf(value_buffer, sizeof(value_buffer), "see %s_%s", table_name, pair->key);
                        }
                        break;
                    default:
                        snprintf(value_buffer, sizeof(value_buffer), "");
                        break;
                }
            }
        }
        add_row_value(row, value_buffer);
        current_col = current_col->next;
    }
    
    // Add row to table
    if (!table->rows) {
        table->rows = row;
    } else {
        Row *last = table->rows;
        while (last->next) last = last->next;
        last->next = row;
    }
    table->row_count++;
}

void write_row_to_csv(FILE *fp, Row *row, Column *columns) {
    Column *col = columns;
    RowValue *rv = row->values;
    int first = 1;
    
    while (col) {
        if (!first) fprintf(fp, ",");
        
        if (rv) {
            // For id and foreign key columns, we have the actual values
            if (strcmp(col->name, "id") == 0) {
                fprintf(fp, "%d", row->id);
            }
            else if (col->is_foreign_key) {
                fprintf(fp, "%d", row->parent_id);
            }
            else {
                // For regular values, use the stored value
                fprintf(fp, "%s", rv->value);
            }
            rv = rv->next;
        }
        
        first = 0;
        col = col->next;
    }
    fprintf(fp, "\n");
}

void generate_csv_files(TableContext *context, const char *out_dir) {
    // Create output directory if it doesn't exist
    struct stat st = {0};
    if (stat(out_dir, &st) == -1) {
        mkdir(out_dir, 0700);
    }
    
    // For each table, create a CSV file
    Table *table = context->tables;
    while (table) {
        char filename[512];
        snprintf(filename, sizeof(filename), "%s/%s.csv", out_dir, table->name);
        
        FILE *fp = fopen(filename, "w");
        if (!fp) {
            perror("Failed to open file");
            continue;
        }
        
        // Write header
        Column *col = table->columns;
        int first = 1;
        while (col) {
            if (!first) fprintf(fp, ",");
            fprintf(fp, "%s", col->name);
            first = 0;
            col = col->next;
        }
        fprintf(fp, "\n");
        
        // Write all rows
        Row *row = table->rows;
        while (row) {
            write_row_to_csv(fp, row, table->columns);
            row = row->next;
        }
        
        fclose(fp);
        table = table->next;
    }
}
/*
void process_ast(ASTNode *node, const char *out_dir) {
    if (!node) return;
    
    TableContext *context = create_table_context();
    
    if (node->type == NODE_OBJECT) {
        process_object(context, node, NULL, 0);
    }
    
    generate_csv_files(context, out_dir);
    
    // Free all allocated memory
    free_table_context(context);
}
*/
int process_ast(ASTNode *node, const char *out_dir) {
    if (!node) {
        report_json_error(0, 0, "Empty JSON input");
        return 1; // Error code
    }
    
    clear_json_error(); // Clear any previous errors
    
    TableContext *context = create_table_context();
    
    if (node->type == NODE_OBJECT) {
        process_object(context, node, NULL, 0);
    } else {
        report_json_error(0, 0, "Expected JSON object as root element");
        free_table_context(context);
        return 1;
    }
    
    if (has_json_error()) {
        free_table_context(context);
        return 1;
    }
    
    generate_csv_files(context, out_dir);
    free_table_context(context);
    return 0; // Success
}

