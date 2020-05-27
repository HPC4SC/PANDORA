#!/bin/bash


# SugarScape_escalable
echo "Executing SugarScape_escalable experiments (set #1)..."
echo "1..." ; ./runExperiment.cmd 30 sugarScape_escalable 50-50-250-5-25-1-6-1-4-60-100 > Outputs/sugarScape_escalable-SET1/50-50-250-5-25-1-6-1-4-60-100.csv
echo "2..." ; ./runExperiment.cmd 30 sugarScape_escalable 100-100-8000-5-25-2-12-1-4-60-100 > Outputs/sugarScape_escalable-SET1/100-100-8000-5-25-2-12-1-4-60-100.csv
echo "3..." ; ./runExperiment.cmd 30 sugarScape_escalable 1000-1000-800000-5-25-20-120-1-4-60-100 > Outputs/sugarScape_escalable-SET1/1000-1000-800000-5-25-20-120-1-4-60-100.csv
echo "4..." ; ./runExperiment.cmd 30 sugarScape_escalable 50-50-250-50-250-1-6-1-4-60-100 > Outputs/sugarScape_escalable-SET1/50-50-250-50-250-1-6-1-4-60-100.csv
echo "5..." ; ./runExperiment.cmd 30 sugarScape_escalable 50-50-250-5-25-1-6-10-40-60-100 > Outputs/sugarScape_escalable-SET1/50-50-250-5-25-1-6-10-40-60-100.csv
echo "6..." ; ./runExperiment.cmd 30 sugarScape_escalable 1000-1000-800000-50-250-20-120-10-40-60-100 > Outputs/sugarScape_escalable-SET1/1000-1000-800000-50-250-20-120-10-40-60-100.csv
echo "SugarScape_escalable experiments (set #1) done."

echo "Executing SugarScape_escalable experiments (set #2)..."
echo "1..." ; ./runExperiment.cmd 30 sugarScape_escalable 50-50-250-5-25-1-6-1-4-60-100 > Outputs/sugarScape_escalable-SET2/50-50-250-5-25-1-6-1-4-60-100.csv
echo "2..." ; ./runExperiment.cmd 30 sugarScape_escalable 50-50-100-5-25-1-6-1-4-60-100 > Outputs/sugarScape_escalable-SET2/50-50-100-5-25-1-6-1-4-60-100.csv
echo "3..." ; ./runExperiment.cmd 30 sugarScape_escalable 50-50-500-5-25-1-6-1-4-60-100 > Outputs/sugarScape_escalable-SET2/50-50-500-5-25-1-6-1-4-60-100.csv
echo "4..." ; ./runExperiment.cmd 30 sugarScape_escalable 50-50-1000-5-25-1-6-1-4-60-100 > Outputs/sugarScape_escalable-SET2/50-50-1000-5-25-1-6-1-4-60-100.csv
echo "5..." ; ./runExperiment.cmd 30 sugarScape_escalable 50-50-1500-5-25-1-6-1-4-60-100 > Outputs/sugarScape_escalable-SET2/50-50-1500-5-25-1-6-1-4-60-100.csv
echo "6..." ; ./runExperiment.cmd 30 sugarScape_escalable 50-50-2000-5-25-1-6-1-4-60-100 > Outputs/sugarScape_escalable-SET2/50-50-2000-5-25-1-6-1-4-60-100.csv
echo "SugarScape_escalable experiments (set #2) done."

# Flocking (Boids)
echo "Executing Flocking experiments..."
echo "1..." ; ./runExperiment.cmd 30 flocking 300-1-3-1-5.00-3.00-1.50 > Outputs/flocking/300-1-3-1-5.00-3.00-1.50.csv
echo "2..." ; ./runExperiment.cmd 30 flocking 600-1-3-1-5.00-3.00-1.50 > Outputs/flocking/600-1-3-1-5.00-3.00-1.50.csv
echo "3..." ; ./runExperiment.cmd 30 flocking 1200-1-3-1-5.00-3.00-1.50 > Outputs/flocking/1200-1-3-1-5.00-3.00-1.50.csv
echo "4..." ; ./runExperiment.cmd 30 flocking 2400-1-3-1-5.00-3.00-1.50 > Outputs/flocking/2400-1-3-1-5.00-3.00-1.50.csv
echo "5..." ; ./runExperiment.cmd 30 flocking 4800-1-3-1-5.00-3.00-1.50 > Outputs/flocking/4800-1-3-1-5.00-3.00-1.50.csv
echo "6..." ; ./runExperiment.cmd 30 flocking 300-3-3-1-5.00-3.00-1.50 > Outputs/flocking/300-3-3-1-5.00-3.00-1.50.csv
echo "7..." ; ./runExperiment.cmd 30 flocking 300-1-9-1-5.00-3.00-1.50 > Outputs/flocking/300-1-9-1-5.00-3.00-1.50.csv
echo "8..." ; ./runExperiment.cmd 30 flocking 300-1-3-3-5.00-3.00-1.50 > Outputs/flocking/300-1-3-3-5.00-3.00-1.50.csv
echo "9..." ; ./runExperiment.cmd 30 flocking 4800-3-9-3-5.00-3.00-1.50 > Outputs/flocking/4800-3-9-3-5.00-3.00-1.50.csv
echo "Flocking experiments done."

# Segregation (Schelling)
echo "Executing Segregation (Schelling) experiments..."
echo "1..." ;  ./runExperiment.cmd 30 segregation 0.5-0.7-0.5-3-5 > Outputs/segregation/0.5-0.7-0.5-3-5.csv
echo "2..." ;  ./runExperiment.cmd 30 segregation 0.1-0.7-0.5-3-5 > Outputs/segregation/0.1-0.7-0.5-3-5.csv
echo "3..." ;  ./runExperiment.cmd 30 segregation 0.3-0.7-0.5-3-5 > Outputs/segregation/0.3-0.7-0.5-3-5.csv
echo "4..." ;  ./runExperiment.cmd 30 segregation 0.4-0.7-0.5-3-5 > Outputs/segregation/0.4-0.7-0.5-3-5.csv
echo "5..." ;  ./runExperiment.cmd 30 segregation 0.75-0.7-0.5-3-5 > Outputs/segregation/0.75-0.7-0.5-3-5.csv
echo "6..." ;  ./runExperiment.cmd 30 segregation 0.8-0.7-0.5-3-5 > Outputs/segregation/0.8-0.7-0.5-3-5.csv
echo "7..." ;  ./runExperiment.cmd 30 segregation 0.9-0.7-0.5-3-5 > Outputs/segregation/0.9-0.7-0.5-3-5.csv
echo "8..." ;  ./runExperiment.cmd 30 segregation 0.5-0.9-0.5-3-5 > Outputs/segregation/0.5-0.9-0.5-3-5.csv
echo "9..." ;  ./runExperiment.cmd 30 segregation 0.5-0.7-0.5-9-5 > Outputs/segregation/0.5-0.7-0.5-9-5.csv
echo "10..." ; ./runExperiment.cmd 30 segregation 0.5-0.7-0.5-3-15 > Outputs/segregation/0.5-0.7-0.5-3-15.csv
echo "11..." ; ./runExperiment.cmd 30 segregation 0.3-0.9-0.5-9-15 > Outputs/segregation/0.3-0.9-0.5-9-15.csv
echo "12..." ; ./runExperiment.cmd 30 segregation 0.75-0.9-0.5-9-15 > Outputs/segregation/0.75-0.9-0.5-9-15.csv
echo "Segregation (Schelling) experiments done."

# StupidModel
echo "Executing StupidModel experiments..."
echo "1..." ; ./runExperiment.cmd 30 stupidModel 300-10-1-0.7-1 > Outputs/stupidModel/300-10-1-0.7-1.csv
echo "2..." ; ./runExperiment.cmd 30 stupidModel 9900-10-1-0.7-1 > Outputs/stupidModel/9900-10-1-0.7-1.csv
echo "3..." ; ./runExperiment.cmd 30 stupidModel 300-30-1-0.7-1 > Outputs/stupidModel/300-30-1-0.7-1.csv
echo "4..." ; ./runExperiment.cmd 30 stupidModel 300-10-3-0.7-1 > Outputs/stupidModel/300-10-3-0.7-1.csv
echo "5..." ; ./runExperiment.cmd 30 stupidModel 300-10-1-0.7-3 > Outputs/stupidModel/300-10-1-0.7-3.csv
echo "6..." ; ./runExperiment.cmd 30 stupidModel 9900-30-3-0.7-3 > Outputs/stupidModel/9900-30-3-0.7-3.csv
echo "StupidModel experiments done."

chown -R 1000:1000 Outputs/*
