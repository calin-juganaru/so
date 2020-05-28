CFLAGS=/nologo /W4 /EHsc /Za

main.exe: main.obj add.obj
	cl $(CFLAGS) /Femain.exe main.obj add.obj
main.obj: main.c
	cl $(CFLAGS) /Fomain.obj /c main.c
add.obj: add.c
	cl $(CFLAGS) /Foadd.obj /c add.c
clean:
	del .\*.obj .\*.exe

