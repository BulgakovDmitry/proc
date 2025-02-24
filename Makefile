COMPILER = g++
FLAGS = -Wall -Wextra -I . -I ./myLib
O = obj/
PR = proc/
C = codeGenerator/
M = myLib/
L = language/


LANG_OBJ = $(O)$(M)vector.o $(O)tokenizer.o $(O)tree.o $(O)parser.o $(O)codeGenerator.o $(O)langMain.o $(O)$(M)myLib.o $(O)$(M)Stack.o
PROC_OBJ = $(O)proc.o $(O)$(M)Stack.o $(O)$(M)myLib.o
ASEM_OBJ = $(O)procAsm.o $(O)$(M)myLib.o

lang: lang.exe 
	./lang.exe 

proc: proc.exe      
	./proc.exe

asem: asem.exe
	./asem.exe

run: asem proc       

clean:
	rm -f $(O)*.o 

#-----------------------------------------------------------------------

asem.exe: $(ASEM_OBJ)
	$(COMPILER) $^ -o asem.exe

proc.exe: $(PROC_OBJ)
	$(COMPILER) $^ -o proc.exe

lang.exe: $(LANG_OBJ)
	$(COMPILER) $^ -o lang.exe

#-----------------------------------------------------------------------

$(O)%.o : $(PR)%.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@	

$(O)%.o : %.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@

$(O)%.o : $(L)%.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@

$(O)%.o : $(C)%.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@

#-----------------------------------------------------------------------
