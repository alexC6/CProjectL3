# Specific part of the Makefile
EXECS = Serveur
EXECC = Client
EXECT = TestUnitaire

ARGS =
# Begin generic part of the Makefile
CC = gcc
CFLAGS = -Wall -g
LDFLAGS =



# Files
SRC_SERVEUR = $(wildcard $(SRC_DIR)/$(SRV_DIR)/*.c)
OBJ_SERVEUR = $(patsubst $(SRC_DIR)/$(SRV_DIR)/%.c, $(OBJ_DIR)/$(SRV_DIR)/%.o, $(SRC_SERVEUR))
HEAD_SERVEUR = $(wildcard $(INC_DIR)/$(SRV_DIR)/*.h)

SRC_CLIENT = $(wildcard $(SRC_DIR)/$(CLT_DIR)/*.c)
OBJ_CLIENT = $(patsubst $(SRC_DIR)/$(CLT_DIR)/%.c, $(OBJ_DIR)/$(CLT_DIR)/%.o, $(SRC_CLIENT))
HEAD_CLIENT = $(wildcard $(INC_DIR)/$(CLT_DIR)/*.h)

SRC_GLOBAL = $(wildcard $(SRC_DIR)/$(GLB_DIR)/*.c)
OBJ_GLOBAL = $(patsubst $(SRC_DIR)/$(GLB_DIR)/%.c, $(OBJ_DIR)/$(GLB_DIR)/%.o, $(SRC_GLOBAL))
HEAD_GLOBAL = $(wildcard $(INC_DIR)/$(GLB_DIR)/*.h)

SRC_TEST = $(wildcard $(TST_DIR)/$(SRC_DIR)/*.c)
OBJ_TEST = $(patsubst $(TST_DIR)/$(SRC_DIR)/%.c, $(TST_DIR)/$(OBJ_DIR)/%.o, $(SRC_TEST))
HEAD_TEST = $(wildcard $(INC_DIR)/*.h)

#folders
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj
INC_DIR = inc
SRV_DIR = serveur
CLT_DIR = client
GLB_DIR = global
TST_DIR = tests
FIL_DIR = files
ANU_DIR = annuaires


all: $(BIN_DIR)/$(EXECS) $(BIN_DIR)/$(EXECC)

#complining execs

$(BIN_DIR)/$(EXECC): $(OBJ_GLOBAL) $(OBJ_CLIENT) $(OBJ_DIR)/client.o
	@echo "CLIENT : Création de l'executabe " $@
	@$(CC) -o $@ $^ $(LDFLAGS)

$(BIN_DIR)/$(EXECS): $(OBJ_GLOBAL) $(OBJ_SERVEUR) $(OBJ_DIR)/serveur.o
	@echo "SERVEUR : Création de l'executabe " $@
	@$(CC) -o $@ $^ $(LDFLAGS)

$(TST_DIR)/$(BIN_DIR)/$(EXECT):  $(OBJ_TEST) $(OBJ_GLOBAL) 
	@echo "TESTS : Création de l'executable " $@
	@$(CC) -o $@ $^ $(LDFLAGS)

#run commands
run: runServeur runClient

runServeur:
	./$(BIN_DIR)/$(EXECS) $(ARGS)

runClient:
	./$(BIN_DIR)/$(EXECC) $(ARGS)

vServeur:
	valgrind ./$(BIN_DIR)/$(EXECS) $(ARGS)

vClient:
	valgrind ./$(BIN_DIR)/$(EXECC) $(ARGS)

vlServeur:
	valgrind --leak-check=full --show-leak-kinds=all ./$(BIN_DIR)/$(EXECS) $(ARGS)

vlClient:
	valgrind --leak-check=full --show-leak-kinds=all ./$(BIN_DIR)/$(EXECC) $(ARGS)

test: $(TST_DIR)/$(BIN_DIR)/$(EXECT)
	./$(TST_DIR)/$(BIN_DIR)/$(EXECT)

reset:
	@echo "Suppression de données du serveur"
	@touch $(FIL_DIR)/tempSuppressionFichier
	@rm -r $(FIL_DIR)/*
	@mkdir $(FIL_DIR)/$(ANU_DIR)

#Compiling
$(OBJ_DIR)/$(SRV_DIR)/%.o: $(SRC_DIR)/$(SRV_DIR)/%.c
	@echo "SERVEUR : Création de "$@
	@$(CC) -o $@ -c $< $(CFLAGS) $(LDFLAGS)

$(OBJ_DIR)/$(CLT_DIR)/%.o: $(SRC_DIR)/$(CLT_DIR)/%.c
	@echo "CLIENT : Création de "$@
	@$(CC) -o $@ -c $< $(CFLAGS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "MAIN : Création de "$@
	@$(CC) -o $@ -c $< $(CFLAGS) $(LDFLAGS)

$(OBJ_DIR)/$(GLB_DIR)/%.o: $(SRC_DIR)/$(GLB_DIR)/%.c
	@echo "GLOBAL : Création de "$@
	@$(CC) -o $@ -c $< $(CFLAGS) $(LDFLAGS)

$(TST_DIR)/$(OBJ_DIR)/%.o: $(TST_DIR)/$(SRC_DIR)/%.c
	@echo "TEST : Création de "$@
	@$(CC) -o $@ -c $< $(CFLAGS) $(LDFLAGS)


#Others Commands

.PHONY: clean mrproper doc

doc: #genère la documentation
	@echo "Creation de la documentation"
	@doxygen doc/doxyfile

clean: #enleve les fichiers intermédiaire (fichiers .o)
	@rm -f $(OBJ_DIR)/$(CLT_DIR)/*.o
	@rm -f $(OBJ_DIR)/$(SRV_DIR)/*.o
	@rm -f $(OBJ_DIR)/*.o
	@rm -f $(OBJ_DIR)/$(GLB_DIR)/*.o
	@rm -f $(TST_DIR)/$(OBJ_DIR)/*.o
	@echo "Fichiers intermédiaires supprimés"

mrproper: clean #netoie le projet des fihciers executables et des fichiers intermédiaires (fichier .o et executable)
	@rm -f $(BIN_DIR)/$(EXECS)
	@rm -f $(BIN_DIR)/$(EXECC)
	@rm -f $(TST_DIR)/$(BIN_DIR)/$(EXECT)
	@echo "Executables supprimé"


print-%  : ; @echo $* = $($*)

# End generic part of the makefile
