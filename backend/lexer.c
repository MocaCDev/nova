#include "lexer.h"
#include <string.h>

NovaLexer *init_lexer(const char *filename) {
    NovaLexer *lexer = (NovaLexer *)calloc(1, sizeof(*lexer));

    lexer->filename = (uint8_t *)filename;

    FILE *source_file = fopen(filename, "rb");

    nova_assert(
        !source_file,
        "\e[1;31mSomething went wrong.\e[0m\n",
        {
            free(lexer);
        },
        NULL
    )

    fseek(source_file, 0, SEEK_END);
    lexer->file_size = ftell(source_file);
    fseek(source_file, 0, SEEK_SET);

    lexer->data = calloc(lexer->file_size, sizeof(*lexer->data));

    fread((void *)lexer->data, sizeof(*lexer->data), lexer->file_size, source_file);
    fclose(source_file);

    lexer->line = 1;
    lexer->current_index = 0;
    lexer->current_char = lexer->data[lexer->current_index];

    return lexer;
}

/* Core of the lexer. */
bool peak(NovaLexer *lexer, uint8_t cv) { // `cv` = check value
    if(lexer->current_index + 1 > lexer->file_size) return false;

    return lexer->data[lexer->current_index + 1] == cv;
}

void advance_lexer(NovaLexer *lexer) {
    if(lexer->current_char == '\n')
        lexer->line++;

    if(lexer->current_index + 1 < lexer->file_size) {
        lexer->current_index++;
        lexer->current_char = lexer->data[lexer->current_index];

        return;
    }

    /* Reached end of the source code file. */
    lexer->current_char = '\0';
}

void skip_whitespace(NovaLexer *lexer) {
    while(lexer->current_char == ' ' || lexer->current_char == '\n')
        advance_lexer(lexer);
}

void get_string(NovaLexer *lexer) {
    lexer->token.token_value = calloc(1, sizeof(*lexer->token.token_value));
    size_t index = 0;

    while(lexer->current_char != '"') {
        lexer->token.token_value[index] = lexer->current_char;
        index++;

        lexer->token.token_value = realloc(
            lexer->token.token_value,
            (index + 1) * sizeof(*lexer->token.token_value)
        );
        advance_lexer(lexer);

        nova_assert(
            lexer->current_char == '\n',
            "\e[1;31mUnexpected newline, \e[4;31mline %d.\e[0m\n",
            {
                free(lexer);
            },
            lexer->line
        )

        nova_assert(
            lexer->current_char == '\0',
            "\e[1;31mUnexpected EOF, line %d.\e[0m\n",
            {
                free(lexer);
            },
            lexer->line
        )
    }

    lexer->token.token_value[index] = '\0';
}

void get_number(NovaLexer *lexer) {
    lexer->token.token_value = calloc(1, sizeof(*lexer->token.token_value));
    size_t index = 0;

    while(is_number(lexer->current_char)) {
        lexer->token.token_value[index] = lexer->current_char;
        index++;

        lexer->token.token_value = realloc(
            lexer->token.token_value,
            (index + 1) * sizeof(*lexer->token.token_value)
        );
        advance_lexer(lexer);

        nova_assert(
            lexer->current_char == '\0',
            "\e[1;31mUnexpected EOF, \e[4;31mline %d.\e[0m\n",
            {
                free(lexer);
            },
            lexer->line
        )
    }

    lexer->token.token_value[index] = '\0';
}

void tokenize_character(NovaLexer *lexer, NovaTokens id) {
    lexer->token.token_value = calloc(2, sizeof(uint8_t));

    lexer->token.token_value[0] = lexer->current_char;
    lexer->token.token_value[1] = '\0';

    lexer->token.token_id = id;
}

void get_token(NovaLexer *lexer) {
    top:
    if(lexer->token.token_id == AT_SYMBOL) {
        /* Ensure we get a user-define variable name. */
        nova_assert(
            peak(lexer, '\0'),
            "\e[1;31mUnexpected EOF, \e[4;31mline %d.\e[0m\n",
            {
                free(lexer);
            },
            lexer->line
        )

        /* Ensure it is an ASCII character. */
        nova_assert(
            !(is_ascii(lexer->current_char)),
            "\e[1;31mExpected character after `@`, \e[4;31mline %d.\e[0m\n",
            {
                free(lexer);
            },
            lexer->line
        )

        lexer->token.token_value = calloc(1, sizeof(*lexer->token.token_value));
        size_t variable_size = 0;

        while(lexer->current_char != '=') {
            lexer->token.token_value[variable_size] = lexer->current_char;
            variable_size++;

            advance_lexer(lexer);

            /* Since this while loop keeps going until `=` is found, we find `\0` first throw an error. */
            nova_assert(
                lexer->current_char == '\0',
                "\e[1;31mExpected `=` after variable declaration, \e[4;31mline %d.\e[0m\n",
                {
                    free(lexer->token.token_value);
                    free(lexer);
                },
                lexer->line
            )

            lexer->token.token_value = realloc(
                lexer->token.token_value,
                (variable_size + 1) * sizeof(*lexer->token.token_value)
            );
        }

        lexer->token.token_value[variable_size] = '\0';

        if(strcmp((const char *)lexer->token.token_value, "base") == 0) {
            lexer->token.token_id = BASE_URL;
            return;
        }

        lexer->token.token_id = USER_DEFINED;
        return;
    }

    if(lexer->current_char == '\0')
    {
        lexer->token.token_id = END_OF_FILE;
        lexer->token.token_value = calloc(1, sizeof(uint8_t));
        lexer->token.token_value[0] = '\0';
        return;
    }

    if(lexer->current_char == ' ' || lexer->current_char == '\n')
    {
        skip_whitespace(lexer);
        goto top;
    }

    if(lexer->current_char == '/') {
        nova_assert(
            !peak(lexer, '/'),
            "\e[1;31mMissing `/` in comment on \e[4;31mline %d.\e[0m\n",
            {
                free(lexer);
            },
            lexer->line
        )
        while(lexer->current_char != '\n' && lexer->current_char != '\0')
            advance_lexer(lexer);

        goto top;
    }

    if(lexer->current_char == '"') {
        advance_lexer(lexer); // `"`
        get_string(lexer);
        advance_lexer(lexer); // `"`

        lexer->token.token_id = STRING_VALUE;

        return;
    }

    if(is_number(lexer->current_char)) {
        get_number(lexer);

        lexer->token.token_id = NUMBER_VALUE;

        return;
    }

    switch(lexer->current_char) {
        case '@': {
            tokenize_character(lexer, AT_SYMBOL);
            advance_lexer(lexer);
            return;
        }
        case '=': {
            tokenize_character(lexer, EQUAL);
            advance_lexer(lexer);
            return;
        }
        default: {
            fprintf(stderr, "\e[1;31mUnknown character, \e[4;31mline %d\e[0m", lexer->line);
            free(lexer);
            exit(EXIT_FAILURE);
        }
    }
}
