#ifndef nova_ast
#define nova_ast
#include "../common.h"

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

#endif
