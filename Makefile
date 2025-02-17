Compiler = g++
B = build_proc_asm/

start: proc.exe

run: proc.exe
	./proc.exe

#-----------------------------------------------------------------------

proc.exe :  $(B)proc.o  $(B)procAsm.o  $(B)main.o  $(B)Stack.o
	$(Compiler)  $(B)proc.o  $(B)procAsm.o  $(B)main.o  $(B)Stack.o -o proc.exe

#-----------------------------------------------------------------------
		
$(B)proc.o : proc.cpp 
	$(Compiler) -c proc.cpp -o $(B)proc.o

$(B)procAsm.o : procAsm.cpp
	$(Compiler) -c procAsm.cpp -o $(B)procAsm.o

$(B)Stack.o : Stack.cpp
	$(Compiler) -c Stack.cpp -o $(B)Stack.o

$(B)main.o : main.cpp
	$(Compiler) -c main.cpp -o $(B)main.o

#-----------------------------------------------------------------------

clean :
	del build_proc_asm\*.o
