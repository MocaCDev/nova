#ifndef nova_ast
#define nova_ast
#include "../common.h"
#include "endpoints.h"
#include "lexer.h"

/* Default base URL. */
static uint8_t *base_url = (uint8_t *)"https://127.0.0.1:3000";

typedef struct {
    VariableType variable_type;
    uint8_t *variable_name;
    void *variable_data;
} Variable;

/* Logical action `print`. */
typedef struct {
    union {
        void *string_data;
        uint32_t number_data;
    } variable_data;
    size_t data_length;
    VariableType data_type;
} Print;

/* Logical action `post`. */
typedef struct {

} Post;

/* Logical action `get`. */
typedef struct {

} Get;

typedef struct {
    LogicalActionTypes logical_action_type; // What is being done?

    union {
        Print print_action;
    } Action;
} LogicalActions;

/* Stores all critical data for the logical operations of the source code. */
typedef struct {
    /* Data over the endpoints. */
    Endpoint *endpoints;
    uint32_t total_endpoints;

    /* Data over variables. */
    Variable *variables;
    uint32_t variable_amount;

    /* Data over logical operations. */
    LogicalActions *logical_actions;
    uint32_t number_of_actions;
} NovaAST;

/* We need to pass the lexer to ensure it gets freed if an error ocurrs. */
NovaAST *init_ast();
void perform_logic(LogicalActions *actions, NovaLexer *lexer, uint32_t noa); // `noa` = number of actions

#endif
