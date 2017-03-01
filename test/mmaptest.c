/* mmaptest.c
 *
 * This code tests a parent and child process sharing the same memory map,
 * and writing a word into it one at a time.  Only when the order is right
 * will the right standard sequence be produced.
 *
 * This version does nothing to synchronise the processes; it merely has
 * a timing scheme involving a second of waiting between word additions.
 *
 * real	0m17.004s
 * user	0m0.000s
 * sys	0m0.000s
 *
 * From: Rick van Rein <rick@openfortress.nl>
 */


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include <sys/mman.h>
#include <sys/wait.h>


void parent (char *map) {
	const char *aapmies [] = {
		"Aap", "Mies", "Zus", "Teun", "Gijs", "Kees", "weide", "hok", "schaaaaapen!", NULL
	};
	int i = 0;
	while (aapmies [i] != NULL) {
		strcat (map, aapmies [i++]);
		if (aapmies [i] != NULL) {
			strcat (map, ", ");
			sleep (2);
		}
	}
}

void child (char *map) {
	const char *nootwim [] = {
		"noot", "Wim", "Jet", "vuur", "lam", "bok", "Does", "duif", NULL
	};
	int i = 0;
	sleep (1);
	while (nootwim [i] != NULL) {
		strcat (map, nootwim [i++]);
		strcat (map, ", ");
		sleep (2);
	}
}



int main (int argc, char *argv []) {

	char *map = mmap (NULL, 1024, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	if (map == NULL) {
		perror ("Failed to mmap()");
		exit (1);
	}
	*map = '\0';
	printf ("Crunching... please wait!\n");
	switch (fork ()) {
	case -1:
		perror ("Failed to fork()");
		exit (1);
	case 0:
		/* Child */
		child (map);
		printf ("Child finishes\n");
		exit (0);
	default:
		/* Parent */
		//CAUSE-SEGFAULT-TEST// munmap (map, 1024);
		parent (map);
		printf ("Parent awaits child exit\n");
		wait (NULL);
		printf ("Parent noted child exit and finishes\n");
		break;
	}
	printf ("%s\n", map);
}
