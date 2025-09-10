#include "ast.h"

NovaAST *init_ast() {
    NovaAST *ast = calloc(1, sizeof(*ast));

    ast->endpoints = NULL;
    ast->total_endpoints = 0;
    
    ast->variables = NULL;
    ast->variable_amount = 0;

    ast->logical_actions = NULL;
    ast->number_of_actions = 0;

    return ast;
}

/* Logical operations. */
void perform_print(Print action, NovaLexer *lexer) {
    if(action.data_type == STRING)
        printf("%s", (const char *)action.variable_data.string_data);
    else
        printf("%d\n", action.variable_data.number_data);
}

void perform_logic(LogicalActions *actions, NovaLexer *lexer, uint32_t noa) {
    for(int i = 0; i < noa; i++) {
        switch(actions[i].logical_action_type) {
            case PRINT: perform_print(actions[i].Action.print_action, lexer);break;
            default: {
                fprintf(stderr, "Unknown logical action.\n");
                free(actions);
                free(lexer);
                exit(EXIT_FAILURE);
            }
        }
    }
}