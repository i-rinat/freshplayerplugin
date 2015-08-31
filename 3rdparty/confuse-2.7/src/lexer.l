%{
/*
 * Copyright (c) 2002,2003,2007 Martin Hedenfalk <martin@bzero.se>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>

#ifdef HAVE_STRING_H
# include <string.h>
#endif
#include "confuse.h"

#include <errno.h>

#if defined(ENABLE_NLS) && defined(HAVE_GETTEXT)
# include <libintl.h>
# define _(str) dgettext(PACKAGE, str)
#else
# define _(str) str
#endif
#define N_(str) str

/*
 * Prevent compilation of static input() function in generated code
 * This function is never used but GCC 4.3 will warn about it.
 */
#define YY_NO_INPUT

typedef char * YYSTYPE;
extern YYSTYPE cfg_yylval;

#define YY_DECL int cfg_yylex ( cfg_t *cfg )

/* temporary buffer for the quoted strings scanner
 */
char *cfg_qstring = NULL;
static unsigned int qstring_index = 0;
static unsigned int qstring_len = 0;
static void qputc(char ch);
#define CFG_QSTRING_BUFSIZ 32

#define MAX_INCLUDE_DEPTH 10
struct {
    YY_BUFFER_STATE state;
    char *filename;
    unsigned int line;
} cfg_include_stack[MAX_INCLUDE_DEPTH];
int cfg_include_stack_ptr = 0;

static YY_BUFFER_STATE pre_string_scan_state = 0;
static YY_BUFFER_STATE string_scan_state = 0;

%}

%option noyywrap

 /* start conditions
  */
%x comment
%x dq_str
%x sq_str

%%

[ \t]+    /* eat up whitespace */

\n   cfg->line++; /* keep track of line number */

("#"|"//")[^\n]*     /* eat up one-line comments */

 /* special keywords/symbols
  */
"{"         { cfg_yylval = yytext; return '{'; }
"}"         { cfg_yylval = yytext; return '}'; }
"("         { cfg_yylval = yytext; return '('; }
")"         { cfg_yylval = yytext; return ')'; }
"="         { cfg_yylval = yytext; return '='; }
"+="        { cfg_yylval = yytext; return '+'; }
","         { cfg_yylval = yytext; return ','; }

 /* handle multi-line C-style comments
  */
"/*"         BEGIN(comment);
<comment>[^*\n]*        /* eat anything that's not a '*' */
<comment>"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
<comment>\n             cfg->line++;
<comment>"*"+"/"        BEGIN(INITIAL);

 /* handle C-style strings
  */
"\""    {
    qstring_index = 0;
    BEGIN(dq_str);
}
<dq_str>\"  { /* saw closing quote - all done */
    BEGIN(INITIAL);
    qputc('\0');
    cfg_yylval = cfg_qstring;
    return CFGT_STR;
 }
<dq_str>$\{[^}]*\} { /* environment variable substitution */
    char *var;
    char *e;
    yytext[strlen(yytext) - 1] = 0;
    e = strchr(yytext+2, ':');
    if(e && e[1] == '-')
        *e = 0;
    else
        e = 0;
    var = getenv(yytext+2);
    if(!var && e)
        var = e+2;
    while(var && *var)
        qputc(*var++);
}
<dq_str>\n   {
    qputc('\n');
    cfg->line++;
}
<dq_str>\\\n { /* allow continuing on next line */
    /* no-op */
    cfg->line++;
}
<dq_str>\\[0-7]{1,3} {  /* octal escape sequence */
    unsigned int result;
    sscanf(yytext + 1, "%o", &result);
    if(result > 0xFF) {
        cfg_error(cfg, _("invalid octal number '%s'"), yytext);
        return 0;
    }
    qputc(result);
 }
<dq_str>\\[0-9]+   {
    cfg_error(cfg, _("bad escape sequence '%s'"), yytext);
    return 0;
}
<dq_str>"\\x"[0-9A-Fa-f]{1,2} { /* hexadecimal escape sequence */
    unsigned int result;
    sscanf(yytext + 2, "%x", &result);
    qputc(result);
}
<dq_str>\\n  {
    qputc('\n');
}
<dq_str>\\r  {
    qputc('\r');
}
<dq_str>\\b  {
    qputc('\b');
}
<dq_str>\\f  {
    qputc('\f');
}
<dq_str>\\a  {
    qputc('\007');
}
<dq_str>\\e  {
    qputc('\033');
}
<dq_str>\\t  {
    qputc('\t');
}
<dq_str>\\v  {
    qputc('\v');
}
<dq_str>\\.  {
    qputc(yytext[1]);
}
<dq_str>[^\\\"\n]+  {
    char *yptr = yytext;
    while(*yptr) {
        qputc(*yptr++);
    }
}

    /* single-quoted string ('...') */
"\'" {
    qstring_index = 0;
    BEGIN(sq_str);
}
<sq_str>\' { /* saw closing quote - all done */
    BEGIN(INITIAL);
    qputc('\0');
    cfg_yylval = cfg_qstring;
    return CFGT_STR;
}
<sq_str>\n   {
    qputc('\n');
    cfg->line++;
}
<sq_str>\\\n { /* allow continuing on next line */
    /* no-op */
    cfg->line++;
}
<sq_str>\\[\\\'] {
    qputc(yytext[1]);
}
<sq_str>\\[^\\\'] {
    qputc(yytext[0]);
    qputc(yytext[1]);
}
<sq_str>[^\\\'\n]+ {
    char *cp = yytext;
    while(*cp != '\0') {
        qputc(*cp++);
    }
}
<sq_str><<EOF>> {
    cfg_error(cfg, _("unterminated string constant"));
    return 0;
}

<<EOF>> {
             if (cfg_include_stack_ptr <= 0)
             {
                 return EOF;
             }
             else
             {
                 yy_delete_buffer( YY_CURRENT_BUFFER );
                 fclose(cfg_yyin);
                 cfg_yyin = 0;
                 --cfg_include_stack_ptr;
                 yy_switch_to_buffer(
                      cfg_include_stack[cfg_include_stack_ptr].state );
                 free(cfg->filename);
                 cfg->filename = cfg_include_stack[cfg_include_stack_ptr].filename;
                 cfg->line = cfg_include_stack[cfg_include_stack_ptr].line;
             }
}

$\{[^}]*\} {
    char *var;
    char *e;
    yytext[strlen(yytext) - 1] = 0;
    e = strchr(yytext+2, ':');
    if(e && e[1] == '-')
        *e = 0;
    else
        e = 0;
    var = getenv(yytext+2);
    if(!var && e)
        var = e+2;
    if(!var)
        var = "";
    cfg_yylval = var;
    return CFGT_STR;
}

 /* an unquoted string
  * a slash can't be followed by another slash (c++
  * comment) or an asterisk (C multi-line comment)
  */
(\/[^ #\"\'\t\n\r={}()+,\/*]|[^ #\"\'\t\n\r={}()+,\*])+ {
    cfg_yylval = yytext;
    return CFGT_STR;
 }

. /* eat any non-matching characters */

%%

void cfg_dummy_function(void)
{
    /* please compiler :-)
     * otherwise "defined but not used" warning
     */
    yyunput(0, 0);
}

int cfg_lexer_include(cfg_t *cfg, const char *filename)
{
    char *xfilename;

    if(cfg_include_stack_ptr >= MAX_INCLUDE_DEPTH) {
        cfg_error(cfg, _("includes nested too deeply"));
        return 1;
    }

    cfg_include_stack[cfg_include_stack_ptr].state = YY_CURRENT_BUFFER;
    cfg_include_stack[cfg_include_stack_ptr].filename = cfg->filename;
    cfg_include_stack[cfg_include_stack_ptr].line = cfg->line;
    cfg_include_stack_ptr++;

    xfilename = cfg_tilde_expand(filename);

    cfg_yyin = fopen(xfilename, "r");

    if(!cfg_yyin) {
        cfg_error(cfg, "%s: %s", xfilename, strerror(errno));
        free(xfilename);
        return 1;
    }

    cfg->filename = xfilename;
    cfg->line = 1;

    yy_switch_to_buffer(yy_create_buffer(cfg_yyin, YY_BUF_SIZE));
    return 0;
}

/* write a character to the quoted string buffer, and reallocate as
 * necessary
 */
static void qputc(char ch)
{
    if(qstring_index >= qstring_len) {
        qstring_len += CFG_QSTRING_BUFSIZ;
        cfg_qstring = (char *)realloc(cfg_qstring, qstring_len);
        assert(cfg_qstring);
        memset(cfg_qstring + qstring_index, 0, CFG_QSTRING_BUFSIZ);
    }
    cfg_qstring[qstring_index++] = ch;
}

void cfg_scan_string_begin(const char *buf)
{
    pre_string_scan_state = YY_CURRENT_BUFFER;

    /* yy_scan_string does a yy_switch_to_buffer call for us
     */
    string_scan_state = yy_scan_string(buf);
}

void cfg_scan_string_end(void)
{
    /* restore to previous state
     */
    yy_delete_buffer(string_scan_state);
    if (pre_string_scan_state)
        yy_switch_to_buffer(pre_string_scan_state);
    free(cfg_qstring);
    cfg_qstring = 0;
    qstring_index = qstring_len = 0;
    string_scan_state = 0;
}

static YY_BUFFER_STATE pre_fp_scan_state;
static YY_BUFFER_STATE fp_scan_state;

void cfg_scan_fp_begin(FILE *fp)
{
    pre_fp_scan_state = YY_CURRENT_BUFFER;
    fp_scan_state = yy_create_buffer(fp, YY_BUF_SIZE);
    yy_switch_to_buffer(fp_scan_state);
}

void cfg_scan_fp_end(void)
{
    /* restore to previous state
     */
    yy_delete_buffer(fp_scan_state);
    if(pre_fp_scan_state)
        yy_switch_to_buffer(pre_fp_scan_state);
    free(cfg_qstring);
    cfg_qstring = 0;
    qstring_index = qstring_len = 0;
}
