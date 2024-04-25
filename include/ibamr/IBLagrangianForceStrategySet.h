// ---------------------------------------------------------------------
//
// Copyright (c) 2007 - 2023 by the IBAMR developers
// All rights reserved.
//
// This file is part of IBAMR.
//
// IBAMR is free software and is distributed under the 3-clause BSD
// license. The full text of the license can be found in the file
// COPYRIGHT at the top level directory of IBAMR.
//
// ---------------------------------------------------------------------

/////////////////////////////// INCLUDE GUARD ////////////////////////////////

#ifndef included_IBAMR_IBLagrangianForceStrategySet
#define included_IBAMR_IBLagrangianForceStrategySet

/////////////////////////////// INCLUDES /////////////////////////////////////

#include <ibamr/config.h>

#include "ibamr/IBLagrangianForceStrategy.h"

#include "tbox/Pointer.h"

#include "petscmat.h"

#include <vector>

namespace IBTK
{
class LData;
class LDataManager;
} // namespace IBTK
namespace SAMRAI
{
namespace hier
{
template <int DIM>
class PatchHierarchy;
} // namespace hier
} // namespace SAMRAI

/////////////////////////////// CLASS DEFINITION /////////////////////////////

namespace IBAMR
{
/*!
 * \brief Class IBLagrangianForceStrategySet is a utility class that allows
 * multiple IBLagrangianForceStrategy objects to be employed by a single
 * IBHierarchyIntegrator.
 */
class IBLagrangianForceStrategySet : public IBLagrangianForceStrategy
{
public:
    /*!
     * \brief Constructor.
     */
    template <typename InputIterator>
    IBLagrangianForceStrategySet(InputIterator first, InputIterator last) : d_strategy_set(first, last)
    {
        // intentionally blank
        return;
    } // IBLagrangianForceStrategySet

    /*!
     * \brief Destructor.
     */
    ~IBLagrangianForceStrategySet() = default;

    /*!
     * \brief Set the current and new times for the present timestep.
     */
    void setTimeInterval(double current_time, double new_time) override;

    /*!
     * \brief Setup the data needed to compute the forces on the specified level
     * of the patch hierarchy.
     */
    void initializeLevelData(SAMRAI::tbox::Pointer<SAMRAI::hier::PatchHierarchy<NDIM> > hierarchy,
                             int level_number,
                             double init_data_time,
                             bool initial_time,
                             IBTK::LDataManager* l_data_manager) override;

    /*!
     * \brief Compute the force generated by the Lagrangian structure on the
     * specified level of the patch hierarchy.
     *
     * \note Nodal forces computed by this method are \em added to the force
     * vector.
     */
    void computeLagrangianForce(SAMRAI::tbox::Pointer<IBTK::LData> F_data,
                                SAMRAI::tbox::Pointer<IBTK::LData> X_data,
                                SAMRAI::tbox::Pointer<IBTK::LData> U_data,
                                SAMRAI::tbox::Pointer<SAMRAI::hier::PatchHierarchy<NDIM> > hierarchy,
                                int level_number,
                                double data_time,
                                IBTK::LDataManager* l_data_manager) override;

    /*!
     * \brief Compute the non-zero structure of the force Jacobian matrix.
     *
     * \note Elements indices must be global PETSc indices.
     */
    void
    computeLagrangianForceJacobianNonzeroStructure(std::vector<int>& d_nnz,
                                                   std::vector<int>& o_nnz,
                                                   SAMRAI::tbox::Pointer<SAMRAI::hier::PatchHierarchy<NDIM> > hierarchy,
                                                   int level_number,
                                                   IBTK::LDataManager* l_data_manager) override;

    /*!
     * \brief Compute the Jacobian of the force with respect to the present
     * structure configuration and velocity.
     *
     * \note The elements of the Jacobian should be "accumulated" in the
     * provided matrix J.
     */
    void computeLagrangianForceJacobian(Mat& J_mat,
                                        MatAssemblyType assembly_type,
                                        double X_coef,
                                        SAMRAI::tbox::Pointer<IBTK::LData> X_data,
                                        double U_coef,
                                        SAMRAI::tbox::Pointer<IBTK::LData> U_data,
                                        SAMRAI::tbox::Pointer<SAMRAI::hier::PatchHierarchy<NDIM> > hierarchy,
                                        int level_number,
                                        double data_time,
                                        IBTK::LDataManager* l_data_manager) override;

    /*!
     * \brief Compute the potential energy with respect to the present structure
     * configuration and velocity.
     */
    double computeLagrangianEnergy(SAMRAI::tbox::Pointer<IBTK::LData> X_data,
                                   SAMRAI::tbox::Pointer<IBTK::LData> U_data,
                                   SAMRAI::tbox::Pointer<SAMRAI::hier::PatchHierarchy<NDIM> > hierarchy,
                                   int level_number,
                                   double data_time,
                                   IBTK::LDataManager* l_data_manager) override;

private:
    /*!
     * \brief Default constructor.
     *
     * \note This constructor is not implemented and should not be used.
     */
    IBLagrangianForceStrategySet() = delete;

    /*!
     * \brief Copy constructor.
     *
     * \note This constructor is not implemented and should not be used.
     *
     * \param from The value to copy to this object.
     */
    IBLagrangianForceStrategySet(const IBLagrangianForceStrategySet& from) = delete;

    /*!
     * \brief Assignment operator.
     *
     * \note This operator is not implemented and should not be used.
     *
     * \param that The value to assign to this object.
     *
     * \return A reference to this object.
     */
    IBLagrangianForceStrategySet& operator=(const IBLagrangianForceStrategySet& that) = delete;

    /*!
     * \brief The set of IBLagrangianForceStrategy objects.
     */
    std::vector<SAMRAI::tbox::Pointer<IBLagrangianForceStrategy> > d_strategy_set;
};
} // namespace IBAMR

//////////////////////////////////////////////////////////////////////////////

#endif // #ifndef included_IBAMR_IBLagrangianForceStrategySet