/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_COLLISION_BARYCENTRICCONTACTMAPPER_H
#define SOFA_COMPONENT_COLLISION_BARYCENTRICCONTACTMAPPER_H
#include "config.h"

#include <sofa/helper/Factory.h>
#include <SofaBaseMechanics/BarycentricMapping.h>
#include <SofaBaseMechanics/IdentityMapping.h>
#include <SofaRigid/RigidMapping.h>
#include <SofaBaseMechanics/SubsetMapping.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include <sofa/simulation/Node.h>
#include <sofa/simulation/Simulation.h>
#include <SofaBaseCollision/BaseContactMapper.h>
#include <SofaBaseCollision/CapsuleModel.h>
#include <SofaBaseCollision/SphereModel.h>
#include <SofaMeshCollision/TriangleModel.h>
#include <SofaMeshCollision/LineModel.h>
#include <SofaMeshCollision/PointModel.h>
#include <SofaBaseMechanics/IdentityMapping.h>
#include <iostream>

namespace sofa
{

namespace component
{

namespace collision
{

/// Base class for all mappers using BarycentricMapping
template < class TCollisionModel, class DataTypes >
class BarycentricContactMapper : public BaseContactMapper<DataTypes>
{
public:
    typedef typename DataTypes::Real Real;
    typedef typename DataTypes::Coord Coord;
    typedef TCollisionModel MCollisionModel;
    typedef typename MCollisionModel::InDataTypes InDataTypes;
    typedef typename MCollisionModel::Topology InTopology;
    typedef core::behavior::MechanicalState< InDataTypes> InMechanicalState;
    typedef core::behavior::MechanicalState<  typename BarycentricContactMapper::DataTypes> MMechanicalState;
    typedef component::container::MechanicalObject<typename BarycentricContactMapper::DataTypes> MMechanicalObject;
    typedef mapping::BarycentricMapping< InDataTypes, typename BarycentricContactMapper::DataTypes > MMapping;
    typedef mapping::TopologyBarycentricMapper<InDataTypes, typename BarycentricContactMapper::DataTypes> MMapper;
    MCollisionModel* model;
    typename MMapping::SPtr mapping;
    typename MMapper::SPtr mapper;

    using index_type = sofa::defaulttype::index_type;

    BarycentricContactMapper()
        : model(nullptr), mapping(nullptr), mapper(nullptr)
    {
    }

    void setCollisionModel(MCollisionModel* model)
    {
        this->model = model;
    }

    void cleanup();

    MMechanicalState* createMapping(const char* name="contactPoints");

    void resize(std::size_t size)
    {
        if (mapping != nullptr)
        {
            mapper->clear();
            mapping->getMechTo()[0]->resize(size);
        }
    }

    void update()
    {
        if (mapping != nullptr)
        {
            core::BaseMapping* map = mapping.get();
            map->apply(core::MechanicalParams::defaultInstance(), core::VecCoordId::position(), core::ConstVecCoordId::position());
            map->applyJ(core::MechanicalParams::defaultInstance(), core::VecDerivId::velocity(), core::ConstVecDerivId::velocity());
        }
    }

    void updateXfree()
    {
        if (mapping != nullptr)
        {
            core::BaseMapping* map = mapping.get();
            map->apply(core::MechanicalParams::defaultInstance(), core::VecCoordId::freePosition(), core::ConstVecCoordId::freePosition());
            map->applyJ(core::MechanicalParams::defaultInstance(), core::VecDerivId::freeVelocity(), core::ConstVecDerivId::freeVelocity());
        }
    }

    void updateX0()
    {
        if (mapping != nullptr)
        {
            core::BaseMapping* map = mapping.get();
            map->apply(core::MechanicalParams::defaultInstance(), core::VecCoordId::restPosition(), core::ConstVecCoordId::restPosition());
        }
    }
};

/// Mapper for LineModel
template<class DataTypes>
class ContactMapper<LineCollisionModel<sofa::defaulttype::Vec3Types>, DataTypes> : public BarycentricContactMapper<LineCollisionModel<sofa::defaulttype::Vec3Types>, DataTypes>
{
public:
    typedef typename DataTypes::Real Real;
    typedef typename DataTypes::Coord Coord;
    using index_type = sofa::defaulttype::index_type;

    index_type addPoint(const Coord& P, index_type index, Real&)
    {
        return this->mapper->createPointInLine(P, this->model->getElemEdgeIndex(index), &this->model->getMechanicalState()->read(core::ConstVecCoordId::position())->getValue());
    }
    index_type addPointB(const Coord& /*P*/, index_type index, Real& /*r*/, const defaulttype::Vector3& baryP)
    {
        return this->mapper->addPointInLine(this->model->getElemEdgeIndex(index), baryP.ptr());
    }

    inline index_type addPointB(const Coord& P, index_type index, Real& r ){return addPoint(P,index,r);}
};

/// Mapper for TriangleModel
template<class DataTypes>
class ContactMapper<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, DataTypes> : public BarycentricContactMapper<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, DataTypes>
{
public:
    typedef typename DataTypes::Real Real;
    typedef typename DataTypes::Coord Coord;
    using index_type = sofa::defaulttype::index_type;

    index_type addPoint(const Coord& P, index_type index, Real&)
    {
        std::size_t nbt = this->model->getCollisionTopology()->getNbTriangles();
        if (index < nbt)
            return this->mapper->createPointInTriangle(P, index, &this->model->getMechanicalState()->read(core::ConstVecCoordId::position())->getValue());
        else
        {
            index_type qindex = (index - nbt)/2;
            std::size_t nbq = this->model->getCollisionTopology()->getNbQuads();
            if (qindex < nbq)
                return this->mapper->createPointInQuad(P, qindex, &this->model->getMechanicalState()->read(core::ConstVecCoordId::position())->getValue());
            else
            {
                msg_error("ContactMapper<TriangleCollisionModel<sofa::defaulttype::Vec3Types>>") << "Invalid contact element index "<<index<<" on a topology with "<<nbt<<" triangles and "<<nbq<<" quads."<<msgendl
                                                              << "model="<<this->model->getName()<<" size="<<this->model->getSize() ;
                return sofa::defaulttype::InvalidID;
            }
        }
    }
    index_type addPointB(const Coord& P, index_type index, Real& /*r*/, const defaulttype::Vector3& baryP)
    {

        std::size_t nbt = this->model->getCollisionTopology()->getNbTriangles();
        if (index < nbt)
            return this->mapper->addPointInTriangle(index, baryP.ptr());
        else
        {
            // TODO: barycentric coordinates usage for quads
            index_type qindex = (index - nbt)/2;
            std::size_t nbq = this->model->getCollisionTopology()->getNbQuads();
            if (qindex < nbq)
                return this->mapper->createPointInQuad(P, qindex, &this->model->getMechanicalState()->read(core::ConstVecCoordId::position())->getValue());
            else
            {
                msg_error("ContactMapper<TriangleCollisionModel<sofa::defaulttype::Vec3Types>>") << "Invalid contact element index "<<index<<" on a topology with "<<nbt<<" triangles and "<<nbq<<" quads."<<msgendl
                            << "model="<<this->model->getName()<<" size="<<this->model->getSize() ;
                return sofa::defaulttype::InvalidID;
            }
        }
    }

    inline index_type addPointB(const Coord& P, index_type index, Real& r ){return addPoint(P,index,r);}

};


template <class DataTypes>
class ContactMapper<CapsuleCollisionModel<sofa::defaulttype::Vec3Types>, DataTypes> : public BarycentricContactMapper<CapsuleCollisionModel<sofa::defaulttype::Vec3Types>, DataTypes>{
    typedef typename DataTypes::Real Real;
    typedef typename DataTypes::Coord Coord;
    using index_type = sofa::defaulttype::index_type;

public:
    index_type addPoint(const Coord& P, index_type index, Real& r){
        r = this->model->radius(index);

        SReal baryCoords[1];
        const Coord & p0 = this->model->point1(index);
        const Coord pA = this->model->point2(index) - p0;
        Coord pos = P - p0;
        baryCoords[0] = ( ( pos*pA ) /pA.norm2() );

        if(baryCoords[0] > 1)
            baryCoords[0] = 1;
        else if(baryCoords[0] < 0)
            baryCoords[0] = 0;

        return this->mapper->addPointInLine ( index, baryCoords );
    }
};

#if !defined(SOFA_COMPONENT_COLLISION_BARYCENTRICCONTACTMAPPER_CPP)
extern template class SOFA_MESH_COLLISION_API ContactMapper<LineCollisionModel<sofa::defaulttype::Vec3Types>, sofa::defaulttype::Vec3Types>;
extern template class SOFA_MESH_COLLISION_API ContactMapper<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, sofa::defaulttype::Vec3Types>;
extern template class SOFA_MESH_COLLISION_API ContactMapper<CapsuleCollisionModel<sofa::defaulttype::Vec3Types>, sofa::defaulttype::Vec3Types>;

#  ifdef _MSC_VER
// Manual declaration of non-specialized members, to avoid warnings from MSVC.
extern template SOFA_MESH_COLLISION_API void BarycentricContactMapper<LineCollisionModel<sofa::defaulttype::Vec3Types>, defaulttype::Vec3Types>::cleanup();
extern template SOFA_MESH_COLLISION_API core::behavior::MechanicalState<defaulttype::Vec3Types>* BarycentricContactMapper<LineCollisionModel<sofa::defaulttype::Vec3Types>, defaulttype::Vec3Types>::createMapping(const char*);
extern template SOFA_MESH_COLLISION_API void BarycentricContactMapper<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, defaulttype::Vec3Types>::cleanup();
extern template SOFA_MESH_COLLISION_API core::behavior::MechanicalState<defaulttype::Vec3Types>* BarycentricContactMapper<TriangleCollisionModel<sofa::defaulttype::Vec3Types>, defaulttype::Vec3Types>::createMapping(const char*);
extern template SOFA_MESH_COLLISION_API void BarycentricContactMapper<CapsuleCollisionModel<sofa::defaulttype::Vec3Types>, defaulttype::Vec3Types>::cleanup();
extern template SOFA_MESH_COLLISION_API core::behavior::MechanicalState<defaulttype::Vec3Types>* BarycentricContactMapper<CapsuleCollisionModel<sofa::defaulttype::Vec3Types>, defaulttype::Vec3Types>::createMapping(const char*);
#  endif // _MSC_VER
#endif

} // namespace collision

} // namespace component

} // namespace sofa

#endif
