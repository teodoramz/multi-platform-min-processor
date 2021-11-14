CC=cl
L=link
CFLAGS = /MD /W3 /nologo /EHsc /Za /MD /D_CRT_SECURE_NO_DEPRECATE

build: so-cpp.exe

so-cpp.exe: source.obj hashMap.obj listNode.obj
	$(L) /nologo /out:$@ $**	

source.obj: source.c hashMap.h
	$(CC) $(CFLAGS) /c source.c

hashMap.obj: hashMap.c hashMap.h
	$(CC) $(CFLAGS) /c hashMap.c

listNode.obj: listNode.c listNode.h
	$(CC) $(CFLAGS) /c listNode.c

clean: 
	rm *.obj so-cpp.exe