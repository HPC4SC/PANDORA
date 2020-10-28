/*
 * Copyright ( c ) 2014
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACI-N
 * http://www.bsc.es

 * This file is part of Pandora Library. This library is free software;
 * you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation;
 * either version 3.0 of the License, or ( at your option ) any later version.
 *
 * Pandora is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __MPIAutoAdjustment_hxx__
#define __MPIAutoAdjustment_hxx__

#include <MPIMultiNode.hxx>

namespace Engine
{

    class MPIMultiNode;

    class MPIAutoAdjustment
    {

        protected:

            const MPIMultiNode* _schedulerInstance;                 //! Instance of the scheduler containing all the variables needed for auto adjustment.

        public:

            /**
             * @brief Construct a new MPIAutoAdjustment object.
             * 
             */
            MPIAutoAdjustment();

            /**
             * @brief Destroy the MPIAutoAdjustment object.
             * 
             */
            virtual ~MPIAutoAdjustment();

            /**
             * @brief Initializes this instance.
             * 
             * @param schedulerInstance const MPIMultiNode&
             */
            void initAutoAdjustment(const MPIMultiNode& schedulerInstance);

            /**
             * @brief Rebalances the space if necessary.
             * 
             */
            void checkForRebalancingSpace();

    };

} // namespace Engine

#endif // __MPIAutoAdjustment_hxx__