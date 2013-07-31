CC=gcc-4.4
CFLAGS = -DARCH_64 -ggdb `xml2-config --cflags` `agar-config --cflags`  -I/usr/include -DLINUX

LOCALPATH=/usr/local
LFLAGS = -lopenjpeg -lglut  -lSDL -lSDL_net -lSDL_Clipboard -lm -lGL -lGLU `xml2-config --libs` `agar-config --libs` -L${LOCALPATH}/lib -L${LOCALPATH}/lib64

knossos: y.tab.o lex.yy.o hash.o loader.o knossos.o coordinate.o viewer.o remote.o renderer.o skeletonizer.o eventHandler.o client.o customStyle.o gui.o treeLUT_fallback.o refreshTime.o sha256.o ftp.o ftplib.o qsort.o
	$(CC) $(LFLAGS) refreshTime.o hash.o loader.o knossos.o coordinate.o viewer.o remote.o lex.yy.o y.tab.o renderer.o skeletonizer.o eventHandler.o client.o gui.o customStyle.o treeLUT_fallback.o sha256.o ftplib/ftplib.o qsort.o ftp.o -o $@

viewer.o: viewer.c
	$(CC) $(CFLAGS) -Wall -c $< -o $@

refreshTime.o: refreshTime.c
	$(CC) $(CFLAGS) -Wall -c $< -o $@

sha256.o: sha256.c
	$(CC) $(CFLAGS) -Wall -c $< -o $@

hash.o: hash.c
	$(CC) $(CFLAGS) -Wall -c $< -o $@

y.tab.c: config.y
	yacc -d $<

lex.yy.c: config.lex
	flex  $<

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c $< -o $@

y.tab.o: y.tab.c
	$(CC) $(CFLAGS) -c $< -o $@

coordinate.o: coordinate.c
	$(CC) $(CFLAGS) -Wall -c $< -o $@

treeLUT_fallback.o: treeLUT_fallback.c
	$(CC) $(CFLAGS) -Wall -c $< -o $@

loader.o: loader.c
	$(CC) $(CFLAGS) -Wall -c $< -o $@

remote.o: remote.c
	$(CC) $(CFLAGS) -Wall -c $< -o $@

client.o: client.c
	$(CC) $(CFLAGS) -Wall -c $< -o $@

knossos.o: knossos.c
	$(CC) $(CFLAGS) -Wall -c $< -o $@

renderer.o: renderer.c
	$(CC) $(CFLAGS) -Wall -c $< -o $@

skeletonizer.o: skeletonizer.c
	$(CC) $(CFLAGS) -Wall -c $< -o $@

gui.o: gui.c
	$(CC) $(CFLAGS) -Wall -c $< -o $@

eventHandler.o: eventHandler.c
	$(CC) $(CFLAGS) -Wall -c $< -o $@

customStyle.o: customStyle.c
	$(CC) $(CFLAGS) -Wall -c $< -o $@

ftp.o: ftp.c
	$(CC) $(CFLAGS) -Wall -c $< -o $@

ftplib.o: ftplib/ftplib.c
	$(CC) $(CFLAGS) -Wall -c $< -o ftplib/$@

qsort.o: qsort.c
	$(CC) $(CFLAGS) -Wall -c $< -o $@

.PHONY: clean
clean:
	rm -f knossos hash.o loader.o knossos.o coordinate.o viewer.o remote.o y.tab.o y.tab.c y.tab.h lex.yy.o lex.yy.c lex.yy.h renderer.o skeletonizer.o eventHandler.o client.o gui.o customStyle.o refreshTime.o ftp.o ftplib/ftplib.o qsort.o
