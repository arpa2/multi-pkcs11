/* mutextest.c
 *
 * This code tests a parent and child process sharing the same memory map,
 * and writing a word into it one at a time.  Only when the order is right
 * will the right standard sequence be produced.
 *
 * This version uses a pthreads mutex variable stored in shared memory
 * to achieve the fastest possible forced ordering between word additions.
 *
 * Linux implementations might use futex(), and that is indeed helpful in
 * situations where there is less contention than here; in such a case,
 * workers can always continue to work, as long as jobs keep pooring in.
 * Having looked at the implementation details of futexes, plus the fact
 * that it is super-specific to Linux, it appears that wrapping them with
 * pthreads is a good idea.
 *
 * real	0m0.003s
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
#include <assert.h>

#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/time.h>

#include <pthread.h>


void parent (char *map, pthread_mutex_t *mine, pthread_mutex_t *thine) {
	const char *aapmies [] = {
		"Aap", "Mies", "Zus", "Teun", "Gijs", "Kees", "weide", "hok", "schaaaaapen!", NULL
	};
	int i = 0;
	while (aapmies [i] != NULL) {
		assert (0 == pthread_mutex_lock (mine));
		strcat (map, aapmies [i++]);
		if (aapmies [i] != NULL) {
			strcat (map, ", ");
		}
		assert (0 == pthread_mutex_unlock (thine));
	}
}

void child (char *map, pthread_mutex_t *mine, pthread_mutex_t *thine) {
	const char *nootwim [] = {
		"noot", "Wim", "Jet", "vuur", "lam", "bok", "Does", "duif", NULL
	};
	int i = 0;
	while (nootwim [i] != NULL) {
		assert (0 == pthread_mutex_lock (mine));
		strcat (map, nootwim [i++]);
		strcat (map, ", ");
		assert (0 == pthread_mutex_unlock (thine));
	}
}



int main (int argc, char *argv []) {
	char *map = mmap (NULL, 1024, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	if (map == NULL) {
		perror ("Failed to mmap()");
		exit (1);
	}
	// Allocate mutexes in shared memory
	pthread_mutex_t *lockmap = (pthread_mutex_t *) map;
	pthread_mutex_t *parent_lock = &lockmap [0];
	pthread_mutex_t * child_lock = &lockmap [1];
	char *remap = (char *) &lockmap [2];
	pthread_mutexattr_t attr;
	pthread_mutexattr_init (&attr);
	pthread_mutexattr_setpshared (&attr, 1);
	assert (0 == pthread_mutex_init (parent_lock, &attr));
	assert (0 == pthread_mutex_init ( child_lock, &attr));
	assert (0 == pthread_mutex_lock ( child_lock));  // Lock out initially
	*remap = '\0';
	printf ("Attacking the work load in full force... brace for impact!\n");
	switch (fork ()) {
	case -1:
		perror ("Failed to fork()");
		exit (1);
	case 0:
		/* Child */
		child (remap, child_lock, parent_lock);
		printf ("Child finishes\n");
		exit (0);
	default:
		/* Parent */
		parent (remap, parent_lock, child_lock);
		printf ("Parent awaits child exit\n");
		wait (NULL);
		printf ("Parent noted child exit and finishes\n");
		break;
	}
	printf ("%s\n", remap);
}
