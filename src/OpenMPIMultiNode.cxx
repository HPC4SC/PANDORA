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

#include <OpenMPIMultiNode.hxx>

namespace Engine {

    OpenMPIMultiNode::OpenMPIMultiNode() 
    {
        _root = NULL;
    }

    OpenMPIMultiNode::~OpenMPIMultiNode()
    {
        destroyTree(_root);
    }

    void OpenMPIMultiNode::init(int argc, char *argv[]) 
    {
        int alreadyInitialized;
        MPI_Initialized(&alreadyInitialized);
        if (not alreadyInitialized) MPI_Init(&argc, &argv);

        MPI_Comm_size(MPI_COMM_WORLD, &_numberOfMPITasks);
        MPI_Comm_rank(MPI_COMM_WORLD, &_id);

        _updateKnowledgeInParallel = false;
        _executeActionsInParallel = false;
        omp_init_lock(&_ompLock);

        // _boundaries._origin = Point2D<int>( 0, 0 );
        // _boundaries._size = _world->getConfig( ).getSize( );
        _boundaries = Rectangle<int>(_world->getConfig().getSize());
    }

    void OpenMPIMultiNode::divideSpace()
    {
        //for (AgentsList::iterator it = _world->beginAgents(); it != _world->endAgents(); ++it) 
        // ITERATE OVER THE _boundaries SPACE, LOOKING FOR AGENTS
    }

    void OpenMPIMultiNode::sendSpaces()
    {

    }

    node* OpenMPIMultiNode::insertNode()
    {

    }

    void OpenMPIMultiNode::destroyTree(node* leaf)
    {
        if (leaf != NULL) 
        {
            destroyTree(leaf->left);
            destroyTree(leaf->right);
            delete leaf;
        }
    }

} // namespace Engine