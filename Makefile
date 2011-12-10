CFILES= main.c list.c node.c listsort.c
finddups: $(CFILES)
	gcc -g -o finddups $(CFILES)
clean:
	rm finddups
