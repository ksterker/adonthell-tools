/*
   $Id: dlg_module_entry.h,v 1.1 2004/07/25 15:52:22 ksterker Exp $

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
 * @file dlg_module_entry.h
 *
 * @author Kai Sterker
 * @brief The textual contents of a DlgModule.
 */

#ifndef DLG_MODULE_ENTRY_H
#define DLG_MODULE_ENTRY_H

#include <string>
#include <vector>

/**
 * The %DlgModuleeEntry keeps the custom Python code of a certain
 * dialogue and a few other settings.
 */
class DlgModuleEntry
{
public:
    /**
     * Create an empty %DlgModuleEntry.
     */
    DlgModuleEntry ();
    
    /**
     * @name Initialization / Cleanup
     */
    //@{
    /**
     * Initialize the entry to a sane state. All members are set to
     * the empty string ("").
     */
    void init ();
    /**
     * Reset the entry to state after construction time. Erases all quest
     * and character information. Sets other members to "".
     */
    void clear ();
    //@}
    
    /**
     * Attribute access.
     */
    //@{
    /**
     * Get the project this module is assigned to.
     * @return the project the module belongs to.
     */
    std::string project ()          { return project_; }
    /**
     * Get the description of this module.
     * @return the module's description.
     */
    std::string description ()      { return description_; }
    /**
     * Get the import statements assigned to this module.
     * @return the module's additional imports.
     */
    std::string imports ()          { return imports_; }
    /**
     * Get the constructor code assigned to this module.
     * @return the module's constructor code.
     */
    std::string ctor ()              { return ctor_; }
    /**
     * Get the destructor code assigned to this module.
     * @return the module's destructor code.
     */
    std::string dtor ()             { return dtor_; }
    /**
     * Get the user defined methods of this module.
     * @return the module's user defined methods.
     */
    std::string methods ()          { return methods_; }
    /**
     * Sets the project this dialogue belongs to. Then loads the NPC and
     * Quest names of this project.
     * @param p Project name
     * @return <b>true</b> if NPC's and Quests could be loaded,
     *         <b>false</b> otherwise.
     */
    bool setProject (std::string p);
    
    /**
     * Set the description of this module.
     * @param d some text describing the module.
     */
    void setDescription (std::string d) { description_ = d; }
    /**
     * Set the import statements assigned to this module.
     * @param i the module's additional imports.
     */
    void setImports (std::string i) { imports_ = i; }
    /**
     * Set the constructor code assigned to this module.
     * @param c the module's constructor code.
     */
    void setCtor (std::string c)    { ctor_ = c; }
    /**
     * Set the destructor code assigned to this module.
     * @param d the module's destructor code.
     */
    void setDtor (std::string d)    { dtor_ = d; }
    /**
     * Set the user defined methods of this module.
     * @param m the module's user defined methods.
     */
    void setMethods (std::string m) { methods_ = m; }
    
    /**
     * Test whether the given string represents an available character.
     * @param c name of the character to check for.
     * @retorn <b>true</b> if the character exists, <b>false</b> otherwise.
     */
    bool isCharacter (const std::string &c);
    /**
     * Test whether the given string represents an available quest.
     * @param q name of the quest to check for.
     * @retorn <b>true</b> if the quest exists, <b>false</b> otherwise.
     */
    bool isQuest (const std::string &q);
    
    /**
     * Iterate over the available characters.
     * @return the character name or the empty string ("") when the 
     *      end of the list is reached.
     */
    std::string character ();
    /**
     * Iterate over the available quests.
     * @return the quest name or the empty string ("") when the 
     *      end of the list is reached.
     */
    std::string quest ();
    //@}
        
private:
    bool loadCharacters ();
    bool loadQuests ();
    
    void addCharacter (std::string c);
    void addQuest (std::string q);

    std::string project_;           // project the dialogue belongs to
    std::string imports_;           // additional import statements
    std::string ctor_;              // constructor code
    std::string dtor_;              // destructor code
    std::string methods_;           // user defined methods
    std::string description_;       // Description of the dialogue
    
    std::vector<std::string> characters;
    std::vector<std::string> quests;
    std::vector<std::string>::iterator itc;
    std::vector<std::string>::iterator itq;
};

#endif // DLG_MODULE_ENTRY_H
