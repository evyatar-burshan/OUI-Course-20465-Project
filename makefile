assembler: main.o macro.o firsttrans.o textToBinary.o addressingModes.o errorTreatment.o operations.o secondtrans.o exportFiles.o
	gcc -ansi -pedantic -Wall main.o macro.o firsttrans.o textToBinary.o addressingModes.o errorTreatment.o operations.o secondtrans.o exportFiles.o -o assembler
main.o: main.c universal.h macro.h firsttrans.h textToBinary.h
	gcc -ansi -pedantic -Wall -c main.c
macro.o: macro.c macro.h errorTreatment.h universal.h
	gcc -ansi -pedantic -Wall -c macro.c
firsttrans.o: firsttrans.c firsttrans.h textToBinary.h operations.h addressingModes.h secondtrans.h errorTreatment.h
	gcc -ansi -pedantic -Wall -c  firsttrans.c	
textToBinary.o: textToBinary.c textToBinary.h universal.h
	gcc -ansi -pedantic -Wall -c textToBinary.c
addressingModes.o: addressingModes.c addressingModes.h textToBinary.h errorTreatment.h
	gcc -ansi -pedantic -Wall -c addressingModes.c
errorTreatment.o: errorTreatment.c errorTreatment.h universal.h
	gcc -ansi -pedantic -Wall -c errorTreatment.c
operations.o: operations.c operations.h universal.h
	gcc -ansi -pedantic -Wall -c operations.c
secondtrans.o: secondtrans.c secondtrans.h errorTreatment.h exportFiles.h textToBinary.h addressingModes.h universal.h
	gcc -ansi -pedantic -Wall -c secondtrans.c
exportFiles.o: exportFiles.c exportFiles.h
	gcc -ansi -pedantic -Wall -c exportFiles.c
