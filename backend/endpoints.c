#include "endpoints.h"

Endpoint init_endpoint() {
    Endpoint endpoint;

    endpoint.endpoint = NULL;

    endpoint.endpoint_expected_http_header = NULL;
    endpoint.expected_http_header_entries = 0;

    endpoint.endpoint_okay_http_header = NULL;
    endpoint.okay_http_header_entries = 0;

    endpoint.endpoint_reject_http_header = NULL;
    endpoint.reject_http_header_entries = 0;

    return endpoint;
}