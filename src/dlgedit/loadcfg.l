%{
/*
   $Id: loadcfg.l,v 1.1 2004/07/25 15:52:23 ksterker Exp $

   Copyright (C) 2002 Kai Sterker <kaisterker@linuxgames.com>
   Part of the Adonthell Project http://adonthell.linuxgames.com

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#include <math.h>
#include <string>
#include "dlg_types.h"

#define YY_DECL int parse_cfgfile (std::string &mytext, int &mynum)
%}

digit  [0-9]
num    -?{digit}+

%x text
%option noyywrap nounput never-interactive prefix="loadcfg"

%%

"#".*[\n]? ;    /* Eat up comments */

File            return LOAD_FILE;
Project         return LOAD_PROJECT;
BaseDir         return LOAD_BASE_DIR;
End             return LOAD_END;

\[              mytext = ""; BEGIN(text);
<text>\]        BEGIN(INITIAL); return LOAD_STR;
<text>[^\]]+    mytext += yytext;

{num}           mynum = atoi (yytext); return LOAD_NUM;

[ \t\n]+ ;      /* Eat whitespace */
.               return LOAD_UNKNOWN;
