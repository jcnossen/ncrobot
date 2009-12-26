@echo off
echo cycle topology
ncrobot -log runsim.log -test 2 -topology 1 -cl -ps 10 -len 20 -maxticks 300 -threads 6 -mlreport t2top1_ps10_2.m
ncrobot -log runsim.log -test 2 -topology 1 -cl -ps 10 -len 20 -maxticks 300 -threads 6 -mlreport t2top1_ps10_3.m

ncrobot -log runsim.log -test 2 -topology 1 -cl -ps 20 -len 20 -maxticks 300 -threads 6 -mlreport t2top1_ps20_2.m
ncrobot -log runsim.log -test 2 -topology 1 -cl -ps 20 -len 20 -maxticks 300 -threads 6 -mlreport t2top1_ps20_3.m

ncrobot -log runsim.log -test 2 -topology 1 -cl -ps 30 -len 20 -maxticks 300 -threads 6 -mlreport t2top1_ps30_2.m
ncrobot -log runsim.log -test 2 -topology 1 -cl -ps 30 -len 20 -maxticks 300 -threads 6 -mlreport t2top1_ps30_3.m

ncrobot -log runsim.log -test 2 -topology 1 -cl -ps 50 -len 20 -maxticks 300 -threads 6 -mlreport t2top1_ps50_2.m
ncrobot -log runsim.log -test 2 -topology 1 -cl -ps 50 -len 20 -maxticks 300 -threads 6 -mlreport t2top1_ps50_3.m

ncrobot -log runsim.log -test 2 -topology 1 -cl -ps 70 -len 20 -maxticks 300 -threads 6 -mlreport t2top1_ps70_2.m
ncrobot -log runsim.log -test 2 -topology 1 -cl -ps 70 -len 20 -maxticks 300 -threads 6 -mlreport t2top1_ps70_3.m

echo Kn topology, PS 10

ncrobot -log runsim.log -test 2 -topology 2 -cl -ps 10 -len 20 -maxticks 300 -threads 6 -mlreport t2top2_ps10_2.m
ncrobot -log runsim.log -test 2 -topology 2 -cl -ps 10 -len 20 -maxticks 300 -threads 6 -mlreport t2top2_ps10_3.m

ncrobot -log runsim.log -test 2 -topology 2 -cl -ps 20 -len 20 -maxticks 300 -threads 6 -mlreport t2top2_ps20_2.m
ncrobot -log runsim.log -test 2 -topology 2 -cl -ps 20 -len 20 -maxticks 300 -threads 6 -mlreport t2top2_ps20_3.m

ncrobot -log runsim.log -test 2 -topology 2 -cl -ps 30 -len 20 -maxticks 300 -threads 6 -mlreport t2top2_ps30_2.m
ncrobot -log runsim.log -test 2 -topology 2 -cl -ps 30 -len 20 -maxticks 300 -threads 6 -mlreport t2top2_ps30_3.m

ncrobot -log runsim.log -test 2 -topology 2 -cl -ps 50 -len 20 -maxticks 300 -threads 6 -mlreport t2top2_ps50_2.m
ncrobot -log runsim.log -test 2 -topology 2 -cl -ps 50 -len 20 -maxticks 300 -threads 6 -mlreport t2top2_ps50_3.m

ncrobot -log runsim.log -test 2 -topology 2 -cl -ps 70 -len 20 -maxticks 300 -threads 6 -mlreport t2top2_ps70_2.m
ncrobot -log runsim.log -test 2 -topology 2 -cl -ps 70 -len 20 -maxticks 300 -threads 6 -mlreport t2top2_ps70_3.m
