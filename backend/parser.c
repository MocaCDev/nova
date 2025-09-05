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

    if(variable_amount >= 1) {
        for(int i = 0; i < variable_amount; i++) {
            printf("%s: %s, type: %s\n",
                variables[i].variable_name,
                (char *)variables[i].variable_data,
                variables[i].variable_type == STRING ? "string" : "integer"
            );
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
            default: {
                fprintf(stderr, "\e[1;31mAn unknown error ocurred\e[0m");
                free(lexer);
                exit(EXIT_FAILURE);
            }
        }
    }

    free(lexer);
    printf("Lexer released.\n");
}
