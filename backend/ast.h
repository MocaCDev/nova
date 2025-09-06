#ifndef nova_ast
#define nova_ast
#include "../common.h"
#include "lexer.h"

/* Default base URL. */
static uint8_t *base_url = (uint8_t *)"https://127.0.0.1:3000";

/* Variables. */
typedef enum {
    STRING,
    INTEGER
} VariableType;

typedef struct {
    VariableType variable_type;
    uint8_t *variable_name;
    void *variable_data;
} Variable;

static Variable *variables = NULL;
static uint32_t variable_amount = 0;

/* Logical actions to be performed in the code. */
typedef enum {
    PRINT,
    POST, GET,
    CHECk
} LogicalActionTypes;

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

static LogicalActions *logical_actions = NULL;
static uint32_t number_of_actions = 0;

/* We need to pass the lexer to ensure it gets freed if an error ocurrs. */
void perform_logic(LogicalActions *actions, NovaLexer *lexer, uint32_t noa); // `noa` = number of actions

#endif
