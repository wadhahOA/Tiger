
// -*- C++ -*-
%require "3.8"
%language "c++"
// Set the namespace name to `parse', instead of `yy'.
%define api.prefix {parse}
%define api.namespace {parse}
%define api.parser.class {parser}
%define api.value.type variant
%define api.token.constructor

// We use a GLR parser because it is able to handle Shift-Reduce and
// Reduce-Reduce conflicts by forking and testing each way at runtime. GLR
// permits, by allowing few conflicts, more readable and maintainable grammars.
%glr-parser
%skeleton "glr2.cc"

// In TC, we expect the GLR to resolve conflicts at runtime. 
// Use %expect and %expect-rr to tell Bison about it.

  // Done: Some code was deleted here (Other directives).

// Tell Bison how many conflicts to expect
%expect 1 // we are allwoed to habev 1 conflict
%expect-rr 0 // this is reduced reduce-reduce conflicts in outcase 0

%define parse.error verbose
%defines
%debug
// Prefix all the tokens with TOK_ to avoid colisions.
%define api.token.prefix {TOK_}

/* We use pointers to store the filename in the locations.  This saves
   space (pointers), time (no deep copy), but leaves the problem of
   deallocation.  This would be a perfect job for a misc::symbol
   object (passed by reference), however Bison locations require the
   filename to be passed as a pointer, thus forcing us to handle the
   allocation and deallocation of this object.

   Nevertheless, all is not lost: we can still use a misc::symbol
   object to allocate a flyweight (constant) string in the pool of
   symbols, extract it from the misc::symbol object, and use it to
   initialize the location.  The allocated data will be freed at the
   end of the program (see the documentation of misc::symbol and
   misc::unique).  */
%define api.filename.type {const std::string}
%locations

/*---------------------.
| Support for tokens.  |
`---------------------*/
%code requires
{
#include <string>
#include <misc/algorithm.hh>
#include <misc/separator.hh>
#include <misc/symbol.hh>
#include <parse/fwd.hh>

  // Pre-declare parse::parse to allow a ``reentrant'' parsing within
  // the parser.
  namespace parse
  {
    ast_type parse(Tweast& input);
  }
}

// The parsing context.
%param { ::parse::TigerDriver& td }
%parse-param { ::parse::Lexer& lexer }

%printer { yyo << $$; } <int> <std::string> <misc::symbol>;

%token <std::string>    STRING "string"
%token <misc::symbol>   ID     "identifier"
%token <int>            INT    "integer"


/*--------------------------------.
| Support for the non-terminals.  |
`--------------------------------*/

%code requires
{
# include <ast/fwd.hh>
// Provide the declarations of the following classes for the
// %destructor clauses below to work properly.
# include <ast/exp.hh>
# include <ast/var.hh>
# include <ast/ty.hh>
# include <ast/name-ty.hh>
# include <ast/field.hh>
# include <ast/field-init.hh>
# include <ast/function-dec.hh>
# include <ast/type-dec.hh>
# include <ast/var-dec.hh>
# include <ast/chunk.hh>
# include <ast/chunk-list.hh>
}

  // DOne: Some code was deleted here (Printers and destructors).
%printer { yyo << $$; } <ast::Exp*> <ast::Var*> <ast::Ty*> <ast::NameTy*>
%printer { yyo << $$; } <ast::Field*> <ast::FieldInit*> <ast::FunctionDec*>
%printer { yyo << $$; } <ast::TypeDec*> <ast::VarDec*> <ast::Chunk*> <ast::ChunkList*>

// nedded to add the delete $$; in the %destructor (not working without it)

%destructor { delete $$; } <ast::Exp*> <ast::Var*> <ast::Ty*> <ast::NameTy*> 
                          <ast::Field*> <ast::FieldInit*> <ast::FunctionDec*> 
                          <ast::TypeDec*> <ast::VarDec*> <ast::Chunk*> <ast::ChunkList*>
/*-----------------------------------------.
| Code output in the implementation file.  |
`-----------------------------------------*/

%code
{
# include <parse/tweast.hh>
# include <misc/separator.hh>
# include <misc/symbol.hh>
# include <ast/all.hh>
# include <ast/libast.hh>
# include <parse/tiger-factory.hh>

  namespace
  {

    /// Get the metavar from the specified map.
    template <typename T>
    T*
    metavar(parse::TigerDriver& td, unsigned key)
    {
      parse::Tweast* input = td.input_;
      return input->template take<T>(key);
    }

  }
}

%code
{
  #include <parse/scantiger.hh>  // header file generated with reflex --header-file
  #undef yylex
  #define yylex lexer.lex  // Within bison's parse() we should invoke lexer.lex(), not the global lex()
}

// Definition of the tokens, and their pretty-printing.
%token AND          "&"
       ARRAY        "array"
       ASSIGN       ":="
       BREAK        "break"
       CAST         "_cast"
       CLASS        "class"
       COLON        ":"
       COMMA        ","
       DIVIDE       "/"
       DO           "do"
       DOT          "."
       ELSE         "else"
       END          "end"
       EQ           "="
       EXTENDS      "extends"
       FOR          "for"
       FUNCTION     "function"
       GE           ">="
       GT           ">"
       IF           "if"
       IMPORT       "import"
       IN           "in"
       LBRACE       "{"
       LBRACK       "["
       LE           "<="
       LET          "let"
       LPAREN       "("
       LT           "<"
       MINUS        "-"
       METHOD       "method"
       NE           "<>"
       NEW          "new"
       NIL          "nil"
       OF           "of"
       OR           "|"
       PLUS         "+"
       PRIMITIVE    "primitive"
       RBRACE       "}"
       RBRACK       "]"
       RPAREN       ")"
       SEMI         ";"
       THEN         "then"
       TIMES        "*"
       TO           "to"
       TYPE         "type"
       VAR          "var"
       WHILE        "while"
       EOF 0        "end of file"


%type <ast::Exp*>             exp
%type <ast::ChunkList*>       chunks

%type <ast::TypeChunk*>       tychunk
%type <ast::TypeDec*>         tydec
%type <ast::NameTy*>          typeid
%type <ast::Ty*>              ty

%type <ast::Field*>           tyfield
%type <ast::fields_type*>     tyfields tyfields.1
  // Fixme: Some code was deleted here (More %types).
%type <ast::Chunk*>           chunk
%type <ast::Exp*>             expseq
%type <ast::exps_type*>       exp.seq exp.seq.1
%type <ast::VarChunk*>        varchunk
%type <ast::VarDec*>          vardec
%type <ast::FunctionChunk*>   funchunk
%type <ast::FunctionDec*>     fundec
%type <ast::FieldInit*>       fieldinit
%type <ast::fieldinits_type*> fieldinits fieldinits.1
%type <ast::Var*>             lvalue



/* %type <ast::FunctionDec*>     fundec */

  // Done: Some code was deleted here (Priorities/associativities).
  // Operators precedence (from lowest to highest) and associativity(from left to right).


// Solving conflicts on:
// let type foo = bar
//     type baz = bat
// which can be understood as a list of two TypeChunk containing
// a unique TypeDec each, or a single TypeChunk containing two TypeDec.
// We want the latter.
%precedence CHUNKS
%precedence TYPE
  // FIXME: Some code was deleted here (Other declarations).
/* %precedence FUNCTION
%precedence VAR */
// Top is lowest precedence 
%precedence LOWER_THAN_ELSE
%precedence OF
%precedence DO
%precedence ELSE
%precedence VARCHUNK
%precedence VAR
%precedence TYCHUNK
%precedence FUNCHUNK    
%precedence FUNCTION    
%precedence ARRAY_CREATE
%precedence LBRACK RBRACK
%precedence ASSIGN      // := has lowest precedence
%left OR               // | (logical OR) has lower precedence than AND
%left AND              // & (logical AND)
%nonassoc EQ NE LT LE GT GE // Comparison operators
%left PLUS MINUS       // + and -
%left TIMES DIVIDE     // * and /
%precedence NEG        // Unary minus (negation)
%precedence DOT        // Record/object access (highest precedence)
%precedence LBRACE RBRACE
%precedence END

%token NAMETY "_namety"
%token CHUNKS "_chunks"
%token EXP "_exp"//
%token LVALUE "_lvalue"


%start program

//https://assignments.lrde.epita.fr/reference_manual/tiger_language_reference_manual/syntactic_specifications/syntactic_specifications.html

%%
program:
  /* Parsing a source program.  */
  exp
   { td.ast_ = $1; }
| /* Parsing an imported file.  */
  chunks
   { td.ast_ = $1; }
;


/* Array and record creations */
// FIXME: Some code was deleted here (More rules).


exp:
  INT
    { $$ = make_IntExp(@$, $1); }
| STRING
    { $$ = make_StringExp(@$, $1); }
| NIL
    { $$ = make_NilExp(@$); }


/* Control structures */
| IF exp THEN exp %prec LOWER_THAN_ELSE
    { $$ = make_IfExp(@$, $2, $4, nullptr); }
| IF exp THEN exp ELSE exp %prec ELSE
    { $$ = make_IfExp(@$, $2, $4, $6); }
| WHILE exp DO exp %prec DO
    { $$ = make_WhileExp(@$, $2, $4); }
| FOR ID ASSIGN exp TO exp DO exp %prec DO
    { $$ = make_ForExp(@$, make_VarDec(@2, $2, nullptr, $4), $6, $8); }
| BREAK
    { $$ = make_BreakExp(@$); }
| LET chunks IN expseq END
    { $$ = make_LetExp(@$, $2, $4); }

/* Operators  */
// PROBELM ICI ->  ast::OpExp::Oper seems incprrect not a member of the class FIXME
/* Operators  */
| exp PLUS exp
    { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::add, $3); }
| exp MINUS exp
    { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::sub, $3); }
| exp TIMES exp
    { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::mul, $3); }
| exp DIVIDE exp
    { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::div, $3); }
| exp EQ exp
    { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::eq, $3); }
| exp NE exp
    { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::ne, $3); }
| exp LT exp
    { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::lt, $3); }
| exp LE exp
    { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::le, $3); }
| exp GT exp
    { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::gt, $3); }
| exp GE exp
    { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::ge, $3); }
| exp AND exp
    { $$ = make_IfExp(@$, $1, make_OpExp(@$, $3, ast::OpExp::Oper::ne, make_IntExp(@$, 0)), make_IntExp(@$, 0)); }
| exp OR exp
    { $$ = make_IfExp(@$, $1, make_IntExp(@$, 1), make_OpExp(@$, $3, ast::OpExp::Oper::ne, make_IntExp(@$, 0))); }
  


/* Variables, function calls, etc. */
| lvalue               { $$ = $1; } // Recuperer lvalue
| lvalue ASSIGN exp    { $$ = make_AssignExp(@$, $1, $3); }
| ID LPAREN RPAREN     { $$ = make_CallExp(@$, $1, nullptr); }
| ID LPAREN exp.seq.1 RPAREN { $$ = make_CallExp(@$, $1, $3); }
| MINUS exp %prec NEG  
    { $$ = make_OpExp(@$, make_IntExp(@$, 0), ast::OpExp::Oper::sub, $2); }
| LPAREN exp.seq RPAREN { $$ = make_SeqExp(@$, $2); }
| ID LBRACE RBRACE     { $$ = make_RecordExp(@$, make_NameTy(@1, $1), nullptr); }
| ID LBRACE fieldinits.1 RBRACE { $$ = make_RecordExp(@$, make_NameTy(@1, $1), $3); }
| ID LBRACK exp RBRACK OF exp 
    { $$ = make_ArrayExp(@$, make_NameTy(@1, $1), $3, $6); }
// The additional syntaxic specification
/* | "_cast" "(" exp "," ty ")"  */
| "_exp" "(" INT ")" { $$ = metavar<ast::Exp>(td, $3);}
/*-----------.
| L-values.  |
`-----------*/
lvalue:
  ID                        { $$ = make_SimpleVar(@$, $1); }
| lvalue DOT ID             { $$ = make_FieldVar(@$, $1, $3); }
| lvalue LBRACK exp RBRACK  { $$ = make_SubscriptVar(@$, $1, $3); }
| "_lvalue" "(" INT ")"     { $$ = metavar<ast::Var>(td, $3); }
;

/*-------------------------.
| Expression sequences.    |
`-------------------------*/
expseq:
 exp.seq            {  $$ = make_SeqExp(@$, $1);  }
;

exp.seq:
  %empty              { $$ = make_exps_type(); }
| exp.seq.1            { $$ = $1; }
;

exp.seq.1:
  exp                  { $$ = make_exps_type($1); }
| exp.seq.1 SEMI exp   { $$ = $1; $$->emplace_back($3); }
;

/*------------------------.
| Field Initializations.  |
`------------------------*/
fieldinit:
  ID EQ exp          { $$ = make_FieldInit(@$, $1, $3); }
;

fieldinits.1:
  fieldinit           { $$ = make_fieldinits_type($1); }
| fieldinits.1 COMMA fieldinit { $$ = $1; $$->emplace_back($3); }
;

/*---------------.
| Chunks.        |
`---------------*/
chunks:
  /* Chunks are contiguous series of declarations of the same type
     (TypeDec, FunctionDec...) to which we allow certain specfic behavior like
     self referencing.
     They are held by a ChunkList, that can be empty, like in this case:
        let
        in
            ..
        end
     which is why we end the recursion with a %empty. */
  %empty                  { $$ = make_ChunkList(@$); }
| tychunk   chunks        { $$ = $2; $$->push_front($1); }
| varchunk  chunks        { $$ = $2; $$->push_front($1); }
| funchunk  chunks        { $$ = $2; $$->push_front($1); }
| CHUNKS LPAREN INT RPAREN chunks
    { 
      $$ = $5; $$->splice_front(*metavar<ast::ChunkList>(td, $3)); 
    }
;

tychunk:
  /* Use `%prec CHUNKS' to do context-dependent precedence and resolve a
     shift-reduce conflict. */
  tydec %prec CHUNKS  { $$ = make_TypeChunk(@1); $$->push_front(*$1); }
| tydec tychunk       { $$ = $2; $$->push_front(*$1); }
;

tydec:
  TYPE ID EQ ty { $$ = make_TypeDec(@$, $2, $4); }
;

ty:
  typeid               { $$ = $1; }
| LBRACE tyfields RBRACE     { $$ = make_RecordTy(@$, $2); }
| ARRAY OF typeid  { $$ = make_ArrayTy(@$, $3); }
;

tyfields:
  %empty               { $$ = make_fields_type(); }
| tyfields.1           { $$ = $1; }
;

tyfields.1:
  tyfield                { $$ = make_fields_type($1); }
| tyfields.1 COMMA tyfield { $$ = $1; $$->emplace_back($3); }
;

tyfield:
  ID COLON typeid     { $$ = make_Field(@$, $1, $3); }
;

typeid:
  ID                    { $$ = make_NameTy(@$, $1); }
  /* This is a metavariable. It it used internally by TWEASTs to retrieve
     already parsed nodes when given an input to parse. */
| NAMETY LPAREN INT RPAREN    { $$ = metavar<ast::NameTy>(td, $3); } //This is the _namety ?
;

/*------------------------.
| Variable Declarations.  |
`------------------------*/
varchunk:
  vardec %prec CHUNKS  { $$ = make_VarChunk(@1); $$->push_front(*$1); }
| vardec varchunk      { $$ = $2; $$->push_front(*$1); }
;

vardec:
  VAR ID ASSIGN exp                   { $$ = make_VarDec(@$, $2, nullptr, $4); }
| VAR ID COLON typeid ASSIGN exp        { $$ = make_VarDec(@$, $2, $4, $6); }
;

/*--------------------------.
| Function Declarations.    |
`--------------------------*/
funchunk:
  fundec %prec CHUNKS  { $$ = make_FunctionChunk(@1); $$->push_front(*$1); }
| fundec funchunk      { $$ = $2; $$->push_front(*$1); }
;

/*
to undesestqnd the pqrser

function factorial(n: int): int = 
  if n = 0 then 1 else n * factorial(n-1)

the above function is a function declaration,
it is supposed to be parsed as by the following rule:


FUNCTION: The keyword "function"
ID: The function name (identifier)
LPAREN: Opening parenthesis "("
tyfields: Parameter list with types (like "n: int, m: string")
RPAREN: Closing parenthesis ")"
COLON: The colon character ":"
typeid: Return type of the function
EQ: Equals sign "="
exp: The function body expression

*/


fundec:
  FUNCTION ID LPAREN tyfields RPAREN EQ exp
    {
      ast::VarChunk* formals = make_VarChunk(@$);
      
      // converting each Field in tyfields to a VarDec and add it to formals
      if ($4) {
        for (ast::Field* field : *$4) {
          // Get the name from the field's type and create a new NameTy
          misc::symbol type_name = field->type_name_get().name_get();
          ast::NameTy* type_ptr = make_NameTy(@$, type_name);
          formals->push_front(*make_VarDec(@$, field->name_get(), type_ptr, nullptr));
        }
      }
      
      $$ = make_FunctionDec(@$, $2, formals, nullptr, $7);
    }
| FUNCTION ID LPAREN tyfields RPAREN COLON typeid EQ exp
    {
      // Create a new VarChunk to hold the formals
      ast::VarChunk* formals = make_VarChunk(@$);
      
      // Convert each Field in tyfields to a VarDec and add it to formals
      if ($4) {
        for (ast::Field* field : *$4) {
          // Get the name from the field's type and create a new NameTy
          misc::symbol type_name = field->type_name_get().name_get();
          ast::NameTy* type_ptr = make_NameTy(@$, type_name);
          formals->push_front(*make_VarDec(@$, field->name_get(), type_ptr, nullptr));
        }
      }
      
      $$ = make_FunctionDec(@$, $2, formals, $7, $9);
    }
;



%%

void
parse::parser::error(const location_type& l, const std::string& m)
{
  // FIXME: Some code was deleted here.
  td.error_ << l << ": " << m << "\n"; // not sure of this
}