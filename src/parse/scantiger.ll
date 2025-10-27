                                                            /* -*- C++ -*- */
// %option defines the parameters with which the reflex will be launched
%option noyywrap
// To enable compatibility with bison
%option bison-complete
%option bison-cc-parser=parser
%option bison_cc_namespace=parse
%option bison-locations

%option lex=lex
// Add a param of function lex() generate in Lexer class
%option params="::parse::TigerDriver& td"
%option namespace=parse
// Name of the class generate by reflex
%option lexer=Lexer

%top{

#define YY_EXTERN_C extern "C" // For linkage rule

#include <cerrno>
#include <climits>
#include <regex>
#include <string>

#include <boost/lexical_cast.hpp>

#include <misc/contract.hh>
  // Using misc::escape is very useful to quote non printable characters.
  // For instance
  //
  //    std::cerr << misc::escape('\n') << '\n';
  //
  // reports about `\n' instead of an actual new-line character.
#include <misc/escape.hh>
#include <misc/symbol.hh>
#include <parse/parsetiger.hh>
#include <parse/tiger-driver.hh>

  // here was a fixxx: Some code was deleted here (Define YY_USER_ACTION to update locations).
#define YY_USER_ACTION                          \
  {                                             \
    td.location_.step();                        \
    td.location_.columns(YYLeng());             \
  }

#define TOKEN(Type)                             \
  parser::make_ ## Type(td.location_)

#define TOKEN_VAL(Type, Value)                  \
  parser::make_ ## Type(Value, td.location_)

# define CHECK_EXTENSION()                              \
  do {                                                  \
    if (!td.enable_extensions_p_)                       \
      td.error_ << misc::error::error_type::scan        \
                << td.location_                         \
                << ": invalid identifier: `"            \
                << misc::escape(text()) << "'\n";       \
  } while (false)


%}

%x SC_COMMENT SC_STRING

/* Abbreviations.  */
int             [0-9]+
  /* here was a fixxx: Some code was deleted here. */
id              [a-zA-Z][a-zA-Z0-9_]*|_main
string          \"([^"\\\n]|\\[abfnrtv\\"]|\\[0-9]{3}|\\x[0-9a-fA-F]{2})*\"
ws              [ \t]+ 
eol             \n|\r\n|\r
comment         \/\*

%class{
  // here was a fixxx: Some code was deleted here (Local variables).
  std::string grown_string;
  int comment_level = 0;
}

%%
{int}         {
                int val = 0;
  // here was a fixxx: Some code was deleted here (Decode, and check the value).
                try {
                  val = boost::lexical_cast<int>(text());
                } catch (const boost::bad_lexical_cast &) {
                  td.error_ << misc::error::error_type::scan
                            << td.location_
                            << ": invalid integer: " << text() << '\n';
                }
                return TOKEN_VAL(INT, val);
              }

  /* here was a fixxx: Some code was deleted here. */
{ws}          { /* Skip whitespace */ }
{eol}         { td.location_.lines(1); }

"&"           { return TOKEN(AND); }
"array"       { return TOKEN(ARRAY); }
":="          { return TOKEN(ASSIGN); }
"break"       { return TOKEN(BREAK); }
"class"       { CHECK_EXTENSION(); return TOKEN(CLASS); }
":"           { return TOKEN(COLON); }
","           { return TOKEN(COMMA); }
"/"           { return TOKEN(DIVIDE); }
"do"          { return TOKEN(DO); }
"."           { return TOKEN(DOT); }
"else"        { return TOKEN(ELSE); }
"end"         { return TOKEN(END); }
"="           { return TOKEN(EQ); }
"extends"     { CHECK_EXTENSION(); return TOKEN(EXTENDS); }
"for"         { return TOKEN(FOR); }
"function"    { return TOKEN(FUNCTION); }
">="          { return TOKEN(GE); }
">"           { return TOKEN(GT); }
"if"          { return TOKEN(IF); }
"import"      { return TOKEN(IMPORT); }
"in"          { return TOKEN(IN); }
"{"           { return TOKEN(LBRACE); }
"["           { return TOKEN(LBRACK); }
"<="          { return TOKEN(LE); }
"let"         { return TOKEN(LET); }
"("           { return TOKEN(LPAREN); }
"<"           { return TOKEN(LT); }
"-"           { return TOKEN(MINUS); }
"method"      { CHECK_EXTENSION(); return TOKEN(METHOD); }
"<>"          { return TOKEN(NE); }
"new"         { CHECK_EXTENSION(); return TOKEN(NEW); }
"nil"         { return TOKEN(NIL); }
"of"          { return TOKEN(OF); }
"|"           { return TOKEN(OR); }
"+"           { return TOKEN(PLUS); }
"primitive"   { return TOKEN(PRIMITIVE); }
"}"           { return TOKEN(RBRACE); }
"]"           { return TOKEN(RBRACK); }
")"           { return TOKEN(RPAREN); }
";"           { return TOKEN(SEMI); }
"then"        { return TOKEN(THEN); }
"*"           { return TOKEN(TIMES); }
"to"          { return TOKEN(TO); }
"type"        { return TOKEN(TYPE); }
"var"         { return TOKEN(VAR); }
"while"       { return TOKEN(WHILE); }
"_cast"       { CHECK_EXTENSION(); return TOKEN(CAST); }
"_chunks"     { CHECK_EXTENSION(); return TOKEN(CHUNKS); }
"_exp"        { CHECK_EXTENSION(); return TOKEN(EXP); }
"_lvalue"     { CHECK_EXTENSION(); return TOKEN(LVALUE); }
"_namety"     { CHECK_EXTENSION(); return TOKEN(NAMETY); }

{id}          { return TOKEN_VAL(ID, misc::symbol(text())); }

/* String handling */
\"            { grown_string.clear(); BEGIN(SC_STRING); }

<SC_STRING>\"          { BEGIN(INITIAL); return TOKEN_VAL(STRING, grown_string); }
<SC_STRING>\\a         { grown_string += '\a'; }
<SC_STRING>\\b         { grown_string += '\b'; }
<SC_STRING>\\f         { grown_string += '\f'; }
<SC_STRING>\\n         { grown_string += '\n'; }
<SC_STRING>\\r         { grown_string += '\r'; }
<SC_STRING>\\t         { grown_string += '\t'; }
<SC_STRING>\\v         { grown_string += '\v'; }
<SC_STRING>\\\\        { grown_string += '\\'; }
<SC_STRING>\\\"        { grown_string += '\"'; }

<SC_STRING>\\[0-9]{3}  {
                std::string str = text();
                int val = std::stoi(str.substr(1), nullptr, 8);
                if (val > 255) {
                  td.error_ << misc::error::error_type::scan
                            << td.location_
                            << ": invalid octal escape: " << text() << '\n';
                } else {
                  grown_string += static_cast<char>(val);
                }
              }
              
<SC_STRING>\\x[0-9a-fA-F]{2} {
                     std::string str = text();
                     int val = std::stoi(str.substr(2), nullptr, 16);
                     grown_string += static_cast<char>(val);
                   }

<SC_STRING>\\.         {
                td.error_ << misc::error::error_type::scan
                          << td.location_
                          << ": invalid escape sequence: " << text() << '\n';
              }
              
<SC_STRING>[^\\\"\n]+  { grown_string += text(); }
  
<SC_STRING><<EOF>>     {
                td.error_ << misc::error::error_type::scan
                          << td.location_
                          << ": unterminated string" << '\n';
                BEGIN(INITIAL);
              }

/* Comment handling */
{comment}     { comment_level = 1; BEGIN(SC_COMMENT); }

<SC_COMMENT>"/*"        { ++comment_level; }
<SC_COMMENT>"*/"        { 
                if (--comment_level == 0)
                  BEGIN(INITIAL);
              }
<SC_COMMENT>[^*/\n]+    { /* Skip comment content */ }
<SC_COMMENT>[*/]        { /* Skip * and / that don't form part of comment delimiters */ }
<SC_COMMENT>\n          { td.location_.lines(1); }
<SC_COMMENT><<EOF>>     {
                td.error_ << misc::error::error_type::scan
                          << td.location_
                          << ": unexpected end of file in a comment" << '\n';
                BEGIN(INITIAL);
              }
_[a-zA-Z0-9_]+ {
                 CHECK_EXTENSION();
                 td.error_ << misc::error::error_type::scan
                           << td.location_
                           << ": invalid identifier: `"
                           << misc::escape(text()) << "'\n";
               }
.             {
                td.error_ << misc::error::error_type::scan
                          << td.location_ << ": invalid character: "
                          << misc::escape(text()) << '\n';
              }
%%



