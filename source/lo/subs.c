#include "defs.h"
#include "y.tab.h"

pop_symbol_frame()
{
    register YYSTYPE *ptr, *prev_ptr;

    ptr = Symbol_ptr;

    while (ptr != NULL) {
        if (ptr->ident == BEGIN_IDENTIFIER || ptr->ident == FUNC_IDENTIFIER ||
                ptr->ident == PROCEDURE_IDENTIFIER)
            break;
         
        ptr = ptr->link;
    }

    if (ptr == NULL) {
        errmsg("Stack empty - possible BEGIN/END mismatch.\n");
        return;
    }

    while (Symbol_ptr != ptr) {
        prev_ptr = Symbol_ptr->link;
        Symbol_ptr->link = NULL;
        Symbol_ptr->fwdlink = NULL;
        if (Symbol_ptr->ident == CONSTANT_IDENTIFIER) {
            indent();
            printf("#undef %s", Symbol_ptr->name);
        }
        free(Symbol_ptr);
        Symbol_ptr = prev_ptr;
    }

    if (Symbol_ptr != NULL && Symbol_ptr->ident == BEGIN_IDENTIFIER) { /* pop off Begin */
        prev_ptr = Symbol_ptr->link;
        free(Symbol_ptr);
        Symbol_ptr = prev_ptr;
    }
    if (Symbol_ptr != NULL)
        Symbol_ptr->fwdlink = NULL;
}


pop_subprogram_frame()
{
    register YYSTYPE  *ptr, *ptr2;

    ptr = Symbol_ptr;

    while (ptr != NULL) {
        if (ptr->ident == FUNC_IDENTIFIER || ptr->ident == PROCEDURE_IDENTIFIER)
            break;

        ptr = ptr->link;
    }

    if (ptr == NULL) {
        errmsg("f:pop_subprogram_frame - Stack empty - possible BEGIN/END mismatch.\n");
        return;
    }

    ptr2 = Symbol_ptr;
    while (ptr2 != ptr) { 
        ptr2->name[0] = '\0';
	ptr2 = ptr2->link;
    }
}



push_symbol(sym)
YYSTYPE *sym;
{
    register YYSTYPE *newsym;

    newsym = (YYSTYPE *) xalloc(sizeof(YYSTYPE));

    copy(newsym, sym, sizeof(YYSTYPE));
    newsym->link = Symbol_ptr;
    newsym->fwdlink = NULL;
    if (Symbol_ptr != NULL)
	Symbol_ptr->fwdlink = newsym;
    Symbol_ptr = newsym;
}


push_type(typ, value)
register short typ[];
short value;
{
    register short *sptr;

    for (sptr = typ + (STACK_SIZE-1); sptr >= typ; --sptr)
        if (*sptr == END_OF_STACK)
            break;

    if (sptr < typ) {
        errmsg("Type stack overflow.\n");
        crash("f:push_type - use a less complex expression.\n");
    }

    *sptr = value;
}


int
peek_type(typ)
register short typ[];
{
    register short *sptr;
    register short value;

    sptr = typ + (STACK_SIZE-1);
    if (*sptr == END_OF_STACK) {
        errmsg("Type stack empty.\n");
        errmsg("f:peek_type - Cannot peek type.\n");
	abort();
    }

    for (--sptr; sptr >= typ; --sptr)
        if (*sptr == END_OF_STACK) {
            value = *++sptr;
            return(value);
        }

    value = *typ;
    return(value);
}



int
pop_type(typ)
register short typ[];
{
    register short *sptr;
    register short value;

    sptr = typ + (STACK_SIZE-1);
    if (*sptr == END_OF_STACK) {
        errmsg("Type stack empty.\n");
        errmsg("f:pop_type - Cannot pop type.\n");
	abort();
    }

    for (--sptr; sptr >= typ; --sptr)
        if (*sptr == END_OF_STACK) {
            value = *++sptr;
            *sptr = END_OF_STACK;
            return(value);
        }

    value = *typ;
    *typ = END_OF_STACK;
    
    return(value);
}


emit_base_type(typ)
short typ[];
{
    register typ_num;

    typ_num = typ[STACK_SIZE-1];

    if (typ_num >= 30000 && typ_num < 32000) /* If RECORD_TYPE */
	return;

    if (typ_num >= 25000 && typ_num < 30000) { /* If Type_identifier */
        register YYSTYPE *ptr;
        int   ttype;

        ptr = Symbol_ptr;
        while (ptr != NULL) {
            if (ptr->ident == TYPE_IDENTIFIER
		&& ptr->type[STACK_SIZE-1] != END_OF_STACK) {
                if ((ttype = peek_type(ptr->type)) == typ_num) {
		    if (ttype == ptr->type[STACK_SIZE-1]
			|| ptr->text != NULL /* MEGAKLUDGE */)
			printf("%s %s ", ptr->value == 1 ? "union" : "struct",
			    ptr->name);
		    else
                        printf("%s ", ptr->name);
                    return;
                }
            }
            ptr = ptr->link;
        }
        errmsg("No such type.\n");
        return;
    }

    switch (typ_num) {                
case INTEGER_TYPE:   fputs("int ", stdout);   break;

case CHARACTER_TYPE:
case BYTE_TYPE:
case BOOLEAN_TYPE:   fputs("char ", stdout);  break;

case SHORT_TYPE:     fputs("short ", stdout); break;
case LONG_TYPE:	     fputs("long ", stdout);  break;

default:
     errmsg("f:emit_base_type - unknown base type, %d.\n", typ_num);
    }
}


expand_type(typ)
short typ[];
{
     short temp_typ[STACK_SIZE];
     register typ_num;
     register  i;

     typ_num = typ[STACK_SIZE-1];

     if (typ_num >= 25000 && typ_num < 30000) { /* if Type_identifier */
        register YYSTYPE *ptr;

        ptr = Symbol_ptr;
        while (ptr != NULL) {
            if (ptr->ident == TYPE_IDENTIFIER
		    && ptr->type[STACK_SIZE-1] != END_OF_STACK) {
                if (peek_type(ptr->type) == typ_num) {
		    for (i= STACK_SIZE-2; i >= 0; --i)
			typ[i+1] = typ[i];
		    typ[0] = END_OF_STACK;
		    copy(temp_typ, ptr->type, sizeof(temp_typ));
		    pop_type(temp_typ);
                    for (i = STACK_SIZE-1; i >= 0; --i) {
                        if (typ[i] == END_OF_STACK)
                            break;
			push_type(temp_typ, typ[i]);
		    }
		    copy(typ, temp_typ, sizeof(temp_typ));	
                    return;
                }
            }
            ptr = ptr->link;
        }
        errmsg("No such type.\n");
    }
}
    

invert_type(typ)
register short typ[];
{
    short  i, type1, type2, type3;

    short  new[STACK_SIZE];
     
    for (i=0; i < STACK_SIZE; ++i)
        new[i] = END_OF_STACK;

    while (typ[STACK_SIZE-1] != END_OF_STACK) {
        type1 = pop_type(typ);
        switch(type1) {

 case ARRAY_TYPE:
    type2 = pop_type(typ);
    type3 = pop_type(typ);
    push_type(new, type3);
    push_type(new, type2);
    push_type(new, type1);
    break;

case RECORD_TYPE:
    type2 = pop_type(typ);
    push_type(new, type2);
    push_type(new, type1);
    break;

default:
    push_type(new, type1);
    break;
        }
    }
    copy(typ, new, sizeof(new));

}


char *rec_name(nam)
char *nam;
{
    static char buf[BUFLEN];
    extern int record_type;
    extern int declare_ident;

    if (declare_ident != DECLARE_RECORD)
	return(nam);

    sprintf(buf, "zz%d_%s", record_type - recnum_start(record_type)+1, nam);
    return(buf);
}
    
                
emit_type(typ, name)
short typ[];
char *name;
{
    short  temp_type[STACK_SIZE];

    copy(temp_type, typ, sizeof(temp_type));

    if (pop_type(temp_type) == RECORD_TYPE)
	pop_type(temp_type);

    if (temp_type[STACK_SIZE-1] == END_OF_STACK) {
	fputs(rec_name(name), stdout);
	return;
    }
    recursive_emit_type(temp_type, name);
}


static recursive_emit_type(typ, name)
short typ[];
char *name;
{
    short  type1, type2, type3, type4;
    short  size;

    type1 = pop_type(typ);
    switch(type1) {

case ARRAY_TYPE:
    type2 = pop_type(typ);
    type3 = pop_type(typ);
    size = type3 - type2 + 1;
    if (typ[STACK_SIZE-1] == END_OF_STACK) {
        if (type2 == -32767)
            printf("%s[]", rec_name(name));
        else
            printf("%s[%d]", rec_name(name), size);
        return;       
    }
    else {
	type4 = peek_type(typ);
        if (type4 != ARRAY_TYPE)
	    putchar('(');
        recursive_emit_type(typ, name);
        if (type2 == -32767)
            printf("%s[]", type4 != ARRAY_TYPE ? ")" : "");
        else
            printf("%s[%d]", type4 != ARRAY_TYPE ? ")" : "", size);
        return;
     }
     break;
     
case POINTER_TYPE:
     if (typ[STACK_SIZE-1] == END_OF_STACK) {
         printf("*%s", rec_name(name));
         return;
     }
     else {
	 putchar('*');
         recursive_emit_type(typ, name);
         return;
     }
     break;

default:
     errmsg("Unexpected type in type stack.\n");
     crash("f:recursive_emit_type - unexpected type, %d, in type stack.\n",
         type1);
    }
    return;
}


static YYSTYPE  *funcptr;
int param_count;

reset_param_match(func)
YYSTYPE *func;
{
    push_func();

    funcptr = Symbol_ptr;
    param_count = 0;

    while (funcptr != NULL) {
        if (equal(funcptr->name, func->name))
            break;

        funcptr = funcptr->link;
    }

    if (funcptr == NULL) 
        crash("f:reset_param_match - cannot backtrack to %s.\n", func->name);

    if (funcptr->ident != FUNC_IDENTIFIER && funcptr->ident != PROCEDURE_IDENTIFIER)
        crash("f:reset_param_match - %s is not a subprogram?\n", func->name);

}


boolean
after_match()
{
    register YYSTYPE  *oldfunc;
    YYSTYPE *pop_func();


    oldfunc = funcptr;
    funcptr = pop_func();

    if (oldfunc == NULL)
	return TRUE;

    oldfunc = oldfunc->fwdlink;
    if (oldfunc == NULL)
	return TRUE;

    if (oldfunc->ident == VAR_IDENTIFIER 
	&& oldfunc->value == 0 /* KLUDGE for optionally */) {
	errmsg("Not enough arguments in function.\n");
	return FALSE;
    }

    return TRUE;
}


boolean
match_param(param)
YYSTYPE *param;
{
    register  type1, type2;
    register i;
  
    funcptr = funcptr->fwdlink;
    ++param_count;

    if (funcptr == NULL)
        return FALSE;

    if (funcptr->ident != VAR_IDENTIFIER)
        return FALSE;

    if (funcptr->value != 0) /* KLUDGE for optionally */
	return TRUE;

    type1 = peek_type(funcptr->type);
    type2 = peek_type(param->type);

    if (type1 != RECORD_TYPE && type1 != ARRAY_TYPE && type1 != POINTER_TYPE
    	&& type2 != RECORD_TYPE && type2 != ARRAY_TYPE
        && type2 != POINTER_TYPE)
        return TRUE;

    if (type1 == ARRAY_TYPE && type2 == ARRAY_TYPE) {
	pop_type(funcptr->type);
	if ((type1 = pop_type(funcptr->type)) != -32767) {
	    push_type(funcptr->type, type1);
	    push_type(funcptr->type, ARRAY_TYPE);
	}
	else {
	    pop_type(funcptr->type);
	    pop_type(param->type); 
	    type1 = pop_type(param->type);  type2 = pop_type(param->type);
	    for (i=0; i < STACK_SIZE; ++i)
		if (funcptr->type[i] != param->type[i])
		    break;
	    push_type(funcptr->type, -32767);  push_type(funcptr->type, -32767);
	    push_type(funcptr->type, ARRAY_TYPE);
	    push_type(param->type, type2);     push_type(param->type, type1);
	    push_type(param->type, ARRAY_TYPE);
	    if (i == STACK_SIZE-1)
		return(check_union(funcptr->type, param->type));
	    return (i >= STACK_SIZE);
	}
    }
	    
	
		

    if (type1 == POINTER_TYPE && type2 == ARRAY_TYPE) {
	pop_type(funcptr->type);
	pop_type(param->type);
	type1 = pop_type(param->type);  type2 = pop_type(param->type);
	for (i=0; i < STACK_SIZE; ++i)
	    if (funcptr->type[i] != param->type[i])
		break;
	push_type(funcptr->type, POINTER_TYPE);
	push_type(param->type, type2); push_type(param->type, type1);
	push_type(param->type, ARRAY_TYPE);
	if (i == STACK_SIZE-1)
		return(check_union(funcptr->type, param->type));
	return (i >= STACK_SIZE);
    }

    if (type1 == POINTER_TYPE && type2 == INTEGER_TYPE)
       if (param->ident == CONSTANT_IDENTIFIER && param->value == 0)
	return TRUE;

    for (i=0; i < STACK_SIZE; ++i)
        if (funcptr->type[i] != param->type[i])
	    break;

    if (i == STACK_SIZE-1)
	return(check_union(funcptr->type, param->type));
    return (i >= STACK_SIZE);
}


check_union(ftyp, ptyp)
short ftyp[], ptyp[];
{
	register frec, prec;
	register YYSTYPE  *ptr;

	frec = ftyp[STACK_SIZE-1];  prec = ptyp[STACK_SIZE-1];
	if (frec < 30000 || frec > 32000 || prec < 30000 || prec > 32000)
		return(FALSE);

	ptr = Symbol_ptr;
	while (ptr != NULL) {
		if (ptr->ident == REC_IDENTIFIER)
			if (ptr->type[STACK_SIZE-1] == prec &&
			    ptr->type[STACK_SIZE-2] == RECORD_TYPE &&
			    ptr->type[STACK_SIZE-3] == frec &&
			    ptr->type[STACK_SIZE-4] == END_OF_STACK)
				break;
		ptr = ptr->link;
	}
	return (ptr != NULL);
}

	


#define INCSTACK_SIZE  8

static FILE *fpstack[INCSTACK_SIZE];
static int fpsp;
static int linestack[INCSTACK_SIZE];
static int linesp;
static YYSTYPE *funcstack[INCSTACK_SIZE];
static int funcsp;

static int fd;

push_fp()
{
	if (fpsp >= INCSTACK_SIZE)
		crash("Include stack overflow.\n");

	new_recnum_base();
	fpstack[fpsp] = fp;
	if (fpsp == 0) { /* If first included file */
	    fflush(stdout);
	    fd = dup(1);
	    close(1);
	    if (open("/dev/null", 1) != 1)
		crash("Cant open /dev/null.\n");
	}
	++fpsp;
}

push_line_number()
{
	if (linesp >= INCSTACK_SIZE)
		crash("Include stack overflow!\n");

	linestack[linesp] = line_number;
	++linesp;
}

push_func()
{
	if (funcsp >= INCSTACK_SIZE)
		crash("Function stack overflow!\n");

	funcstack[funcsp] = funcptr;
	++funcsp;
}

FILE *
pop_fp()
{
	if (fpsp <= 0)
		return(NULL);

	--fpsp;
	if (fpsp == 0) {
	    fflush(stdout);
	    close(1);
	    if (dup(fd) != 1)
		crash("Dup'ed fd is not 1.\n");
	    close(fd);
	}
	new_recnum_base();
	return(fpstack[fpsp]);
}

pop_line_number()
{
	if (linesp <= 0)
		return(-1);

	--linesp;
	return(linestack[linesp]);
}

YYSTYPE *
pop_func()
{
	if (funcsp <= 0)
		return(NULL);

	--funcsp;
	return(funcstack[funcsp]);
}


static int reclist[MAX_INCLUDES];
static int numbases = 0;

new_recnum_base()
{
    extern int nextrectype;

    reclist[numbases++] = nextrectype;
    if (numbases > MAX_INCLUDES)
	crash("Too many included files.\n");
}


recnum_start(recnum)
register int recnum;
{
    register i;

    for (i=0; i < numbases; ++i)
	if (reclist[i] > recnum)
		break;

    if (i <= 0)
	crash("recnum %d less than reclist[0] %d\n", recnum, reclist[0]);

    return(reclist[i-1]);
}
