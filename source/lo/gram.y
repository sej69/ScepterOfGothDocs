%{
/*
 * gram.y - Grammar for Lo
 * 84/07/12.  A. Klietz
 */

#include "defs.h"

#define  yyreturn(x)    copy(&(yyval), &(x), sizeof(YYSTYPE));

static int  margin_position;
static int next_enum;

int declare_ident;
int record_type;
int reclexnum;
boolean is_parameter;

static int symbol_depth;

static boolean inside_loop, inside_function, inside_body, inside_select;
boolean is_external;

YYSTYPE *subprogram_ptr;

boolean optional_flag;

extern int param_count;
extern char *include_dir;
%}

%token ERROR
%token CONSTANT_IDENTIFIER
%token NULL_CONSTANT
%token STRING_CONSTANT
%token QUOTED_IDENTIFIER
%token BEGIN_IDENTIFIER
%token NEW_IDENTIFIER
%token NUMBER
%token DEFINE
%token INCLUDE
%token C
%token CONTENTS
%token REC_IDENTIFIER
%token PROC_IDENTIFIER
%token FUNC_IDENTIFIER
%token PROCEDURE_IDENTIFIER
%token VAR_IDENTIFIER
%token TYPE_IDENTIFIER
%token RETURNING
%token IF THEN ELSE
%token WHILE DO
%token REPEAT UNTIL
%token FOR EACH
%token CASE SELECT DEFAULT
%token RETURN
%token CONTINUE
%token LET
%token BE
%token BREAK OUT
%token LOOP
%token INCREMENT
%token DECREMENT
%token DECLARE
%token TYPE
%token ENUMERATION
%token STATEMENT
%token FUNCTION
%token MEANS
%token OPTIONALLY
%token UNSIGNED
%token INTEGER
%token SHORT_INTEGER
%token LONG_INTEGER
%token BYTE
%token BOOLEAN
%token CHARACTER
%token STRING
%token ARRAY
%token POINTER
%token PERMANENT EXTERNAL GGLOBAL LLOCAL REGISTER
%token RECORD
%token UNION
%token BEGIN END


%left OR
%left AND
%left EQ NE EXISTS NOT_EXIST
%left LT LE GT GE
%right FUNC_IDENTIFIER
%left PLUS MINUS
%left TIMES DIVIDED_BY MODULUS
%right UNARYMINUS NOT CONTENTS ADDRESS
%left '[' ']' '(' ')' APOSTROPHE_S


%start program

%%

program:        program_block
                | program program_block
                ;


program_block:  global_declaration
                | subprogram
                | definition
		| inclusion
		| c
		| error program_block
		    {
                        inside_function = FALSE;
                        inside_body = FALSE;
			is_parameter = FALSE;
		    }
                ;


c:		C
		    { pass_c(); }
		;

		    
inclusion:      INCLUDE STRING_CONSTANT
		    {
			register FILE  *newfp;
			register char  *s;

			s = $2.text;
			++s;
			s[strlen(s)-1] = '\0';
			if ((newfp = fopen(s, "r")) == NULL) {
			    char buf[80];

			    strcpy(buf, include_dir);
			    strncat(buf, s, 80);
			    if ((newfp = fopen(buf, "r")) == NULL) {
				strcpy(buf, INCLUDEDIR);
				strncat(buf, s, 80);
				if ((newfp = fopen(buf, "r")) == NULL) {
				    errmsg("Unable to open %s\n", s);
				    YYERROR;
				}
			    }
			    s = buf;
			}
			indent();
			printf("#include \"%s.h\"", s);
			push_fp();
			push_line_number();
			fp = newfp;
		    }
		;


definition:     DEFINE QUOTED_IDENTIFIER BE
                    {
                        indent();
                        printf("#define %s ", $2.name);
                    }
                constant
                    {
                        strcpy($5.name, $2.name);
                        $5.ident = CONSTANT_IDENTIFIER;
                        push_symbol(&($5));
                    }
                ;
  

global_declaration:
                var_declaration
                | type_declaration
                ;


declaration:    DECLARE
                     {
                         symbol_depth = 0;
			 $1.value = record_type;  /* KLUDGE */
			 yyreturn($1);
                     }
                quoted_list 
		BE
		     {
			$4.text = (declare_ident == DECLARE_FORMAL ?
				((char *) subprogram_ptr) :  /* KLUDGE */
				((char *) Symbol_ptr));       /* KLUDGE */
			$4.value = declare_ident;
			if (!is_external)
			    indent();
			yyreturn($4);
		     }
		storage_class type
                     {
                         register YYSTYPE *ptr;
                         register int  depth;
  
			 declare_ident = $5.value;
			 record_type = $2.value;
			 if (declare_ident != DECLARE_FORMAL || !is_external)
			     emit_base_type($7.type);
			     
                         ptr = (YYSTYPE *) $5.text; /* KLUDGE */
                         depth = symbol_depth;
                         for (; depth > 0; --depth) {
                             if (ptr != NULL) {
                                 copy(ptr->type, $7.type, sizeof(ptr->type));

				 if (declare_ident != DECLARE_FORMAL
				     || !is_external) {
				     invert_type(ptr->type);
				     emit_type(ptr->type, ptr->name);
				     invert_type(ptr->type);
				 }

				 if (declare_ident != DECLARE_RECORD)
				     expand_type(ptr->type);
				 else {
				     strncpy(ptr->name, rec_name(ptr->name),
					IDENT_LEN);
				     ptr->name[IDENT_LEN-1] = '\0';
				     push_type(ptr->type, record_type);
				 }
                                 if (depth > 1 && !is_external) {
				     putchar(',');
				     putchar(' ');
				 }
                             }
                             else
                                 crash("Symbol stack empty??\n");
                             ptr = ptr->link;
                         }
			 if (declare_ident != DECLARE_FORMAL || !is_external)
			     putchar(';');

			 if (declare_ident != DECLARE_RECORD)
			     expand_type($7.type);

                         if (declare_ident == DECLARE_FORMAL
                             && peek_type($7.type) == RECORD_TYPE) {
                             errmsg("You can't pass a record as a parameter.  Use a pointer instead.\n");
                             YYERROR;
                         }
                         yyreturn($7);
                     }
                 ;


var_declaration:     { declare_ident = DECLARE_VAR; }
                 declaration
                 ;
                

quoted_list:    quoted_decl_ident
                | quoted_list quoted_decl_ident
                ;

optional_and:  /* nothing */
              | AND
              ;

quoted_decl_ident:
                optional_and QUOTED_IDENTIFIER
                    {
                        ++symbol_depth;

                        switch (declare_ident) {
case DECLARE_VAR:
    $2.ident = VAR_IDENTIFIER;
    push_symbol(&($2));
    break;

case DECLARE_FORMAL:
    $2.ident = VAR_IDENTIFIER;
    if (subprogram_ptr->fwdlink == NULL ||
            !streql($2.name, subprogram_ptr->fwdlink->name)) {
        errmsg("Formal declaration name doesn't match formal parameter name.\n");
        YYERROR;
    }
    subprogram_ptr = subprogram_ptr->fwdlink;
    break;

case DECLARE_TYPE:
    $2.ident = TYPE_IDENTIFIER;
    push_symbol(&($2));
    break;

case DECLARE_RECORD:
    $2.ident = REC_IDENTIFIER;
    push_symbol(&($2));
    break;

default:
    crash("Bad value for declare_ident (%d)\n", declare_ident);
                        }
                        yyreturn($2);
                    }
                ;


storage_class:  /* nothing */
                | PERMANENT
                    {
                        if (!inside_body) 
                            errmsg("'Permanent' declaration is redundant outside procedure body.\n");
			if (declare_ident == DECLARE_FORMAL) 
			    errmsg("Formal declarations cannot be 'Permanent'.\n");
                        fputs("static ", stdout);
                    }
                | EXTERNAL
                    {
                        if (declare_ident == DECLARE_FORMAL) {
                            errmsg("'External' declaration is not valid in formal declaration.\n");
                        }
                        fputs("extern ", stdout);
                    }
                | GGLOBAL
                    {
                        if (inside_body || declare_ident == DECLARE_FORMAL) {
                            errmsg("'Global' declaration is only valid outside procedure body.\n");
                        }
                    }
                | LLOCAL
                    {
                        if (inside_body || declare_ident == DECLARE_FORMAL) {
                            errmsg("'Local' declaration is redundant inside procedure body.\n");
                        }
                        fputs("static ", stdout);
                    }
                | REGISTER
                    {
                       if (declare_ident != DECLARE_FORMAL &&
                            (!inside_body || declare_ident != DECLARE_VAR)) {
                            errmsg("Only variables inside a subprogram can be of class 'REGISTER'.\n");
                       }
                       fputs("register ", stdout);
                    }
                ;
  

type_declaration:
                TYPE
                    {
                        indent();
                        fputs("typedef ", stdout);
                        symbol_depth = 0;
                        declare_ident = DECLARE_TYPE;
                    }
                quoted_list
		MEANS
		     {
			register YYSTYPE *ptr;
			register int	depth;

			$4.text = (char *) Symbol_ptr;  /* KLUDGE */
			$4.value = create_type();   /* KLUDGE */
			depth = symbol_depth;
			ptr = Symbol_ptr;
                        for (; depth > 0; --depth) {
                            if (ptr != NULL) 
                                push_type(ptr->type, $4.value); /* KLUDGE */
                            else
                                crash("Symbol stack empty?? (type)\n");
                            ptr = ptr->link;
			}
			yyreturn($4);
		     }
		type
                    {
                         register YYSTYPE *ptr;
                         register int  depth;
  
			 declare_ident = DECLARE_TYPE;
                         emit_base_type($6.type);
                         ptr = (YYSTYPE *) $5.text; /* KLUDGE */
			 depth = symbol_depth;
                         for (; depth > 0; --depth) {
                             if (ptr != NULL) {

				 invert_type($6.type);
                                 emit_type($6.type, ptr->name);
				 invert_type($6.type);

                                 expand_type($6.type);
                                 copy(ptr->type, $6.type, sizeof(ptr->type));
                                 push_type(ptr->type, $5.value); /* KLUDGE */
                                 if (depth > 1) {
				     putchar(',');
				     putchar(' ');
				 }
                             }
                             else
                                 crash("Symbol stack empty??\n");
                             ptr = ptr->link;
                         }
			 putchar(';');
			 for (ptr = Symbol_ptr; ptr != (YYSTYPE *) $5.text
			     && ptr != NULL; ptr = ptr->link) {
			     expand_type(ptr->type);
			    if (ptr->ident == TYPE_IDENTIFIER) {
				indent();
				printf("typedef %s %s %s;", ptr->value == 1 ?
				    "union" : "struct", ptr->name, ptr->name);
			    }
			 }
                         yyreturn($6);
                     }
                ;


type:  simple_type
                    { yyreturn($1); }
                | structured_type
                    { yyreturn($1); }
                ;


simple_type:    scalar_type
                    { yyreturn($1); }
                | TYPE_IDENTIFIER
                    {
                        register int  ttype, i;

                        ttype = pop_type($1.type);
			for (i=0; i < STACK_SIZE; ++i)
				$1.type[i] = END_OF_STACK;
                        push_type($1.type, ttype);
                        yyreturn($1);
                    }
                ;
                

scalar_type:    signed_type
                    { yyreturn($1); }
                | unsigned_type
                    { yyreturn($1); }
                ;


unsigned_type:  UNSIGNED
		    {
		      if (!is_external || declare_ident == DECLARE_FUNC)
		         fputs("unsigned ", stdout);
		    }
                signed_type
                    { yyreturn($3); }
                ;


signed_type:    INTEGER
                    { yyreturn($1); }
                | BYTE
                    { yyreturn($1); }
                | CHARACTER
                    { yyreturn($1); }
                | STRING
                    { yyreturn($1); }
                | BOOLEAN
                    { yyreturn($1); }
                | SHORT_INTEGER
                    { yyreturn($1); }
		| LONG_INTEGER
		    { yyreturn($1); }
                ;

structured_type:
                array_type
                    { yyreturn($1); }
                | pointer_type
                    { yyreturn($1); }
                | record_type
                    { yyreturn($1); }
                | enumerated_type
                    { yyreturn($1); }
                ;


array_type:     bounded_array
                    { yyreturn($1); }
                | unbounded_array
                    { yyreturn($1); }
                ;


bounded_array:  ARRAY '[' array_constant array_constant ']' type
                     {
                        if ($3.value > $4.value) {
                            errmsg("Array indexes must be non-decreasing.\n");
                        }
                        push_type($6.type, $4.value);
                        push_type($6.type, $3.value);
                        push_type($6.type, ARRAY_TYPE);
                        yyreturn($6);
                     }
                 ;


array_constant: NUMBER
                    { yyreturn($1); }
                | CONSTANT_IDENTIFIER 
                    { yyreturn($1); }
                | QUOTED_IDENTIFIER
                    {
		      char buf[10];

                      if (strlen($1.name) > 1 && $1.name[0] != '\\' 
                         || $1.name[0] == '\\' && strlen($1.name) != 2) {
                         errmsg("Only 1 character allowed in a character constant.\n");
                      }
		      sprintf(buf, $1.name, 0);
		      $1.value = buf[0];
                      push_type($1.type, CHARACTER_TYPE);
                      yyreturn($1);
                    }
                ;


unbounded_array:
                ARRAY type
                    { 
                        push_type($2.type, -32767);
                        push_type($2.type, -32767);
                        push_type($2.type, ARRAY_TYPE);
                        yyreturn($2);
                    }
                ;


pointer_type:   POINTER type
                    {
                        push_type($2.type, POINTER_TYPE);
                        yyreturn($2);
                    }
                ;


rec_or_union:   RECORD
		| UNION
		;


new_rec_or_union:  RECORD
		    {
			indent();
			printf("struct %s {", Symbol_ptr->name);
			margin_right();
			yyreturn($1);
		    }
		| UNION
		    {
			indent();
			printf("union %s {", Symbol_ptr->name);
			margin_right();
			Symbol_ptr->value = 1;
			yyreturn($1);
		    }
		;


record_type:    new_rec_or_union 
                    {
                        pop_type($1.type);
                        push_type($1.type, (record_type = create_rectype()));
			if (declare_ident == DECLARE_RECORD) {
			    push_symbol(&($1));
			    push_type(Symbol_ptr->type, RECORD_TYPE);
			    push_type(Symbol_ptr->type, create_type());
			    Symbol_ptr->ident = TYPE_IDENTIFIER;
			    Symbol_ptr->text = Symbol_ptr->name; /* MEGAKLUDGE */
			    strcpy(Symbol_ptr->name, Symbol_ptr->link->name);
			}
			$1.value = symbol_depth;  /* KLUDGE */
                        yyreturn($1);
                    }
                record_declarations END rec_or_union
                    {
                        symbol_depth = $2.value;  /* KLUDGE */
                        $2.value = 0;             /* KLUDGE */
                        margin_left();
                        indent();
			putchar('}');
			putchar(' ');
                        push_type($2.type, RECORD_TYPE);
                        yyreturn($2);
                    }
                ;


record_declarations:
                record_declaration
                | record_declarations record_declaration
                ;


record_declaration:
                    { declare_ident = DECLARE_RECORD; }
                declaration
                    { yyreturn($2); }
                ;


enumerated_type:
                ENUMERATION
                    {   next_enum = 0;  }
                enum_identifiers
                    {
			indent();
                        push_type($1.type, INTEGER_TYPE);
			yyreturn($1);
                    }
                ;
                

enum_identifiers:
                NEW_IDENTIFIER  /* type stack must be empty */
                     {
                        push_type($1.type, INTEGER_TYPE);
                        $1.value = next_enum++;
                        $1.ident = CONSTANT_IDENTIFIER;
                        indent();
                        printf("#define %s %d", $1.name, $1.value);
                        push_symbol(&($1));
                     }
                 | enum_identifiers optional_and NEW_IDENTIFIER
                     {
                        push_type($3.type, INTEGER_TYPE);
                        $3.value = next_enum++;
                        $3.ident = CONSTANT_IDENTIFIER;
                        indent();
                        printf("#define %s %d", $3.name, $3.value);
                        push_symbol(&($3));
                     }
                 ;


subprogram:      procedure
                 | function
                 | error END of_subprogram
                     {
                         inside_function = FALSE;
                         inside_body = FALSE;
			 is_external = FALSE;
			 is_parameter = FALSE;
                         pop_subprogram_frame();
                     }
                 ;


procedure:       procedure_heading 
                     {
                         inside_function = FALSE;
                         inside_loop = 0;
			 inside_select = 0;
                     }
                 formal_declarations
                 body
                 ;


procedure_heading:
                subprogram_class STATEMENT QUOTED_IDENTIFIER
                    {
                        $3.ident = PROCEDURE_IDENTIFIER;
                        push_symbol(&($3));
                        subprogram_ptr = Symbol_ptr;
			symbol_depth = 0;
			optional_flag = FALSE;
                        yyreturn($3);
                    }
                formal_parameters MEANS
		    {
                        register YYSTYPE *ptr;
                        register int  depth;

#ifndef unos
                        fputs("void ", stdout);
#endif
			printf("%s(", $4.name);
			if (!is_external) {
			    ptr = subprogram_ptr;
			    depth = symbol_depth;
			    for (; depth > 0; --depth) {
				ptr = ptr->fwdlink;
				if (ptr != NULL) {
				    fputs(ptr->name, stdout);
				    if (depth > 1) {
					putchar(',');
					putchar(' ');
				    }
				}
				else
				    crash("Symbol stack empty??\n");
			    }
			}
			putchar(')');
			yyreturn($4);
		    }
                ;


formal_parameters: /* nothing */
                | formal_parameters formal_parameter
                ;
                

optional_optionally: /* nothing */
		| OPTIONALLY
		    { optional_flag = TRUE; }
		;


formal_parameter:
                optional_and optional_optionally NEW_IDENTIFIER
                    {
                        push_type($3.type, INTEGER_TYPE);  /* default temp */
                        $3.ident = VAR_IDENTIFIER; /* default temp */
			if (optional_flag)
				$3.value = 1;  /* KLUDGE */
                        push_symbol(&($3));
			++symbol_depth;
                        yyreturn($3);
                    }
                ;

                              
formal_declarations: /* nothing */
                | formal_declarations formal_declaration
                ;
                  

formal_declaration:
                    { declare_ident = DECLARE_FORMAL; }
                declaration
                ;


function:	function_heading function_return
		{

                        register YYSTYPE *ptr;
			register int	ftype;

                        $2.ident = FUNC_IDENTIFIER;
                        strcpy($2.name, $1.name);
			ftype = peek_type($2.type);
			if (ftype == RECORD_TYPE || ftype == ARRAY_TYPE) {
			    errmsg("Function type must be a pointer or scalar.\n");
			    YYERROR;
			}
                        emit_base_type($2.type);

			invert_type($2.type);
                        emit_type($2.type, $2.name);
                        invert_type($2.type);

                        expand_type($2.type);
			ptr = subprogram_ptr;
			putchar('(');
			if (!is_external) {
			    ptr = ptr->fwdlink;
			    while (ptr != NULL) {
				    fputs(ptr->name, stdout);
				    if (ptr->fwdlink != NULL)
					    fputs(", ", stdout);
				    ptr = ptr->fwdlink;
			    }
			}
			putchar(')');

                        copy(subprogram_ptr->type, $2.type, sizeof($2.type));
                        inside_function = TRUE;
                        inside_loop = 0;
			inside_select = 0;
                        yyreturn($2);
                     }
                 formal_declarations
                 body
                 ;


function_return: RETURNING 
		   { declare_ident = DECLARE_FUNC; }
		 type MEANS
                   { yyreturn($3); }
                 ;


function_heading:
                 subprogram_class FUNCTION QUOTED_IDENTIFIER
                    {
                        $3.ident = FUNC_IDENTIFIER;
                        push_symbol(&($3));
                        subprogram_ptr = Symbol_ptr;
			symbol_depth = 0;
			optional_flag = FALSE;
                        yyreturn($3);
                    }
                formal_parameters
                    { yyreturn($4); }
                ;


subprogram_class: /* Nothing */
                 { indent(); indent(); is_external = FALSE; }
                | LLOCAL
                 { indent(); indent(); is_external = FALSE; fputs("static ", stdout); }
                | EXTERNAL
                 { indent(); indent(); is_external = TRUE; fputs("extern ", stdout); }
                ;


body:		compound_statement of_subprogram
                    {
                        if (is_external) {
                            errmsg("Statements not allowed in external subprogram.\n");
			    is_external = FALSE;
                        }
                        inside_body = FALSE;
                        inside_function = FALSE;
			margin_left();
                        pop_subprogram_frame();
			$2.ident = BEGIN_IDENTIFIER;  /* Mark end of var list */
			strcpy($2.name, "begin");
			push_symbol(&($2));
                    }
                | END of_subprogram
                    {
			putchar(';');
                        if (!is_external) {
                            errmsg("Missing statements following subprogram heading.\n");
                        }
                        inside_body = FALSE;
                        inside_function = FALSE;
                        pop_subprogram_frame();
			$1.ident = BEGIN_IDENTIFIER; /* Mark end of var list */
			strcpy($1.name, "begin");
			push_symbol(&($1));
			is_external = FALSE;
                    }
                ;


of_subprogram:  STATEMENT
		  { yyreturn($1); }
                | FUNCTION
		  { yyreturn($1); }
                ;


compound_statement:
                BEGIN
                    {
			if (inside_body)
			    putchar(' ');
			else
			    indent();
			putchar('{');
			if (!inside_body)
			    margin_right();
                        $1.ident = BEGIN_IDENTIFIER;
                        strcpy($1.name, "begin");
                        inside_body = TRUE;
                        push_symbol(&($1));
                    }
                stmt_decl_list statements
                    {
                        pop_symbol_frame();
                        margin_left();
                        indent();
			putchar('}');
			margin_right();
                    }
                END
                | error END
		    {
			pop_symbol_frame();
			is_parameter = FALSE;
		    }
                ;

stmt_decl_list: /* Nothing */
		| stmt_declarations
		    { indent(); }
		;


stmt_declarations:
		stmt_declaration
                | stmt_declarations stmt_declaration
                ;


stmt_declaration:
                var_declaration
                | type_declaration
                ;


statements:     /* nothing */
                | statements statement
                ;


statement:      compound_statement
                | simple_statement
                ;


simple_statement:
                assignment_stmt
                | procedure_stmt
                | if_stmt
                | while_stmt
		| repeat_stmt
		| for_stmt
		| select_stmt
                | return_stmt
                | increment_stmt
                | decrement_stmt
		| break_select_stmt
                | break_loop_stmt
                | continue_stmt
		| definition
		| inclusion
		| c
		| error statement
		    {
			is_parameter = FALSE;
			push_type($1.type, INTEGER_TYPE);
			yyreturn($1);
		    }

                ;


assignment_stmt:
                LET
                  { indent(); }
                lvalue BE assign_modifier
                expr
                  {
                      int type3, type6, dtype;
                      register  ptr3, ptr6;

		      type3 = peek_type($3.type);
		      type6 = peek_type($6.type);
		      putchar(';');
                      if (type6 == RECORD_TYPE) {
                          errmsg("Records cannot be assigned.\n");
                      }
                      ptr3 = (type3 == POINTER_TYPE || type3 == ARRAY_TYPE);
                      ptr6 = (type6 == POINTER_TYPE || type6 == ARRAY_TYPE);
                        
                      if (!ptr3 && ptr6) {
                          errmsg("You cannot assign a pointer to an integer variable.\n");
                      }
                      if (ptr3 && !ptr6) 
                          if ($6.ident != CONSTANT_IDENTIFIER || $6.value != 0) {
                              errmsg("Pointer can only be assigned with another pointer or null.\n");
                          }
                      if (ptr3 && ptr6) {
                          register   i;

                          for (i=0; i < STACK_SIZE; ++i)
                              if ($3.type[i] != $6.type[i]) {
				  if (i < STACK_SIZE-1 || i == STACK_SIZE-1 && 
				      !check_union($3.type, $6.type) &&
				      !check_union($6.type, $3.type))
				      errmsg("Pointer type conflict on assignment.\n");
				  break;
                              }
                      }
                  }
              ;


assign_modifier:  { fputs(" = ", stdout); }
		| PLUS
		  {
		    if (streql($1.name, "+"))
			errmsg("Must spell out 'PLUS' in plus-assignments.\n");
		    else
			fputs(" += ", stdout);
		  }
		| MINUS
		  {
		    if (streql($1.name, "-"))
			fputs(" = -", stdout);
		    else
			fputs(" -= ", stdout);
		  }
		| TIMES
		  { fputs(" *= ", stdout); }
		| DIVIDED_BY
		  { fputs(" /= ", stdout); }
		| MODULUS
		  { fputs(" %= ", stdout); }
		;


procedure_stmt: PROCEDURE_IDENTIFIER
                  {
                      reset_param_match(&($1));
                      indent();
                      printf("%s(", $1.name);
		      is_parameter = TRUE;
                  }
                parameter_list
                  {
		      is_parameter = FALSE;
		      putchar(')');
		      putchar(';');
		      after_match();
                      yyreturn($1);
                  }
              ;


parameter_list:  /* nothing */
              | parameters
              ;


parameters:   parameter
                { yyreturn($1); }
              | parameters 
                { fputs(", ", stdout); }
                parameter
                { yyreturn($3); }
              ;


parameter:    expr
                  {
                      if (!match_param(&($1))) {
                          errmsg("Type mismatch on parameter %d.\n",
			      param_count);
                      }
                      yyreturn($1);
                  }
               ;


if_stmt:      IF if_expr THEN if_statement
              | IF if_expr THEN if_statement ELSE
                  {
                      indent();
                      fputs("else", stdout);
                  }
                if_statement
              ;


if_expr:          {
                      indent();
                      fputs("if (", stdout);
                  }
              expr
                  {   putchar(')');  }
              ;


if_statement:     {   margin_right(); }
               statement
                  {   margin_left();  }
               ;
                  

select_stmt:   SELECT
		  {
		      indent();
		      fputs("switch (", stdout);
		  }
	      expr BEGIN
		  {
		      register int etype;

		      fputs(") {", stdout);
		      etype = $3.type[STACK_SIZE-1];
		      if (etype == POINTER_TYPE || etype == RECORD_TYPE ||
			etype == ARRAY_TYPE) 
			errmsg("Bad type for Select statement.\n");
		      ++inside_select;
	 	  }
	      case_stmts END
		  {
		      --inside_select;
		      indent();
		      putchar('}');
		  }
	      ;


case_stmts:   case_stmt
	      | case_stmts case_stmt
	      ;


case_stmt:    CASE case_constant_list ':'
		  { margin_right(); }
	      statements
		  {
		      indent();
		      fputs("break;", stdout);
		      indent();
		      margin_left();
		  }
	      ;


case_constant_list: /* Nothing */
	      | case_constant_list case_constant
	      ;


case_constant:
	      DEFAULT
		  {
		      indent();
		      fputs("default:", stdout);
		  }
	      | /* Nothing */
		  {
		      indent();
		      fputs("case ", stdout);
		  }
	      constant
		  { putchar(':'); }
	      ;


while_stmt:   WHILE
                  {
                      indent();
                      fputs("while (", stdout);
                  }
              expr optional_do
                  {
                      margin_right();
                      ++inside_loop;
                  }
              statement
                  {
                      margin_left();
                      --inside_loop;
                  }
              ;
                      

for_stmt:     FOR
		  {
		      indent();
		      putchar('{');
		      margin_right();
		      indent();
		      printf("register zztop%d;\n", margin_position);
		  }
	      EACH VAR_IDENTIFIER
		  {
		      indent();
		      printf("%s = ", $4.name);
		  }
	      expr 
		  {
		      putchar(';');
		      indent();
		      printf("zztop%d = ", margin_position);
		  }
	      expr DO
		  {
		      putchar(';');
		      indent();
		      printf("for (; %s <= zztop%d; ++%s)", $4.name,
			margin_position, $4.name);
		      margin_right();
		      ++inside_loop;
		  }
	      statement
		  {
		      margin_left();
		      --inside_loop;
		      margin_left();
		      indent();
		      putchar('}');
		  }
	      ;


repeat_stmt:  REPEAT
		  {
		      indent();
		      fputs("do {", stdout);
		      margin_right();
		      ++inside_loop;
		  }
	      statements UNTIL
		  {
		      --inside_loop;
		      margin_left();
		      indent();
		      fputs("} while (!(", stdout);
		  }
	      expr
		  {
		      fputs("));", stdout);
		  }
	      ;
		  

optional_do:  /* nothing */
                  { putchar(')'); }
              | DO
                  { putchar(')'); }
              ;


return_stmt:  RETURN
                  {
                      indent();
                      fputs("return ", stdout);
                  }
               return_expr
               ;
                       
          
return_expr:  /* nothing */
                  {
                      if (inside_function) {
                          errmsg("RETURN statement must include a value.\n");
                      }
		      putchar(';');
                  }
              | expr
                  {
		      register YYSTYPE  *ptr;
		      register int  type1, type2;

                      if (!inside_function) {
                          errmsg("Cannot return value from non-function.\n");
                      }
		      putchar(';');
		      for (ptr = Symbol_ptr; ptr != NULL; ptr = ptr->link)
			if (ptr->ident == FUNC_IDENTIFIER)
				break;
		      if (ptr != NULL) {
			type1 = peek_type(ptr->type);
			type2 = peek_type($1.type);
			if (type1 == POINTER_TYPE || type1 == RECORD_TYPE ||
			  type1 == ARRAY_TYPE || type2 == POINTER_TYPE ||
			  type2 == RECORD_TYPE || type2 == ARRAY_TYPE) 
			  if (type1 != POINTER_TYPE || type2 != INTEGER_TYPE ||
			  $1.ident != CONSTANT_IDENTIFIER || $1.value != 0) {
			    int i;

			    for (i=0; i < STACK_SIZE; ++i)
				if ($1.type[i] != ptr->type[i])
				    break;
			    if (i < STACK_SIZE-1 || i == STACK_SIZE-1 &&
			      !check_union(ptr->type, $1.type) &&
			      !check_union($1.type, ptr->type))
				errmsg("Return value does not match function type.\n");
			  }
		      }
                  }
              ;
                      

increment_stmt:
              INCREMENT
                  {
                      indent();
                      fputs("++(", stdout);
                  }
              lvalue
                  {
		      putchar(')');
		      putchar(';');
                  }
              ;


decrement_stmt:
              DECREMENT 
                  {
                      indent();
                      fputs("--(", stdout);
                  }
              lvalue
                  {
		      putchar(')');
		      putchar(';');
                  }
              ;


break_select_stmt: BREAK OUT SELECT
                  {
                      if (!inside_select) {
                          errmsg("BREAK OUT OF SELECT is only valid inside a select.\n");
                      }
                      indent();
                      fputs("break;", stdout);
                   }
              ;
			

break_loop_stmt:   BREAK OUT LOOP
                  {
                      if (!inside_loop) {
                          errmsg("BREAK OUT is only valid inside a loop.\n");
                      }
                      indent();
                      fputs("break;", stdout);
                   }
              ;


continue_stmt:
              CONTINUE LOOP
                  {
                      if (!inside_loop) {
                          errmsg("CONTINUE is only valid inside a loop.\n");
                      }
                      indent();
                      fputs("continue;", stdout);
                   }
               ;


expr:
	      primary
		  { yyreturn($1); }
              | addr_expr
                  { yyreturn($1); }
              | minus_expr
                  { yyreturn($1); }
              | not_expr
                  { yyreturn($1); }
              | exist_expr
                  { yyreturn($1); }
              | add_expr
                  { yyreturn($1); }
              | mult_expr
                  { yyreturn($1); }
              | boolean_expr
                  { yyreturn($1); }
              | relational_expr
                  { yyreturn($1); }
              | equality_expr
                  { yyreturn($1); }
              | error
                  { 
		    push_type($1.type, INTEGER_TYPE);
                    yyreturn($1);
                  }
              ;


                  
minus_expr:   MINUS  %prec UNARYMINUS
		  { left_paren(&($0));  fputs(" -", stdout); }
	      expr 
                  {
                     register int ttype;

                     if ((ttype = peek_type($3.type)) == POINTER_TYPE ||
                         ttype == ARRAY_TYPE || ttype == RECORD_TYPE ||
                         ttype == BOOLEAN_TYPE) {
                          errmsg("Unary minus parameter must be numeric.\n");
                     }
                     yyreturn($3);
                 }
             ;



not_expr:     NOT 
                  { left_paren(&($0));  fputs(" !", stdout); }
              expr
                  {
                      if (pop_type($3.type) != BOOLEAN_TYPE) {
                          errmsg("NOT parameter must be a boolean.\n");
                      }
                      yyreturn($1);
                  }
              ;


exist_expr:   expr exist_op  %prec EXISTS
                  {
                      if (pop_type($1.type) != POINTER_TYPE) {
                          errmsg("EXISTS parameter must be a pointer.\n");
                      }
                      yyreturn($2);
                  }
               ;
                      

add_expr:     expr add_op  %prec PLUS 
                  {
                      register int ttype, ltype;

                      if ((ttype = peek_type($1.type)) == ARRAY_TYPE) {
                          if ((ltype = peek_type($1.type)) != -32767 &&
                              bounds_checking)
                              fputs("zzchk((", stdout);
                      }
                  }
              expr
                  {
                      register type1, type4, lobound, hibound;

		      type1 = pop_type($1.type);
		      type4 = pop_type($4.type);
		      if (type1 != POINTER_TYPE || type4 != POINTER_TYPE
			  || $1.type[STACK_SIZE-1] != $4.type[STACK_SIZE-1])
			  if (type1 == RECORD_TYPE || type4 == RECORD_TYPE ||
			      type4 == POINTER_TYPE || type4 == ARRAY_TYPE) {
			      errmsg("Improper type for additive operator.\n");
			  }

                      if (type1 == ARRAY_TYPE) {
                          lobound = pop_type($1.type); hibound = pop_type($1.type);
                          if (bounds_checking && lobound != -32767)
                              printf(")-%d,%d,%d)", lobound, lobound, hibound);
                          type1 = POINTER_TYPE;
                      }
                      if (type1 == POINTER_TYPE && caution) {
                          errmsg("Caution - pointer arithmetic is hazardous.\n");
                      }
		      if (type1 == POINTER_TYPE && type4 == POINTER_TYPE)
			  push_type($1.type, INTEGER_TYPE);
		      else
			  push_type($1.type, type1);
                      yyreturn($1);
                  }
              ;
                              
                      
mult_expr:	expr mult_op expr  %prec TIMES
                  {
                      register int type1, type3;

		      type1 = pop_type($1.type);
		      type3 = pop_type($3.type);
                      if (type1 == ARRAY_TYPE || type1 == POINTER_TYPE ||
			  type1 == RECORD_TYPE || type3 == ARRAY_TYPE ||
                          type3 == POINTER_TYPE || type3 == RECORD_TYPE) {
                          errmsg("Improper type for multiplicative operators.\n");
                      }
                      push_type($2.type, INTEGER_TYPE);
                      yyreturn($2);
                  }
              ;


boolean_expr: expr boolean_op expr  %prec AND
                  {
                      if (pop_type($1.type) != BOOLEAN_TYPE ||
                          pop_type($3.type) != BOOLEAN_TYPE) {
                          errmsg("Expression in boolean operator is not boolean.\n");
                      }
                      yyreturn($2);
                  }
              ;


equality_expr:
              expr equality_op expr  %prec EQ
                  {
                      register  type1, type3;

		      type1 = pop_type($1.type);
		      type3 = pop_type($3.type);
                      if (type1 == RECORD_TYPE || type3 == RECORD_TYPE) 
                          errmsg("Can't compare records for equality.\n");

                      if ((type1 == POINTER_TYPE || type1 == ARRAY_TYPE) &&
                          (type3 == POINTER_TYPE || type3 == ARRAY_TYPE)) {
                          if (type1 == ARRAY_TYPE) {
                              pop_type($1.type);   pop_type($1.type);
                          }
                          if (type3 == ARRAY_TYPE) {
                              pop_type($3.type);   pop_type($3.type);
                          }
                          type1 = pop_type($1.type);
                          type3 = pop_type($3.type);
                          if (type1 != type3) {
                              errmsg("Type conflict in equality operator.\n");
                           }
                           if ($3.text != NULL)
                               errmsg("Cant compare against a string for equality.\n");
                      }
		      else
                      if (type1 == POINTER_TYPE || type1 == ARRAY_TYPE) {
                         if ($3.ident != CONSTANT_IDENTIFIER || $3.value != 0) {
                              errmsg("Pointer can only be compared with another pointer or null.\n");
                         }
		      }
                      else /* if type1 is scalar */
                          if (type3 == POINTER_TYPE || type3 == ARRAY_TYPE)
                              errmsg("TYPE conflict in equality operator.\n");
			 
                      yyreturn($2);
                  }
              ;
                

relational_expr:
              expr relational_op expr  %prec GT
                  {
                      register int  type1, type3;

		      type1 = pop_type($1.type);
		      type3 = pop_type($3.type);
                      if (type1 == POINTER_TYPE ||
                          type1 == ARRAY_TYPE || type1 == RECORD_TYPE
                          || type3 == POINTER_TYPE ||
                          type3 == ARRAY_TYPE || type3 == RECORD_TYPE) {
                          errmsg("Can't use relational operator on non-numeric expression.\n");
                      }
                      yyreturn($2);
                  }
              ;


exist_op:     EXISTS
                  { fputs(" != NULL", stdout); yyreturn($1); }
              | NOT_EXIST
                  { fputs(" == NULL", stdout); yyreturn($1); }
              ;


add_op:       PLUS
                  { fputs(" + ", stdout); yyreturn($1); }
              | MINUS
                  { fputs(" - ", stdout); yyreturn($1); }
              ;


mult_op:     TIMES
                  { fputs(" * ", stdout); yyreturn($1); }
              | DIVIDED_BY
                  { fputs(" / ", stdout); yyreturn($1); }
              | MODULUS
                  { fputs(" % ", stdout); yyreturn($1); }
              ;


boolean_op:   AND
                  { fputs(" && ", stdout); yyreturn($1); }
              | OR
                  { fputs(" || ", stdout); yyreturn($1); }
              ;


equality_op:  NE
                  { fputs(" != ", stdout); yyreturn($1); }
              | EQ
                  { fputs(" == ", stdout); yyreturn($1); }
              ;


relational_op:
              GT
                  { fputs(" > ", stdout); yyreturn($1); }
              | LT
                  { fputs(" < ", stdout); yyreturn($1); }
              | GE
                  { fputs(" >= ", stdout); yyreturn($1); }
              | LE
                  { fputs(" <= ", stdout); yyreturn($1); }
              ;


primary:      constant
                  { yyreturn($1); }
              | STRING_CONSTANT
                  {
		    left_paren(&($0));
                    fputs($1.text, stdout);
                    yyreturn($1);
                  }
              | '(' expr ')'
		  { putchar(')'); yyreturn($2); }
	      | left_value
                  { yyreturn($1); }
              | '(' VAR_IDENTIFIER FUNC_IDENTIFIER
                  { 
		     left_paren(&($0));
                     reset_param_match(&($3));
                     if (!match_param(&($2))) {
                          errmsg("Type mismatch on parameter 1.\n");
                     }
                     printf("%s(%s", $3.name, $2.name);
		     is_parameter = TRUE;
                  }
                parameter_list ')'
                  {
		      is_parameter = FALSE;
                      putchar(')');
		      after_match();
                      yyreturn($3);
                  }
              | '(' FUNC_IDENTIFIER
                 {
		     left_paren(&($0));
                     reset_param_match(&($2));
                     printf("%s(", $2.name);
		     is_parameter = TRUE;
                 }
                parameter_list ')'
                 {
		     is_parameter = FALSE;
                     putchar(')');
		     after_match();
                     yyreturn($2);
                 }
	      | FUNC_IDENTIFIER
                 {
		     left_paren(&($0));
                     printf("%s()", $1.name);
		     yyreturn($1);
                 }
              ;
	      

lvalue:         left_value
                  {
                    register int ttype;

		    if ((ttype = peek_type($1.type)) == ARRAY_TYPE) {
			 errmsg("An array is not a valid left-hand value.\n");
			}
		    if (ttype == RECORD_TYPE) {
			 errmsg("A record is not a valid left-hand value.\n");
		    }
                    yyreturn($1);
                  }
              ;

                     
left_value:   VAR_IDENTIFIER
                  {
		    left_paren(&($0));
                    fputs($1.name, stdout);
                    yyreturn($1);
                  }
              | array_expr
                  { yyreturn($1); }
              | record_expr
                  { yyreturn($1); }
              | pointer_expr
                  { yyreturn($1); }
              | '(' left_value ')'
		  { putchar(')'); yyreturn($2); }
              ;

             
array_expr:   primary '[' 
                      {
                          if (bounds_checking)
                              fputs("[zzchk(", stdout);
                          else
                              fputs("[(", stdout);
                      }
                expr ']'
                     {
                          register int ttype, lobound, hibound;
 
                          if (pop_type($1.type) != ARRAY_TYPE) {
                              errmsg("That is not an array name.\n");
                              YYERROR;
                          }
                          if ((ttype = pop_type($4.type)) == POINTER_TYPE ||
                              ttype == ARRAY_TYPE || ttype == RECORD_TYPE) {
                              errmsg("Attempted to use a nonnumeric as an array index.\n");
                              YYERROR;
                          }
                          lobound = pop_type($1.type);
                          hibound = pop_type($1.type);
                          if (bounds_checking && lobound != -32767)
                              printf(",%d,%d)]",
                                  lobound, hibound);
                          else
                              if (lobound != 0 && lobound != -32767)
                                  printf(")-%d]", lobound);
                              else
                                  fputs(")]", stdout);
                          yyreturn($1);
                     }
                 ;
                                 

record_expr:    primary APOSTROPHE_S
                     {
                          register int   ttype;
                          boolean is_pointer = FALSE;

                          if ((ttype = pop_type($1.type)) == POINTER_TYPE) {
                              ttype = pop_type($1.type);
                              is_pointer = TRUE;
                          }
                          if (ttype != RECORD_TYPE) {
                              errmsg("Improper record parameter.\n");
                              YYERROR;
                          }
			  record_type = pop_type($1.type);
			  reclexnum = record_type - recnum_start(record_type)+1;
			  fputs((is_pointer ? "->" : "."), stdout);
		      }		      
		 REC_IDENTIFIER
		      {
                          if (record_type != pop_type($4.type)) {
                              errmsg("Type conflict between record and record parameter.\n");
                          }
                          fputs($4.name, stdout);
                          yyreturn($4);
                      }
                ;


pointer_expr:   CONTENTS { left_paren(&($0)); putchar('*'); } expr
                    { if (pop_type($3.type) != POINTER_TYPE) {
                        errmsg("The CONTENTS operator requires a pointer expression.\n");
                      }
                      yyreturn($3);
                    }
                ;


addr_expr:      ADDRESS { left_paren(&($0)); putchar('&'); } expr
                    {
                        register int ttype;

			ttype = peek_type($3.type);
                        if ($3.value != 0 || $3.text != NULL 
                            || $3.ident == CONSTANT_IDENTIFIER) {
                            errmsg("Cannot take the address of a constant.\n");
                        }
                        push_type($3.type, POINTER_TYPE);
                        yyreturn($3);
                    }
                ;


constant:       NUMBER
                    { left_paren(&($0)); printf("%d", $1.value); yyreturn($1); }
                | CONSTANT_IDENTIFIER 
                    { left_paren(&($0)); fputs($1.name, stdout); yyreturn($1); }
		| NULL_CONSTANT
		    { left_paren(&($0)); fputs("NULL", stdout); yyreturn($1); }
                | QUOTED_IDENTIFIER
                    {
		      char buf[10];

		      left_paren(&($0));
                      if (strlen($1.name) > 1 && $1.name[0] != '\\'
                         || $1.name[0] == '\\' && strlen($1.name) != 2) {
                         errmsg("Only 1 character allowed in a character constant.\n");
                         YYERROR;
                      }
                      printf("'%s'", $1.name);
		      sprintf(buf, $1.name, 0);
		      $1.value = buf[0];
                      push_type($1.type, CHARACTER_TYPE);
                      yyreturn($1);
                    }
                ;

%%



void
margin_right()
{
    ++margin_position;
}


void
margin_left()
{
    --margin_position;
    if (margin_position < 0)
        margin_position = 0;
}


void
indent()
{
    register  i;

    putchar('\n');
    for (i=1; i < margin_position; i += 2)
	putchar('\t');
    
    if ((margin_position & 1) != 0) /* If odd */
	    fputs("    ", stdout);
}


int nextrectype = 30000;

int
create_rectype()
{
    return(nextrectype++);
}

int
create_type()
{
    static nexttype;

    if (nexttype == 0)
        nexttype = 25000;

    return(nexttype++);
}


void
left_paren(ptr)
YYSTYPE *ptr;
{
	register i;

	i = 0;
	while (ptr[i].name[0] == '(') {
		putchar('(');
		--i;
	}
}
