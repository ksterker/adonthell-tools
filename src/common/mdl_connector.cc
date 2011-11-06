/*
 Copyright (C) 2011 Kai Sterker <kaisterker@linuxgames.com>
 Part of the Adonthell Project http://adonthell.linuxgames.com
 
 Adonthell is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 Adonthell is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Adonthell; if not, write to the Free Software 
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/** 
 * @file /adonthell-tools/src/common/mdl_connector.cpp/mdl_connector.cpp
 *
 * @author Kai Sterker
 * @brief <...>
 */

#include <algorithm>
#include <vector>

#include <base/diskio.h>

#include "mdl_connector.h"
#include "uid.h"

#define CONNECTOR_TEMPLATE_DATA "connectors.xml"

// create new template
MdlConnectorTemplate::MdlConnectorTemplate(const u_int32 & id)
{
    Uid = uid::create(id);
    Name = "";
    Length = 0;
    Width = 0;
}

// create template from file
MdlConnectorTemplate::MdlConnectorTemplate(base::flat & record)
{
    Uid = uid::from_string(record.get_string("uid"));
    Name = record.get_string("name");
    Length = record.get_uint16("length");
    Width = record.get_uint16("width");
}

// save template to file
void MdlConnectorTemplate::save (base::flat & record) const
{
    record.put_string ("uid", uid::as_string(Uid));
    record.put_string ("name", Name);
    record.put_uint16 ("length", Length);
    record.put_uint16 ("width", Width);
}

// create connector from template
MdlConnector::MdlConnector(const MdlConnectorTemplate *tmpl)
{
    Template = tmpl;
    Pos = 0;
    Side = LEFT;
}

// all known connector templates
std::hash_map<u_int32, MdlConnectorTemplate*> MdlConnectorManager::Templates;

// create new connector template
MdlConnectorTemplate *MdlConnectorManager::create()
{
    u_int32 counter = Templates.size();

    MdlConnectorTemplate* tmpl = new MdlConnectorTemplate (counter);
    while (Templates.count (tmpl->uid()))
    {
        delete tmpl;
        tmpl = new MdlConnectorTemplate (++counter);
    }

    Templates[tmpl->uid()] = tmpl;
    return tmpl;
}

// get existing template by its id
MdlConnectorTemplate *MdlConnectorManager::get (const u_int32 & uid)
{
    if (Templates.count (uid))
    {
        return Templates[uid];
    }

    fprintf (stderr, "*** MdlConnectorManager::get: template with id %u not found.\n", uid);
    return NULL;
}

// delete existing template by its id
void MdlConnectorManager::remove (const u_int32 & uid)
{
    Templates.erase(uid);
}

// freshly load connector templates
bool MdlConnectorManager::load (const std::string & path)
{
    Templates.clear();
    return reload (path, false);
}

// update connector templates from file
bool MdlConnectorManager::reload (const std::string & path, const bool & merge)
{
    base::diskio file;

    // try to load character
    if (!file.get_record (path + "/" + CONNECTOR_TEMPLATE_DATA)) return false;

    u_int32 size;
    char *data;
    char *id;

    MdlConnectorTemplate *tmpl;
    bool result = true;

    // iterate over all saved characters
    while (file.next ((void**) &data, &size, &id) == base::flat::T_FLAT)
    {
        base::flat record (data, size);
        tmpl = new MdlConnectorTemplate (record);

        if (Templates.count (tmpl->uid()))
        {
            if (!merge)
            {
                fprintf (stderr, "*** MdlConnectorManager::load: duplicate template id %u found.\n", tmpl->uid());
                result = false;
            }
            else
            {
                Templates[tmpl->uid()]->set_name(tmpl->name());
                Templates[tmpl->uid()]->set_length(tmpl->length());
                Templates[tmpl->uid()]->set_width(tmpl->width());
            }

            delete tmpl;
            continue;
        }

        Templates[tmpl->uid()] = tmpl;
    }

    return result;
}

struct sort_connector_templates
{
    bool operator() (const MdlConnectorTemplate *a, const MdlConnectorTemplate *b)
    {
        return (a->uid() < b->uid());
    }
};

// save connector templates
bool MdlConnectorManager::save (const std::string & path)
{
    static sort_connector_templates sorter;

    // make sure model connector file entries stay sorted ... makes it easier when committing changes
    std::vector<const MdlConnectorTemplate*> sortedTemplates;
    for (iterator i = Templates.begin(); i != Templates.end(); i++)
    {
        sortedTemplates.push_back(i->second);
    }
    std::sort (sortedTemplates.begin(), sortedTemplates.end(), sorter);

    // write connector templates to file
    base::diskio file;

    for (std::vector<const MdlConnectorTemplate*>::const_iterator i = sortedTemplates.begin(); i < sortedTemplates.end(); i++)
    {
        base::flat record;
        (*i)->save (record);
        file.put_flat ("", record);
    }

    // write character to disk
    return file.put_record (path + "/" + CONNECTOR_TEMPLATE_DATA);
}
