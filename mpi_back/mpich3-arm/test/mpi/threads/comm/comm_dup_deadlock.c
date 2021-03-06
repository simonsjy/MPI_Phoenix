/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *  (C) 2012 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include <unistd.h>
#include <stdio.h>
#include <mpi.h>
#include "mpitest.h"
#include "mpithreadtest.h"

#define NUM_THREADS 2
#define NUM_ITER    1

#define check(X_)       \
    do {                \
        if (!(X_)) {    \
            printf("[%s:%d] -- Assertion failed: %s\n", __FILE__, __LINE__, #X_);\
            MPI_Abort(MPI_COMM_WORLD, 1); \
        }               \
    } while (0)

#ifdef HAVE_WINDOWS_H
#define sleep(a) Sleep(a*1000)
#endif

MPI_Comm               comms[NUM_THREADS];
MTEST_THREAD_LOCK_TYPE comm_lock;
int                    rank, size;
int                    verbose = 0;

MTEST_THREAD_RETURN_TYPE test_comm_dup(void *arg)
{
    int rank;
    int i;

    MPI_Comm_rank(comms[*(int*)arg], &rank);

    for (i = 0; i < NUM_ITER; i++) {
        MPI_Comm    comm, self_dup;

        if (*(int*)arg == rank) {
            sleep(1);
        }

        MTest_thread_lock(&comm_lock);
        if (verbose) printf("%d: Thread %d - Comm_dup %d start\n", rank, *(int*)arg, i);
        MPI_Comm_dup(MPI_COMM_SELF, &self_dup);
        if (verbose) printf("%d: Thread %d - Comm_dup %d finish\n", rank, *(int*)arg, i);
        MTest_thread_unlock(&comm_lock);

        MPI_Comm_free(&self_dup);

        if (verbose) printf("%d: Thread %d - comm_dup %d start\n", rank, *(int*)arg, i);
        MPI_Comm_dup(comms[*(int*)arg], &comm);
        MPI_Comm_free(&comm);
        if (verbose) printf("%d: Thread %d - comm_dup %d finish\n", rank, *(int*)arg, i);
    }

    if (verbose) printf("%d: Thread %d - Done.\n", rank, *(int*)arg);
    return (MTEST_THREAD_RETURN_TYPE)0;
}


int main(int argc, char **argv)
{
    int         thread_args[NUM_THREADS];
    int         i, err, provided;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    check(provided == MPI_THREAD_MULTIPLE);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MTest_thread_lock_create(&comm_lock);

    for (i = 0; i < NUM_THREADS; i++) {
        MPI_Comm_dup(MPI_COMM_WORLD, &comms[i]);
    }

    for (i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;
        MTest_Start_thread( test_comm_dup, (void *)&thread_args[i] );
    }

    MTest_Join_threads();

    for (i = 0; i < NUM_THREADS; i++) {
        MPI_Comm_free(&comms[i]);
    }

    if (rank == 0)
        printf(" No Errors\n");

    MPI_Finalize();

    return 0;
}
