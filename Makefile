.SILENT:

#--------------------------------------------------------------------------------------------------
COMPILER = g++
#--------------------------------------------------------------------------------------------------


#--------------------------------------------------------------------------------------------------
SRC  = src/
HPP  = headers/
STK  = stack/
LIB  = myLib/
PROC = proc/
ASM  = asm/
COM  = common/
OBJ  = obj/
#--------------------------------------------------------------------------------------------------


#--------------------------------------------------------------------------------------------------
INCLUDE_FLAGS = -I$(LIB) -I$(STK) -I$(HPP) 
SANITAZER     = -fsanitize=address
SFML_FLAGS    = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio 

FLAGS		  = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations \
			    -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wctor-dtor-privacy -Wempty-body -Wfloat-equal          \
			    -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd  \
				-Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel     \
				-Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override            \
				-Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros              \
				-Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector        \
				$(INCLUDE_FLAGS)                                                                                                             \
				$(SANITAZER)
#--------------------------------------------------------------------------------------------------


#--------------------------------------------------------------------------------------------------
STACK_OBJ = $(OBJ)stack.o $(OBJ)myLib.o
PROC_OBJ  = $(OBJ)proc.o  $(OBJ)myLib.o
ASM_OBJ   = $(OBJ)asm.o   $(OBJ)myLib.o
#--------------------------------------------------------------------------------------------------


#--------------------------------------------------------------------------------------------------
asm_: asm
	./asm.out

proc_: proc
	./proc.out

run: asm proc

run_: asm_ proc_
#--------------------------------------------------------------------------------------------------
     

#--------------------------------------------------------------------------------------------------
clean: 
	rm -f $(OBJ)*
#--------------------------------------------------------------------------------------------------


#--------------------------------------------------------------------------------------------------
proc: $(PROC_OBJ) $(STACK_OBJ)
	$(COMPILER) $^ -o proc.out $(FLAGS)
#--------------------------------------------------------------------------------------------------


#--------------------------------------------------------------------------------------------------
asm: $(ASM_OBJ)
	$(COMPILER) $^ -o asm.out $(FLAGS)
#--------------------------------------------------------------------------------------------------


#--------------------------------------------------------------------------------------------------
$(OBJ)%.o : $(ASM)$(SRC)%.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@

$(OBJ)%.o : $(PROC)$(SRC)%.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@

$(OBJ)%.o : $(LIB)%.cpp		
	$(COMPILER) $(FLAGS) -c $< -o $@

$(OBJ)%.o : $(STK)%.cpp
	$(COMPILER) $(FLAGS) -c $< -o $@
#--------------------------------------------------------------------------------------------------