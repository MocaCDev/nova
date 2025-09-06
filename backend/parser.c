#include "parser.h"
#include "ast.h"
#include "lexer.h"

void parse_variable_declaration(NovaLexer *lexer) {
    get_token(lexer);

    /* Most likely not needed, as the lexer handles most of this logic. */
    nova_assert(
        !(lexer->token.token_id == USER_DEFINED) && !(lexer->token.token_id == BASE_URL),
        "\e[1;31mExpected `base` or a user-defined variable after `@`, \e[4;31mline %d.\e[0m\n",
        {
            free(lexer);
        },
        lexer->line
    )

    if(lexer->token.token_id == BASE_URL) {
        get_token(lexer);
        nova_assert(
            lexer->token.token_id != EQUAL,
            "\e[1;31mExpected `=` after `base`, \e[4;31mline %d.\e[0m\n",
            {
                free(lexer);
            },
            lexer->line
        )
        get_token(lexer);

        nova_assert(
            lexer->token.token_id != STRING_VALUE,
            "\e[1;31mExpected string for `base`, \e[4;31mline %d.\e[0m\n",
            {
                free(lexer);
            },
            lexer->line
        )

        base_url = lexer->token.token_value;
        printf("Base URL: %s\n", base_url);

        get_token(lexer);
        return;
    }

    uint8_t *var_name = lexer->token.token_value;

    /* If there are existing variables, ensure the variable name does not exist yet. */
    if(variable_amount > 0) {
        for(int i = 0; i < variable_amount; i++) {
            nova_assert(
                (strcmp((char *)variables[i].variable_name, (const char *)var_name) == 0),
                "\e[1;31mVariable \e[4;31m`%s`\e[0m\e[1;31m exists, \e[4;31mline %d.\e[0m\n",
                {
                    free(lexer);
                },
                var_name, lexer->line
            )
        }
    }

    get_token(lexer);
    nova_assert(
        lexer->token.token_id != EQUAL,
        "\e[1;31mExpected `=` after variable name, \e[4;31mline %d.\e[0m\n",
        {
            free(lexer);
        },
        lexer->line
    )
    get_token(lexer);

    VariableType variable_type = lexer->token.token_id == STRING_VALUE ? STRING : INTEGER;

    if(!variables) {
        variables = calloc(1, sizeof(*variables));

        variables[0].variable_name = var_name;
        variables[0].variable_data = lexer->token.token_value;
        variables[0].variable_type = variable_type;

        variable_amount++;
    } else {
        variables = realloc(
            variables,
            (variable_amount + 1) * sizeof(*variables)
        );

        variables[variable_amount].variable_name = var_name;
        variables[variable_amount].variable_data = lexer->token.token_value;
        variables[variable_amount].variable_type = variable_type;

        variable_amount++;
    }

    get_token(lexer);
}

void parse_print_statement(NovaLexer *lexer) {
    get_token(lexer);

    nova_assert(
        !(lexer->token.token_id == STRING_VALUE) && !(lexer->token.token_id == NUMBER_VALUE) && !(lexer->token.token_id == AT_SYMBOL),
        "\e[1;31mExpected a string or number for `print`, \e[4;31mline %d.\e[0m\n",
        {
            free(lexer->token.token_value);
            free(lexer);
        },
        lexer->line
    )

    void *data_to_print = NULL;
    VariableType data_type;
    
    if(lexer->token.token_id == AT_SYMBOL) {
        get_token(lexer);

        nova_assert(
            lexer->token.token_id != USER_DEFINED,
            "Expected variable name, line %d.\n",
            {
                free(lexer->token.token_value);
                free(lexer);
            },
            lexer->line
        )

        nova_assert(
            !(variable_amount > 0),
            "No variable declarations were found.\n",
            {
                free(lexer->token.token_value);
                free(lexer);
            },
            NULL
        )

        for(int i = 0; i < variable_amount; i++) {
            if(strcmp((const char *)variables[i].variable_name, (const char *)lexer->token.token_value) == 0) {
                data_to_print = (void *)variables[i].variable_data;
                data_type = variables[i].variable_type;
                break;
            }

            nova_assert(
                i == variable_amount - 1,
                "Variable %s not found, line %d.\n",
                {
                    free(lexer->token.token_value);
                    free(lexer);
                },
                lexer->token.token_value, lexer->line
            )
        }
    } else {
        data_to_print = (void *)lexer->token.token_value;
        data_type = lexer->token.token_id == STRING_VALUE ? STRING : INTEGER;
    }

    if(!logical_actions) {
        logical_actions = calloc(1, sizeof(*logical_actions));
        
        logical_actions[0].logical_action_type = PRINT;
        if(data_type == STRING) {
            logical_actions[0].Action.print_action.variable_data.string_data = data_to_print;
            logical_actions[0].Action.print_action.data_length = strlen((const char *)data_to_print);
        } else {
            logical_actions[0].Action.print_action.variable_data.number_data = atoi((const char *)data_to_print);
            logical_actions[0].Action.print_action.data_length = sizeof(uint32_t);
        }
        
        logical_actions[0].Action.print_action.data_type = data_type;

        number_of_actions++;
    } else {
        logical_actions = realloc(
            logical_actions,
            (number_of_actions + 1) * sizeof(*logical_actions)
        );

        logical_actions[number_of_actions].logical_action_type = PRINT;

        if(data_type == STRING) {
            logical_actions[number_of_actions].Action.print_action.variable_data.string_data = data_to_print;
            logical_actions[number_of_actions].Action.print_action.data_length = strlen((const char *)data_to_print);
        } else {
            logical_actions[number_of_actions].Action.print_action.variable_data.number_data = atoi((const char *)data_to_print);
            logical_actions[number_of_actions].Action.print_action.data_length = sizeof(uint32_t);
        }

        logical_actions[number_of_actions].Action.print_action.data_type = data_type;

        number_of_actions++;
    }

    get_token(lexer);
}

void parse_expected_header(NovaLexer *lexer) {
    get_token(lexer); // `]`

    nova_assert(
        lexer->token.token_id != RIGHT_SB,
        "Expected `]`, line %d.\n",
        {
            free(lexer->token.token_value);
            free(lexer);
        },
        lexer->line
    )

    get_token(lexer);

    nova_assert(
        lexer->token.token_id != LEFT_CB,
        "Expected `{` following `[expected]`, line %d.\n",
        {
            free(lexer->token.token_value);
            free(lexer);
        },
        lexer->line
    )

    get_token(lexer);

    while(lexer->token.token_id != RIGHT_CB) {

    }

    get_token(lexer);
}

void parse_okay_header(NovaLexer *lexer) {
    get_token(lexer); // `]`

    nova_assert(
        lexer->token.token_id != RIGHT_SB,
        "Expected `]`, line %d.\n",
        {
            free(lexer->token.token_value);
            free(lexer);
        },
        lexer->line
    )

    get_token(lexer);

    nova_assert(
        lexer->token.token_id != LEFT_CB,
        "Expected `{` following `[expected]`, line %d.\n",
        {
            free(lexer->token.token_value);
            free(lexer);
        },
        lexer->line
    )

    get_token(lexer);

    while(lexer->token.token_id != RIGHT_CB) {

    }

    get_token(lexer);
}

void parse_rejected_header(NovaLexer *lexer) {
    get_token(lexer); // `]`

    nova_assert(
        lexer->token.token_id != RIGHT_SB,
        "Expected `]`, line %d.\n",
        {
            free(lexer->token.token_value);
            free(lexer);
        },
        lexer->line
    )

    get_token(lexer);

    nova_assert(
        lexer->token.token_id != LEFT_CB,
        "Expected `{` following `[expected]`, line %d.\n",
        {
            free(lexer->token.token_value);
            free(lexer);
        },
        lexer->line
    )

    get_token(lexer);

    while(lexer->token.token_id != RIGHT_CB) {

    }

    get_token(lexer);
}

void parse_headers_statement(NovaLexer *lexer) {
    get_token(lexer);

    nova_assert(
        lexer->token.token_id != LEFT_CB,
        "Expected '{` after `headers`, line %d.\n",
        {
            free(lexer->token.token_value);
            free(lexer);
        },
        lexer->line
    )

    get_token(lexer);

    while(lexer->token.token_id != RIGHT_CB) {
        switch(lexer->token.token_id) {
            case LEFT_SB: {
                get_token(lexer); // `[`

                nova_assert(
                    lexer->token.token_id != EXPECTED_KW && lexer->token.token_id != OKAY_KW && lexer->token.token_id != REJECT_KW,
                    "Expected `expected`, `okay`, or `reject`, line %d.\n",
                    {
                        free(lexer->token.token_value);
                        free(lexer);
                    },
                    lexer->line
                )

                switch(lexer->token.token_id) {
                    case EXPECTED_KW: parse_expected_header(lexer);break;
                    case OKAY_KW: parse_okay_header(lexer);break;
                    case REJECT_KW: parse_rejected_header(lexer);break;
                    default: {
                        fprintf(stderr, "Unexpected error.\n");
                        free(lexer->token.token_value);
                        free(lexer);
                        exit(EXIT_FAILURE);
                    }
                }

                break;
            }
            default: {
                fprintf(stderr, "Unrecognized format for `headers`, line %d.\n", lexer->line);
                free(lexer->token.token_value);
                free(lexer);
                exit(EXIT_FAILURE);
            }
        }
    }

    get_token(lexer);
}

void parse_endpoint_config(NovaLexer *lexer) {
    get_token(lexer);

    nova_assert(
        lexer->token.token_id != LEFT_P,
        "Expected `(` after `endpoint`, line %d.\n",
        {
            free(lexer->token.token_value);
            free(lexer);
        },
        lexer->line
    )

    get_token(lexer);

    nova_assert(
        lexer->token.token_id != STRING_VALUE,
        "Expected string for endpoint, line %d.\n",
        {
            free(lexer->token.token_value);
            free(lexer);
        },
        lexer->line
    )

    uint8_t *endpoint = lexer->token.token_value;

    get_token(lexer);

    nova_assert(
        lexer->token.token_id != RIGHT_P,
        "Expected `)`, line %d.\n",
        {
            free(lexer->token.token_value);
            free(lexer);
        },
        lexer->line
    )

    get_token(lexer);

    nova_assert(
        lexer->token.token_id != LEFT_CB,
        "Expected `{`, line %d.\n",
        {
            free(lexer->token.token_value);
            free(lexer);
        },
        lexer->line
    )

    get_token(lexer);

    while(lexer->token.token_id != RIGHT_CB) {
        switch(lexer->token.token_id) {
            case HEADERS_KW: parse_headers_statement(lexer);break;
            default: {
                fprintf(stderr, "Unexpected keyword %s, line %d.\n", lexer->token.token_value, lexer->line);
                free(lexer->token.token_value);
                free(lexer);
                exit(EXIT_FAILURE);
            }
        }
    }

    get_token(lexer);
}

void start_parsing(const char *filename) {
    NovaLexer *lexer = init_lexer(filename);
    printf("Lexer initialized.\n");

    get_token(lexer);

    while(lexer->token.token_id != END_OF_FILE) {
        switch(lexer->token.token_id) {
            case AT_SYMBOL: parse_variable_declaration(lexer);break;
            case PRINT_KW: parse_print_statement(lexer);break;
            case ENDPOINT_KW: parse_endpoint_config(lexer);break;
            default: {
                fprintf(stderr, "\e[1;31mAn unknown error ocurred\e[0m.\n");
                free(lexer);
                exit(EXIT_FAILURE);
            }
        }
    }

    perform_logic(logical_actions, lexer, number_of_actions);

    free(lexer);
    printf("Lexer released.\n");
}
