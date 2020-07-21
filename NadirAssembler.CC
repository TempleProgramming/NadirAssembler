
// Debug flags for printouts
#define DEBUG_TOKENS	1	// Prints tokens

// General tokens that can be resolved in one character
#define TOK_EOF		1	// End of file
#define TOK_COMMA	2	// ,
#define TOK_COLON	3	// :
#define TOK_DOT		4	// .
#define TOK_EQUAL	5	// =
#define TOK_LEFTSB	6	// [
#define TOK_RIGHTSB	7	// ]

// Abstract tokens that are multiple characters in length
#define TOK_IDENT	8	// Identifier (ie. Operations/Variables)
#define TOK_STR		9	// String literal
#define TOK_INT		10	// Integer literal

/* Token class */
class CTok
{
	I64 type;			// Type of token
	I64 line, column;	// For returning parsing errors/debugging
	U8 *symbol;			// Symbol/Value/String of token if applicable
}

/******************************************************************************
* DEBUG/PRINTOUT FUNCTIONS                                                    *
******************************************************************************/

/* Print a token, line/col, and symbol. */ 
U0 PrintToken(CTok *tok)
{
	"[TOKEN] LINE: %d COL: %d TYPE: ", line, column;
	switch (tok->type)
	{
		case TOK_EOF:
			"TOK_EOF\n";
			return;

		case TOK_COMMA:
			"TOK_COMMA\n";
			return;

		case TOK_COLON:
			"TOK_COLON\n";
			return;

		case TOK_DOT:
			"TOK_DOT\n";
			return;

		case TOK_EQUAL:
			"TOK_EQUAL\n";
			return;

		case TOK_LEFTSB:
			"TOK_LEFTSB\n";
			return;

		case TOK_IDENT:
			"TOK_IDENT SYMBOL: ";
			break;

		case TOK_STR:
			"TOK_STR SYMBOL: ";
			break;

		case TOK_INT:
			"TOK_INT SYMBOL: ";
			break;
	}

	if (tok->symbol != NULL)
	{
		switch (tok->type)
		{
			case TOK_STR:
			case TOK_IDENT:
				"%s\n", tok->symbol;
				return;

			case TOK_INT:
				"%d\n", *tok->symbol;
		}
	}
	else
	{
		"[NULL]\n";
	}
}

/******************************************************************************
* LEXER                                                                       *
******************************************************************************/
