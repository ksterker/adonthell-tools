/*
   $Id: cfg_data.h,v 1.1 2004/07/25 15:52:22 ksterker Exp $
   
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
 * @file cfg_data.h
 *
 * @author Kai Sterker
 * @brief dlgedit configuration data
 */

#ifndef CFG_DATA_H
#define CFG_DATA_H

#include <vector>
#include <deque>
#include <list>
#include "cfg_project.h"

/**
 * This class keeps track of the actual configuration settings.
 * It also checks those settings for correctnesss as far as that
 * is possible.
 *
 * Settings will be added or changed in two ways: at program
 * startup through reading ~/.adonthell/dlgeditrc and at run-time
 * through changes made by the user.
 */
class CfgData
{
public:
    /**
     * Constructor. Initialize global CfgData::data pointer.
     */
    CfgData ();
    /**
     * Destructor. Delete all Project entries.
     */
    ~CfgData ();
    
    /**
     * Adds an entry to the list of previously opened files. Only
     * existing files will be added. There's also a maximum of 15
     * files - when adding more than that, the oldest entries will
     * be discarded.
     * @param file full path/name of the file to add.
     */
    void addFile (const std::string & file);
    /**
     * Get the list of files, sorted alphabetically.
     * @return list of previously opened files.
     */
    std::list<std::string> getFiles ();
    /**    
     * Remove the given file from the list, if possible.
     * @param file The file to remove.
     */
    void removeFile (const std::string & file);

    /**
     * Adds a project to the list of projects and loads all project
     * specific data. The number of projects isn't limited.
     * @param project name of the project to add.
     */
    void addProject (const std::string & project);
    /**
     * Return the names of all projects in the current data directory.
     * This list also includes the default project 'none', so it is
     * non-empty even if the data directory contains no projects yet.
     */
    std::vector<std::string> projectsFromDatadir ();
    
    /**
     * Assign a (new) base directory to the given project. If no
     * such project exists yet, it will be created.
     * @param project name of the project to modify/create.
     * @param basedir directory to assign to this project.
     */
    void setBasedir (const std::string & project, const std::string & basedir);
    /**
     * Return the base directory associated with a given project.
     * In case there is no such project, or no basedir assigned,
     * return "" (the empty string).
     */    
    std::string getBasedir (const std::string & project);
    
    /**
     * Save data to disk.
     * @param file file to save data to.
     */
    void save (std::ofstream & file);
    
    /**
     * Global access to the dlgedit configuration
     */
    static CfgData *data;

private:
    std::deque <std::string> Files;         // list of previously opened files
    std::vector <CfgProject*> Projects;     // list of defined projects
};

#endif // CFG_DATA_H
