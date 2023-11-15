CC = gcc
CFLAGS = -ansi -Wall -pedantic
LDFLAGS = -lm

Assembler: assembler.o mtable.o utils.o macro.o firstpass.o label_table.o general_assembler_functions.o secondpass.o
	$(CC) $(CFLAGS) assembler.o mtable.o utils.o macro.o firstpass.o label_table.o general_assembler_functions.o secondpass.o -o Assembler $(LDFLAGS)
assembler.o: assembler.c mtable.h macro.h label_table.h firstpass.h secondpass.h
	$(CC) $(CFLAGS) -c assembler.c -o assembler.o
utils.o: utils.c utils.h 
	$(CC) $(CFLAGS) -c utils.c -o utils.o
mtable.o: mtable.c mtable.h globals.h utils.h
	$(CC) $(CFLAGS) -c mtable.c -o mtable.o
macro.o: macro.c macro.h globals.h utils.h mtable.h
	$(CC) $(CFLAGS) -c macro.c -o macro.o
firstpass.o: firstpass.c firstpass.h globals.h utils.h label_table.h general_assembler_functions.h
	$(CC) $(CFLAGS) -c firstpass.c -o firstpass.o
secondpass.o: secondpass.c secondpass.h globals.h utils.h label_table.h general_assembler_functions.h
	$(CC) $(CFLAGS) -c secondpass.c -o secondpass.o
general_assembler_functions.o: general_assembler_functions.c general_assembler_functions.h
	$(CC) $(CFLAGS) -c general_assembler_functions.c -o general_assembler_functions.o
label_table.o: label_table.c label_table.h
	$(CC) $(CFLAGS) -c label_table.c -o label_table.o
clean:
	rm -f *.o Assembler