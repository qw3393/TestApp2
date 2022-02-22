echo copying the output to release output folder
copy .\Release\TestApp2.exe ..\Exec
copy .\Release\TestApp2.pdb ..\Exec
copy .\Release\TestApp2.map ..\Exec

echo copying the output to NTG folder
copy .\Release\TestApp2.exe C:\TestApp2\Bin\
copy .\Release\TestApp2.pdb C:\TestApp2\Bin\