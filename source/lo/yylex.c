/*
 * yylex - Lexical analyzer for Lo
 */


#include "y.tab.h"
#include "defs.h"

#define IsNumeric(x)   ((x) >= '0' && (x) <= '9')

YYSTYPE  *Symbol_ptr;

extern YYSTYPE yylval;
extern char *delim_chars;

struct ke_keyword {
    char *ke_name;
    int ke_num;
} keyword[] = {
"!", 61,
"%", 39,
"'", 5,
"(", 1,
")", 2,
"*", 3,
"+", 4,
"-", 6,
"/", 7,
":", 93,
"<", 58,
"=", 59,
">", 60,
"[", 56,
"]", 57,
"address", 8,
"and",     9,
"are",     72,
"array",   10,
"arrays",  10,
"be",      11,
"begin",   12,
"boolean", 66,
"booleans",66,
"break",   13,
"by",      14,
"byte",    15,
"bytes",   15,
"case",	   94,
"character", 16,
"characters", 16,
"content", 17,
"contents",17,
"continue",19,
"declare", 20,
"decrement",70,
"default", 95,
"define",  62,
"divided", 7,
"do",      61,
"does",    61,
"dump",    71,
"each",    92,
"eight",   88,
"else",    23,
"end",     24,
"enumeration",64,
"equals",  59,
"exist",   27,
"exists",  27,
"external",28,
"false",   76,
"five",    85,
"for",     91,
"four",    84,
"function",63,
"global",  29,
"greater", 60,
"if",      31,
"include", 73,
"increment",69,
"integer", 32,
"integers",32,
"is",      72,
"less",    58,
"let",     34,
"local",   35,
"long",    98,
"loop",    36,
"means",   37,
"minus",   6,
"modulus", 39,
"more",    60,
"nine",    89,
"not",     40,
"null",    97,
"one",	   81,
"optionally",74,
"or",      41,
"out",     42,
"permanent",43,
"plus",    4,
"pointer", 45,
"pointers",45,
"record",  46,
"records", 46,
"register",65,
"repeat",  77,
"return",  67,
"returning",68,
"select",  96,
"seven",   87,
"short",   47,
"six",     86,
"statement",48,
"string",  49,
"strings", 49,
"ten",     90,
"than",    50,
"then",    51,
"three",   83,
"times",   3,
"true",    75,
"two",     82,
"type",    53,
"union",   79,
"unions",  79,
"unsigned",54,
"until",   78,
"while",   55,
"zero",	   80,
};        


#define KEYWORD_LEN  (sizeof(keyword) / sizeof(struct ke_keyword))
static char string[256];


int
yylex()
{
    register char buf[256], buf2[256];
    register YYSTYPE *ptr;
    int code;
    int i;
    extern int reclexnum;
    extern boolean is_parameter;

    if (get_word(buf) <= 0)
        return(-1);

    if (reclexnum) {
	buf[240] = '\0';
	sprintf(buf2, "zz%d_%s", reclexnum, buf);
	reclexnum = 0;
	strcpy(buf, buf2);
    }

    code = p_binsearch(buf, &keyword[0].ke_name, sizeof(struct ke_keyword),
        KEYWORD_LEN);


    if (code >= 0)
	if (!equal(keyword[code].ke_name, buf))
		code = -1;
	else
	    code = keyword[code].ke_num;

    zero(&yylval, sizeof(YYSTYPE));
    for (i=0; i < STACK_SIZE; ++i)
        yylval.type[i] = END_OF_STACK;
    if (code > 0)
        switch (code) {

case 1: /* ( */
    strcpy(yylval.name, buf);
    return('(');

case 2: /* ')' */
    return(')');

case 3: /* TIMES */
    return(TIMES);

case 4: /* PLUS */
    strcpy(yylval.name, buf);
    return(PLUS);

case 5: /* ' */
    get_word(buf);
    if (equal(buf, "s")) {
        get_word(buf2);
        if (*buf2 != '\'') {
            unget_word(buf2);
            return(APOSTROPHE_S);
        }
        else
            unget_word(buf2);
    }
    if (*buf == '\\') {
	get_word(buf2);
	strcat(buf, buf2);
    }

    get_word(buf2);
    if (*buf2 != '\'')
	errmsg("Missing closing quote '\n");
    buf[IDENT_LEN-1] = '\0';
    strcpy(yylval.name, buf);

    get_word(buf);
    if (strchr(delim_chars, buf[0]) == NULL)
	unget_word(buf);

    return(QUOTED_IDENTIFIER);

    
    
case 6: /* MINUS */
    strcpy(yylval.name, buf);
    return(MINUS);

case 7: /* DIVIDED */
    get_word(buf);
    if (!equal(buf, "by"))
        unget_word(buf);
    return(DIVIDED_BY);

case 58: /* less, <, */
    push_type(yylval.type, BOOLEAN_TYPE);
    get_word(buf2); 
    if (*buf2 == '=')
        return(LE);
    if (!equal(buf, "less")) {
	unget_word(buf2);
        return(LT);
    }
    if (equal(buf2, "than")) {
        get_word(buf);
        if (!equal(buf, "or")) {
	    unget_word(buf);
            return(LT);
	}
        else {
            get_word(buf);
            if (equal(buf, "equal"))
                return(LE);
	    unget_word(buf);
        }
    }
    errmsg("Unexpected word following 'LESS' in LESS THAN OR EQUAL TO.\n");
    return(LT);


case 60: /* greater, >, */
    push_type(yylval.type, BOOLEAN_TYPE);
    get_word(buf2); 
    if (*buf2 == '=')
        return(GE);
    if (!equal(buf, "greater") && !equal(buf, "more")) {
	unget_word(buf2);
        return(GT);
    }
    if (equal(buf2, "than")) {
        get_word(buf);
        if (!equal(buf, "or")) {
	    unget_word(buf);
            return(GT);
	}
        else {
            get_word(buf);
            if (equal(buf, "equal"))
                return(GE);
	    unget_word(buf);
        }
    }
    errmsg("Unexpected word following 'GREATER' in GREATER THAN OR EQUAL TO.\n");
    return(GT);

case 61: /* does not equal, ! */
    push_type(yylval.type, BOOLEAN_TYPE);
    get_word(buf2);
    if (*buf == '!')
	return ((*buf2 == '=') ? NE : NOT);

    if ((equal(buf, "does") || equal(buf, "do")) && equal(buf2, "not")) {
        get_word(buf);
        if (equal(buf, "equal") || *buf == '=')
            return(NE);
        if (equal(buf, "exist"))
            return(NOT_EXIST);
    }
    if (equal(buf, "do")) {
	unget_word(buf2);
	return(DO);
    }
    errmsg("Unexpected word following 'DOES' or '!' in DOES NOT EQUAL.\n");
    return(NE);


case 59: /* '=', EQUALS */
    push_type(yylval.type, BOOLEAN_TYPE);
    return(EQ);

case 8: /* ADDRESS */
    return(ADDRESS);

case 9: /* AND */
    if (is_parameter)
	return(yylex());
    push_type(yylval.type, BOOLEAN_TYPE);
    return(AND);

case 10: /* ARRAY */
    return(ARRAY);

case 11: /* be */
    return(BE);

case 12: /* begin */
    return(BEGIN);

case 13: /* break */
    return(BREAK);

case 15: /* byte */
    push_type(yylval.type, BYTE_TYPE);
    return(BYTE);

case 16: /* character */
    push_type(yylval.type, CHARACTER_TYPE);
    return(CHARACTER);

case 17: /* content */
    return(CONTENTS);

case 19: /* continue */
    return(CONTINUE);

case 20: /* declare */
    return(DECLARE);

case 23:
    return(ELSE);

case 24:
    return(END);

case 27:
    push_type(yylval.type, BOOLEAN_TYPE);
    return(EXISTS);

case 28:
    return(EXTERNAL);

case 29:
    return(GGLOBAL);

case 31:
    return(IF);

case 32:
    push_type(yylval.type, INTEGER_TYPE);
    return(INTEGER);

case 34:
    return(LET);

case 35:
    return(LLOCAL);

case 36:
    return(LOOP);

case 37:
    return(MEANS);

case 39:
    get_word(buf);
    if (*buf == '{')
	return(C);
    else
	unget_word(buf);
    return(MODULUS);

case 40:
    push_type(yylval.type, BOOLEAN_TYPE);
    return(NOT);

case 41:
    push_type(yylval.type, BOOLEAN_TYPE);
    return(OR);

case 42:
    return(OUT);

case 43:
    return(PERMANENT);

case 45:
    push_type(yylval.type, POINTER_TYPE);
    return(POINTER);

case 46:
    push_type(yylval.type, RECORD_TYPE);
    return(RECORD);

case 47:
    get_word(buf);
    if (!equal(buf, "integer") && !equal(buf, "integers"))
        errmsg("'INTEGER' expected after 'SHORT'.\n");
    push_type(yylval.type, SHORT_TYPE);
    return(SHORT_INTEGER);


case 48:
    return(STATEMENT);

case 49:
    push_type(yylval.type, CHARACTER_TYPE);
    push_type(yylval.type, POINTER_TYPE);
    return(STRING);

case 51:
    return(THEN);

case 53:
    return(TYPE);

case 54:
    return(UNSIGNED);

case 55:
    return(WHILE);

case 56:
    return('[');

case 57:
    return(']');

case 62:
    return(DEFINE);

case 63:
    return(FUNCTION);

case 64:
    return(ENUMERATION);

case 65:
    return(REGISTER);

case 66:
    push_type(yylval.type, BOOLEAN_TYPE);
    return(BOOLEAN);

case 67:
    return(RETURN);

case 68:
    return(RETURNING);

case 69:
    return(INCREMENT);

case 70:
    return(DECREMENT);

case 71:  /* DUMP */
    get_word(buf);

    ptr = Symbol_ptr;
    while (ptr != NULL) {
        if (equal(ptr->name, buf))
            break;
        ptr = ptr->link;
    }
    if (ptr == NULL) {
	fprintf(stderr, "DUMP: %s - identifier not in symbol stack.\n", buf);
	return(yylex());
    }
    fprintf(stderr, "DUMP: name=%s, value=%d, ident=%d\n", ptr->name,
	ptr->value, ptr->ident);
    fprintf(stderr, "DUMP: text=%s\nDUMP: type=", ptr->text);
    for (i=0; i < STACK_SIZE; ++i)
	fprintf(stderr, "%d ", ptr->type[i]);
    fprintf(stderr, "\nDUMP: link=%06lx, fwdlink=%06lx\n", ptr->link,
	ptr->fwdlink);
    return(yylex());
    break;


case 72: /* is */
    get_word(buf);
    if (equal(buf, "not")) {
	push_type(yylval.type, BOOLEAN_TYPE);
	return(NE);
    }
    unget_word(buf);
    if ((code = p_binsearch(buf, &keyword[0].ke_name, sizeof(struct ke_keyword),
        KEYWORD_LEN)) >= 0) {
	code = keyword[code].ke_num;
	if (code != 5 && code != 75 && code != 76 && (code < 80 || code > 90)
	    && code != 6 && code != 8 && code != 17 && code != 97)
	    return(yylex());
    }

    push_type(yylval.type, BOOLEAN_TYPE);
    return(EQ);

case 73:
    return(INCLUDE);

case 74:
    return(OPTIONALLY);

case 75: /* TRUE */
    yylval.value = 1;
    push_type(yylval.type, BOOLEAN_TYPE);
    return(NUMBER);

case 76: /* FALSE */
    yylval.value = 0;
    push_type(yylval.type, BOOLEAN_TYPE);
    return(NUMBER);

case 77:
    return(REPEAT);

case 78:
    return(UNTIL);

case 79:
    push_type(yylval.type, RECORD_TYPE);
    return(UNION);

case 80:
case 81:
case 82:
case 83:
case 84:
case 85:
case 86:
case 87:
case 88:
case 89:
case 90:
    yylval.value = code - 80;
    push_type(yylval.type, INTEGER_TYPE);
    return(NUMBER);
	
case 91:
    return(FOR);

case 92:
    return(EACH);

case 93: /* : */
    return(':');

case 94:
    return(CASE);

case 95:
    return(DEFAULT);

case 96:
    return(SELECT);

case 97:
    yylval.value = 0;
    yylval.ident = CONSTANT_IDENTIFIER;
    push_type(yylval.type, INTEGER_TYPE);
    return(NULL_CONSTANT);

case 98:
    get_word(buf);
    if (!equal(buf, "integer") && !equal(buf, "integers"))
        errmsg("'INTEGER' expected after 'LONG'.\n");
    push_type(yylval.type, LONG_TYPE);
    return(LONG_INTEGER);

default:
    errmsg("Improper use of keyword \"%s\"\n", buf);
    return(ERROR);

        } /* end case */

    if (buf[0] == '"') {
        i = strlen(buf);
        if (i <= 1 || buf[i-1] != '"')
            strcat(buf, "\"");
        push_type(yylval.type, CHARACTER_TYPE);
        push_type(yylval.type, POINTER_TYPE);
	strcpy(string, buf);
        yylval.text = string;    /* string can be overwritten on next call */
        return(STRING_CONSTANT);
    }

    if (IsNumeric(buf[0])) {
#ifdef unos
        if (*astoi(buf, &yylval.value) != '\0')
#else
	if ((yylval.value = atoi(buf)) == 0 && *buf != '0')
#endif
            errmsg("Bad numberic constant.\n");
        push_type(yylval.type, INTEGER_TYPE);
        return(NUMBER);
    }

    ptr = Symbol_ptr;

    while (ptr != NULL) {
        if (equal(ptr->name, buf))
            break;
        ptr = ptr->link;
    }
    if (ptr == NULL) {
        buf[IDENT_LEN-1] = '\0';
        strcpy(yylval.name, buf);
        return(NEW_IDENTIFIER);
    }
    else {
        copy(&yylval, ptr, sizeof(yylval));
        yylval.link = NULL;
        yylval.fwdlink = NULL;
        if (yylval.ident == 0)
            crash("f:yylex - Ident_type of zero on stack, name=%s\n", yylval.name);
        return(yylval.ident);
    }
}
