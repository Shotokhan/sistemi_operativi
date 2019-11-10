all: lettori_scrittori

lettori_scrittori: lettori_scrittori.o waitProc.o procedure.o
	gcc -g -o lettori_scrittori lettori_scrittori.o waitProc.o procedure.o
	rm -f *.o
	rm -f *.~

lettori_scrittori.o: lettori_scrittori.c waitProc.h header.h
	gcc -g -c lettori_scrittori.c

waitProc.o: waitProc.c waitProc.h
	gcc -g -c waitProc.c

procedure.o: procedure.c header.h
	gcc -g -c procedure.c

clean:
	rm -f *.o
	rm -f ./lettori_scrittori
	rm -f *.~
