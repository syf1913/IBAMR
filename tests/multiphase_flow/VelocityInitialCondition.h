// ---------------------------------------------------------------------
//
// Copyright (c) 2018 - 2023 by the IBAMR developers
// All rights reserved.
//
// This file is part of IBAMR.
//
// IBAMR is free software and is distributed under the 3-clause BSD
// license. The full text of the license can be found in the file
// COPYRIGHT at the top level directory of IBAMR.
//
// ---------------------------------------------------------------------

#ifndef included_IBAMR_multiphase_flow_VelocityInitialCondition
#define included_IBAMR_multiphase_flow_VelocityInitialCondition

/////////////////////////////// INCLUDES /////////////////////////////////////

// Config files

#include <SAMRAI_config.h>

// IBAMR INCLUDES
#include <ibamr/AdvDiffHierarchyIntegrator.h>

#include <ibamr/app_namespaces.h>

// SAMRAI INCLUDES
#include <HierarchyDataOpsManager.h>

// Application includes
#include "LSLocateCircularInterface.h"

/////////////////////////////// STATIC ///////////////////////////////////////
// Various options to setting side-centered initial velocities
#define SMOOTH_SC_U 1
#define DESJARDINS_SC_U 0

/////////////////////////////// CLASS DEFINITION /////////////////////////////

/*!
 * \brief Class VelocityInitialCondition provides forcing an initial condition for velocity
 * based on the initial level set information.
 */
class VelocityInitialCondition : public CartGridFunction
{
public:
    /*!
     * \brief Class constructor.
     */
    VelocityInitialCondition(const std::string& object_name,
                             const double num_interface_cells,
                             std::vector<double> inside_velocity,
                             std::vector<double> outside_velocity,
                             CircularInterface init_circle)
        : d_object_name(object_name),
          d_num_interface_cells(num_interface_cells),
          d_inside_velocity(inside_velocity),
          d_outside_velocity(outside_velocity),
          d_init_circle(init_circle)
    {
        // intentionally blank
        return;
    } // VelocityInitialCondition

    /*!
     * \brief Empty destructor.
     */
    ~VelocityInitialCondition()
    {
        // intentionally blank
        return;
    } // ~VelocityInitialCondition

    /*!
     * \name Methods to set patch data.
     */
    //\{

    /*!
     * \brief Indicates whether the concrete VelocityInitialCondition object is
     * time-dependent.
     */
    bool isTimeDependent() const
    {
        return true;
    } // isTimeDependent

    /*!
     * \brief Evaluate the function on the patch interiors on the specified
     * levels of the patch hierarchy.
     */
    void setDataOnPatchHierarchy(const int /*data_idx*/,
                                 SAMRAI::tbox::Pointer<Variable<NDIM> > /*var*/,
                                 SAMRAI::tbox::Pointer<PatchHierarchy<NDIM> > /*hierarchy*/,
                                 const double /*data_time*/,
                                 const bool /*initial_time*/,
                                 const int /*coarsest_ln_in*/,
                                 const int /*finest_ln_in*/)
    {
        // Intentionally left blank
        return;
    } // setDataOnPatchHierarchy

    /*!
     * \brief Evaluate the function on the patch interior.
     */
    void setDataOnPatch(const int data_idx,
                        SAMRAI::tbox::Pointer<Variable<NDIM> > /*var*/,
                        SAMRAI::tbox::Pointer<Patch<NDIM> > patch,
                        const double /*data_time*/,
                        const bool initial_time,
                        SAMRAI::tbox::Pointer<SAMRAI::hier::PatchLevel<NDIM> > /*patch_level*/)
    {
        // Set the initial velocity inside and outside the level set
        if (initial_time)
        {
            // Get the circle parameters
            const double& R = d_init_circle.R;
            const IBTK::Vector3d& X0 = d_init_circle.X0;

            // Initial velocity patch data
            Pointer<SideData<NDIM, double> > U_data = patch->getPatchData(data_idx);

            Pointer<CartesianPatchGeometry<NDIM> > patch_geom = patch->getPatchGeometry();
            const double* const patch_dx = patch_geom->getDx();
            double vol_cell = 1.0;
            for (int d = 0; d < NDIM; ++d) vol_cell *= patch_dx[d];
            double alpha = d_num_interface_cells * std::pow(vol_cell, 1.0 / static_cast<double>(NDIM));
            const Box<NDIM>& patch_box = patch->getBox();
            for (int axis = 0; axis < NDIM; ++axis)
            {
                for (Box<NDIM>::Iterator it(SideGeometry<NDIM>::toSideBox(patch_box, axis)); it; it++)
                {
                    SideIndex<NDIM> s_i(it(), axis, /*lower index*/ 0);
                    double h;
                    double u_inside = d_inside_velocity[axis];
                    double u_outside = d_outside_velocity[axis];

                    // Get the values of the distance function of adjacent cell centers
                    CellIndex<NDIM> c_l = s_i.toCell(0);
                    CellIndex<NDIM> c_u = s_i.toCell(1);

                    // Get physical coordinates
                    IBTK::Vector coord_lower = IBTK::Vector::Zero();
                    IBTK::Vector coord_upper = IBTK::Vector::Zero();
                    const double* patch_X_lower = patch_geom->getXLower();
                    const hier::Index<NDIM>& patch_lower_idx = patch_box.lower();
                    const double* const patch_dx = patch_geom->getDx();
                    for (int d = 0; d < NDIM; ++d)
                    {
                        coord_lower[d] =
                            patch_X_lower[d] + patch_dx[d] * (static_cast<double>(c_l(d) - patch_lower_idx(d)) + 0.5);
                        coord_upper[d] =
                            patch_X_lower[d] + patch_dx[d] * (static_cast<double>(c_u(d) - patch_lower_idx(d)) + 0.5);
                    }

                    const double phi_lower =
                        std::sqrt(std::pow((coord_lower[0] - X0(0)), 2.0) + std::pow((coord_lower[1] - X0(1)), 2.0)
#if (NDIM == 3)
                                  + std::pow((coord_lower[2] - X0(2)), 2.0)
#endif
                                      ) -
                        R;

                    const double phi_upper =
                        std::sqrt(std::pow((coord_upper[0] - X0(0)), 2.0) + std::pow((coord_upper[1] - X0(1)), 2.0)
#if (NDIM == 3)
                                  + std::pow((coord_upper[2] - X0(2)), 2.0)
#endif
                                      ) -
                        R;
#if (DESJARDINS_SC_U)
                    // Desjardins way to set side-centered density
                    if (phi_lower >= 0.0 && phi_upper >= 0.0)
                    {
                        h = 1.0;
                    }
                    else if (phi_lower < 0.0 && phi_upper < 0.0)
                    {
                        h = 0.0;
                    }
                    else
                    {
                        h = (std::max(phi_lower, 0.0) + std::max(phi_upper, 0.0)) /
                            (std::abs(phi_lower) + std::abs(phi_upper));
                    }
                    (*U_data)(s_i) = u_inside + (u_outside - u_inside) * h;
#endif
#if (SMOOTH_SC_U)
                    // Simple average of phi onto side centers and set rho_sc directly
                    const double phi = 0.5 * (phi_lower + phi_upper);

                    if (phi < -alpha)
                    {
                        h = 0.0;
                    }
                    else if (std::abs(phi) <= alpha)
                    {
                        h = 0.5 + 0.5 * phi / alpha + 1.0 / (2.0 * M_PI) * std::sin(M_PI * phi / alpha);
                    }
                    else
                    {
                        h = 1.0;
                    }

                    (*U_data)(s_i) = (u_outside - u_inside) * h + u_inside;
#endif
                }
            }
        }
        return;
    } // setDataOnPatch

    //\}

private:
    VelocityInitialCondition();

    VelocityInitialCondition(const VelocityInitialCondition& from);

    VelocityInitialCondition& operator=(const VelocityInitialCondition& that);

    /*!
     * Name of this object.
     */
    std::string d_object_name;

    /*!
     * Number of interface cells over which to smooth the material properties
     */
    double d_num_interface_cells;

    /*!
     * Velocities of the fluid level set
     */
    std::vector<double> d_inside_velocity;
    std::vector<double> d_outside_velocity;

    /*!
     * Initial level set information.
     */
    CircularInterface d_init_circle;
};

//////////////////////////////////////////////////////////////////////////////

#endif // #ifndef included_VelocityInitialCondition
