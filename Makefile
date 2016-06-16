CC = gcc -Wall
PROGS = petit15term\
		petit15writer\
		petitPanCake

all: lpc21isp/lpc21isp $(PROGS)

GLOBAL_DEP  = lpc21isp/adprog.h lpc21isp/lpc21isp.h lpc21isp/lpcprog.h lpc21isp/lpcterm.h

lpc21isp/adprog.o: lpc21isp/adprog.c $(GLOBAL_DEP)
	$(CC) $(CDEBUG) $(CFLAGS) -c -o lpc21isp/adprog.o lpc21isp/adprog.c

lpc21isp/lpcprog.o: lpc21isp/lpcprog.c $(GLOBAL_DEP)
	$(CC) $(CDEBUG) $(CFLAGS) -c -o lpc21isp/lpcprog.o lpc21isp/lpcprog.c

lpc21isp/lpcterm.o: lpc21isp/lpcterm.c $(GLOBAL_DEP)
	$(CC) $(CDEBUG) $(CFLAGS) -c -o lpc21isp/lpcterm.o lpc21isp/lpcterm.c

lpc21isp/lpc21isp: lpc21isp/lpc21isp.c lpc21isp/adprog.o lpc21isp/lpcprog.o lpc21isp/lpcterm.o $(GLOBAL_DEP)
	$(CC) $(CDEBUG) $(CFLAGS) -o lpc21isp/lpc21isp lpc21isp/lpc21isp.c lpc21isp/adprog.o lpc21isp/lpcprog.o lpc21isp/lpcterm.o

clean:
	$(RM) lpc21isp/adprog.o lpc21isp/lpcprog.o lpc21isp/lpcterm.o lpc21isp/lpc21isp
	rm -f $(PROGS)
