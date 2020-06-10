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

#ifndef __OpenMPIMultiNode_hxx__
#define __OpenMPIMultiNode_hxx__

#include <mpi.h>
#include <World.hxx>
#include <Scheduler.hxx>

namespace Engine
{

    class OpenMPIMultiNode :  public Scheduler
    {
        protected:

            int _numberOfMPITasks;

            template <typename T>
            struct node {
                T value;
                bool isLeaf;
                node* left;
                node* right;
            };

            node* _root;
            
            /**
             * @brief It inserts a new node 
             * 
             * @return node* 
             */
            node* insertNode();

            /**
             * @brief It destroys the whole tree starting at 'leaf'.
             * 
             * @param leaf node*
             */
            void destroyTree(node* leaf);

        public:

            /**
             * @brief Construct a new OpenMPIMultiNode object
             * 
             */
            OpenMPIMultiNode();

            /**
             * @brief Destroy the OpenMPIMultiNode object
             * 
             */
            virtual ~OpenMPIMultiNode();

            /**
             * @brief Initializes everything needed before creation of agents and rasters. 
             * 
             * @param argc Not used.
             * @param argv Not used.
             */
            void init(int argc, char *argv[]);

            /**
             * @brief It creates the binary tree () representing the partitions of the world for each of the MPI tasks.
             * 
             */
            void divideSpace();

            /**
             * @brief It sends the created spaces to the rest of MPI processes.
             * 
             */
            void sendSpaces();

    };

} // namespace Engine

#endif // __SpacePartition_hxx__