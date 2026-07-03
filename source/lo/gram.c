
# line 2 "gram.y"
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
# define ERROR 257
# define CONSTANT_IDENTIFIER 258
# define NULL_CONSTANT 259
# define STRING_CONSTANT 260
# define QUOTED_IDENTIFIER 261
# define BEGIN_IDENTIFIER 262
# define NEW_IDENTIFIER 263
# define NUMBER 264
# define DEFINE 265
# define INCLUDE 266
# define C 267
# define CONTENTS 268
# define REC_IDENTIFIER 269
# define PROC_IDENTIFIER 270
# define FUNC_IDENTIFIER 271
# define PROCEDURE_IDENTIFIER 272
# define VAR_IDENTIFIER 273
# define TYPE_IDENTIFIER 274
# define RETURNING 275
# define IF 276
# define THEN 277
# define ELSE 278
# define WHILE 279
# define DO 280
# define REPEAT 281
# define UNTIL 282
# define FOR 283
# define EACH 284
# define CASE 285
# define SELECT 286
# define DEFAULT 287
# define RETURN 288
# define CONTINUE 289
# define LET 290
# define BE 291
# define BREAK 292
# define OUT 293
# define LOOP 294
# define INCREMENT 295
# define DECREMENT 296
# define DECLARE 297
# define TYPE 298
# define ENUMERATION 299
# define STATEMENT 300
# define FUNCTION 301
# define MEANS 302
# define OPTIONALLY 303
# define UNSIGNED 304
# define INTEGER 305
# define SHORT_INTEGER 306
# define LONG_INTEGER 307
# define BYTE 308
# define BOOLEAN 309
# define CHARACTER 310
# define STRING 311
# define ARRAY 312
# define POINTER 313
# define PERMANENT 314
# define EXTERNAL 315
# define GGLOBAL 316
# define LLOCAL 317
# define REGISTER 318
# define RECORD 319
# define UNION 320
# define BEGIN 321
# define END 322
# define OR 323
# define AND 324
# define EQ 325
# define NE 326
# define EXISTS 327
# define NOT_EXIST 328
# define LT 329
# define LE 330
# define GT 331
# define GE 332
# define PLUS 333
# define MINUS 334
# define TIMES 335
# define DIVIDED_BY 336
# define MODULUS 337
# define UNARYMINUS 338
# define NOT 339
# define ADDRESS 340
# define APOSTROPHE_S 341
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#ifndef YYSTYPE
#define YYSTYPE int
#endif
YYSTYPE yylval, yyval;
typedef int yytabelem;
# define YYERRCODE 256

# line 1768 "gram.y"




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
yytabelem yyexca[] ={
-1, 0,
	297, 18,
	300, 97,
	301, 97,
	-2, 0,
-1, 1,
	0, -1,
	297, 18,
	300, 97,
	301, 97,
	-2, 0,
-1, 8,
	297, 18,
	300, 97,
	301, 97,
	-2, 0,
-1, 45,
	297, 89,
	-2, 0,
-1, 62,
	297, 89,
	-2, 0,
-1, 102,
	297, 18,
	-2, 108,
-1, 113,
	275, 96,
	-2, 22,
-1, 117,
	297, 18,
	-2, 109,
-1, 124,
	263, 22,
	-2, 72,
-1, 140,
	322, 105,
	-2, 0,
-1, 192,
	265, 145,
	266, 145,
	267, 145,
	272, 145,
	276, 145,
	278, 145,
	279, 145,
	281, 145,
	282, 145,
	283, 145,
	285, 145,
	286, 145,
	288, 145,
	289, 145,
	290, 145,
	292, 145,
	295, 145,
	296, 145,
	321, 145,
	322, 145,
	-2, 0,
-1, 199,
	265, 185,
	266, 185,
	267, 185,
	272, 185,
	276, 185,
	278, 185,
	279, 185,
	281, 185,
	282, 185,
	283, 185,
	285, 185,
	286, 185,
	288, 185,
	289, 185,
	290, 185,
	292, 185,
	295, 185,
	296, 185,
	321, 185,
	322, 185,
	-2, 0,
-1, 206,
	91, 234,
	341, 234,
	-2, 240,
-1, 218,
	258, 148,
	259, 148,
	260, 148,
	261, 148,
	264, 148,
	268, 148,
	271, 148,
	273, 148,
	334, 148,
	339, 148,
	340, 148,
	40, 148,
	-2, 146,
-1, 251,
	256, 237,
	258, 237,
	259, 237,
	260, 237,
	261, 237,
	264, 237,
	268, 237,
	271, 237,
	273, 237,
	334, 237,
	339, 237,
	340, 237,
	40, 237,
	41, 237,
	-2, 239,
-1, 296,
	41, 145,
	-2, 0,
-1, 317,
	41, 145,
	-2, 0,
-1, 347,
	285, 164,
	322, 164,
	-2, 0,
	};
# define YYNPROD 258
# define YYLAST 578
yytabelem yyact[]={

   343,   211,   140,    44,   151,   331,   275,   277,   274,   276,
   265,   266,   267,   268,   269,   284,   253,   327,   189,   287,
   294,   271,   270,   273,   272,   263,   264,   275,   277,   274,
   276,   265,   266,   267,   268,   269,   211,   265,   266,   267,
   268,   269,   105,   271,   270,   273,   272,   263,   264,   275,
   277,   274,   276,   265,   266,   267,   268,   269,   271,   270,
   273,   272,   263,   264,   275,   277,   274,   276,   265,   266,
   267,   268,   269,   214,   267,   268,   269,   220,   271,   270,
   273,   272,   263,   264,   275,   277,   274,   276,   265,   266,
   267,   268,   269,   273,   272,   263,   264,   275,   277,   274,
   276,   265,   266,   267,   268,   269,   289,   290,   291,   292,
   293,   154,   328,   131,    61,    97,   281,    53,   142,   103,
    13,    14,    15,   148,    92,   184,   185,   172,    37,    38,
    29,   173,    34,    35,   174,    44,   175,    17,   176,    44,
   217,   177,   203,   178,   182,   171,   202,   181,    44,   334,
   179,   180,   135,   136,   137,   138,   139,   246,   154,   190,
   248,    39,   328,   240,   310,   247,   236,    13,    14,    15,
    67,   286,    33,   188,   172,   295,    60,   103,   173,    60,
    58,   174,   320,   175,   285,   176,   339,   186,   177,   125,
   178,   182,   171,    54,   181,    78,    49,   179,   180,   239,
    86,    79,    84,    85,    80,    83,    81,    82,    87,    76,
    48,    26,   110,    27,   219,    88,    89,   231,   127,    94,
    95,   215,    96,    60,   211,    93,    28,    94,    95,   213,
    96,   122,   216,    93,   207,   271,   270,   273,   272,   263,
   264,   275,   277,   274,   276,   265,   266,   267,   268,   269,
   118,    72,   231,   187,    94,    95,   215,    96,   232,   129,
    93,   221,   130,   205,   213,   128,   254,   251,   112,   250,
    36,    55,   237,   238,    45,   337,   241,   243,   330,   152,
    42,   297,     8,   278,   255,    41,   101,   311,     2,   249,
    23,    13,    14,    15,   299,   234,   298,    24,   210,    43,
   235,   233,   271,   270,   273,   272,   263,   264,   275,   277,
   274,   276,   265,   266,   267,   268,   269,   323,   170,     7,
     7,    62,    52,   209,    17,    57,    51,     7,    99,   100,
   234,   208,    52,   300,   104,   235,   233,   303,   304,   305,
   306,    22,    57,    21,   335,   296,   146,   347,    25,   317,
   144,   348,   262,   143,   154,   261,   307,   308,   309,   260,
   113,   126,     8,    13,    14,    15,   316,   259,   141,    10,
   172,    13,    14,    15,   173,     9,   319,   174,   302,   175,
   321,   176,   258,   257,   177,   280,   178,   182,   171,    63,
   181,   324,   325,   179,   180,    67,    79,    84,    85,    80,
    83,    81,    82,   279,    17,   169,     6,     6,   168,     5,
     5,   338,   133,   133,     6,   341,   345,     5,   230,    60,
    78,    22,   229,    21,   145,    86,    79,    84,    85,    80,
    83,    81,    82,    87,    76,   228,   227,   318,   226,   332,
    88,    89,    94,    95,   215,    96,   225,   224,    93,   223,
   206,   222,   213,   212,   201,   216,   200,   207,   329,   206,
   206,   242,   212,   212,   244,   245,   106,   199,   313,   196,
   252,   301,   120,   346,   333,   314,   197,   111,   119,   312,
   283,   195,   342,   340,   344,   336,   326,   120,   115,   315,
   198,   282,   194,   119,   322,   193,   256,   218,   192,   288,
   191,   167,   166,   165,   164,   163,   162,   161,   160,   159,
   158,   157,   156,   155,   153,   117,   150,   116,   102,    91,
    47,    46,    32,    19,   149,    59,    56,   147,   132,    90,
    20,    31,    18,    12,    11,   124,   108,   123,   121,   107,
    77,   183,    75,    74,    71,    70,    69,    68,   109,    73,
    66,    65,    64,    98,    30,    16,   134,   114,    40,    50,
     4,     3,     1,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   204 };
yytabelem yypact[]={

   106,   106, -1000, -1000, -1000, -1000, -1000, -1000,    26, -1000,
 -1000, -1000, -1000,   -50,   -47, -1000,  -167, -1000, -1000,  -103,
  -168, -1000, -1000, -1000, -1000,  -172,  -130, -1000, -1000, -1000,
  -321, -1000, -1000, -1000,   -51,   -65, -1000, -1000, -1000, -1000,
  -321,  -185, -1000,   -68, -1000,  -142, -1000,  -104, -1000, -1000,
   -31,  -176, -1000, -1000, -1000, -1000, -1000,  -172,  -172,  -167,
 -1000,  -203,  -142,  -260, -1000, -1000, -1000, -1000, -1000, -1000,
 -1000, -1000, -1000, -1000, -1000, -1000,  -104, -1000, -1000, -1000,
 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   121, -1000, -1000,
 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -104, -1000,
 -1000, -1000,  -161, -1000, -1000, -1000, -1000, -1000,   -74,    91,
     1, -1000,  -189,  -321,  -162, -1000, -1000,  -161, -1000, -1000,
 -1000,  -204, -1000,  -167,  -321, -1000, -1000,     1, -1000, -1000,
 -1000, -1000, -1000,  -180,  -104, -1000, -1000, -1000, -1000, -1000,
    98, -1000,  -194, -1000, -1000,   -76,   160,   -90, -1000, -1000,
  -304, -1000, -1000, -1000,  -145, -1000, -1000, -1000, -1000, -1000,
 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
 -1000,  -147,  -152, -1000, -1000, -1000, -1000,  -104, -1000, -1000,
 -1000,   184,   -39,  -111,   -39,   -39, -1000,  -121,   -39,   -39,
   184,   184,  -129, -1000, -1000,  -131, -1000, -1000, -1000, -1000,
 -1000,    -4,   -75, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
  -245,   -75, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -245,  -265,   -98,
  -102,  -302, -1000,  -245, -1000, -1000, -1000, -1000,  -227,   -21,
   -96, -1000,   240, -1000, -1000,   -39,   -39, -1000, -1000,   -39,
   -39,   -39,   -39, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   -39,   -39,
   -39,  -114,    98, -1000, -1000, -1000, -1000, -1000,   -39, -1000,
 -1000, -1000, -1000, -1000, -1000, -1000,   -39, -1000,   -39,   -87,
 -1000, -1000,   -39, -1000,  -232,  -323,  -296, -1000, -1000, -1000,
 -1000, -1000,    98,   -39,   -39,  -123,  -245,   -39,   237,   -88,
 -1000,  -261, -1000, -1000,  -245,  -245,  -173, -1000, -1000,   234,
 -1000, -1000, -1000,   -39, -1000, -1000,   128, -1000,  -280, -1000,
 -1000, -1000,   -31, -1000, -1000, -1000,    98,    98, -1000 };
yytabelem yypgo[]={

     0,   562,   288,   561,   560,   408,   405,   318,   559,    73,
   375,   369,   226,   558,   285,   557,   556,   389,   555,   280,
   299,   554,   553,   552,   551,   550,   251,   549,   548,   547,
   546,   545,   544,   543,   542,   218,   541,   540,   539,   538,
   231,   537,   536,   535,   534,   533,   270,   532,   531,   274,
   271,   530,   529,   268,   528,   527,   526,   525,   523,   522,
   521,   520,   519,   279,   518,   517,     2,   516,   515,   250,
     4,   514,   513,   512,   511,   510,   509,   508,   507,   506,
   505,   504,   503,   502,   501,   500,   263,   499,    77,   498,
   140,   497,   214,   496,   495,   116,   494,   492,   491,   490,
   489,   486,    17,   485,   484,   483,   482,   481,   480,   479,
   476,   475,   474,   473,   469,   468,   467,   461,   456,   454,
   261,   451,   449,   447,   446,   438,   436,   435,   422,   418,
   403,   385,   383,   382,   378,   367,   359,   355,   352,   258,
   349,   345,   331,   323,   298,   296,   294,   284,   283 };
yytabelem yyr1[]={

     0,     1,     1,     2,     2,     2,     2,     2,     2,     7,
     6,     8,     5,     3,     3,    13,    15,    12,    18,    10,
    14,    14,    20,    20,    19,    16,    16,    16,    16,    16,
    16,    21,    22,    11,    17,    17,    23,    23,    25,    25,
    28,    27,    26,    26,    26,    26,    26,    26,    26,    24,
    24,    24,    24,    29,    29,    33,    35,    35,    35,    34,
    30,    36,    36,    37,    37,    38,    31,    39,    39,    41,
    40,    42,    32,    43,    43,     4,     4,     4,    48,    44,
    52,    47,    53,    53,    55,    55,    54,    49,    49,    57,
    56,    60,    45,    61,    59,    62,    58,    51,    51,    51,
    50,    50,    46,    46,    64,    67,    63,    63,    65,    65,
    68,    68,    69,    69,    66,    66,    70,    70,    71,    71,
    71,    71,    71,    71,    71,    71,    71,    71,    71,    71,
    71,    71,    71,    71,    71,    85,    72,    87,    87,    87,
    87,    87,    87,    89,    73,    90,    90,    91,    93,    91,
    92,    74,    96,    74,    97,    94,    98,    95,    99,   100,
    78,   101,   101,   104,   102,   103,   103,   105,   106,   105,
   107,   109,    75,   110,   111,   112,   113,    77,   114,   115,
    76,   108,   108,   116,    79,   117,   117,   118,    80,   119,
    81,    82,    83,    84,    88,    88,    88,    88,    88,    88,
    88,    88,    88,    88,    88,   130,   122,   131,   123,   124,
   134,   125,   126,   127,   129,   128,   132,   132,   133,   133,
   135,   135,   135,   136,   136,   137,   137,   138,   138,   138,
   138,   120,   120,   120,   120,   140,   120,   141,   120,   120,
    86,   139,   139,   139,   139,   139,   145,   142,   146,   143,
   147,   144,   148,   121,     9,     9,     9,     9 };
yytabelem yyr2[]={

     0,     2,     4,     2,     2,     2,     2,     2,     5,     3,
     5,     1,    11,     2,     2,     1,     1,    15,     1,     4,
     2,     4,     0,     2,     5,     0,     3,     3,     3,     3,
     3,     1,     1,    13,     3,     3,     3,     3,     3,     3,
     1,     7,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,    13,     3,     3,     3,     5,
     5,     2,     2,     3,     3,     1,    11,     2,     4,     1,
     5,     1,     7,     3,     7,     2,     2,     7,     1,     8,
     1,    13,     0,     4,     0,     3,     7,     0,     4,     1,
     4,     1,    10,     1,     9,     1,    11,     1,     3,     3,
     5,     5,     3,     3,     1,     1,    12,     5,     0,     3,
     2,     4,     2,     2,     0,     4,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     5,     1,    13,     1,     3,     3,
     3,     3,     3,     1,     7,     0,     2,     3,     1,     7,
     3,     8,     1,    14,     1,     5,     1,     5,     1,     1,
    15,     2,     4,     1,    11,     0,     4,     3,     1,     5,
     1,     1,    13,     1,     1,     1,     1,    23,     1,     1,
    13,     1,     3,     1,     6,     1,     3,     1,     7,     1,
     7,     7,     7,     5,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     1,     7,     1,     7,     5,
     1,     9,     7,     7,     7,     7,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     7,     3,     1,    13,     1,    11,     3,
     3,     3,     3,     3,     3,     7,     1,    11,     1,     9,
     1,     7,     1,     7,     3,     3,     3,     3 };
yytabelem yychk[]={

 -1000,    -1,    -2,    -3,    -4,    -5,    -6,    -7,   256,   -10,
   -11,   -44,   -45,   265,   266,   267,   -18,   298,   -47,   -58,
   -51,   317,   315,    -2,    -2,   322,   261,   260,   -12,   297,
   -21,   -48,   -59,   275,   300,   301,   -46,   300,   301,   291,
   -13,   -14,   -19,   -20,   324,   -49,   -60,   -61,   261,   261,
    -8,   -14,   -19,   302,   261,   -50,   -56,   -63,   322,   -57,
   321,   256,   -49,   -17,   -23,   -24,   -25,   274,   -29,   -30,
   -31,   -32,   -26,   -27,   -33,   -34,   313,   -37,   299,   305,
   308,   310,   311,   309,   306,   307,   304,   312,   319,   320,
   -52,   -62,    -9,   264,   258,   259,   261,   291,   -22,   -46,
   -46,   -12,   -64,   322,   -50,   302,   -17,   -38,   -42,   -28,
    91,   -17,   -53,   -53,   -15,   -17,   -65,   -68,   -69,   -10,
   -11,   -39,   -40,   -41,   -43,   263,   -26,   -35,   264,   258,
   261,   302,   -54,   -20,   -16,   314,   315,   316,   317,   318,
   -66,   -69,   322,   -40,   -12,   -20,   -35,   -55,   303,   -17,
   -67,   -70,   -63,   -71,   256,   -72,   -73,   -74,   -75,   -76,
   -77,   -78,   -79,   -80,   -81,   -82,   -83,   -84,    -5,    -6,
    -7,   290,   272,   276,   279,   281,   283,   286,   288,   295,
   296,   292,   289,   -36,   319,   320,   263,    93,   263,   322,
   -70,   -85,   -89,   -94,   -97,  -107,  -114,  -110,   -99,  -116,
  -118,  -119,   293,   294,   -17,   -86,  -139,   273,  -142,  -143,
  -144,    40,  -120,   268,    -9,   260,   271,   -90,   -91,   -92,
   -88,  -120,  -121,  -122,  -123,  -124,  -125,  -126,  -127,  -128,
  -129,   256,  -139,   340,   334,   339,   277,   -88,   -88,   -66,
   284,   -88,  -117,   -88,   -86,   -86,   286,   294,   291,   -88,
   273,   271,  -139,    91,   341,  -147,   -93,  -132,  -133,  -135,
  -136,  -137,  -138,   327,   328,   333,   334,   335,   336,   337,
   324,   323,   326,   325,   331,   329,   332,   330,  -148,  -130,
  -131,   -95,   -98,  -108,   280,   282,   273,   321,   -87,   333,
   334,   335,   336,   337,    41,   271,  -141,    41,  -145,  -146,
   -88,   -92,  -134,   -88,   -88,   -88,   -88,   -88,   -88,   -88,
   278,   -70,  -109,  -115,  -111,  -100,   -88,  -140,   -90,   -88,
   269,   -88,   -96,   -70,   -88,   -88,  -101,  -102,   285,   -90,
    41,    93,   -95,  -112,   322,  -102,  -103,    41,   -88,    58,
  -105,   287,  -106,   280,  -104,    -9,  -113,   -66,   -70 };
yytabelem yydef[]={

    -2,    -2,     1,     3,     4,     5,     6,     7,    -2,    13,
    14,    75,    76,     0,     0,     9,     0,    31,    78,     0,
     0,    98,    99,     2,     8,     0,     0,    10,    19,    15,
    22,    87,    91,    93,     0,     0,    77,   102,   103,    11,
    22,    22,    20,     0,    23,    -2,    87,     0,    80,    95,
     0,    22,    21,    32,    24,    79,    88,     0,     0,     0,
   104,     0,    -2,     0,    34,    35,    36,    37,    49,    50,
    51,    52,    38,    39,    53,    54,     0,    65,    71,    42,
    43,    44,    45,    46,    47,    48,    40,     0,    63,    64,
    82,    82,    12,   254,   255,   256,   257,    16,     0,   100,
   101,    90,    -2,   107,    92,    94,    60,    69,     0,     0,
     0,    59,    22,    -2,    25,    33,   114,    -2,   110,   112,
   113,    69,    67,     0,    -2,    73,    41,     0,    56,    57,
    58,    81,    83,    84,     0,    26,    27,    28,    29,    30,
    -2,   111,     0,    68,    70,     0,     0,     0,    85,    17,
     0,   115,   116,   117,     0,   118,   119,   120,   121,   122,
   123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
   133,   135,   143,   154,   170,   178,   173,   158,   183,   187,
   189,     0,     0,    66,    61,    62,    74,     0,    86,   106,
   134,     0,    -2,     0,     0,     0,   114,     0,     0,    -2,
     0,     0,     0,   193,    55,     0,    -2,   241,   242,   243,
   244,     0,     0,   250,   231,   232,   239,   144,    -2,   147,
   150,   194,   195,   196,   197,   198,   199,   200,   201,   202,
   203,   204,   234,   252,   205,   207,   156,   155,   181,     0,
     0,     0,   184,   186,   188,   190,   191,   192,   137,     0,
   241,    -2,   234,   246,   248,     0,     0,   209,   210,     0,
     0,     0,     0,   216,   217,   218,   219,   220,   221,   222,
   223,   224,   225,   226,   227,   228,   229,   230,     0,     0,
     0,   151,     0,   171,   182,   179,   174,   159,     0,   138,
   139,   140,   141,   142,   233,   235,    -2,   245,     0,     0,
   251,   149,     0,   212,   213,   214,   215,   253,   206,   208,
   152,   157,     0,     0,     0,     0,   136,    -2,     0,     0,
   249,   211,   156,   172,   180,   175,     0,   161,   165,     0,
   238,   247,   153,     0,   160,   162,   168,   236,     0,   163,
   166,   167,     0,   176,   114,   169,     0,    -2,   177 };
typedef struct { char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"ERROR",	257,
	"CONSTANT_IDENTIFIER",	258,
	"NULL_CONSTANT",	259,
	"STRING_CONSTANT",	260,
	"QUOTED_IDENTIFIER",	261,
	"BEGIN_IDENTIFIER",	262,
	"NEW_IDENTIFIER",	263,
	"NUMBER",	264,
	"DEFINE",	265,
	"INCLUDE",	266,
	"C",	267,
	"CONTENTS",	268,
	"REC_IDENTIFIER",	269,
	"PROC_IDENTIFIER",	270,
	"FUNC_IDENTIFIER",	271,
	"PROCEDURE_IDENTIFIER",	272,
	"VAR_IDENTIFIER",	273,
	"TYPE_IDENTIFIER",	274,
	"RETURNING",	275,
	"IF",	276,
	"THEN",	277,
	"ELSE",	278,
	"WHILE",	279,
	"DO",	280,
	"REPEAT",	281,
	"UNTIL",	282,
	"FOR",	283,
	"EACH",	284,
	"CASE",	285,
	"SELECT",	286,
	"DEFAULT",	287,
	"RETURN",	288,
	"CONTINUE",	289,
	"LET",	290,
	"BE",	291,
	"BREAK",	292,
	"OUT",	293,
	"LOOP",	294,
	"INCREMENT",	295,
	"DECREMENT",	296,
	"DECLARE",	297,
	"TYPE",	298,
	"ENUMERATION",	299,
	"STATEMENT",	300,
	"FUNCTION",	301,
	"MEANS",	302,
	"OPTIONALLY",	303,
	"UNSIGNED",	304,
	"INTEGER",	305,
	"SHORT_INTEGER",	306,
	"LONG_INTEGER",	307,
	"BYTE",	308,
	"BOOLEAN",	309,
	"CHARACTER",	310,
	"STRING",	311,
	"ARRAY",	312,
	"POINTER",	313,
	"PERMANENT",	314,
	"EXTERNAL",	315,
	"GGLOBAL",	316,
	"LLOCAL",	317,
	"REGISTER",	318,
	"RECORD",	319,
	"UNION",	320,
	"BEGIN",	321,
	"END",	322,
	"OR",	323,
	"AND",	324,
	"EQ",	325,
	"NE",	326,
	"EXISTS",	327,
	"NOT_EXIST",	328,
	"LT",	329,
	"LE",	330,
	"GT",	331,
	"GE",	332,
	"PLUS",	333,
	"MINUS",	334,
	"TIMES",	335,
	"DIVIDED_BY",	336,
	"MODULUS",	337,
	"UNARYMINUS",	338,
	"NOT",	339,
	"ADDRESS",	340,
	"[",	91,
	"]",	93,
	"(",	40,
	")",	41,
	"APOSTROPHE_S",	341,
	"-unknown-",	-1	/* ends search */
};

char * yyreds[] =
{
	"-no such reduction-",
	"program : program_block",
	"program : program program_block",
	"program_block : global_declaration",
	"program_block : subprogram",
	"program_block : definition",
	"program_block : inclusion",
	"program_block : c",
	"program_block : error program_block",
	"c : C",
	"inclusion : INCLUDE STRING_CONSTANT",
	"definition : DEFINE QUOTED_IDENTIFIER BE",
	"definition : DEFINE QUOTED_IDENTIFIER BE constant",
	"global_declaration : var_declaration",
	"global_declaration : type_declaration",
	"declaration : DECLARE",
	"declaration : DECLARE quoted_list BE",
	"declaration : DECLARE quoted_list BE storage_class type",
	"var_declaration : /* empty */",
	"var_declaration : declaration",
	"quoted_list : quoted_decl_ident",
	"quoted_list : quoted_list quoted_decl_ident",
	"optional_and : /* empty */",
	"optional_and : AND",
	"quoted_decl_ident : optional_and QUOTED_IDENTIFIER",
	"storage_class : /* empty */",
	"storage_class : PERMANENT",
	"storage_class : EXTERNAL",
	"storage_class : GGLOBAL",
	"storage_class : LLOCAL",
	"storage_class : REGISTER",
	"type_declaration : TYPE",
	"type_declaration : TYPE quoted_list MEANS",
	"type_declaration : TYPE quoted_list MEANS type",
	"type : simple_type",
	"type : structured_type",
	"simple_type : scalar_type",
	"simple_type : TYPE_IDENTIFIER",
	"scalar_type : signed_type",
	"scalar_type : unsigned_type",
	"unsigned_type : UNSIGNED",
	"unsigned_type : UNSIGNED signed_type",
	"signed_type : INTEGER",
	"signed_type : BYTE",
	"signed_type : CHARACTER",
	"signed_type : STRING",
	"signed_type : BOOLEAN",
	"signed_type : SHORT_INTEGER",
	"signed_type : LONG_INTEGER",
	"structured_type : array_type",
	"structured_type : pointer_type",
	"structured_type : record_type",
	"structured_type : enumerated_type",
	"array_type : bounded_array",
	"array_type : unbounded_array",
	"bounded_array : ARRAY '[' array_constant array_constant ']' type",
	"array_constant : NUMBER",
	"array_constant : CONSTANT_IDENTIFIER",
	"array_constant : QUOTED_IDENTIFIER",
	"unbounded_array : ARRAY type",
	"pointer_type : POINTER type",
	"rec_or_union : RECORD",
	"rec_or_union : UNION",
	"new_rec_or_union : RECORD",
	"new_rec_or_union : UNION",
	"record_type : new_rec_or_union",
	"record_type : new_rec_or_union record_declarations END rec_or_union",
	"record_declarations : record_declaration",
	"record_declarations : record_declarations record_declaration",
	"record_declaration : /* empty */",
	"record_declaration : declaration",
	"enumerated_type : ENUMERATION",
	"enumerated_type : ENUMERATION enum_identifiers",
	"enum_identifiers : NEW_IDENTIFIER",
	"enum_identifiers : enum_identifiers optional_and NEW_IDENTIFIER",
	"subprogram : procedure",
	"subprogram : function",
	"subprogram : error END of_subprogram",
	"procedure : procedure_heading",
	"procedure : procedure_heading formal_declarations body",
	"procedure_heading : subprogram_class STATEMENT QUOTED_IDENTIFIER",
	"procedure_heading : subprogram_class STATEMENT QUOTED_IDENTIFIER formal_parameters MEANS",
	"formal_parameters : /* empty */",
	"formal_parameters : formal_parameters formal_parameter",
	"optional_optionally : /* empty */",
	"optional_optionally : OPTIONALLY",
	"formal_parameter : optional_and optional_optionally NEW_IDENTIFIER",
	"formal_declarations : /* empty */",
	"formal_declarations : formal_declarations formal_declaration",
	"formal_declaration : /* empty */",
	"formal_declaration : declaration",
	"function : function_heading function_return",
	"function : function_heading function_return formal_declarations body",
	"function_return : RETURNING",
	"function_return : RETURNING type MEANS",
	"function_heading : subprogram_class FUNCTION QUOTED_IDENTIFIER",
	"function_heading : subprogram_class FUNCTION QUOTED_IDENTIFIER formal_parameters",
	"subprogram_class : /* empty */",
	"subprogram_class : LLOCAL",
	"subprogram_class : EXTERNAL",
	"body : compound_statement of_subprogram",
	"body : END of_subprogram",
	"of_subprogram : STATEMENT",
	"of_subprogram : FUNCTION",
	"compound_statement : BEGIN",
	"compound_statement : BEGIN stmt_decl_list statements",
	"compound_statement : BEGIN stmt_decl_list statements END",
	"compound_statement : error END",
	"stmt_decl_list : /* empty */",
	"stmt_decl_list : stmt_declarations",
	"stmt_declarations : stmt_declaration",
	"stmt_declarations : stmt_declarations stmt_declaration",
	"stmt_declaration : var_declaration",
	"stmt_declaration : type_declaration",
	"statements : /* empty */",
	"statements : statements statement",
	"statement : compound_statement",
	"statement : simple_statement",
	"simple_statement : assignment_stmt",
	"simple_statement : procedure_stmt",
	"simple_statement : if_stmt",
	"simple_statement : while_stmt",
	"simple_statement : repeat_stmt",
	"simple_statement : for_stmt",
	"simple_statement : select_stmt",
	"simple_statement : return_stmt",
	"simple_statement : increment_stmt",
	"simple_statement : decrement_stmt",
	"simple_statement : break_select_stmt",
	"simple_statement : break_loop_stmt",
	"simple_statement : continue_stmt",
	"simple_statement : definition",
	"simple_statement : inclusion",
	"simple_statement : c",
	"simple_statement : error statement",
	"assignment_stmt : LET",
	"assignment_stmt : LET lvalue BE assign_modifier expr",
	"assign_modifier : /* empty */",
	"assign_modifier : PLUS",
	"assign_modifier : MINUS",
	"assign_modifier : TIMES",
	"assign_modifier : DIVIDED_BY",
	"assign_modifier : MODULUS",
	"procedure_stmt : PROCEDURE_IDENTIFIER",
	"procedure_stmt : PROCEDURE_IDENTIFIER parameter_list",
	"parameter_list : /* empty */",
	"parameter_list : parameters",
	"parameters : parameter",
	"parameters : parameters",
	"parameters : parameters parameter",
	"parameter : expr",
	"if_stmt : IF if_expr THEN if_statement",
	"if_stmt : IF if_expr THEN if_statement ELSE",
	"if_stmt : IF if_expr THEN if_statement ELSE if_statement",
	"if_expr : /* empty */",
	"if_expr : expr",
	"if_statement : /* empty */",
	"if_statement : statement",
	"select_stmt : SELECT",
	"select_stmt : SELECT expr BEGIN",
	"select_stmt : SELECT expr BEGIN case_stmts END",
	"case_stmts : case_stmt",
	"case_stmts : case_stmts case_stmt",
	"case_stmt : CASE case_constant_list ':'",
	"case_stmt : CASE case_constant_list ':' statements",
	"case_constant_list : /* empty */",
	"case_constant_list : case_constant_list case_constant",
	"case_constant : DEFAULT",
	"case_constant : /* empty */",
	"case_constant : constant",
	"while_stmt : WHILE",
	"while_stmt : WHILE expr optional_do",
	"while_stmt : WHILE expr optional_do statement",
	"for_stmt : FOR",
	"for_stmt : FOR EACH VAR_IDENTIFIER",
	"for_stmt : FOR EACH VAR_IDENTIFIER expr",
	"for_stmt : FOR EACH VAR_IDENTIFIER expr expr DO",
	"for_stmt : FOR EACH VAR_IDENTIFIER expr expr DO statement",
	"repeat_stmt : REPEAT",
	"repeat_stmt : REPEAT statements UNTIL",
	"repeat_stmt : REPEAT statements UNTIL expr",
	"optional_do : /* empty */",
	"optional_do : DO",
	"return_stmt : RETURN",
	"return_stmt : RETURN return_expr",
	"return_expr : /* empty */",
	"return_expr : expr",
	"increment_stmt : INCREMENT",
	"increment_stmt : INCREMENT lvalue",
	"decrement_stmt : DECREMENT",
	"decrement_stmt : DECREMENT lvalue",
	"break_select_stmt : BREAK OUT SELECT",
	"break_loop_stmt : BREAK OUT LOOP",
	"continue_stmt : CONTINUE LOOP",
	"expr : primary",
	"expr : addr_expr",
	"expr : minus_expr",
	"expr : not_expr",
	"expr : exist_expr",
	"expr : add_expr",
	"expr : mult_expr",
	"expr : boolean_expr",
	"expr : relational_expr",
	"expr : equality_expr",
	"expr : error",
	"minus_expr : MINUS",
	"minus_expr : MINUS expr",
	"not_expr : NOT",
	"not_expr : NOT expr",
	"exist_expr : expr exist_op",
	"add_expr : expr add_op",
	"add_expr : expr add_op expr",
	"mult_expr : expr mult_op expr",
	"boolean_expr : expr boolean_op expr",
	"equality_expr : expr equality_op expr",
	"relational_expr : expr relational_op expr",
	"exist_op : EXISTS",
	"exist_op : NOT_EXIST",
	"add_op : PLUS",
	"add_op : MINUS",
	"mult_op : TIMES",
	"mult_op : DIVIDED_BY",
	"mult_op : MODULUS",
	"boolean_op : AND",
	"boolean_op : OR",
	"equality_op : NE",
	"equality_op : EQ",
	"relational_op : GT",
	"relational_op : LT",
	"relational_op : GE",
	"relational_op : LE",
	"primary : constant",
	"primary : STRING_CONSTANT",
	"primary : '(' expr ')'",
	"primary : left_value",
	"primary : '(' VAR_IDENTIFIER FUNC_IDENTIFIER",
	"primary : '(' VAR_IDENTIFIER FUNC_IDENTIFIER parameter_list ')'",
	"primary : '(' FUNC_IDENTIFIER",
	"primary : '(' FUNC_IDENTIFIER parameter_list ')'",
	"primary : FUNC_IDENTIFIER",
	"lvalue : left_value",
	"left_value : VAR_IDENTIFIER",
	"left_value : array_expr",
	"left_value : record_expr",
	"left_value : pointer_expr",
	"left_value : '(' left_value ')'",
	"array_expr : primary '['",
	"array_expr : primary '[' expr ']'",
	"record_expr : primary APOSTROPHE_S",
	"record_expr : primary APOSTROPHE_S REC_IDENTIFIER",
	"pointer_expr : CONTENTS",
	"pointer_expr : CONTENTS expr",
	"addr_expr : ADDRESS",
	"addr_expr : ADDRESS expr",
	"constant : NUMBER",
	"constant : CONSTANT_IDENTIFIER",
	"constant : NULL_CONSTANT",
	"constant : QUOTED_IDENTIFIER",
};
#endif /* YYDEBUG */
/*	@(#)yaccpar	1.9	*/

/*
** Skeleton parser driver for yacc output
*/

/*
** yacc user known macros and defines
*/
#define YYERROR		goto yyerrlab
#define YYACCEPT	return(0)
#define YYABORT		return(1)
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		yyerror( "syntax error - cannot backup" );\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#define YYRECOVERING()	(!!yyerrflag)
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int yydebug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define YYFLAG		(-1000)

/*
** global variables used by the parser
*/
YYSTYPE yyv[ YYMAXDEPTH ];	/* value stack */
int yys[ YYMAXDEPTH ];		/* state stack */

YYSTYPE *yypv;			/* top of value stack */
int *yyps;			/* top of state stack */

int yystate;			/* current state */
int yytmp;			/* extra var (lasts between blocks) */

int yynerrs;			/* number of errors */
int yyerrflag;			/* error recovery flag */
int yychar;			/* current input token number */



/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
*/
int
yyparse()
{
	register YYSTYPE *yypvt;	/* top of value stack for $vars */

	/*
	** Initialize externals - yyparse may be called more than once
	*/
	yypv = &yyv[-1];
	yyps = &yys[-1];
	yystate = 0;
	yytmp = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = -1;

	goto yystack;
	{
		register YYSTYPE *yy_pv;	/* top of value stack */
		register int *yy_ps;		/* top of state stack */
		register int yy_state;		/* current state */
		register int  yy_n;		/* internal state number info */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	yynewstate:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
		goto yy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	yystack:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	yy_stack:
		/*
		** put a state and value onto the stacks
		*/
#if YYDEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( yydebug )
		{
			register int yy_i;

			printf( "State %d, token ", yy_state );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++yy_ps >= &yys[ YYMAXDEPTH ] )	/* room on stack? */
		{
			yyerror( "yacc stack overflow" );
			YYABORT;
		}
		*yy_ps = yy_state;
		*++yy_pv = yyval;

		/*
		** we have a new state - find out what to do
		*/
	yy_newstate:
		if ( ( yy_n = yypact[ yy_state ] ) <= YYFLAG )
			goto yydefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		yytmp = yychar < 0;
#endif
		if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
			yychar = 0;		/* reached EOF */
#if YYDEBUG
		if ( yydebug && yytmp )
		{
			register int yy_i;

			printf( "Received token " );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
			goto yydefault;
		if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )	/*valid shift*/
		{
			yychar = -1;
			yyval = yylval;
			yy_state = yy_n;
			if ( yyerrflag > 0 )
				yyerrflag--;
			goto yy_stack;
		}

	yydefault:
		if ( ( yy_n = yydef[ yy_state ] ) == -2 )
		{
#if YYDEBUG
			yytmp = yychar < 0;
#endif
			if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
				yychar = 0;		/* reached EOF */
#if YYDEBUG
			if ( yydebug && yytmp )
			{
				register int yy_i;

				printf( "Received token " );
				if ( yychar == 0 )
					printf( "end-of-file\n" );
				else if ( yychar < 0 )
					printf( "-none-\n" );
				else
				{
					for ( yy_i = 0;
						yytoks[yy_i].t_val >= 0;
						yy_i++ )
					{
						if ( yytoks[yy_i].t_val
							== yychar )
						{
							break;
						}
					}
					printf( "%s\n", yytoks[yy_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				register int *yyxi = yyexca;

				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( yy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( yyerrflag )
			{
			case 0:		/* new error */
				yyerror( "syntax error" );
				goto skip_init;
			yyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				yy_pv = yypv;
				yy_ps = yyps;
				yy_state = yystate;
				yynerrs++;
			skip_init:
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				yyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( yy_ps >= yys )
				{
					yy_n = yypact[ *yy_ps ] + YYERRCODE;
					if ( yy_n >= 0 && yy_n < YYLAST &&
						yychk[yyact[yy_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						yy_state = yyact[ yy_n ];
						goto yy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( yydebug )
						printf( _POP_, *yy_ps,
							yy_ps[-1] );
#	undef _POP_
#endif
					yy_ps--;
					yy_pv--;
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				YYABORT;
			case 3:		/* no shift yet; eat a token */
#if YYDEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( yydebug )
				{
					register int yy_i;

					printf( "Error recovery discards " );
					if ( yychar == 0 )
						printf( "token end-of-file\n" );
					else if ( yychar < 0 )
						printf( "token -none-\n" );
					else
					{
						for ( yy_i = 0;
							yytoks[yy_i].t_val >= 0;
							yy_i++ )
						{
							if ( yytoks[yy_i].t_val
								== yychar )
							{
								break;
							}
						}
						printf( "token %s\n",
							yytoks[yy_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( yychar == 0 )	/* reached EOF. quit */
					YYABORT;
				yychar = -1;
				goto yy_newstate;
			}
		}/* end if ( yy_n == 0 ) */
		/*
		** reduction by production yy_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( yydebug )
			printf( "Reduce by (%d) \"%s\"\n",
				yy_n, yyreds[ yy_n ] );
#endif
		yytmp = yy_n;			/* value to switch over */
		yypvt = yy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using yy_state here as temporary
		** register variable, but why not, if it works...
		** If yyr2[ yy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto yy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int yy_len = yyr2[ yy_n ];

			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
				yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= YYLAST ||
					yychk[ yy_state =
					yyact[ yy_state ] ] != -yy_n )
				{
					yy_state = yyact[ yypgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
			yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= YYLAST ||
				yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
			{
				yy_state = yyact[ yypgo[ yy_n ] ];
			}
		}
					/* save until reenter driver code */
		yystate = yy_state;
		yyps = yy_ps;
		yypv = yy_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/
	switch( yytmp )
	{
		
case 8:
# line 108 "gram.y"
{
                        inside_function = FALSE;
                        inside_body = FALSE;
			is_parameter = FALSE;
		    } break;
case 9:
# line 117 "gram.y"
{ pass_c(); } break;
case 10:
# line 122 "gram.y"
{
			register FILE  *newfp;
			register char  *s;

			s = yypvt[-0].text;
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
		    } break;
case 11:
# line 154 "gram.y"
{
                        indent();
                        printf("#define %s ", yypvt[-1].name);
                    } break;
case 12:
# line 159 "gram.y"
{
                        strcpy(yypvt[-0].name, yypvt[-3].name);
                        yypvt[-0].ident = CONSTANT_IDENTIFIER;
                        push_symbol(&(yypvt[-0]));
                    } break;
case 15:
# line 174 "gram.y"
{
                         symbol_depth = 0;
			 yypvt[-0].value = record_type;  /* KLUDGE */
			 yyreturn(yypvt[-0]);
                     } break;
case 16:
# line 181 "gram.y"
{
			yypvt[-0].text = (declare_ident == DECLARE_FORMAL ?
				((char *) subprogram_ptr) :  /* KLUDGE */
				((char *) Symbol_ptr));       /* KLUDGE */
			yypvt[-0].value = declare_ident;
			if (!is_external)
			    indent();
			yyreturn(yypvt[-0]);
		     } break;
case 17:
# line 191 "gram.y"
{
                         register YYSTYPE *ptr;
                         register int  depth;
  
			 declare_ident = yypvt[-2].value;
			 record_type = yypvt[-5].value;
			 if (declare_ident != DECLARE_FORMAL || !is_external)
			     emit_base_type(yypvt[-0].type);
			     
                         ptr = (YYSTYPE *) yypvt[-2].text; /* KLUDGE */
                         depth = symbol_depth;
                         for (; depth > 0; --depth) {
                             if (ptr != NULL) {
                                 copy(ptr->type, yypvt[-0].type, sizeof(ptr->type));

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
			     expand_type(yypvt[-0].type);

                         if (declare_ident == DECLARE_FORMAL
                             && peek_type(yypvt[-0].type) == RECORD_TYPE) {
                             errmsg("You can't pass a record as a parameter.  Use a pointer instead.\n");
                             YYERROR;
                         }
                         yyreturn(yypvt[-0]);
                     } break;
case 18:
# line 246 "gram.y"
{ declare_ident = DECLARE_VAR; } break;
case 24:
# line 261 "gram.y"
{
                        ++symbol_depth;

                        switch (declare_ident) {
case DECLARE_VAR:
    yypvt[-0].ident = VAR_IDENTIFIER;
    push_symbol(&(yypvt[-0]));
    break;

case DECLARE_FORMAL:
    yypvt[-0].ident = VAR_IDENTIFIER;
    if (subprogram_ptr->fwdlink == NULL ||
            !streql(yypvt[-0].name, subprogram_ptr->fwdlink->name)) {
        errmsg("Formal declaration name doesn't match formal parameter name.\n");
        YYERROR;
    }
    subprogram_ptr = subprogram_ptr->fwdlink;
    break;

case DECLARE_TYPE:
    yypvt[-0].ident = TYPE_IDENTIFIER;
    push_symbol(&(yypvt[-0]));
    break;

case DECLARE_RECORD:
    yypvt[-0].ident = REC_IDENTIFIER;
    push_symbol(&(yypvt[-0]));
    break;

default:
    crash("Bad value for declare_ident (%d)\n", declare_ident);
                        }
                        yyreturn(yypvt[-0]);
                    } break;
case 26:
# line 300 "gram.y"
{
                        if (!inside_body) 
                            errmsg("'Permanent' declaration is redundant outside procedure body.\n");
			if (declare_ident == DECLARE_FORMAL) 
			    errmsg("Formal declarations cannot be 'Permanent'.\n");
                        fputs("static ", stdout);
                    } break;
case 27:
# line 308 "gram.y"
{
                        if (declare_ident == DECLARE_FORMAL) {
                            errmsg("'External' declaration is not valid in formal declaration.\n");
                        }
                        fputs("extern ", stdout);
                    } break;
case 28:
# line 315 "gram.y"
{
                        if (inside_body || declare_ident == DECLARE_FORMAL) {
                            errmsg("'Global' declaration is only valid outside procedure body.\n");
                        }
                    } break;
case 29:
# line 321 "gram.y"
{
                        if (inside_body || declare_ident == DECLARE_FORMAL) {
                            errmsg("'Local' declaration is redundant inside procedure body.\n");
                        }
                        fputs("static ", stdout);
                    } break;
case 30:
# line 328 "gram.y"
{
                       if (declare_ident != DECLARE_FORMAL &&
                            (!inside_body || declare_ident != DECLARE_VAR)) {
                            errmsg("Only variables inside a subprogram can be of class 'REGISTER'.\n");
                       }
                       fputs("register ", stdout);
                    } break;
case 31:
# line 340 "gram.y"
{
                        indent();
                        fputs("typedef ", stdout);
                        symbol_depth = 0;
                        declare_ident = DECLARE_TYPE;
                    } break;
case 32:
# line 348 "gram.y"
{
			register YYSTYPE *ptr;
			register int	depth;

			yypvt[-0].text = (char *) Symbol_ptr;  /* KLUDGE */
			yypvt[-0].value = create_type();   /* KLUDGE */
			depth = symbol_depth;
			ptr = Symbol_ptr;
                        for (; depth > 0; --depth) {
                            if (ptr != NULL) 
                                push_type(ptr->type, yypvt[-0].value); /* KLUDGE */
                            else
                                crash("Symbol stack empty?? (type)\n");
                            ptr = ptr->link;
			}
			yyreturn(yypvt[-0]);
		     } break;
case 33:
# line 366 "gram.y"
{
                         register YYSTYPE *ptr;
                         register int  depth;
  
			 declare_ident = DECLARE_TYPE;
                         emit_base_type(yypvt[-0].type);
                         ptr = (YYSTYPE *) yypvt[-1].text; /* KLUDGE */
			 depth = symbol_depth;
                         for (; depth > 0; --depth) {
                             if (ptr != NULL) {

				 invert_type(yypvt[-0].type);
                                 emit_type(yypvt[-0].type, ptr->name);
				 invert_type(yypvt[-0].type);

                                 expand_type(yypvt[-0].type);
                                 copy(ptr->type, yypvt[-0].type, sizeof(ptr->type));
                                 push_type(ptr->type, yypvt[-1].value); /* KLUDGE */
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
			 for (ptr = Symbol_ptr; ptr != (YYSTYPE *) yypvt[-1].text
			     && ptr != NULL; ptr = ptr->link) {
			     expand_type(ptr->type);
			    if (ptr->ident == TYPE_IDENTIFIER) {
				indent();
				printf("typedef %s %s %s;", ptr->value == 1 ?
				    "union" : "struct", ptr->name, ptr->name);
			    }
			 }
                         yyreturn(yypvt[-0]);
                     } break;
case 34:
# line 409 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 35:
# line 411 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 36:
# line 416 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 37:
# line 418 "gram.y"
{
                        register int  ttype, i;

                        ttype = pop_type(yypvt[-0].type);
			for (i=0; i < STACK_SIZE; ++i)
				yypvt[-0].type[i] = END_OF_STACK;
                        push_type(yypvt[-0].type, ttype);
                        yyreturn(yypvt[-0]);
                    } break;
case 38:
# line 431 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 39:
# line 433 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 40:
# line 438 "gram.y"
{
		      if (!is_external || declare_ident == DECLARE_FUNC)
		         fputs("unsigned ", stdout);
		    } break;
case 41:
# line 443 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 42:
# line 448 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 43:
# line 450 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 44:
# line 452 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 45:
# line 454 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 46:
# line 456 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 47:
# line 458 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 48:
# line 460 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 49:
# line 465 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 50:
# line 467 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 51:
# line 469 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 52:
# line 471 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 53:
# line 476 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 54:
# line 478 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 55:
# line 483 "gram.y"
{
                        if (yypvt[-3].value > yypvt[-2].value) {
                            errmsg("Array indexes must be non-decreasing.\n");
                        }
                        push_type(yypvt[-0].type, yypvt[-2].value);
                        push_type(yypvt[-0].type, yypvt[-3].value);
                        push_type(yypvt[-0].type, ARRAY_TYPE);
                        yyreturn(yypvt[-0]);
                     } break;
case 56:
# line 496 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 57:
# line 498 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 58:
# line 500 "gram.y"
{
		      char buf[10];

                      if (strlen(yypvt[-0].name) > 1 && yypvt[-0].name[0] != '\\' 
                         || yypvt[-0].name[0] == '\\' && strlen(yypvt[-0].name) != 2) {
                         errmsg("Only 1 character allowed in a character constant.\n");
                      }
		      sprintf(buf, yypvt[-0].name, 0);
		      yypvt[-0].value = buf[0];
                      push_type(yypvt[-0].type, CHARACTER_TYPE);
                      yyreturn(yypvt[-0]);
                    } break;
case 59:
# line 517 "gram.y"
{ 
                        push_type(yypvt[-0].type, -32767);
                        push_type(yypvt[-0].type, -32767);
                        push_type(yypvt[-0].type, ARRAY_TYPE);
                        yyreturn(yypvt[-0]);
                    } break;
case 60:
# line 527 "gram.y"
{
                        push_type(yypvt[-0].type, POINTER_TYPE);
                        yyreturn(yypvt[-0]);
                    } break;
case 63:
# line 540 "gram.y"
{
			indent();
			printf("struct %s {", Symbol_ptr->name);
			margin_right();
			yyreturn(yypvt[-0]);
		    } break;
case 64:
# line 547 "gram.y"
{
			indent();
			printf("union %s {", Symbol_ptr->name);
			margin_right();
			Symbol_ptr->value = 1;
			yyreturn(yypvt[-0]);
		    } break;
case 65:
# line 558 "gram.y"
{
                        pop_type(yypvt[-0].type);
                        push_type(yypvt[-0].type, (record_type = create_rectype()));
			if (declare_ident == DECLARE_RECORD) {
			    push_symbol(&(yypvt[-0]));
			    push_type(Symbol_ptr->type, RECORD_TYPE);
			    push_type(Symbol_ptr->type, create_type());
			    Symbol_ptr->ident = TYPE_IDENTIFIER;
			    Symbol_ptr->text = Symbol_ptr->name; /* MEGAKLUDGE */
			    strcpy(Symbol_ptr->name, Symbol_ptr->link->name);
			}
			yypvt[-0].value = symbol_depth;  /* KLUDGE */
                        yyreturn(yypvt[-0]);
                    } break;
case 66:
# line 573 "gram.y"
{
                        symbol_depth = yypvt[-3].value;  /* KLUDGE */
                        yypvt[-3].value = 0;             /* KLUDGE */
                        margin_left();
                        indent();
			putchar('}');
			putchar(' ');
                        push_type(yypvt[-3].type, RECORD_TYPE);
                        yyreturn(yypvt[-3]);
                    } break;
case 69:
# line 593 "gram.y"
{ declare_ident = DECLARE_RECORD; } break;
case 70:
# line 595 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 71:
# line 601 "gram.y"
{   next_enum = 0;  } break;
case 72:
# line 603 "gram.y"
{
			indent();
                        push_type(yypvt[-2].type, INTEGER_TYPE);
			yyreturn(yypvt[-2]);
                    } break;
case 73:
# line 613 "gram.y"
{
                        push_type(yypvt[-0].type, INTEGER_TYPE);
                        yypvt[-0].value = next_enum++;
                        yypvt[-0].ident = CONSTANT_IDENTIFIER;
                        indent();
                        printf("#define %s %d", yypvt[-0].name, yypvt[-0].value);
                        push_symbol(&(yypvt[-0]));
                     } break;
case 74:
# line 622 "gram.y"
{
                        push_type(yypvt[-0].type, INTEGER_TYPE);
                        yypvt[-0].value = next_enum++;
                        yypvt[-0].ident = CONSTANT_IDENTIFIER;
                        indent();
                        printf("#define %s %d", yypvt[-0].name, yypvt[-0].value);
                        push_symbol(&(yypvt[-0]));
                     } break;
case 77:
# line 636 "gram.y"
{
                         inside_function = FALSE;
                         inside_body = FALSE;
			 is_external = FALSE;
			 is_parameter = FALSE;
                         pop_subprogram_frame();
                     } break;
case 78:
# line 647 "gram.y"
{
                         inside_function = FALSE;
                         inside_loop = 0;
			 inside_select = 0;
                     } break;
case 80:
# line 659 "gram.y"
{
                        yypvt[-0].ident = PROCEDURE_IDENTIFIER;
                        push_symbol(&(yypvt[-0]));
                        subprogram_ptr = Symbol_ptr;
			symbol_depth = 0;
			optional_flag = FALSE;
                        yyreturn(yypvt[-0]);
                    } break;
case 81:
# line 668 "gram.y"
{
                        register YYSTYPE *ptr;
                        register int  depth;

#ifndef unos
                        fputs("void ", stdout);
#endif
			printf("%s(", yypvt[-2].name);
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
			yyreturn(yypvt[-2]);
		    } break;
case 85:
# line 705 "gram.y"
{ optional_flag = TRUE; } break;
case 86:
# line 711 "gram.y"
{
                        push_type(yypvt[-0].type, INTEGER_TYPE);  /* default temp */
                        yypvt[-0].ident = VAR_IDENTIFIER; /* default temp */
			if (optional_flag)
				yypvt[-0].value = 1;  /* KLUDGE */
                        push_symbol(&(yypvt[-0]));
			++symbol_depth;
                        yyreturn(yypvt[-0]);
                    } break;
case 89:
# line 729 "gram.y"
{ declare_ident = DECLARE_FORMAL; } break;
case 91:
# line 735 "gram.y"
{

                        register YYSTYPE *ptr;
			register int	ftype;

                        yypvt[-0].ident = FUNC_IDENTIFIER;
                        strcpy(yypvt[-0].name, yypvt[-1].name);
			ftype = peek_type(yypvt[-0].type);
			if (ftype == RECORD_TYPE || ftype == ARRAY_TYPE) {
			    errmsg("Function type must be a pointer or scalar.\n");
			    YYERROR;
			}
                        emit_base_type(yypvt[-0].type);

			invert_type(yypvt[-0].type);
                        emit_type(yypvt[-0].type, yypvt[-0].name);
                        invert_type(yypvt[-0].type);

                        expand_type(yypvt[-0].type);
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

                        copy(subprogram_ptr->type, yypvt[-0].type, sizeof(yypvt[-0].type));
                        inside_function = TRUE;
                        inside_loop = 0;
			inside_select = 0;
                        yyreturn(yypvt[-0]);
                     } break;
case 93:
# line 779 "gram.y"
{ declare_ident = DECLARE_FUNC; } break;
case 94:
# line 781 "gram.y"
{ yyreturn(yypvt[-1]); } break;
case 95:
# line 787 "gram.y"
{
                        yypvt[-0].ident = FUNC_IDENTIFIER;
                        push_symbol(&(yypvt[-0]));
                        subprogram_ptr = Symbol_ptr;
			symbol_depth = 0;
			optional_flag = FALSE;
                        yyreturn(yypvt[-0]);
                    } break;
case 96:
# line 796 "gram.y"
{ yyreturn(yypvt[-1]); } break;
case 97:
# line 801 "gram.y"
{ indent(); indent(); is_external = FALSE; } break;
case 98:
# line 803 "gram.y"
{ indent(); indent(); is_external = FALSE; fputs("static ", stdout); } break;
case 99:
# line 805 "gram.y"
{ indent(); indent(); is_external = TRUE; fputs("extern ", stdout); } break;
case 100:
# line 810 "gram.y"
{
                        if (is_external) {
                            errmsg("Statements not allowed in external subprogram.\n");
			    is_external = FALSE;
                        }
                        inside_body = FALSE;
                        inside_function = FALSE;
			margin_left();
                        pop_subprogram_frame();
			yypvt[-0].ident = BEGIN_IDENTIFIER;  /* Mark end of var list */
			strcpy(yypvt[-0].name, "begin");
			push_symbol(&(yypvt[-0]));
                    } break;
case 101:
# line 824 "gram.y"
{
			putchar(';');
                        if (!is_external) {
                            errmsg("Missing statements following subprogram heading.\n");
                        }
                        inside_body = FALSE;
                        inside_function = FALSE;
                        pop_subprogram_frame();
			yypvt[-1].ident = BEGIN_IDENTIFIER; /* Mark end of var list */
			strcpy(yypvt[-1].name, "begin");
			push_symbol(&(yypvt[-1]));
			is_external = FALSE;
                    } break;
case 102:
# line 841 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 103:
# line 843 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 104:
# line 849 "gram.y"
{
			if (inside_body)
			    putchar(' ');
			else
			    indent();
			putchar('{');
			if (!inside_body)
			    margin_right();
                        yypvt[-0].ident = BEGIN_IDENTIFIER;
                        strcpy(yypvt[-0].name, "begin");
                        inside_body = TRUE;
                        push_symbol(&(yypvt[-0]));
                    } break;
case 105:
# line 863 "gram.y"
{
                        pop_symbol_frame();
                        margin_left();
                        indent();
			putchar('}');
			margin_right();
                    } break;
case 107:
# line 872 "gram.y"
{
			pop_symbol_frame();
			is_parameter = FALSE;
		    } break;
case 109:
# line 880 "gram.y"
{ indent(); } break;
case 134:
# line 924 "gram.y"
{
			is_parameter = FALSE;
			push_type(yypvt[-1].type, INTEGER_TYPE);
			yyreturn(yypvt[-1]);
		    } break;
case 135:
# line 935 "gram.y"
{ indent(); } break;
case 136:
# line 938 "gram.y"
{
                      int type3, type6, dtype;
                      register  ptr3, ptr6;

		      type3 = peek_type(yypvt[-3].type);
		      type6 = peek_type(yypvt[-0].type);
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
                          if (yypvt[-0].ident != CONSTANT_IDENTIFIER || yypvt[-0].value != 0) {
                              errmsg("Pointer can only be assigned with another pointer or null.\n");
                          }
                      if (ptr3 && ptr6) {
                          register   i;

                          for (i=0; i < STACK_SIZE; ++i)
                              if (yypvt[-3].type[i] != yypvt[-0].type[i]) {
				  if (i < STACK_SIZE-1 || i == STACK_SIZE-1 && 
				      !check_union(yypvt[-3].type, yypvt[-0].type) &&
				      !check_union(yypvt[-0].type, yypvt[-3].type))
				      errmsg("Pointer type conflict on assignment.\n");
				  break;
                              }
                      }
                  } break;
case 137:
# line 974 "gram.y"
{ fputs(" = ", stdout); } break;
case 138:
# line 976 "gram.y"
{
		    if (streql(yypvt[-0].name, "+"))
			errmsg("Must spell out 'PLUS' in plus-assignments.\n");
		    else
			fputs(" += ", stdout);
		  } break;
case 139:
# line 983 "gram.y"
{
		    if (streql(yypvt[-0].name, "-"))
			fputs(" = -", stdout);
		    else
			fputs(" -= ", stdout);
		  } break;
case 140:
# line 990 "gram.y"
{ fputs(" *= ", stdout); } break;
case 141:
# line 992 "gram.y"
{ fputs(" /= ", stdout); } break;
case 142:
# line 994 "gram.y"
{ fputs(" %= ", stdout); } break;
case 143:
# line 999 "gram.y"
{
                      reset_param_match(&(yypvt[-0]));
                      indent();
                      printf("%s(", yypvt[-0].name);
		      is_parameter = TRUE;
                  } break;
case 144:
# line 1006 "gram.y"
{
		      is_parameter = FALSE;
		      putchar(')');
		      putchar(';');
		      after_match();
                      yyreturn(yypvt[-2]);
                  } break;
case 147:
# line 1022 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 148:
# line 1024 "gram.y"
{ fputs(", ", stdout); } break;
case 149:
# line 1026 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 150:
# line 1031 "gram.y"
{
                      if (!match_param(&(yypvt[-0]))) {
                          errmsg("Type mismatch on parameter %d.\n",
			      param_count);
                      }
                      yyreturn(yypvt[-0]);
                  } break;
case 152:
# line 1043 "gram.y"
{
                      indent();
                      fputs("else", stdout);
                  } break;
case 154:
# line 1051 "gram.y"
{
                      indent();
                      fputs("if (", stdout);
                  } break;
case 155:
# line 1056 "gram.y"
{   putchar(')');  } break;
case 156:
# line 1060 "gram.y"
{   margin_right(); } break;
case 157:
# line 1062 "gram.y"
{   margin_left();  } break;
case 158:
# line 1067 "gram.y"
{
		      indent();
		      fputs("switch (", stdout);
		  } break;
case 159:
# line 1072 "gram.y"
{
		      register int etype;

		      fputs(") {", stdout);
		      etype = yypvt[-1].type[STACK_SIZE-1];
		      if (etype == POINTER_TYPE || etype == RECORD_TYPE ||
			etype == ARRAY_TYPE) 
			errmsg("Bad type for Select statement.\n");
		      ++inside_select;
	 	  } break;
case 160:
# line 1083 "gram.y"
{
		      --inside_select;
		      indent();
		      putchar('}');
		  } break;
case 163:
# line 1097 "gram.y"
{ margin_right(); } break;
case 164:
# line 1099 "gram.y"
{
		      indent();
		      fputs("break;", stdout);
		      indent();
		      margin_left();
		  } break;
case 167:
# line 1115 "gram.y"
{
		      indent();
		      fputs("default:", stdout);
		  } break;
case 168:
# line 1120 "gram.y"
{
		      indent();
		      fputs("case ", stdout);
		  } break;
case 169:
# line 1125 "gram.y"
{ putchar(':'); } break;
case 170:
# line 1130 "gram.y"
{
                      indent();
                      fputs("while (", stdout);
                  } break;
case 171:
# line 1135 "gram.y"
{
                      margin_right();
                      ++inside_loop;
                  } break;
case 172:
# line 1140 "gram.y"
{
                      margin_left();
                      --inside_loop;
                  } break;
case 173:
# line 1148 "gram.y"
{
		      indent();
		      putchar('{');
		      margin_right();
		      indent();
		      printf("register zztop%d;\n", margin_position);
		  } break;
case 174:
# line 1156 "gram.y"
{
		      indent();
		      printf("%s = ", yypvt[-0].name);
		  } break;
case 175:
# line 1161 "gram.y"
{
		      putchar(';');
		      indent();
		      printf("zztop%d = ", margin_position);
		  } break;
case 176:
# line 1167 "gram.y"
{
		      putchar(';');
		      indent();
		      printf("for (; %s <= zztop%d; ++%s)", yypvt[-5].name,
			margin_position, yypvt[-5].name);
		      margin_right();
		      ++inside_loop;
		  } break;
case 177:
# line 1176 "gram.y"
{
		      margin_left();
		      --inside_loop;
		      margin_left();
		      indent();
		      putchar('}');
		  } break;
case 178:
# line 1187 "gram.y"
{
		      indent();
		      fputs("do {", stdout);
		      margin_right();
		      ++inside_loop;
		  } break;
case 179:
# line 1194 "gram.y"
{
		      --inside_loop;
		      margin_left();
		      indent();
		      fputs("} while (!(", stdout);
		  } break;
case 180:
# line 1201 "gram.y"
{
		      fputs("));", stdout);
		  } break;
case 181:
# line 1208 "gram.y"
{ putchar(')'); } break;
case 182:
# line 1210 "gram.y"
{ putchar(')'); } break;
case 183:
# line 1215 "gram.y"
{
                      indent();
                      fputs("return ", stdout);
                  } break;
case 185:
# line 1224 "gram.y"
{
                      if (inside_function) {
                          errmsg("RETURN statement must include a value.\n");
                      }
		      putchar(';');
                  } break;
case 186:
# line 1231 "gram.y"
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
			type2 = peek_type(yypvt[-0].type);
			if (type1 == POINTER_TYPE || type1 == RECORD_TYPE ||
			  type1 == ARRAY_TYPE || type2 == POINTER_TYPE ||
			  type2 == RECORD_TYPE || type2 == ARRAY_TYPE) 
			  if (type1 != POINTER_TYPE || type2 != INTEGER_TYPE ||
			  yypvt[-0].ident != CONSTANT_IDENTIFIER || yypvt[-0].value != 0) {
			    int i;

			    for (i=0; i < STACK_SIZE; ++i)
				if (yypvt[-0].type[i] != ptr->type[i])
				    break;
			    if (i < STACK_SIZE-1 || i == STACK_SIZE-1 &&
			      !check_union(ptr->type, yypvt[-0].type) &&
			      !check_union(yypvt[-0].type, ptr->type))
				errmsg("Return value does not match function type.\n");
			  }
		      }
                  } break;
case 187:
# line 1267 "gram.y"
{
                      indent();
                      fputs("++(", stdout);
                  } break;
case 188:
# line 1272 "gram.y"
{
		      putchar(')');
		      putchar(';');
                  } break;
case 189:
# line 1281 "gram.y"
{
                      indent();
                      fputs("--(", stdout);
                  } break;
case 190:
# line 1286 "gram.y"
{
		      putchar(')');
		      putchar(';');
                  } break;
case 191:
# line 1294 "gram.y"
{
                      if (!inside_select) {
                          errmsg("BREAK OUT OF SELECT is only valid inside a select.\n");
                      }
                      indent();
                      fputs("break;", stdout);
                   } break;
case 192:
# line 1305 "gram.y"
{
                      if (!inside_loop) {
                          errmsg("BREAK OUT is only valid inside a loop.\n");
                      }
                      indent();
                      fputs("break;", stdout);
                   } break;
case 193:
# line 1317 "gram.y"
{
                      if (!inside_loop) {
                          errmsg("CONTINUE is only valid inside a loop.\n");
                      }
                      indent();
                      fputs("continue;", stdout);
                   } break;
case 194:
# line 1329 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 195:
# line 1331 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 196:
# line 1333 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 197:
# line 1335 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 198:
# line 1337 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 199:
# line 1339 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 200:
# line 1341 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 201:
# line 1343 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 202:
# line 1345 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 203:
# line 1347 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 204:
# line 1349 "gram.y"
{ 
		    push_type(yypvt[-0].type, INTEGER_TYPE);
                    yyreturn(yypvt[-0]);
                  } break;
case 205:
# line 1358 "gram.y"
{ left_paren(&(yypvt[-1]));  fputs(" -", stdout); } break;
case 206:
# line 1360 "gram.y"
{
                     register int ttype;

                     if ((ttype = peek_type(yypvt[-0].type)) == POINTER_TYPE ||
                         ttype == ARRAY_TYPE || ttype == RECORD_TYPE ||
                         ttype == BOOLEAN_TYPE) {
                          errmsg("Unary minus parameter must be numeric.\n");
                     }
                     yyreturn(yypvt[-0]);
                 } break;
case 207:
# line 1375 "gram.y"
{ left_paren(&(yypvt[-1]));  fputs(" !", stdout); } break;
case 208:
# line 1377 "gram.y"
{
                      if (pop_type(yypvt[-0].type) != BOOLEAN_TYPE) {
                          errmsg("NOT parameter must be a boolean.\n");
                      }
                      yyreturn(yypvt[-2]);
                  } break;
case 209:
# line 1387 "gram.y"
{
                      if (pop_type(yypvt[-1].type) != POINTER_TYPE) {
                          errmsg("EXISTS parameter must be a pointer.\n");
                      }
                      yyreturn(yypvt[-0]);
                  } break;
case 210:
# line 1397 "gram.y"
{
                      register int ttype, ltype;

                      if ((ttype = peek_type(yypvt[-1].type)) == ARRAY_TYPE) {
                          if ((ltype = peek_type(yypvt[-1].type)) != -32767 &&
                              bounds_checking)
                              fputs("zzchk((", stdout);
                      }
                  } break;
case 211:
# line 1407 "gram.y"
{
                      register type1, type4, lobound, hibound;

		      type1 = pop_type(yypvt[-3].type);
		      type4 = pop_type(yypvt[-0].type);
		      if (type1 != POINTER_TYPE || type4 != POINTER_TYPE
			  || yypvt[-3].type[STACK_SIZE-1] != yypvt[-0].type[STACK_SIZE-1])
			  if (type1 == RECORD_TYPE || type4 == RECORD_TYPE ||
			      type4 == POINTER_TYPE || type4 == ARRAY_TYPE) {
			      errmsg("Improper type for additive operator.\n");
			  }

                      if (type1 == ARRAY_TYPE) {
                          lobound = pop_type(yypvt[-3].type); hibound = pop_type(yypvt[-3].type);
                          if (bounds_checking && lobound != -32767)
                              printf(")-%d,%d,%d)", lobound, lobound, hibound);
                          type1 = POINTER_TYPE;
                      }
                      if (type1 == POINTER_TYPE && caution) {
                          errmsg("Caution - pointer arithmetic is hazardous.\n");
                      }
		      if (type1 == POINTER_TYPE && type4 == POINTER_TYPE)
			  push_type(yypvt[-3].type, INTEGER_TYPE);
		      else
			  push_type(yypvt[-3].type, type1);
                      yyreturn(yypvt[-3]);
                  } break;
case 212:
# line 1438 "gram.y"
{
                      register int type1, type3;

		      type1 = pop_type(yypvt[-2].type);
		      type3 = pop_type(yypvt[-0].type);
                      if (type1 == ARRAY_TYPE || type1 == POINTER_TYPE ||
			  type1 == RECORD_TYPE || type3 == ARRAY_TYPE ||
                          type3 == POINTER_TYPE || type3 == RECORD_TYPE) {
                          errmsg("Improper type for multiplicative operators.\n");
                      }
                      push_type(yypvt[-1].type, INTEGER_TYPE);
                      yyreturn(yypvt[-1]);
                  } break;
case 213:
# line 1455 "gram.y"
{
                      if (pop_type(yypvt[-2].type) != BOOLEAN_TYPE ||
                          pop_type(yypvt[-0].type) != BOOLEAN_TYPE) {
                          errmsg("Expression in boolean operator is not boolean.\n");
                      }
                      yyreturn(yypvt[-1]);
                  } break;
case 214:
# line 1467 "gram.y"
{
                      register  type1, type3;

		      type1 = pop_type(yypvt[-2].type);
		      type3 = pop_type(yypvt[-0].type);
                      if (type1 == RECORD_TYPE || type3 == RECORD_TYPE) 
                          errmsg("Can't compare records for equality.\n");

                      if ((type1 == POINTER_TYPE || type1 == ARRAY_TYPE) &&
                          (type3 == POINTER_TYPE || type3 == ARRAY_TYPE)) {
                          if (type1 == ARRAY_TYPE) {
                              pop_type(yypvt[-2].type);   pop_type(yypvt[-2].type);
                          }
                          if (type3 == ARRAY_TYPE) {
                              pop_type(yypvt[-0].type);   pop_type(yypvt[-0].type);
                          }
                          type1 = pop_type(yypvt[-2].type);
                          type3 = pop_type(yypvt[-0].type);
                          if (type1 != type3) {
                              errmsg("Type conflict in equality operator.\n");
                           }
                           if (yypvt[-0].text != NULL)
                               errmsg("Cant compare against a string for equality.\n");
                      }
		      else
                      if (type1 == POINTER_TYPE || type1 == ARRAY_TYPE) {
                         if (yypvt[-0].ident != CONSTANT_IDENTIFIER || yypvt[-0].value != 0) {
                              errmsg("Pointer can only be compared with another pointer or null.\n");
                         }
		      }
                      else /* if type1 is scalar */
                          if (type3 == POINTER_TYPE || type3 == ARRAY_TYPE)
                              errmsg("TYPE conflict in equality operator.\n");
			 
                      yyreturn(yypvt[-1]);
                  } break;
case 215:
# line 1508 "gram.y"
{
                      register int  type1, type3;

		      type1 = pop_type(yypvt[-2].type);
		      type3 = pop_type(yypvt[-0].type);
                      if (type1 == POINTER_TYPE ||
                          type1 == ARRAY_TYPE || type1 == RECORD_TYPE
                          || type3 == POINTER_TYPE ||
                          type3 == ARRAY_TYPE || type3 == RECORD_TYPE) {
                          errmsg("Can't use relational operator on non-numeric expression.\n");
                      }
                      yyreturn(yypvt[-1]);
                  } break;
case 216:
# line 1525 "gram.y"
{ fputs(" != NULL", stdout); yyreturn(yypvt[-0]); } break;
case 217:
# line 1527 "gram.y"
{ fputs(" == NULL", stdout); yyreturn(yypvt[-0]); } break;
case 218:
# line 1532 "gram.y"
{ fputs(" + ", stdout); yyreturn(yypvt[-0]); } break;
case 219:
# line 1534 "gram.y"
{ fputs(" - ", stdout); yyreturn(yypvt[-0]); } break;
case 220:
# line 1539 "gram.y"
{ fputs(" * ", stdout); yyreturn(yypvt[-0]); } break;
case 221:
# line 1541 "gram.y"
{ fputs(" / ", stdout); yyreturn(yypvt[-0]); } break;
case 222:
# line 1543 "gram.y"
{ fputs(" % ", stdout); yyreturn(yypvt[-0]); } break;
case 223:
# line 1548 "gram.y"
{ fputs(" && ", stdout); yyreturn(yypvt[-0]); } break;
case 224:
# line 1550 "gram.y"
{ fputs(" || ", stdout); yyreturn(yypvt[-0]); } break;
case 225:
# line 1555 "gram.y"
{ fputs(" != ", stdout); yyreturn(yypvt[-0]); } break;
case 226:
# line 1557 "gram.y"
{ fputs(" == ", stdout); yyreturn(yypvt[-0]); } break;
case 227:
# line 1563 "gram.y"
{ fputs(" > ", stdout); yyreturn(yypvt[-0]); } break;
case 228:
# line 1565 "gram.y"
{ fputs(" < ", stdout); yyreturn(yypvt[-0]); } break;
case 229:
# line 1567 "gram.y"
{ fputs(" >= ", stdout); yyreturn(yypvt[-0]); } break;
case 230:
# line 1569 "gram.y"
{ fputs(" <= ", stdout); yyreturn(yypvt[-0]); } break;
case 231:
# line 1574 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 232:
# line 1576 "gram.y"
{
		    left_paren(&(yypvt[-1]));
                    fputs(yypvt[-0].text, stdout);
                    yyreturn(yypvt[-0]);
                  } break;
case 233:
# line 1582 "gram.y"
{ putchar(')'); yyreturn(yypvt[-1]); } break;
case 234:
# line 1584 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 235:
# line 1586 "gram.y"
{ 
		     left_paren(&(yypvt[-3]));
                     reset_param_match(&(yypvt[-0]));
                     if (!match_param(&(yypvt[-1]))) {
                          errmsg("Type mismatch on parameter 1.\n");
                     }
                     printf("%s(%s", yypvt[-0].name, yypvt[-1].name);
		     is_parameter = TRUE;
                  } break;
case 236:
# line 1596 "gram.y"
{
		      is_parameter = FALSE;
                      putchar(')');
		      after_match();
                      yyreturn(yypvt[-3]);
                  } break;
case 237:
# line 1603 "gram.y"
{
		     left_paren(&(yypvt[-2]));
                     reset_param_match(&(yypvt[-0]));
                     printf("%s(", yypvt[-0].name);
		     is_parameter = TRUE;
                 } break;
case 238:
# line 1610 "gram.y"
{
		     is_parameter = FALSE;
                     putchar(')');
		     after_match();
                     yyreturn(yypvt[-3]);
                 } break;
case 239:
# line 1617 "gram.y"
{
		     left_paren(&(yypvt[-1]));
                     printf("%s()", yypvt[-0].name);
		     yyreturn(yypvt[-0]);
                 } break;
case 240:
# line 1626 "gram.y"
{
                    register int ttype;

		    if ((ttype = peek_type(yypvt[-0].type)) == ARRAY_TYPE) {
			 errmsg("An array is not a valid left-hand value.\n");
			}
		    if (ttype == RECORD_TYPE) {
			 errmsg("A record is not a valid left-hand value.\n");
		    }
                    yyreturn(yypvt[-0]);
                  } break;
case 241:
# line 1641 "gram.y"
{
		    left_paren(&(yypvt[-1]));
                    fputs(yypvt[-0].name, stdout);
                    yyreturn(yypvt[-0]);
                  } break;
case 242:
# line 1647 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 243:
# line 1649 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 244:
# line 1651 "gram.y"
{ yyreturn(yypvt[-0]); } break;
case 245:
# line 1653 "gram.y"
{ putchar(')'); yyreturn(yypvt[-1]); } break;
case 246:
# line 1658 "gram.y"
{
                          if (bounds_checking)
                              fputs("[zzchk(", stdout);
                          else
                              fputs("[(", stdout);
                      } break;
case 247:
# line 1665 "gram.y"
{
                          register int ttype, lobound, hibound;
 
                          if (pop_type(yypvt[-4].type) != ARRAY_TYPE) {
                              errmsg("That is not an array name.\n");
                              YYERROR;
                          }
                          if ((ttype = pop_type(yypvt[-1].type)) == POINTER_TYPE ||
                              ttype == ARRAY_TYPE || ttype == RECORD_TYPE) {
                              errmsg("Attempted to use a nonnumeric as an array index.\n");
                              YYERROR;
                          }
                          lobound = pop_type(yypvt[-4].type);
                          hibound = pop_type(yypvt[-4].type);
                          if (bounds_checking && lobound != -32767)
                              printf(",%d,%d)]",
                                  lobound, hibound);
                          else
                              if (lobound != 0 && lobound != -32767)
                                  printf(")-%d]", lobound);
                              else
                                  fputs(")]", stdout);
                          yyreturn(yypvt[-4]);
                     } break;
case 248:
# line 1693 "gram.y"
{
                          register int   ttype;
                          boolean is_pointer = FALSE;

                          if ((ttype = pop_type(yypvt[-1].type)) == POINTER_TYPE) {
                              ttype = pop_type(yypvt[-1].type);
                              is_pointer = TRUE;
                          }
                          if (ttype != RECORD_TYPE) {
                              errmsg("Improper record parameter.\n");
                              YYERROR;
                          }
			  record_type = pop_type(yypvt[-1].type);
			  reclexnum = record_type - recnum_start(record_type)+1;
			  fputs((is_pointer ? "->" : "."), stdout);
		      } break;
case 249:
# line 1710 "gram.y"
{
                          if (record_type != pop_type(yypvt[-0].type)) {
                              errmsg("Type conflict between record and record parameter.\n");
                          }
                          fputs(yypvt[-0].name, stdout);
                          yyreturn(yypvt[-0]);
                      } break;
case 250:
# line 1720 "gram.y"
{ left_paren(&(yypvt[-1])); putchar('*'); } break;
case 251:
# line 1721 "gram.y"
{ if (pop_type(yypvt[-0].type) != POINTER_TYPE) {
                        errmsg("The CONTENTS operator requires a pointer expression.\n");
                      }
                      yyreturn(yypvt[-0]);
                    } break;
case 252:
# line 1729 "gram.y"
{ left_paren(&(yypvt[-1])); putchar('&'); } break;
case 253:
# line 1730 "gram.y"
{
                        register int ttype;

			ttype = peek_type(yypvt[-0].type);
                        if (yypvt[-0].value != 0 || yypvt[-0].text != NULL 
                            || yypvt[-0].ident == CONSTANT_IDENTIFIER) {
                            errmsg("Cannot take the address of a constant.\n");
                        }
                        push_type(yypvt[-0].type, POINTER_TYPE);
                        yyreturn(yypvt[-0]);
                    } break;
case 254:
# line 1745 "gram.y"
{ left_paren(&(yypvt[-1])); printf("%d", yypvt[-0].value); yyreturn(yypvt[-0]); } break;
case 255:
# line 1747 "gram.y"
{ left_paren(&(yypvt[-1])); fputs(yypvt[-0].name, stdout); yyreturn(yypvt[-0]); } break;
case 256:
# line 1749 "gram.y"
{ left_paren(&(yypvt[-1])); fputs("NULL", stdout); yyreturn(yypvt[-0]); } break;
case 257:
# line 1751 "gram.y"
{
		      char buf[10];

		      left_paren(&(yypvt[-1]));
                      if (strlen(yypvt[-0].name) > 1 && yypvt[-0].name[0] != '\\'
                         || yypvt[-0].name[0] == '\\' && strlen(yypvt[-0].name) != 2) {
                         errmsg("Only 1 character allowed in a character constant.\n");
                         YYERROR;
                      }
                      printf("'%s'", yypvt[-0].name);
		      sprintf(buf, yypvt[-0].name, 0);
		      yypvt[-0].value = buf[0];
                      push_type(yypvt[-0].type, CHARACTER_TYPE);
                      yyreturn(yypvt[-0]);
                    } break;
	}
	goto yystack;		/* reset registers in driver code */
}
