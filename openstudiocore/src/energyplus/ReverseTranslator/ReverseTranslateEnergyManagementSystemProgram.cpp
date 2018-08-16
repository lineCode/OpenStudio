/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2008-2018, Alliance for Sustainable Energy, LLC. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
*  disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
*  derived from this software without specific prior written permission from the respective party.
*
*  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
*  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
*  written permission from Alliance for Sustainable Energy, LLC.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
*  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************************************************************/

#include "../ReverseTranslator.hpp"

#include "../../model/EnergyManagementSystemProgram.hpp"
#include "../../model/EnergyManagementSystemProgram_Impl.hpp"

#include "../../utilities/idf/IdfExtensibleGroup.hpp"
#include "../../utilities/idf/WorkspaceExtensibleGroup.hpp"
#include "../../utilities/core/StringHelpers.hpp"

#include <utilities/idd/EnergyManagementSystem_Program_FieldEnums.hxx>
#include "../../utilities/idd/IddEnums.hpp"
#include <utilities/idd/IddEnums.hxx>

using namespace openstudio::model;

namespace openstudio {

namespace energyplus {

OptionalModelObject ReverseTranslator::translateEnergyManagementSystemProgram(const WorkspaceObject & workspaceObject)
{
  if (workspaceObject.iddObject().type() != IddObjectType::EnergyManagementSystem_Program) {
    LOG(Error, "WorkspaceObject is not IddObjectType: EnergyManagementSystem_Program");
    return boost::none;
  }

  OptionalString s = workspaceObject.getString(EnergyManagementSystem_ProgramFields::Name);
  if(!s){
    LOG(Error, "WorkspaceObject EnergyManagementSystem_Program has no Name");
    return boost::none;
  }


  // Make sure all necessary objects are translated first
  // TODO: JM 2018-08-16: Is this really necessary? Really we should just call the translation of the objects that
  // can be referenced by the EMS program, and these objects should be handling the call to reverseTranslation of the objects
  // they themselves can reference
  for (const WorkspaceObject& workspaceObject : m_workspace.objects()) {

     // &&(workspaceObject.iddObject().type() != IddObjectType::EnergyManagementSystem_Program)
     // && (workspaceObject.iddObject().type() != IddObjectType::EnergyManagementSystem_ProgramCallingManager)
     // && (workspaceObject.iddObject().type() != IddObjectType::EnergyManagementSystem_MeteredOutputVariable)
     // && (workspaceObject.iddObject().type() != IddObjectType::EnergyManagementSystem_OutputVariable)

      // These I'm sure we do need.
    if (   (workspaceObject.iddObject().type() == IddObjectType::EnergyManagementSystem_Subroutine)
        || (workspaceObject.iddObject().type() == IddObjectType::EnergyManagementSystem_Actuator)
        || (workspaceObject.iddObject().type() == IddObjectType::EnergyManagementSystem_Sensor)
        || (workspaceObject.iddObject().type() == IddObjectType::EnergyManagementSystem_ConstructionIndexVariable)
        || (workspaceObject.iddObject().type() == IddObjectType::EnergyManagementSystem_CurveOrTableIndexVariable)
        || (workspaceObject.iddObject().type() == IddObjectType::EnergyManagementSystem_GlobalVariable)
        || (workspaceObject.iddObject().type() == IddObjectType::EnergyManagementSystem_InternalVariable)
        || (workspaceObject.iddObject().type() == IddObjectType::EnergyManagementSystem_TrendVariable)
       ) {
      translateAndMapWorkspaceObject(workspaceObject);
    }
  }

  openstudio::model::EnergyManagementSystemProgram emsProgram(m_model);
  emsProgram.setName(*s);


  // Get all model objects that can be referenced int he EMS Program so we can do name / uid substitution
  const std::vector<IddObjectType> validIddObjectTypes{
    IddObjectType::OS_EnergyManagementSystem_Subroutine,
    IddObjectType::OS_EnergyManagementSystem_Actuator,
    IddObjectType::OS_EnergyManagementSystem_Sensor,
    IddObjectType::OS_EnergyManagementSystem_ConstructionIndexVariable,
    IddObjectType::OS_EnergyManagementSystem_CurveOrTableIndexVariable,
    IddObjectType::OS_EnergyManagementSystem_GlobalVariable,
    IddObjectType::OS_EnergyManagementSystem_InternalVariable,
    IddObjectType::OS_EnergyManagementSystem_TrendVariable
  };

  std::vector<model::ModelObject> modelObjects;
  for (const model::ModelObject& mo: m_model.modelObjects()) {
    if( std::find(validIddObjectTypes.begin(), validIddObjectTypes.end(), mo.iddObjectType()) != validIddObjectTypes.end() ) {
      modelObjects.push_back(mo);
    }
  }


  // Now, we should do the actual name/uid substitution on all lines of the program

  size_t pos, len;
  std::string newline,  uid;

  unsigned n = workspaceObject.numExtensibleGroups();
  OptionalString line;

  // Loop on each line of the program
  for (unsigned i = 0; i < n; ++i) {
    line = workspaceObject.getExtensibleGroup(i).cast<WorkspaceExtensibleGroup>().getString(EnergyManagementSystem_ProgramExtensibleFields::ProgramLine);
    if (line) {
      newline = line.get();

      // Split line on whitespaces to get look for modelobject names
      // TODO: really this parser should match the E+ one, so probably split on operators, handle parenthesis, etc
      // TODO: Split on ' +-*/^=<>' then trim paranthesis, that should about do it
      // So I just replace every operator with a space, then split on space...
      // TODO: check escape sequences... I'm pretty sure all are correct except '<>' where I'm guessing
      std::vector<std::string> tokens = splitString(boost::regex_replace(newline, boost::regex("[\\+\\-\\*\\^/=<>]"), " "), ' ');

      for (std::string& token: tokens) {
        // TODO: trim parenthesis
        boost::replace_all(token, "(", "");
        boost::replace_all(token, ")", "");
        for (const model::ModelObject& mo: modelObjects) {
          // Check if program item is the name of a model object
          boost::optional<std::string> _name = mo.name();
          if ( _name && (_name.get() == token) ) {
            // replace model object's name with its handle
            pos = newline.find(token);
            len = token.length();
            uid = toString(mo.handle());
            newline.replace(pos, len, uid);
            // Now that we have done the replacement, no need to keep looping.
            // Plus, we should break out of the nested loop and go to the next "j"
            // Otherwise pos could become giberish if there's another object named the same
            // since it won't be able to find the already-replaced string
            break;
          }
        } // end loop on all modelObjects

      } // end loop on all results in line
      emsProgram.addLine(newline);
    } // end if(line)
  } // End loop on each line of the program

  return emsProgram;
}

} // energyplus

} // openstudio

