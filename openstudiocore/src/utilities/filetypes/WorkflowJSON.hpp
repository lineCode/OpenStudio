/**********************************************************************
 *  Copyright (c) 2008-2016, Alliance for Sustainable Energy.
 *  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **********************************************************************/

#ifndef UTILITIES_FILETYPES_WORKFLOWJSON_HPP
#define UTILITIES_FILETYPES_WORKFLOWJSON_HPP

#include "../UtilitiesAPI.hpp"

#include "../core/Logger.hpp"
#include "../core/Path.hpp"
#include "../data/Variant.hpp"
#include "../data/Attribute.hpp"

#include <jsoncpp/json.h>

namespace openstudio{

  /** TODO:
  * Variant class: remove
  * Attribute class: simplify interface, not use QVariant, handle JSON objects (currently just does arrays), provide better JSON support
  * WorkflowJSON class: ensure that this can read and write arbitrary JSON 
  * WorkflowStep class: build around Attribute
  */
  
class WorkflowStep;

namespace detail{
  class WorkflowJSON_Impl;
}
  
/** Class for accessing the OpenStudio Workflow (OSW) JSON format. */
class UTILITIES_API WorkflowJSON
{
public:

  /** Create a new, empty workflow. */
  WorkflowJSON();

  /** Constructor with string, will throw if string is incorrect. */
  WorkflowJSON(const std::string& s);

  /** Constructor with path, will throw if path does not exist or file is incorrect. */
  WorkflowJSON(const openstudio::path& p);
  
  /** Clones this WorkflowJSON into a separate one. */
  WorkflowJSON clone() const;

  /** Attempt to load a WorkflowJSON from string */
  static boost::optional<WorkflowJSON> load(const std::string& s);

  /** Attempt to load a WorkflowJSON from path */
  static boost::optional<WorkflowJSON> load(const openstudio::path& p);

  /** Get the workflow as a string. */
  std::string string(bool includeHash=true) const;

  /** Get a stored hash of the workflow. */
  std::string hash() const;

  /** Compute the current hash of the workflow. */
  std::string computeHash() const;

  /** Check for updates and return true if there are any, updates value of the stored hash. */
  bool checkForUpdates();

  /** Saves this file to the current location. */
  bool save() const;

  /** Saves this file to a new location. */
  bool saveAs(const openstudio::path& p) const;

  /** Returns the original path this workflow was loaded from, can be empty. */
  openstudio::path path() const;

  /** Returns the absolute path to the root directory, can be empty. 
   ** Key name is 'root', default value is '.' */
  openstudio::path rootPath() const;

  /** Returns the absolute path to the seed file, can be empty. 
   ** Key name is 'seed', default value is '' */
  openstudio::path seedPath() const;

  /** Returns the absolute path to the weather file, can be empty. 
   ** Key name is 'weather_file', default value is '' */
  openstudio::path weatherPath() const;

  /** Returns the absolute path to the measures directory, can be empty. */
  openstudio::path measuresDir() const;

  /** Returns the attributes (other than steps). */
  std::vector<Attribute> attributes() const;

  /** Gets an attribute (other than steps). */
  boost::optional<Attribute> getAttribute(const std::string& name) const;

  /** Removes an attribute (other than steps). */
  bool removeAttribute(const std::string& name);

  /** Sets an attribute (other than steps). */
  bool setAttribute(const Attribute& attribute);
  bool setAttribute(const std::string& name, bool value);
  bool setAttribute(const std::string& name, double value);
  bool setAttribute(const std::string& name, int value);
  bool setAttribute(const std::string& name, const std::string& value);
  
  /** Clears all attributes (other than steps). */
  void clearAttributes();

  /** Returns the workflow steps. */
  std::vector<WorkflowStep> workflowSteps() const;

  /** Assigns the workflow steps. */
  bool setWorkflowSteps(const std::vector<WorkflowStep>& steps);

protected:
  
  // get the impl
  template<typename T>
  std::shared_ptr<T> getImpl() const {
    return std::dynamic_pointer_cast<T>(m_impl);
  }
  
  friend class detail::WorkflowJSON_Impl;

  /** Protected constructor from impl. */
  WorkflowJSON(std::shared_ptr<detail::WorkflowJSON_Impl> impl);
  
private:

  // configure logging
  REGISTER_LOGGER("openstudio.WorkflowJSON");

  // pointer to implementation
  std::shared_ptr<detail::WorkflowJSON_Impl> m_impl;
};

} // openstudio

#endif //UTILITIES_FILETYPES_WORKFLOWJSON_HPP