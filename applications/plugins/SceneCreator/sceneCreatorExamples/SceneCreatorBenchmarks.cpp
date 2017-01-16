/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2016 INRIA, USTL, UJF, CNRS, MGH                    *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU General Public License as published by the Free  *
* Software Foundation; either version 2 of the License, or (at your option)   *
* any later version.                                                          *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
* more details.                                                               *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program; if not, write to the Free Software Foundation, Inc., 51  *
* Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.                   *
*******************************************************************************
*                            SOFA :: Applications                             *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/

#include <SceneCreator/SceneCreator.h>
#include <sofa/helper/ArgumentParser.h>

#include <sofa/gui/GUIManager.h>
#include <sofa/gui/Main.h>

#include <SofaSimulationTree/init.h>
#include <SofaSimulationTree/TreeSimulation.h>

#include <SofaComponentCommon/initComponentCommon.h>
#include <SofaComponentBase/initComponentBase.h>

#include <sofa/component/typedef/Sofa_typedef.h>

void cubeExample(sofa::simulation::Node::SPtr root)
{
    //Add objects
    for (unsigned int i=0; i<10; ++i)
        sofa::modeling::addCube(root, "cubeFEM_"+ std::to_string(i), sofa::defaulttype::Vec3Types::Deriv(5, 5, 5),
                                10, 1000, 0.45,
                                sofa::defaulttype::Vec3Types::Deriv(0, (i+1)*5, 0));

    // Add floor
    sofa::modeling::addFloor(root, "Floor", sofa::defaulttype::Vec3Types::Deriv(50, 1, 50),
                             sofa::defaulttype::Vec3Types::Deriv(0, 0, 0), sofa::defaulttype::Vec3Types::Deriv(0, 0, 0), sofa::defaulttype::Vec3Types::Deriv(40, 0, 40));
}

int main(int argc, char** argv)
{
    sofa::simulation::tree::init();
    sofa::component::initComponentBase();
    sofa::component::initComponentCommon();

    unsigned int idExample = 0;
    sofa::helper::parse("This is a SOFA application. Here are the command line arguments")
            .option(&idExample,'e',"example","Example Number to enter from (0 - 9)")
    (argc,argv);

    // init GUI
    sofa::gui::initMain();
    sofa::gui::GUIManager::Init(argv[0]);

    // Create simulation tree
    sofa::simulation::setSimulation(new sofa::simulation::tree::TreeSimulation());


    // Create the graph root node with collision
    sofa::simulation::Node::SPtr root = sofa::modeling::createRootWithCollisionPipeline();
    root->setGravity( Coord3(0,-10.0,0) );


    // Create scene example (depends on user input)
    switch (idExample)
    {
    case 0:
        cubeExample(root);
        break;
    default:
        cubeExample(root);
        break;
    }


    root->setAnimate(false);

    sofa::simulation::getSimulation()->init(root.get());

    //=======================================
    // Run the main loop
    sofa::gui::GUIManager::MainLoop(root);

    sofa::simulation::tree::cleanup();

    return 0;
}
