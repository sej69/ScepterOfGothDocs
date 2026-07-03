#include <stdio.h>

#define boolean char
#define byte char
#define TRUE 1
#define FALSE 0
#define BUFLEN 80


extern char *xalloc();


#ifdef unos
#define INCLUDEDIR  "/include/"
#else
#define INCLUDEDIR "/usr/include"
#endif

#define MAX_INCLUDES  20

#define STACK_SIZE  10
#define END_OF_STACK  -32765

#define IDENT_LEN   32

extern boolean bounds_checking, caution;

extern boolean error_flag, optional_flag;

extern FILE *fp;
extern int line_number;

#define DECLARE_VAR 7001
#define DECLARE_FORMAL 7002
#define DECLARE_TYPE 7003
#define DECLARE_RECORD 7004
#define DECLARE_FUNC 7005


#define BYTE_TYPE 1001
#define CHARACTER_TYPE 1002
#define BOOLEAN_TYPE 1003
#define SHORT_TYPE 1004
#define INTEGER_TYPE 1005
#define POINTER_TYPE 1006
#define ARRAY_TYPE 1007
#define RECORD_TYPE 1008
#define LONG_TYPE 1009

typedef  struct yystype_struct {                
             int   value;        
             short ident;   
             short type[STACK_SIZE];
             char  name[IDENT_LEN];
             char  *text;
             struct  yystype_struct *link;
	     struct  yystype_struct *fwdlink;
} yystype;

#define YYSTYPE yystype

extern YYSTYPE *Symbol_ptr;
