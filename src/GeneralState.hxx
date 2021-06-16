#ifndef __GeneralState_hxx__
#define __GeneralState_hxx__

#include <LoggerBase.hxx>
#include <Statistics.hxx>
#include <RasterLoader.hxx>
#include <ShpLoader.hxx>

namespace Engine
{
    class GeneralState
    {
    private:
        static GeneralState   * _instance;      //! Instance of the GeneralState class.
        Logger                  _logger;        //! Instance of the Logger class.
        Logger                  _loggerCP;      //! Instance of the Logger class for checkpointing.
        Statistics              _statistics;    //! Instance of the Statistics class.
        RasterLoader            _rasterLoader;  //! Instance of the RasterLoader class.
        ShpLoader               _shpLoader;     //! Instance of the ShpLoader class.

    protected:
        GeneralState( );

    public:
        /**
         * @brief Creates the instance of GeneralState. Otherwise it returns a pointer to the instance.
         * 
         * @return GeneralState& 
         */
        static GeneralState & instance( );

        /**
         * @brief Destroy the GeneralState object.
         * 
         */
        virtual ~GeneralState( );

        /**
         * @brief Returns the Logger instance.
         * 
         * @return Logger& 
         */
        static Logger & logger( )
        {
            return instance( )._logger;
        }

        /**
         * @brief Returns the LoggerCP instance.
         * 
         * @return Logger& 
         */
        static Logger& loggerCP()
        {
            return instance()._loggerCP;
        }

        /**
         * @brief Returns the Statistics instance.
         * 
         * @return Statistics& 
         */
        static Statistics & statistics( )
        {
            return instance( )._statistics;
        }

        /**
         * @brief Returns the RasterLoader instance.
         * 
         * @return RasterLoader& 
         */
        static RasterLoader & rasterLoader( )
        {
            return instance( )._rasterLoader;
        }

        /**
         * @brief Returns the ShpLoader instance.
         * 
         * @return ShpLoader& 
         */
        static ShpLoader & shpLoader( )
        {
            return instance( )._shpLoader;
        }
    };
} // namespace Engine
#endif // __GeneralState_hxx__

