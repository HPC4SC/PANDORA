# cmake_minimum_required(VERSION <specify CMake version here>)
project(cassandra)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(tests/unitTests)

add_executable(cassandra
        projects/neolithic.cas
        scripts/createMosaic.py
        scripts/createVideo.py
        tests/data/EDA/groupResults.csv
        tests/data/EDA/run_0000.csv
        tests/data/EDA/run_0001.csv
        tests/data/EDA/run_0002.csv
        tests/data/EDA/run_0003.csv
        tests/data/EDA/run_0004.csv
        tests/data/EDA/run_0005.csv
        tests/data/EDA/run_0006.csv
        tests/data/EDA/run_0007.csv
        tests/data/EDA/run_0008.csv
        tests/data/EDA/run_0009.csv
        tests/data/EDA/run_0010.csv
        tests/data/EDA/run_0011.csv
        tests/data/EDA/run_0012.csv
        tests/data/EDA/run_0013.csv
        tests/data/EDA/run_0014.csv
        tests/data/EDA/run_0015.csv
        tests/data/EDA/run_0016.csv
        tests/data/EDA/run_0017.csv
        tests/data/EDA/run_0018.csv
        tests/data/EDA/run_0019.csv
        tests/data/EDA/run_0020.csv
        tests/data/EDA/run_0021.csv
        tests/data/EDA/run_0022.csv
        tests/data/EDA/run_0023.csv
        tests/data/EDA/run_0024.csv
        tests/data/EDA/run_0025.csv
        tests/data/EDA/run_0026.csv
        tests/data/EDA/run_0027.csv
        tests/data/EDA/run_0028.csv
        tests/data/EDA/run_0029.csv
        tests/data/EDA/run_0030.csv
        tests/data/EDA/run_0031.csv
        tests/data/EDA/run_0032.csv
        tests/data/EDA/run_0033.csv
        tests/data/EDA/run_0034.csv
        tests/data/EDA/run_0035.csv
        tests/data/EDA/run_0036.csv
        tests/data/EDA/run_0037.csv
        tests/data/EDA/run_0038.csv
        tests/data/EDA/run_0039.csv
        tests/data/EDA/run_0040.csv
        tests/data/EDA/run_0041.csv
        tests/data/EDA/run_0042.csv
        tests/data/EDA/run_0043.csv
        tests/data/EDA/run_0044.csv
        tests/data/EDA/run_0045.csv
        tests/data/EDA/run_0046.csv
        tests/data/EDA/run_0047.csv
        tests/data/EDA/run_0048.csv
        tests/data/EDA/run_0049.csv
        tests/data/EDA/run_0050.csv
        tests/data/EDA/run_0051.csv
        tests/data/EDA/run_0052.csv
        tests/data/EDA/run_0053.csv
        tests/data/EDA/run_0054.csv
        tests/data/EDA/run_0055.csv
        tests/data/EDA/run_0056.csv
        tests/data/EDA/run_0057.csv
        tests/data/EDA/run_0058.csv
        tests/data/EDA/run_0059.csv
        tests/data/EDA/run_0060.csv
        tests/data/EDA/run_0061.csv
        tests/data/EDA/run_0062.csv
        tests/data/EDA/run_0063.csv
        tests/data/EDA/run_0064.csv
        tests/data/EDA/run_0065.csv
        tests/data/EDA/run_0066.csv
        tests/data/EDA/run_0067.csv
        tests/data/EDA/run_0068.csv
        tests/data/EDA/run_0069.csv
        tests/data/EDA/run_0070.csv
        tests/data/EDA/run_0071.csv
        tests/data/EDA/run_0072.csv
        tests/data/EDA/run_0073.csv
        tests/data/EDA/run_0074.csv
        tests/data/EDA/run_0075.csv
        tests/data/EDA/run_0076.csv
        tests/data/EDA/run_0077.csv
        tests/data/EDA/run_0078.csv
        tests/data/EDA/run_0079.csv
        tests/data/EDA/run_0080.csv
        tests/data/EDA/run_0081.csv
        tests/data/EDA/run_0082.csv
        tests/data/EDA/run_0083.csv
        tests/data/EDA/run_0084.csv
        tests/data/EDA/run_0085.csv
        tests/data/EDA/run_0086.csv
        tests/data/EDA/run_0087.csv
        tests/data/EDA/run_0088.csv
        tests/data/EDA/run_0089.csv
        tests/data/EDA/run_0090.csv
        tests/data/EDA/run_0091.csv
        tests/data/EDA/run_0092.csv
        tests/data/EDA/run_0093.csv
        tests/data/EDA/run_0094.csv
        tests/data/EDA/run_0095.csv
        tests/data/EDA/run_0096.csv
        tests/data/EDA/run_0097.csv
        tests/data/EDA/run_0098.csv
        tests/data/EDA/run_0099.csv
        tests/incrementalXRaster/agents-0.abm
        tests/incrementalXRaster/randomWalkers.h5
        tests/largeDEM/agents-0.abm
        tests/largeDEM/panic.h5
        tests/miniIncrementalXRaster/agents-0.abm
        tests/miniIncrementalXRaster/randomWalkers.h5
        tests/unitTests/CassandraTests.cxx
        tests/unitTests/CassandraTests.hxx
        tests/unitTests/unitTests.pro
        tests/README
        uis/AgentAnalysis.ui
        uis/AgentConfigurator.ui
        uis/ColorInterval.ui
        uis/HeatMapDialog.ui
        uis/Laboratory.ui
        uis/LoadingProgressBar.ui
        uis/RasterAnalysis.ui
        uis/RasterAnalysisSelection.ui
        uis/RasterConfigurator.ui
        uis/RunProcess.ui
        uis/Settings.ui
        uis/TimeSeriesDialog.ui
        uis/TraitAnalysisSelection.ui
        AgentAnalysis.cxx
        AgentAnalysis.hxx
        AgentConfiguration.cxx
        AgentConfiguration.hxx
        AgentConfigurator.cxx
        AgentConfigurator.hxx
        AgentTraitSelection.cxx
        AgentTraitSelection.hxx
        AgentTypeSelection.cxx
        AgentTypeSelection.hxx
        AnalysisControlThread.cxx
        AnalysisControlThread.hxx
        cassandra.pro
        cassandra.qrc
        cassandra_osx.pro
        ColorInterval.cxx
        ColorInterval.hxx
        ColorSelector.hxx
        COPYING
        DataPlot.cxx
        DataPlot.hxx
        DefaultColorSelector.cxx
        DefaultColorSelector.hxx
        Display2D.cxx
        Display2D.hxx
        Exception.hxx
        GenericStatistics.cxx
        GenericStatistics.hxx
        HeatMapDialog.cxx
        HeatMapDialog.hxx
        HeatMapModel.cxx
        HeatMapModel.hxx
        HeatMapView.cxx
        HeatMapView.hxx
        Laboratory.cxx
        Laboratory.hxx
        Loader3DS.cxx
        Loader3DS.hxx
        LoadingProgressBar.cxx
        LoadingProgressBar.hxx
        LoadSimulationThread.cxx
        LoadSimulationThread.hxx
        main.cxx
        MainWindow.cxx
        MainWindow.hxx
        Makefile
        Material.cxx
        Material.hxx
        MeanDataPlot.cxx
        MeanDataPlot.hxx
        Model3D.cxx
        Model3D.hxx
        MpiStubCode.cxx
        Object3D.cxx
        Object3D.hxx
        ProjectConfiguration.cxx
        ProjectConfiguration.hxx
        QuadTree.cxx
        QuadTree.hxx
        RasterAnalysis.cxx
        RasterAnalysis.hxx
        RasterAnalysisSelection.cxx
        RasterAnalysisSelection.hxx
        RasterConfiguration.cxx
        RasterConfiguration.hxx
        RasterConfigurator.cxx
        RasterConfigurator.hxx
        RasterSelection.cxx
        RasterSelection.hxx
        RunAnalysis.cxx
        RunAnalysis.hxx
        RunSimulations.cxx
        RunSimulations.hxx
        Settings.cxx
        Settings.hxx
        SimulationControlThread.cxx
        SimulationControlThread.hxx
        StepDataPlot.cxx
        StepDataPlot.hxx
        SumDataPlot.cxx
        SumDataPlot.hxx
        TimeSeriesDialog.cxx
        TimeSeriesDialog.hxx
        TimeSeriesModel.cxx
        TimeSeriesModel.hxx
        TimeSeriesView.cxx
        TimeSeriesView.hxx
        TraitAnalysisSelection.cxx
        TraitAnalysisSelection.hxx)
