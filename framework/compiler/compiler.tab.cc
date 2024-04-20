/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "compiler.yy"

#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <chrono>
#include "ast.hpp"
#include "../re2automata/automata.hpp"
#include "compiler.tab.hh"
#define REPS 15

extern int yylex();
extern FILE *yyin;
AstNodePtr fixOR(AstNodePtr root);
void  yyerror(FILE *fp, const char *s);
AstNodePtr regular;
std::vector<AstNodePtr> *mrg = new std::vector<AstNodePtr>;
std::vector<int> fe;


#line 94 "compiler.tab.cc"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "compiler.tab.hh"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_CHAR = 3,                       /* CHAR  */
  YYSYMBOL_DOT = 4,                        /* DOT  */
  YYSYMBOL_OR_OP = 5,                      /* OR_OP  */
  YYSYMBOL_RANGE_OP = 6,                   /* RANGE_OP  */
  YYSYMBOL_STAR_OP = 7,                    /* STAR_OP  */
  YYSYMBOL_PLUS_OP = 8,                    /* PLUS_OP  */
  YYSYMBOL_NOT_OP = 9,                     /* NOT_OP  */
  YYSYMBOL_OP = 10,                        /* OP  */
  YYSYMBOL_CP = 11,                        /* CP  */
  YYSYMBOL_QUANTIFIER = 12,                /* QUANTIFIER  */
  YYSYMBOL_OR = 13,                        /* OR  */
  YYSYMBOL_CR = 14,                        /* CR  */
  YYSYMBOL_LAZY_OP = 15,                   /* LAZY_OP  */
  YYSYMBOL_ANCHOR_SOL = 16,                /* ANCHOR_SOL  */
  YYSYMBOL_END = 17,                       /* END  */
  YYSYMBOL_YYACCEPT = 18,                  /* $accept  */
  YYSYMBOL_Input = 19,                     /* Input  */
  YYSYMBOL_Line = 20,                      /* Line  */
  YYSYMBOL_Expressions = 21,               /* Expressions  */
  YYSYMBOL_Expression = 22,                /* Expression  */
  YYSYMBOL_QuantifiedExpression = 23,      /* QuantifiedExpression  */
  YYSYMBOL_AndExpression = 24,             /* AndExpression  */
  YYSYMBOL_OpCpExpression = 25,            /* OpCpExpression  */
  YYSYMBOL_OrExpression = 26,              /* OrExpression  */
  YYSYMBOL_RangeExpression = 27,           /* RangeExpression  */
  YYSYMBOL_Range = 28                      /* Range  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   51

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  18
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  11
/* YYNRULES -- Number of rules.  */
#define YYNRULES  36
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  48

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   272


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    48,    48,    49,    54,    55,    63,    66,    71,    97,
     117,   143,   175,   210,   212,   217,   221,   223,   228,   232,
     236,   241,   246,   252,   257,   262,   269,   279,   295,   302,
     311,   356,   379,   405,   416,   448,   457
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "CHAR", "DOT", "OR_OP",
  "RANGE_OP", "STAR_OP", "PLUS_OP", "NOT_OP", "OP", "CP", "QUANTIFIER",
  "OR", "CR", "LAZY_OP", "ANCHOR_SOL", "END", "$accept", "Input", "Line",
  "Expressions", "Expression", "QuantifiedExpression", "AndExpression",
  "OpCpExpression", "OrExpression", "RangeExpression", "Range", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-8)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      -8,     1,    -8,    -8,    -8,     3,    -1,    21,    -8,    -8,
      21,    -5,    -8,    25,    31,     4,    33,    17,    14,    24,
       9,    -8,    15,    27,    -8,    -8,    -8,    -8,    -8,    -8,
       3,    -8,    -8,    -8,    -8,    -8,    26,    12,    30,    -8,
      29,    -8,    21,    -8,    -8,    43,    -8,    -8
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     1,    26,    27,     0,     0,     0,     4,     3,
      29,     6,    22,    13,    19,     0,    16,     0,    35,     0,
       0,     7,    12,     8,    15,    14,    23,    21,    20,    24,
       0,     5,    18,    17,    25,    28,     0,     0,    36,    31,
      11,     9,    30,    33,    32,     0,    10,    34
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
      -8,    -8,    -8,    18,    -7,    -8,    -8,    -8,    42,    -8,
      32
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     1,     9,    10,    11,    12,    13,    14,    15,    16,
      20
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      21,     2,    18,    22,     3,     4,     3,     4,    19,    30,
      23,     5,    38,     5,     6,    38,     6,     7,     8,     7,
      36,    31,    30,    39,     3,     4,    44,    18,    35,    43,
      40,     5,    24,    25,     6,    22,    45,    26,    27,    28,
      32,    33,    41,    29,    46,    34,    47,    17,    42,     0,
       0,    37
};

static const yytype_int8 yycheck[] =
{
       7,     0,     3,    10,     3,     4,     3,     4,     9,     5,
      15,    10,     3,    10,    13,     3,    13,    16,    17,    16,
       6,    17,     5,    14,     3,     4,    14,     3,    11,     3,
      15,    10,     7,     8,    13,    42,     6,    12,     7,     8,
       7,     8,    15,    12,    15,    12,     3,     5,    30,    -1,
      -1,    19
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    19,     0,     3,     4,    10,    13,    16,    17,    20,
      21,    22,    23,    24,    25,    26,    27,    26,     3,     9,
      28,    22,    22,    15,     7,     8,    12,     7,     8,    12,
       5,    17,     7,     8,    12,    11,     6,    28,     3,    14,
      15,    15,    21,     3,    14,     6,    15,     3
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    18,    19,    19,    20,    20,    21,    21,    21,    21,
      21,    21,    21,    22,    22,    22,    22,    22,    22,    22,
      22,    22,    22,    23,    23,    23,    24,    24,    25,    26,
      26,    27,    27,    28,    28,    28,    28
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     2,     1,     2,     2,     3,
       4,     3,     2,     1,     2,     2,     1,     2,     2,     1,
       2,     2,     1,     2,     2,     2,     1,     1,     3,     1,
       3,     3,     4,     3,     4,     1,     2
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (fp, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, fp); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, FILE *fp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (fp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, FILE *fp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep, fp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule, FILE *fp)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)], fp);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, fp); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, FILE *fp)
{
  YY_USE (yyvaluep);
  YY_USE (fp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (FILE *fp)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 3: /* Input: Input Line  */
#line 49 "compiler.yy"
                  {
     }
#line 1136 "compiler.tab.cc"
    break;

  case 5: /* Line: OrExpression END  */
#line 55 "compiler.yy"
                         { regular = fixOR((yyvsp[-1].nodePtr)); 
        // printAST(regular, 0);
        mrg->push_back(regular);
        // printf("size is now %d\n", mrg->size());
     }
#line 1146 "compiler.tab.cc"
    break;

  case 6: /* Expressions: Expression  */
#line 63 "compiler.yy"
                   {
     	  (yyval.nodePtr) = (yyvsp[0].nodePtr); 
     	}
#line 1154 "compiler.tab.cc"
    break;

  case 7: /* Expressions: ANCHOR_SOL Expression  */
#line 66 "compiler.yy"
                          {
    (yyval.nodePtr) = new_ExprNode(ANCHOR_SOL_NODE, NULL, 0);
    (yyval.nodePtr)->brother = (yyvsp[0].nodePtr);
  }
#line 1163 "compiler.tab.cc"
    break;

  case 8: /* Expressions: Expression LAZY_OP  */
#line 71 "compiler.yy"
                      {
        if((yyvsp[-1].nodePtr)->eKind == QUANTIFIER_EXP || (yyvsp[-1].nodePtr)->eKind == OPCPSTAR_EXP || 
              (yyvsp[-1].nodePtr)->eKind == OPCPLUS_EXP){
          (yyval.nodePtr) = new_ExprNode(LAZY_EXP, NULL, 0);
          (yyval.nodePtr)->child = (yyvsp[-1].nodePtr);
        }
        else 
          // printf("type %d\n", $1->eKind);
          if((yyvsp[-1].nodePtr)->eKind == AND_EXP || (yyvsp[-1].nodePtr)->eKind == RANGE_EXP 
            || (yyvsp[-1].nodePtr)->eKind == NOT_EXP){
            (yyval.nodePtr) = new_ExprNode(QUANTIFIER_EXP, strdup("0,1"), 3);
            (yyval.nodePtr)->child = (yyvsp[-1].nodePtr);
          }
          else
            if((yyvsp[-1].nodePtr)->eKind == OPCP_EXP){
              (yyval.nodePtr) = (yyvsp[-1].nodePtr);
              (yyval.nodePtr)->reference = strdup("0,1");
              (yyval.nodePtr)->referenceLength = 3;
              (yyval.nodePtr)->eKind = QUANTIFIER_EXP;
           }
           else{
              yyerror(stderr, "syntax error");
              YYERROR;
           }
  }
#line 1193 "compiler.tab.cc"
    break;

  case 9: /* Expressions: Expression LAZY_OP LAZY_OP  */
#line 97 "compiler.yy"
                             {
        (yyval.nodePtr) = new_ExprNode(LAZY_EXP, NULL, 0);
        if((yyvsp[-2].nodePtr)->eKind == AND_EXP || (yyvsp[-2].nodePtr)->eKind == RANGE_EXP 
          || (yyvsp[-2].nodePtr)->eKind == NOT_EXP){
          (yyval.nodePtr)->child = new_ExprNode(QUANTIFIER_EXP, strdup("0,1"), 3);
          (yyval.nodePtr)->child->child = (yyvsp[-2].nodePtr);
        }
        else
          if((yyvsp[-2].nodePtr)->eKind == OPCP_EXP){
            (yyval.nodePtr)->child = (yyvsp[-2].nodePtr);
            (yyval.nodePtr)->child->reference = strdup("0,1");
            (yyval.nodePtr)->child->referenceLength = 3;
            (yyval.nodePtr)->child->eKind = QUANTIFIER_EXP;
         }
         else{
            yyerror(stderr, "syntax error");
            YYERROR;
         }
  }
#line 1217 "compiler.tab.cc"
    break;

  case 10: /* Expressions: Expressions Expression LAZY_OP LAZY_OP  */
#line 117 "compiler.yy"
                                         {
    AstNodePtr last = new_ExprNode(LAZY_EXP, NULL, 0);
        if((yyvsp[-2].nodePtr)->eKind == AND_EXP || (yyvsp[-2].nodePtr)->eKind == RANGE_EXP 
          || (yyvsp[-2].nodePtr)->eKind == NOT_EXP){
          last->child = new_ExprNode(QUANTIFIER_EXP, strdup("0,1"), 3);
          last->child->child = (yyvsp[-2].nodePtr);
        }
        else
          if((yyvsp[-2].nodePtr)->eKind == OPCP_EXP){
            last->child = (yyvsp[-2].nodePtr);
            last->child->reference = strdup("0,1");
            last->child->referenceLength = 3;
            last->child->eKind = QUANTIFIER_EXP;
         }
         else{
            yyerror(stderr, "syntax error");
            YYERROR;
         }
      AstNodePtr next = (yyvsp[-3].nodePtr);
          while(next->brother != NULL)
            next = next->brother;
          next->brother = last;     

      (yyval.nodePtr) = (yyvsp[-3].nodePtr);
  }
#line 1247 "compiler.tab.cc"
    break;

  case 11: /* Expressions: Expressions Expression LAZY_OP  */
#line 143 "compiler.yy"
                                 {
    AstNodePtr last;
        if((yyvsp[-1].nodePtr)->eKind == QUANTIFIER_EXP || (yyvsp[-1].nodePtr)->eKind == OPCPSTAR_EXP || 
              (yyvsp[-1].nodePtr)->eKind == OPCPLUS_EXP){
          last = new_ExprNode(LAZY_EXP, NULL, 0);
          last->child = (yyvsp[-1].nodePtr);
        }
        else 
          if((yyvsp[-1].nodePtr)->eKind == AND_EXP || (yyvsp[-1].nodePtr)->eKind == RANGE_EXP 
            || (yyvsp[-1].nodePtr)->eKind == NOT_EXP){
            last = new_ExprNode(QUANTIFIER_EXP, strdup("0,1"), 3);
            last->child = (yyvsp[-1].nodePtr);
          }
          else
            if((yyvsp[-1].nodePtr)->eKind == OPCP_EXP){
              last = (yyvsp[-1].nodePtr);
              last->reference = strdup("0,1");
              last->referenceLength = 3;
              last->eKind = QUANTIFIER_EXP;
           }
           else{
              yyerror(stderr, "syntax error");
              YYERROR;
           }
      AstNodePtr next = (yyvsp[-2].nodePtr);
          while(next->brother != NULL)
            next = next->brother;
          next->brother = last;     

      (yyval.nodePtr) = (yyvsp[-2].nodePtr);
  }
#line 1283 "compiler.tab.cc"
    break;

  case 12: /* Expressions: Expressions Expression  */
#line 175 "compiler.yy"
                                {
		char *ref;
   		int length;
   		AstNodePtr prev, last;
   		prev = (yyvsp[-1].nodePtr);
   		do{
   			last = prev;
   			prev = prev->brother;
   		}while(prev != NULL);
		if(last->eKind == AND_EXP && (yyvsp[0].nodePtr)->eKind == AND_EXP){
			length = last->referenceLength;
	    	if(ref = (char *)malloc(sizeof(char)*(length+1))){
	    	  memcpy(ref, last->reference, last->referenceLength);
	    	  *(ref + last->referenceLength) = *((yyvsp[0].nodePtr)->reference);
	    	  free((yyvsp[0].nodePtr)->reference);
	    	  free((yyvsp[0].nodePtr)); 
	    	  free(last->reference);
	    	  last->reference = ref;
          last->referenceLength = last->referenceLength + 1;
	     	}
	     	else
	     		printf("fatal error");
        }
        else{
			AstNodePtr next = (yyvsp[-1].nodePtr);
	    	  while(next->brother != NULL)
	    	  	next = next->brother;
	     	  next->brother = (yyvsp[0].nodePtr);			
        }

	     	  (yyval.nodePtr) = (yyvsp[-1].nodePtr);
        }
#line 1320 "compiler.tab.cc"
    break;

  case 13: /* Expression: AndExpression  */
#line 210 "compiler.yy"
                    { (yyval.nodePtr) = (yyvsp[0].nodePtr);}
#line 1326 "compiler.tab.cc"
    break;

  case 14: /* Expression: AndExpression PLUS_OP  */
#line 212 "compiler.yy"
                          {
      (yyval.nodePtr) = new_ExprNode(OPCPLUS_EXP, NULL, 0);
      (yyval.nodePtr)->child = (yyvsp[-1].nodePtr);
    }
#line 1335 "compiler.tab.cc"
    break;

  case 15: /* Expression: AndExpression STAR_OP  */
#line 217 "compiler.yy"
                          {
      (yyval.nodePtr) = new_ExprNode(OPCPSTAR_EXP, NULL, 0);
      (yyval.nodePtr)->child = (yyvsp[-1].nodePtr);
    }
#line 1344 "compiler.tab.cc"
    break;

  case 16: /* Expression: RangeExpression  */
#line 221 "compiler.yy"
                     { (yyval.nodePtr) = (yyvsp[0].nodePtr);}
#line 1350 "compiler.tab.cc"
    break;

  case 17: /* Expression: RangeExpression PLUS_OP  */
#line 223 "compiler.yy"
                            {
      (yyval.nodePtr) = new_ExprNode(OPCPLUS_EXP, NULL, 0);
      (yyval.nodePtr)->child = (yyvsp[-1].nodePtr);
    }
#line 1359 "compiler.tab.cc"
    break;

  case 18: /* Expression: RangeExpression STAR_OP  */
#line 228 "compiler.yy"
                            {
      (yyval.nodePtr) = new_ExprNode(OPCPSTAR_EXP, NULL, 0);
      (yyval.nodePtr)->child = (yyvsp[-1].nodePtr);
    }
#line 1368 "compiler.tab.cc"
    break;

  case 19: /* Expression: OpCpExpression  */
#line 232 "compiler.yy"
                    { 
      (yyval.nodePtr) = (yyvsp[0].nodePtr);
    }
#line 1376 "compiler.tab.cc"
    break;

  case 20: /* Expression: OpCpExpression PLUS_OP  */
#line 236 "compiler.yy"
                           { 
      (yyval.nodePtr) = (yyvsp[-1].nodePtr);
      (yyval.nodePtr)->eKind = OPCPLUS_EXP;
    }
#line 1385 "compiler.tab.cc"
    break;

  case 21: /* Expression: OpCpExpression STAR_OP  */
#line 241 "compiler.yy"
                           { 
      (yyval.nodePtr) = (yyvsp[-1].nodePtr);
      (yyval.nodePtr)->eKind = OPCPSTAR_EXP;
    }
#line 1394 "compiler.tab.cc"
    break;

  case 22: /* Expression: QuantifiedExpression  */
#line 246 "compiler.yy"
                          { 
      (yyval.nodePtr) = (yyvsp[0].nodePtr);
    }
#line 1402 "compiler.tab.cc"
    break;

  case 23: /* QuantifiedExpression: AndExpression QUANTIFIER  */
#line 252 "compiler.yy"
                              { 
      (yyval.nodePtr) = new_ExprNode(QUANTIFIER_EXP, (yyvsp[0].string), strlen((yyvsp[0].string)));
      (yyval.nodePtr)->child = (yyvsp[-1].nodePtr);
    }
#line 1411 "compiler.tab.cc"
    break;

  case 24: /* QuantifiedExpression: OpCpExpression QUANTIFIER  */
#line 257 "compiler.yy"
                                 {     
      (yyval.nodePtr) = new_ExprNode(QUANTIFIER_EXP, (yyvsp[0].string), strlen((yyvsp[0].string)));
      (yyval.nodePtr)->child = (yyvsp[-1].nodePtr);
    }
#line 1420 "compiler.tab.cc"
    break;

  case 25: /* QuantifiedExpression: RangeExpression QUANTIFIER  */
#line 262 "compiler.yy"
                                  {
      (yyval.nodePtr) = new_ExprNode(QUANTIFIER_EXP, (yyvsp[0].string), strlen((yyvsp[0].string)));
      (yyval.nodePtr)->child = (yyvsp[-1].nodePtr);
    }
#line 1429 "compiler.tab.cc"
    break;

  case 26: /* AndExpression: CHAR  */
#line 269 "compiler.yy"
         {
      char *ref;
    	if(ref = (char *)malloc(sizeof(char)*1)){
      	  *(ref) = (yyvsp[0].character);
       	  (yyval.nodePtr) = new_ExprNode(AND_EXP, ref, 1);
     	}
     	else
     		printf("fatal error");
    }
#line 1443 "compiler.tab.cc"
    break;

  case 27: /* AndExpression: DOT  */
#line 279 "compiler.yy"
          { 
          char *ref;
          if(ref = (char *)malloc(sizeof(char)*1)){
            *(ref) = '\n';
            (yyval.nodePtr) = new_ExprNode(NOT_EXP, NULL, 0);
            (yyval.nodePtr)->child = new_ExprNode(OR_EXP, "", 0);
            (yyval.nodePtr)->child->child = new_ExprNode(AND_EXP, ref, 1);

          }
          else
            printf("fatal error");

    }
#line 1461 "compiler.tab.cc"
    break;

  case 28: /* OpCpExpression: OP OrExpression CP  */
#line 295 "compiler.yy"
                           {  
	 	  (yyval.nodePtr) = new_ExprNode(OPCP_EXP, NULL, 0);
     	  (yyval.nodePtr)->child = fixOR((yyvsp[-1].nodePtr));
     	}
#line 1470 "compiler.tab.cc"
    break;

  case 29: /* OrExpression: Expressions  */
#line 302 "compiler.yy"
                 { 
      if((yyvsp[0].nodePtr)->brother != NULL){
      		(yyval.nodePtr) = new_ExprNode(CMPLX_EXP, NULL, 0);
     	    (yyval.nodePtr)->child = (yyvsp[0].nodePtr);
        }
        else{
        	(yyval.nodePtr) = (yyvsp[0].nodePtr);
        }
    }
#line 1484 "compiler.tab.cc"
    break;

  case 30: /* OrExpression: OrExpression OR_OP Expressions  */
#line 311 "compiler.yy"
                                   {
    	char *ref;
    	int length;
    	AstNodePtr tmp,next;
    	if((yyvsp[0].nodePtr)->brother != NULL){
      		tmp = new_ExprNode(CMPLX_EXP, NULL, 0);
     	    tmp->child = (yyvsp[0].nodePtr);
     	    (yyvsp[0].nodePtr) = tmp;
     	 }
      if((yyvsp[-2].nodePtr)->eKind != OR_EXP || ((yyvsp[-2].nodePtr)->eKind == OR_EXP && (yyvsp[-2].nodePtr)->child == NULL)){
    		tmp = new_ExprNode(OR_EXP, NULL, 0);
     	  tmp->child = (yyvsp[-2].nodePtr);
      }
    	else{
    		tmp = (yyvsp[-2].nodePtr);
      }
		next = tmp->child;
		while(next->brother != NULL){
			next = next->brother;
		}

		if(next->reference != NULL)
			length = next->referenceLength;
    	if((yyvsp[0].nodePtr)->eKind == AND_EXP && (yyvsp[0].nodePtr)->referenceLength == 0 && 
    		((next->eKind == AND_EXP && length == 1 ) || next->eKind == OR_EXP)) {
    		next->eKind = OR_EXP;
	    	if(ref = (char *)malloc(sizeof(char)*(length+1))){
          memcpy(ref, next->reference, next->referenceLength);
          *(ref + next->referenceLength) = *((yyvsp[0].nodePtr)->reference);
	    	  free(next->reference);
	    	  free((yyvsp[0].nodePtr));
	    	  next->reference = ref;
          next->referenceLength = next->referenceLength + 1;
	     	}
	     	else
	     		printf("fatal error");
    	}
    	else
			  next->brother = (yyvsp[0].nodePtr);
	    (yyval.nodePtr) = tmp;
    }
#line 1530 "compiler.tab.cc"
    break;

  case 31: /* RangeExpression: OR Range CR  */
#line 356 "compiler.yy"
                        { 
              AstNodePtr next, tmp, new_OR, brother;
              next = (yyvsp[-1].nodePtr);
              if(next->brother != NULL){
                brother = NULL;
                new_OR = new_ExprNode(OR_EXP, NULL, 0);
                while(next != NULL){
                  tmp = new_ExprNode(OPCP_EXP, NULL, 0);
                  tmp->child = next;
                  next = next->brother;
                  tmp->child->brother = NULL;
                  if(brother != NULL)
                    brother->brother = tmp;
                  else
                    new_OR->child = tmp;
                  brother = tmp;
                }
                next = new_OR;
              }
              tmp = new_ExprNode(OPCP_EXP, NULL, 0);
              tmp->child = next;
              (yyval.nodePtr) = tmp;
            }
#line 1558 "compiler.tab.cc"
    break;

  case 32: /* RangeExpression: OR NOT_OP Range CR  */
#line 379 "compiler.yy"
                                {
                AstNodePtr next, tmp, new_OR, brother;
                next = (yyvsp[-1].nodePtr);
                if(next->brother != NULL){
                  brother = NULL;
                  new_OR = new_ExprNode(OR_EXP, NULL, 0);
                  while(next != NULL){
                    tmp = new_ExprNode(OPCP_EXP, NULL, 0);
                    tmp->child = next;
                    next = next->brother;
                    tmp->child->brother = NULL;
                    if(brother != NULL)
                      brother->brother = tmp;
                    else
                      new_OR->child = tmp;
                    brother = tmp;
                  }
                  next = new_OR;
                }
                tmp = new_ExprNode(NOT_EXP, NULL, 0);
                tmp->child = next;
                (yyval.nodePtr) = tmp;
            }
#line 1586 "compiler.tab.cc"
    break;

  case 33: /* Range: CHAR RANGE_OP CHAR  */
#line 405 "compiler.yy"
                         {
  			char *ref;
	    	if(ref = (char *)malloc(sizeof(char)*2)){
	    	  *(ref) = (yyvsp[-2].character);
	    	  *(ref + 1) = (yyvsp[0].character);
	     	  (yyval.nodePtr) = new_ExprNode(RANGE_EXP, ref, 2);
	     	}
	     	else
	     		printf("fatal error");
	     }
#line 1601 "compiler.tab.cc"
    break;

  case 34: /* Range: Range CHAR RANGE_OP CHAR  */
#line 416 "compiler.yy"
                                  {	
		   		char *ref;
		   		int length;
          AstNodePtr next;
          next = (yyvsp[-3].nodePtr);
          while(next->brother != NULL){
            next = next->brother;
          }
          if(next->eKind == RANGE_EXP){
            length = next->referenceLength;
  		    	if(ref = (char *)malloc(sizeof(char)*(length+2))){
    		    	  memcpy(ref, next->reference, next->referenceLength);
                *(ref + length) = (yyvsp[-2].character);
    		    	  *(ref + length + 1) = (yyvsp[0].character);
    		    	  free(next->reference);
                next->reference = ref;
                next->referenceLength = next->referenceLength + 2; 
  		     	}
  		     	else
  		     		printf("fatal error");
         }
         else
            if(ref = (char *)malloc(sizeof(char)*2)){
              *(ref) = (yyvsp[-2].character);
              *(ref + 1) = (yyvsp[0].character);
              next->brother = new_ExprNode(RANGE_EXP, ref, 2);
            }
            else
              printf("fatal error");
         (yyval.nodePtr) = (yyvsp[-3].nodePtr);
       }
#line 1637 "compiler.tab.cc"
    break;

  case 35: /* Range: CHAR  */
#line 448 "compiler.yy"
             {      
            char *ref;
            if(ref = (char *)malloc(sizeof(char)*1)){
              *(ref) = (yyvsp[0].character);
              (yyval.nodePtr) = new_ExprNode(OR_EXP, ref, 1);
            }
            else
              printf("fatal error");
        }
#line 1651 "compiler.tab.cc"
    break;

  case 36: /* Range: Range CHAR  */
#line 457 "compiler.yy"
                   {
         char *ref;
          int length;
          AstNodePtr next;
          next = (yyvsp[-1].nodePtr);
          while(next->brother != NULL){
            next = next->brother;
          }
          if(next->eKind == OR_EXP){
            length = next->referenceLength;
            if(ref = (char *)malloc(sizeof(char)*(length+1))){
                memcpy(ref, next->reference, next->referenceLength);
                *(ref + length) = (yyvsp[0].character);
                free(next->reference);
                next->reference = ref;
                next->referenceLength = next->referenceLength + 1; 
            }
            else
              printf("fatal error");
         }
         else
            if(ref = (char *)malloc(sizeof(char)*1)){
              *(ref) = (yyvsp[0].character); 
              next->brother = new_ExprNode(OR_EXP, ref, 1);
            }
            else
              printf("fatal error");
         (yyval.nodePtr) = (yyvsp[-1].nodePtr);
       }
#line 1685 "compiler.tab.cc"
    break;


#line 1689 "compiler.tab.cc"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (fp, YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, fp);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, fp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (fp, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, fp);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, fp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 488 "compiler.yy"


AstNodePtr fixOR(AstNodePtr root){
	AstNodePtr tmp = root;
	if(root->eKind == OR_EXP && root->child != NULL &&
		root->child->eKind == OR_EXP && root->child->brother == NULL){
		tmp = root->child;
		free(root);
	}
	return tmp;
}

void yyerror(FILE *fp, const char *s) {
  fprintf(fp, "%s\n", s);		
}

int main(int argc, char *argv[]) {
  OptStruct opt;
  int index;
  int c, b=0;
  opterr = 0;
  OptStructInit(&opt);
  char *time_dir = NULL;

  while ((c = getopt (argc, argv, "m:o:a:b:i:w:t:l:f")) != -1)
    switch (c)
      {
      case 'm':
        opt.numInstr = (1 << (atoi(optarg)));
        break;
      case 'w':
        opt.clusterWidth = atoi(optarg);
        break;
      case 'l':
        opt.lineWidth = atoi(optarg);
        break;
      case 'i':
        opt.inputFile = optarg;
        opt.fileInPtr = fopen(optarg, "r");
        if(opt.fileInPtr == NULL){
          opt.fileInPtr = stdin;
          printf("ciao");
        }
        break;
      case 't': 
        time_dir = optarg;
        break;
      case 'o':
        opt.outputFile = optarg;
        if(!opt.automata)
          opt.fileOutPtr = fopen(optarg, "w");
        if(opt.fileOutPtr == NULL)
          opt.fileOutPtr = stdout;
        break;
      case 'b':
        b = atoi(optarg);
        opt.automata=1;
        break;
      case 'f':
        opt.nopFill = 1;
        break;
      case '?':
        if (optopt == 'l' || optopt == 'w' || optopt == 'i' || optopt == 'o' || optopt == 'a')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }


  printf ("line_width = %d\ncpu_width = %d\ninput_file = %s\noutput_file = %s\n",
          opt.lineWidth, opt.clusterWidth, opt.inputFile, opt.outputFile);

  for (index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);
  yyin = opt.fileInPtr;
  int parser = 1;
  auto start_f = std::chrono::high_resolution_clock::now();
  parser = yyparse(opt.fileInPtr);
  auto end_f = std::chrono::high_resolution_clock::now();
  auto duration_f = std::chrono::duration_cast<std::chrono::microseconds>(end_f - start_f);
  int time_fe = duration_f.count();
  
  
  int output = 0;
  if (!parser){
    if(opt.automata){
      /* opt.fileOutPtr="tmp"; */
      if(b==-1)
        b=mrg->size();
      if(opt.outputFile==""){
        fprintf(stderr, "output file is mandatory\n");
        abort();
      }
      if (mrg->size()<b){
        fprintf(stderr, "not enough REs\n");
      }
      // profiler(mrg);
      float avg_fe=avg(fe);
      output = merge_automata(mrg, b, opt.outputFile, time_dir, time_fe);
      if(output)
        fprintf(stderr, "Could not parse, missing arguments.\n");
    } else {
      clock_t start, end;
      double execution_time;
      start = clock();
  	  translateAST(&opt, regular);
      end = clock();
      execution_time = ((double)(end - start))/CLOCKS_PER_SEC;
      printf("automata single comp. time %f\n", execution_time);
    } 
    if(!output)
      fprintf(stderr, "Successful parsing.\n");
  }
  else
     fprintf(stderr, "error found.\n");
}
