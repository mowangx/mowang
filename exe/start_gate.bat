cd gate
start gate.exe 1
cd ../
ping 127.0.0.1 -n 1 -w 1000 > nul

cd gate
start gate.exe 2
cd ../
ping 127.0.0.1 -n 1 -w 1000 > nul

cd gate
start gate.exe 3
cd ../
ping 127.0.0.1 -n 1 -w 1000 > nul