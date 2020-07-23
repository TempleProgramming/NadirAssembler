
// Debug flags for printouts
#define DEBUG_TOKENS	1	// Prints tokens

// Error codes
#define SUCCESS				0
#define FAIL				1
#define FAIL_TOK_CHARINT	2	// Failure to read character int
#define FAIL_TOK_EOF		3	// End of file before completion of token

// General tokens that can be resolved in one character
#define TOK_EOF		1	// End of file
#define TOK_COMMA	2	// ,
#define TOK_COLON	3	// :
#define TOK_DOT		4	// .
#define TOK_ASSIGN	5	// =
#define TOK_LEFTSB	6	// [
#define TOK_RIGHTSB	7	// ]

// Abstract tokens that are multiple characters in length
#define TOK_IDENT	8	// Identifier (ie. Operations/Variables)
#define TOK_STR		9	// String literal ("symbol")
#define TOK_INT		10	// Integer literal (#symbol)

/* Variables/state relating to source code being compiled. */
class CSrc
{
	U8 *buf;			// Buffer for source file
	I64 len;			// Length of buffer
	CTok *firstTok;		// First token in the token chain
	I64 cursor;			// Current position in file to be parsed
	I64 line;			// Current line in the file
	I64 lastNewLine;	// Position of last new line (for calculating column)
};

/* Token class */
class CTok
{
	CTok *prev, *next;	// Previous/Next token in chain
	I64 type;			// Type of token
	I64 line, col;		// For returning parsing errors/debugging
	U8 *symbol;			// Symbol/Value/String of token if applicable
};

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

		case TOK_ASSIGN:
			"TOK_ASSIGN\n";
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
				"%d\n", tok->symbol;
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

/* Fetches next token from source file. */
I64 FetchToken(CSrc *src, CTok *tok)
{
	// Points to end of identifiers to MemCopy into symbol
	I64 endCursor;
	I64 strLen;
	I64 i;
	U8 *tempStr;

	while (src->cursor < src->len)
	{
		switch (src->buf[src->cursor])
		{
			// Comment, skip ahead to next line
			case ';':
				while (src->buf[src->cursor] != '\n' && src->cursor < src->len)
				{
					src->cursor++;
				}
				if (src->buf[src->cursor] == '\n')
					goto FetchToken_NewLine;

				if (src->cursor >= src->len)
					goto FetchToken_EOF;

				break;

			// New line, update line number and go to next character
			case '\n':
FetchToken_NewLine:
				src->lastNewLine = src->cursor;
				src->line++;
				src->cursor++;
				break;

			// Simple single character tokens
			case ',':
				tok->type = TOK_COMMA;
				goto FetchToken_SaveLineColAndInc;

			case ':':
				tok->type = TOK_COLON;
				goto FetchToken_SaveLineColAndInc;

			case '.':
				tok->type = TOK_DOT;
				goto FetchToken_SaveLineColAndInc;

			case '=':
				tok->type = TOK_ASSIGN;
				goto FetchToken_SaveLineColAndInc;

			case '[':
				tok->type = TOK_LEFTSB;
				goto FetchToken_SaveLineColAndInc;

			case ']':
				tok->type = TOK_RIGHTSB;
				goto FetchToken_SaveLineColAndInc;

			case '"':
				tok->type = TOK_STR;
				goto FetchToken_SaveLineColAndInc;

			// Char integer, read in symbol and convert to binary integer
			case '\'':
				if (src->cursor + 2 < src->len)
				{
					if (src->cursor + 3 < src->len)
					{
						// Escape character, ie. '\n'
						if (src->buf[src->cursor + 3] == '\'' && 
							src->buf[src->cursor + 1] == '\\')
						{
							switch (src->buf[src->cursor + 2])
							{
								case 'n':
									tok->symbol = '\n';
									break;

								case 't':
									tok->symbol = '\t';
									break;

								case '\"':
									tok->symbol = '\"';
									break;

								case '\'':
									tok->symbol = '\'';
									break;

								case '\\':
									tok->symbol = '\\';
									break;

								default:
									goto FetchToken_FailCharInt;
							}
							src->cursor += 4;
							goto FetchToken_SaveLineCol;
						}
					}
					// Regular character, ie. 'a'
					if (src->buf[src->cursor + 2] == '\'')
					{
						tok->symbol = src->buf[src->cursor + 1];
						src->cursor += 3;
						goto FetchToken_SaveLineCol;
					}
FetchToken_FailCharInt:
					tok->line = src->line;
					tok->col = src->cursor - src->lastNewLine;
					return FAIL_TOK_CHARINT;
				}
				// End of file before completion of character integer
				goto FetchToken_FailEOF;

			// Integer, read in symbol and convert to binary integer
			case '#':
				tok->type = TOK_INT;
				endCursor = src->cursor + 1;
				while (endCursor < src->len)
				{
					switch (src->buf[endCursor])
					{
						case 'x':	// Hex
						case 'o':	// Octal
						case 'a'...'f':	// A-F for hex numbers and dec/bin intsv
						case 'A'...'F':
						case '0'...'9':
							endCursor++;
							break;

						// End of integer
						default:
							goto FetchToken_EndOfInt;
					}
				}
FetchToken_EndOfInt:
				// Convert string of int to actual int
				strLen = endCursor - (src->cursor + 1);	// +1 to skip #
				tempStr = CAlloc(strLen + 1);	// +1 for null terminator
				MemCopy(tempStr, src->buf + src->cursor + 1, strLen);
				"[INT STRING] %s\n", tempStr;
				tok->symbol = Str2I64(tempStr);
				src->cursor = endCursor;
				Free(tempStr);
				return SUCCESS;

			// String, read in symbol
			case '\"':
				tok->type = TOK_STR;
				endCursor = src->cursor + 1;
				strLen = 0;
				while (endCursor < src->len)
				{
					if (src->buf[endCursor] == '\"')
					{
						// End of string, load into symbol and convert esc seqs
						tok->symbol = CAlloc(strLen + 1); // +1 for null term
						for (i = 0; i < strLen
					}
					else
					{
						if (src->buf[endCursor] == '\\')
						{
							if (src->buf[endCursor - 1] == 
						}
						else
						{
							strLen++;
						}
						endCursor++;
					}
				}
				// Hit end of file before end of string
				goto FetchToken_FailEOF;

			// Identifier, read in symbol until a non-identifier character
			case 'A'...'Z':
			case 'a'...'z':
			case '_':
				tok->type = TOK_IDENT;
				endCursor = src->cursor + 1;
				while (endCursor < src->len)
				{
					switch (src->buf[endCursor])
					{
						case 'A'...'Z':
						case 'a'...'z':
						case '_':
						case '0'...'9':
							endCursor++;
							break;
					}
				}
FetchToken_EndOfIdent:
				strLen = endCursor - src->cursor;
				tok->symbol = CAlloc(strLen + 1); // +1 for null terminator
				MemCopy(tok->symbol, src->buf + src->cursor, strLen);
				src->cursor = endCursor;
				goto FetchToken_SaveLineCol;
		}

		// No particular token detected, read next character
		src->cursor++;		
	}
FetchToken_EOF:
	tok->type = TOK_EOF;

FetchToken_SaveLineColAndInc:
	src->cursor++;

FetchToken_SaveLineCol:
	tok->line = src->line;
	tok->col = src->cursor - src->lastNewLine;
	return SUCCESS;

FetchToken_FailEOF:
	tok->line = src->line;
	tok->col = src->cursor - src->lastNewLine;
	return FAIL_TOK_EOF;
}
