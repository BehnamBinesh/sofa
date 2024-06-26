cmake_minimum_required(VERSION 3.22)
project(Sofa.Component.Constraint.Projective LANGUAGES CXX)

set(SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR "src/sofa/component/constraint/projective")

set(HEADER_FILES
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/config.h.in
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/AffineMovementConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/AffineMovementConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/AttachConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/AttachConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedPlaneConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedPlaneConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedRotationConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedRotationConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedTranslationConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedTranslationConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/HermiteSplineConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/HermiteSplineConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/LinearMovementConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/LinearMovementConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/LinearVelocityConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/LinearVelocityConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/OscillatorConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/OscillatorConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/ParabolicConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/ParabolicConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PartialFixedConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PartialFixedConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PartialLinearMovementConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PartialLinearMovementConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PatchTestMovementConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PatchTestMovementConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PositionBasedDynamicsConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PositionBasedDynamicsConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/SkeletalMotionConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/SkeletalMotionConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/ProjectToLineConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/ProjectToLineConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/ProjectToPlaneConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/ProjectToPlaneConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/ProjectToPointConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/ProjectToPointConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/ProjectDirectionConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/ProjectDirectionConstraint.inl

    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/AffineMovementProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/AffineMovementProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/AttachProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/AttachProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedPlaneProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedPlaneProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedRotationProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedRotationProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedTranslationProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedTranslationProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/HermiteSplineProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/HermiteSplineProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/LinearMovementProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/LinearMovementProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/LinearVelocityProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/LinearVelocityProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/OscillatorProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/OscillatorProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/ParabolicProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/ParabolicProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PartialFixedProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PartialFixedProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PartialLinearMovementProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PartialLinearMovementProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PatchTestMovementProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PatchTestMovementProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PositionBasedDynamicsProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PositionBasedDynamicsProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/SkeletalMotionProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/SkeletalMotionProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/LineProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/LineProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PlaneProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PlaneProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PointProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PointProjectiveConstraint.inl
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/DirectionProjectiveConstraint.h
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/DirectionProjectiveConstraint.inl

)

set(SOURCE_FILES
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/init.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/AffineMovementProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/AttachProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedPlaneProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedRotationProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/FixedTranslationProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/HermiteSplineProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/LinearMovementProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/LinearVelocityProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/OscillatorProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/ParabolicProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PartialFixedProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PartialLinearMovementProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PatchTestMovementProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PositionBasedDynamicsProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/SkeletalMotionProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/LineProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PlaneProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/PointProjectiveConstraint.cpp
    ${SOFACOMPONENTCONSTRAINTPROJECTIVE_SOURCE_DIR}/DirectionProjectiveConstraint.cpp
)

sofa_find_package(Sofa.Simulation.Core REQUIRED)

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC Sofa.Simulation.Core)

sofa_create_package_with_targets(
    PACKAGE_NAME ${PROJECT_NAME}
    PACKAGE_VERSION ${Sofa_VERSION}
    TARGETS ${PROJECT_NAME} AUTO_SET_TARGET_PROPERTIES
    INCLUDE_SOURCE_DIR "src"
    INCLUDE_INSTALL_DIR "${PROJECT_NAME}"
)

# Tests
# If SOFA_BUILD_TESTS exists and is OFF, then these tests will be auto-disabled
cmake_dependent_option(SOFA_COMPONENT_CONSTRAINT_PROJECTIVE_BUILD_TESTS "Compile the automatic tests" ON "SOFA_BUILD_TESTS OR NOT DEFINED SOFA_BUILD_TESTS" OFF)
if(SOFA_COMPONENT_CONSTRAINT_PROJECTIVE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
