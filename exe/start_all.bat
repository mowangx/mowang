cd db_manager
for /l %%i in (1,1,2) do (
	start db_manager.exe %%i
	ping 127.0.0.1 -n 1 -w 1000 > nul
)

cd ../game
for /l %%i in (1,1,3) do (
	start game.exe %%i
	ping 127.0.0.1 -n 1 -w 1000 > nul
)

cd ../gate
for /l %%i in (1,1,3) do (
	start gate.exe %%i
	ping 127.0.0.1 -n 1 -w 1000 > nul
)


cd ../http_client
for /l %%i in (1,1,1) do (
	start http_client.exe %%i
	ping 127.0.0.1 -n 1 -w 1000 > nul
)
