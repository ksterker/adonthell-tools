/*
   $Id: dlg_compiler.h,v 1.1 2004/07/25 15:52:22 ksterker Exp $

   Copyright (C) 2002 Kai Sterker <kaisterker@linuxgames.com>
   Part of the Adonthell Project http://adonthell.linuxgames.com

   Dlgedit is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Dlgedit is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Dlgedit; if not, write to the Free Software 
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/** 
 * @file dlg_compiler.h
 *
 * @author Kai Sterker
 * @brief The dialogue compiler. 
 */

#ifndef DLG_COMPILER_H
#define DLG_COMPILER_H

#include "dlg_module.h"
#include "dlg_circle.h"

/**
 * Python tokens
 */ 
enum token 
{ 
    EQ      = 0, 
    NEQ     = 1, 
    LT      = 2, 
    LEQ     = 3, 
    GT      = 4, 
    GEQ     = 5, 
    ASSIGN  = 6, 
    ACCESS  = 7, 
    COLON   = 8, 
    IF      = 9, 
    ELIF    = 10, 
    ELSE    = 11, 
    PASS    = 12, 
    RETURN  = 13, 
    BAND    = 14, 
    BOR     = 15, 
    NOT     = 16, 
    ADD     = 17, 
    SUB     = 18, 
    MUL     = 19, 
    DIV     = 20, 
    QUOT    = 21, 
    SQUOT   = 22, 
    LBRACE  = 23, 
    RBRACE  = 24, 
    LBRACKET= 25, 
    RBRACKET= 26, 
    COMMA   = 27, 
    COMMENT = 28, 
    MOD     = 29, 
    AND     = 30, 
    OR      = 31, 
    XOR     = 32,
    FIXED   = 33,
    VARIABLE= 34,
    CONSTANT= 35,
    NONE    = 36
};

#define NUM_OPS 33
#define NUM_FXD 5
    
/**
 * It transforms the dialogue into the Python script needed by
 * the Dialogue Engine. 
 *
 * The Engine itself consists of two parts: 
 * - the dialogue base class defined in the dialogue.py module. It 
 *   contains executable code shared by all dialogues.
 * - the C++ part of the Engine uses the methods the Python dialogue
 *   class provides to step through the dialogue and retrieve the 
 *   respective text.
 *
 * The main reason for this architecture is to keep the individual 
 * dialogue scripts as small as possible. Therefore, the scripts
 * created by the compiler contain mainly data and little or no code.
 */
class DlgCompiler
{
public:
    /**
     * Instantiate the compiler.
     * @param module Module to be compiled
     */
    DlgCompiler (DlgModule *module);
    /**
     * Dtor.
     */
    ~DlgCompiler ();
    
    /**
     * Compile the module passed to DlgCompiler
     */
    void run ();
    
private:
    void writeHeader (const std::string &theClass);
    void writeText ();
    void writeCode ();
    void writeLoop ();
    void writeConditions ();
    void writeStart ();
    void writeDialogue ();
    void writeFollower (DlgNode *node);
    void writeCustomCode ();
    
    void addStart (DlgNode *node);
    void addCode (const std::string &cde, int index);
    bool addCondition (DlgCircle *circle, int index);
    
    int checkFollowers (DlgCircle *node);
    bool checkConditions (DlgCircle* node);

    std::string escapeCode (std::string code);
    std::string splitCode (std::string code, int space = 0);
    std::string inflateCode (std::string code);
    
    token getKeyword (const std::string &statement);
    token getToken (const std::string &statement);
    
    std::ofstream file;
    DlgModule *dialogue;            // The dialogue to be compiled    
    
    DlgCircle start;                // Start node of the dialogue
    std::vector<std::string> code;  // Temporary storage for all code
    std::vector<std::string> conditions; // Temporary storage for all conditions
    std::vector<int> loop;          // nodes that are allowed to looped
    int errors;                     // number of errors in dialogue
    
    int *codeTable;                 // Mapping between nodes and code
    int *conditionTable;            // Mapping between nodes and conditions
    int *operationTable;            // Mapping between nodes and condition type

    static std::string operators[NUM_OPS];
    static std::string fixed[NUM_FXD];
};

#endif // DLG_COMPILER_H
