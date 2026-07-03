#include "defs.h"
#include "y.tab.h"

static char line[256];
static char ungetbuf[256];

int  line_number;

boolean error_flag;

FILE *pop_fp();
boolean IsBlank();

#define IsAlpha(x)  ((x) >= 'a' && (x) <= 'z' || (x) >= 'A' && (x) <= 'Z' || (x) == '_')

#define IsNumeric(x)  ((x) >= '0' && (x) <= '9')

#define IsAlphanum(x)  (IsAlpha(x) || IsNumeric(x))

static char *ptr;

int
get_word(buf)
char *buf;
{
    register char *p, *s;
    int  len;

    p = buf;
    if (ungetbuf[0] != '\0') {
        strcpy(buf, ungetbuf);
        ungetbuf[0] = '\0';
        return(strlen(buf));
    }

    if (ptr == NULL || *ptr == '\0') 
	if ((len = inputline()) < 0) {
	    buf[0] = '\0';
	    return(-1);
	}

    s = ptr;
    if ((*s == '{' && (s == line || *(s-1) != '%')) || *s == '(' && *(s+1) == '*' ||
	*s == '/' && *(s+1) == '*') { /* comment */
	for (;;) {
	    for (; *s != '\0'; ++s) {
		if (*s == '}')
		    break;
		if (s != line && *(s-1) == '*' && (*s == ')' || *s == '/'))
		    break;
	    }
	    if (*s == '\0') {
		if ((len = inputline()) < 0)
		    return(-1);
		s = ptr;
	    }
	    else
		break;
	}
	++s;
	while (IsBlank(*s)) 
	    ++s;
	ptr = s;
	if (*s == '\0') {
	    if ((len = inputline()) < 0)
		return(-1);
	    s = ptr;
	}
	return(get_word(buf));
    }

    if (IsAlpha(*s)) {
        *p++ = *s++;
        while (IsAlphanum(*s))
            *p++ = *s++;
        *p = '\0';
        while (IsBlank(*s))
            ++s;

        ptr = s;
	if (*ptr != '\'' || *(ptr-2) != '\'')
	    string_tolower(buf);

        switch(len = strlen(buf)) {
case 1:
        if (*ptr != '\'' && equal(buf, "a"))
            get_word(buf);
        break;

case 2: if (equal(buf, "an") || equal(buf, "to") || equal(buf, "on")
            || equal(buf, "in") || equal(buf, "of") || equal(buf, "at"))
            get_word(buf);
        break;

case 3:
        if (equal(buf, "the"))
            get_word(buf);
        break;

case 4:
        if (equal(buf, "with") || equal(buf, "into") || equal(buf, "from"))
            get_word(buf);
        break;

case 5:
        if (equal(buf, "using"))
            get_word(buf);
        break;

        }
        
        return(strlen(buf));
    }

    if (IsNumeric(*s)) {
        *p++ = *s++;
        if (*(s-1) == '0' && (*s == 'x' || *s == 'X'))
            *p++ = *s++;
        while (IsNumeric(*s))
            *p++ = *s++;
        *p = '\0';
        while (IsBlank(*s))
            ++s;
        ptr = s;
        return(strlen(buf));
    }
        
    if (*s == '"' && (s == line || *(s-1) != '\\' && *(s-1) != '\'')) {
        *p++ = *s++;
        while ((*s != '"' || *(s-1) == '\\') && *s != '\0')
            *p++ = *s++;
        *p++ = '"';
        *p = '\0';
	if (*s != '\0')
	    ++s;
        while (IsBlank(*s))
            ++s;
        ptr = s;
        return(strlen(buf));
    }

    *p++ = *s++;
    *p = '\0';
    if (buf[0] != '\'')
	while (IsBlank(*s))
	    ++s;
    ptr = s;
    return(strlen(buf));
}


void
unget_word(buf)
{
    strcpy(ungetbuf, buf);
}


char *delim_chars = " ,.;\t";

boolean
IsBlank(ch)
register char ch;
{
    register char *s;

    for (s = delim_chars; *s != '\0'; ++s)
        if (ch == *s)
            return(TRUE);

    return(FALSE);
}


void
errmsg(text, arg1, arg2, arg3, arg4)
char *text, *arg1, *arg2, *arg3, *arg4;
{
    putchar('\n');
    printf(text, arg1, arg2, arg3, arg4);
    fprintf(stderr, "%d: %s\n", line_number, line);
    fprintf(stderr, text, arg1, arg2, arg3, arg4);

    error_flag = TRUE;
}


void
yyerror(s)
char *s;
{
	putchar('\n');
	fprintf(stderr, "%s\n", s);
	fprintf(stderr, "%d: %s\n", line_number, line);
	error_flag = TRUE;
}


inputline()
{
    int  len;
    FILE *newfp;

        for (;;) {
            len = wide_freadline(fp, line, 255);
            ++line_number;
            if (len < 0) {
		if ((newfp = pop_fp()) == NULL) 
		    return(-1);
		line_number = pop_line_number();
		fclose(fp);
		fp = newfp;
		return(inputline());
            }
            clean_string(line);
            len = strlen(line);
            if (len == 0)
                continue;
            ptr = &line[0];
            while (IsBlank(*ptr)) 
                ++ptr;
            if (*ptr == '\0')
                continue;
            break;
        }
    return(len);
}


pass_c()
{
    register char *s;

    putchar('\n');
    s = ptr;
	for (;;) {
	    for (; *s != '\0'; ++s) {
		if (*s == '%' && *(s+1) == '}')
		    break;
		putchar(*s);
	    }
	    if (*s == '\0') {
		putchar('\n');
		if (inputline() < 0)
		    return(-1);
		s = line;
		while (s != ptr) {
		   putchar(*s);
		   ++s;
		}
	    }
	    else
		break;
	}
	++s;
	++s;
	while (IsBlank(*s)) {
	    putchar(' ');
	    ++s;
	}
	ptr = s;
	if (*s == '\0') {
	    putchar('\n');
	    if (inputline() < 0)
		return(-1);
	}
}
