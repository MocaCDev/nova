#ifndef nova_endpoints
#define nova_endpoints
#include "../common.h"

typedef struct {
    // Couner on how many items are in the given header configuration.
    uint32_t entries;

    uint8_t *key;
    uint8_t *value;
    bool is_variable;
} HTTPHeader;

typedef struct {
    uint8_t *key;
    VariableType data_type;
} HTTPBody;

typedef struct {
    uint8_t *endpoint;
    LogicalActionTypes endpoint_type;

    /* Configuration for the HTTP header. */
    HTTPHeader *endpoint_expected_http_header;
    uint32_t expected_http_header_entries;

    HTTPHeader *endpoint_okay_http_header;
    uint32_t okay_http_header_entries;

    HTTPHeader *endpoint_reject_http_header;
    uint32_t reject_http_header_entries;

    /* Configuration for the HTTP body. */
    HTTPBody *endpoint_expected_http_body;
    HTTPBody *endpoint_okay_http_body;
    HTTPBody *endpoint_rejected_http_body;
} Endpoint;

//static Endpoint *endpoints = NULL;
//static uint32_t total_endpoints = 0;

Endpoint init_endpoint();

#endif