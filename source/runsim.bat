@echo off

set initiallength=200
set percent= 100
setlocal ENABLEDELAYEDEXPANSION

gcc simulation.c -o simulation

cd "C:\Users\Erik\Desktop\Mattekurser\RoadsideParking\source\sim_data"
del *.out
cd "C:\Users\Erik\Desktop\Mattekurser\RoadsideParking\source\"

for /L %%A IN (1,1,1) DO (
  echo  -- Running:  simulation %%A %%A !initiallength! !percent!
  echo  .

  REM *** PARAMS ***
  REM simulation   sim_num   seed   street_len   %gooddrivers 



  REM simulation %%A %%A !leninit! %%A
  simulation %%A 25 !initiallength! !percent!
  REM simulation 17 17 !initiallength! !percent!
  
  REM simulation 17 17 550.0 57
  REM simulation 18 18 550.0 57
  
  REM simulation 19 19 550.0 57
  REM simulation 20 20 550.0 57
  REM simulation 21 21 550.0 57
  REM set /A percent = !percent!+1
  REM set /A initialLength = !initialLength!+2
  REM for /f  "delims=" %%x in  ('powershell %%A * 0.5') do set 
  REM set /A mod=%%A%%3
  REM if "!mod!" EQU "0" (set /A perc=!perc!+1)
  REM set /A leninit=!leninit!+1
)

gnuplot -persist "plotsim_aggr.gpl" 


