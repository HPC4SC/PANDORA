#include <GeneralState.hxx>

namespace Engine
{
    GeneralState& GeneralState::instance( )
    {
        static GeneralState instance;
        return instance;
    }

    GeneralState::GeneralState( )
    {
    }

    GeneralState::~GeneralState( )
    {
    }
} // namespace Engine

