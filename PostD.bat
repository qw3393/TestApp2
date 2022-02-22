echo copying the output to release output folder
copy .\Debug\TestApp2.exe ..\Exec
copy .\Debug\TestApp2.pdb ..\Exec
copy .\Debug\TestApp2.map ..\Exec

echo copying the output to NTG folder
copy .\Debug\TestApp2.exe C:\TestApp2\Bin\
copy .\Debug\TestApp2.pdb C:\TestApp2\Bin\