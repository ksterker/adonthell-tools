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
 * @file common/mdl_connector.h
 *
 * @author Kai Sterker
 * @brief <...>
 */

#ifndef MDL_CONNECTOR_H
#define MDL_CONNECTOR_H

#include <string>
#include <adonthell/base/types.h>
#include <adonthell/base/hash_map.h>
#include <adonthell/base/flat.h>

/**
 * Common connector attributes, possibly shared by
 * different connectors.
 */
class MdlConnectorTemplate
{
public:
    /**
     * @name Template properties
     */
    //@{
    /**
     * Get connector unique id. This never changes.
     * @return human readable name of connector.
     */
    u_int32 uid () const { return Uid; }

    /**
     * Get connector name.
     * @return a unique id for a connector.
     */
    std::string name () const { return Name; }

    /**
     * Set connector name.
     * @param name a unique id for a connector.
     */
    void set_name (const std::string & name) { Name = name; }

    /**
     * Get connector length.
     * @return size when used on horizontal edge
     */
    u_int16 length () const { return Length; }

    /**
     * Set connector length.
     * @param length size when used on horizontal edge
     */
    void set_length (const u_int16 & length) { Length = length; }

    /**
     * Get connector width.
     * @return size when used on vertical edge
     */
    u_int16 width () const { return Width; }

    /**
     * Set connector width.
     * @param width size when used on vertical edge
     */
    void set_width (const u_int16 & width) { Width = width; }
    //@}

private:
    // allow connection manager to create model templates
    friend class MdlConnectorManager;

    /**
     * Create a model connector template.
     * @param id a partially unique number.
     */
    MdlConnectorTemplate(const u_int32 & id);

    /**
     * Restore a model connector template from file.
     * @param record the record to load template data from.
     */
    MdlConnectorTemplate(base::flat & record);

    /**
     * Save connector template to stream.
     * @param record stream to save template to
     */
    void save (base::flat & record) const;

    /**
     * @name Template properties
     */
    //@{
    /// unique id for the connector
    u_int32 Uid;
    /// connector name
    std::string Name;
    /// size when used on horizontal edge
    u_int16 Length;
    /// size when used on vertical edge
    u_int16 Width;
    //@}
};

/**
 * A concrete model connector attached to a certain
 * face of a model with a certain position.
 */
class MdlConnector
{
public:
    typedef enum
    {
        LEFT,
        RIGHT,
        FRONT,
        BACK
    } face;

    /**
     * Create a model connector from the given template.
     * @param tmpl a connector template.
     */
    MdlConnector(const MdlConnectorTemplate *tmpl);

    /**
     * @name Template properties
     */
    //@{
    /**
     * Get connector unique id.
     * @return a unique id for a connector.
     */
    u_int32 uid () const { return Template->uid(); }

    /**
     * Get connector name.
     * @return human readable name of connector.
     */
    std::string name () const { return Template->name(); }

    /**
     * Get connector length.
     * @return size when used on horizontal edge
     */
    u_int16 length () const { return Template->length(); }

    /**
     * Get connector width.
     * @return size when used on vertical edge
     */
    u_int16 width () const { return Template->width(); }
    //@}

    /**
     * @name Connector properties
     */
    //@{
    /**
     * Get the face to which the connector is attached.
     * @return the face the connector is attached to.
     */
    face side() const { return Side; }

    /**
     * Set the face to which the connector is attached.
     * @param side the face the connector is attached to.
     */
    void set_side (const face & side ) { Side = side; }

    /**
     * Get the position of the connector.
     * @return the connectors position.
     */
    s_int16 pos () const { return Pos; }

    /**
     * Set the position of the connector.
     * @param the connectors position.
     */
    void set_pos ( const s_int16 & pos ) { Pos = pos; }
    //@}

    /**
     * Check if the given side is the opposite of this connector's side.
     * @param side the side to check this connector against.
     * @return true if this is the case, false otherwise.
     */
    bool opposite (const face & side)
    {
        switch (side)
        {
            case LEFT: return Side == RIGHT;
            case RIGHT: return Side == LEFT;
            case FRONT: return Side == BACK;
            case BACK: return Side == FRONT;
        }

        return false;
    }

private:
    /// the unchangeable parts of the model connector
    const MdlConnectorTemplate *Template;
    /// the connector position
    s_int16 Pos;
    /// the side of the model the connector is attached to
    face Side;
};

/**
 * Keeps track of model connector templates
 */
class MdlConnectorManager
{
public:
    /**
     * Create a new template.
     * @return a new, unique connector template.
     */
    static MdlConnectorTemplate *create();

    /**
     * Get an existing connector template with the given id.
     * @return connector template or NULL on error.
     */
    static MdlConnectorTemplate *get(const u_int32 & uid);

    /**
     * Delete an existing connector template with the given id.
     */
    static void remove(const u_int32 & uid);

    /**
     * @name Loading/Saving
     */
    //@{
    /**
     * Load list existing connector templates.
     * @param path directory to load templates from.
     * @return \b true if loading successful, \b false otherwise.
     */
    static bool load (const std::string & path);

    /**
     * Reload existing connector templates.
     * @param path directory to load templates from.
     * @param merge whether to skip duplicates or update from new values.
     * @return \b true if loading successful, \b false otherwise.
     */
    static bool reload (const std::string & path, const bool & merge = true);

    /**
     * Save all existing connector templates to given directory.
     * @param path directory to save templates to.
     * @return \b true if saving successful, \b false otherwise.
     */
    static bool save (const std::string & path);
    //@}

    /**
     * @name Iteration
     */
    //@{
    /// iterator type for connector templates
    typedef std::hash_map<u_int32, MdlConnectorTemplate*>::const_iterator iterator;

    /**
     * Get begin of connector template map
     * @return beginning of connector template map.
     */
    static iterator begin()
    {
        return Templates.begin();
    }

    /**
     * Get end of connector template map.
     * @return end of connector template map.
     */
    static iterator end()
    {
        return Templates.end();
    }
    //@}

private:
    /// forbid construction
    MdlConnectorManager();

    /// list of available templates
    static std::hash_map<u_int32, MdlConnectorTemplate*> Templates;
};

#endif
