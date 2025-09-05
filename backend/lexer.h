#ifndef nova_lexer
#define nova_lexer
#include "../common.h"

/* Token information. */
typedef enum {
    COMMENT = 0x1, // `//`
    LEFT_SB, RIGHT_SB, // `[` `]`
    LEFT_CB, RIGHT_CB, // `{`, `}`
    AT_SYMBOL, // `@`
    BASE_URL, // `base`
    USER_DEFINED,
    INCOMING_REQUEST_TYPE, OUTGOING_REQUEST_TYPE, // `incoming`, `outgoing`
    /* Useable with HEADER and BODY of a HTTP request. */
    EXPECTED, OKAY, REJECT, // `expected`, `okay`, `reject`
    COLON, // `:`
    COMMA, // `,`
    EQUAL, // `=`
    DOUBLE_QUOTE, SINGLE_QUOTE, // `"`, `'`
    NUMBER_VALUE, STRING_VALUE,
    POST, GET,
    END_OF_FILE
} NovaTokens;

typedef struct {
    uint8_t *token_value;
    NovaTokens token_id;
} NovaToken;

/* Lexer information. */
typedef struct {
    /* File information. */
    uint8_t *filename;
    uint8_t *data;
    size_t file_size;

    /* Line information. */
    uint32_t line;

    /* Current character information. */
    uint8_t current_char;
    size_t current_index;

    /* Token information. */
    NovaToken token;
} NovaLexer;

NovaLexer *init_lexer(const char *filename);
void get_token(NovaLexer *lexer);

#endif
