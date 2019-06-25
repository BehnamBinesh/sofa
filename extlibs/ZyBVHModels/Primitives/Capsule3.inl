// Geometric Tools, LLC
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Capsule3.h"

using namespace BVHModels;

//----------------------------------------------------------------------------
template <typename Real>
Capsule3<Real>::Capsule3()
{
}

//----------------------------------------------------------------------------
template <typename Real>
Capsule3<Real>::~Capsule3()
{
}

//----------------------------------------------------------------------------
template <typename Real>
Capsule3<Real>::Capsule3(const Segment3<Real>& segment, Real radius)
    :
    Segment(segment),
    Radius(radius)
{
}

//----------------------------------------------------------------------------
template <typename Real>
bool Capsule3<Real>::IsIntersectionQuerySupported(const PrimitiveType &other)
{
    if (other == PT_CAPSULE3)
        return true;

    return false;
}

//----------------------------------------------------------------------------
template <typename Real>
bool Capsule3<Real>::Test(const Intersectable<Real, Vec<3, Real>>& other)
{
    if (!IsIntersectionQuerySupported(other.GetIntersectableType()))
        return false;

    const Capsule3<Real>* capsule = dynamic_cast<const Capsule3<Real>*>(&other);

    if (!capsule)
        return false;

    Segment3<Real> other_seg = capsule->Segment;

    Segment3Segment3DistanceResult dist_result;
    Real distance = this->Segment.GetDistance(other_seg, dist_result);
    Real rSum = this->Radius + capsule->Radius;

    return distance <= rSum;
}
