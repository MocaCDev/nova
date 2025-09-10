#ifndef nova_parser
#define nova_parser
#include "lexer.h"
#include "ast.h"
#include "endpoints.h"

/* Stores all critical data for the logical operations of the source code. */
typedef struct {
    /* Data over the endpoints. */
    Endpoint *endpoints;
    uint32_t total_endpoints;

    
} NovaParser;

void start_parsing(const char *filename);

#endif
