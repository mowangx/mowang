
cd gate
for /l %%i in (1,1,3) do (
	start gate.exe %%i
	ping 127.0.0.1 -n 1 -w 1000 > nul
)
