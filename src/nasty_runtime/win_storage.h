#ifndef __NASTY_MPI_WIN_H_
#define __NASTY_MPI_WIN_H_

#include <mpi.h>
#include <collections/darray.h>
#include <nasty_ops/nasty_mpi_op.h>

#define NASTY_ID_LEN 6
#define NASTY_ID_MAX_INT 1000000

typedef struct {
  int disp_unit;
  DArray pending_operations;
} win_info_t;

extern int KEY_NASTY_ID;
extern int KEY_ORIGIN_RANK;

int win_storage_init(void);
void win_storage_finalize(void);

int nasty_win_init(MPI_Win win, MPI_Comm win_comm, int disp_unit);
int nasty_win_lock(MPI_Win win);
int nasty_win_unlock(MPI_Win win);

win_info_t nasty_win_get_info(MPI_Win win);

#endif
