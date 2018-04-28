cd db_manager
start db_manager.exe 1
cd ../
ping 127.0.0.1 -n 1 -w 1000 > nul

cd db_manager
start db_manager.exe 2
cd ../
ping 127.0.0.1 -n 1 -w 1000 > nul