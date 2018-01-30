@echo off
setlocal ENABLEDELAYEDEXPANSION
set leninit=100
gcc simulation.c -o simulation
for /L %%A IN (1,1,2000) DO (
  simulation !leninit! %%A
  REM set /A leninit=!leninit!+1
)