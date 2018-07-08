// Already defined variables in the original source code. Do not uncomment any of those, just use them directly!
// Process rank 0 should be the source of the broadcast
//
// int num_procs;
// int rank;
// char *bcast_implementation_name:   the bcast implementation name (argument #1)
// int chunk_size:                    the chunk size (optional argument #2)
// int NUM_BYTES:                     the number of bytes to broadcast
// char *buffer:                      the buffer to broadcast
//
// The method names should be:
// default_bcast
// naive_bcast
// ring_bcast
// pipelined_ring_bcast
// asynchronous_pipelined_ring_bcast
// asynchronous_pipelined_bintree_bcast
//
// GOOD LUCK (gonna need it)!

MPI_Status status;
MPI_Request request;
int num_full_chunks = NUM_BYTES / chunk_size;

if (strcmp(bcast_implementation_name, "default_bcast") == 0) { // Just call the library routine.
  MPI_Bcast(buffer, NUM_BYTES, MPI_CHAR, 0, MPI_COMM_WORLD);
} else if (strcmp(bcast_implementation_name, "naive_bcast") == 0) { // Send to all processes one-by-one from the root.
  if (rank == 0) { // Root process
    int dest_proc;
    for (dest_proc = 1; dest_proc < num_procs; dest_proc++) {
      MPI_Send(buffer, NUM_BYTES, MPI_CHAR, dest_proc, 0, MPI_COMM_WORLD);
    }
  } else { // Others
    MPI_Recv(buffer, NUM_BYTES, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
  }
} else if (strcmp(bcast_implementation_name, "ring_bcast") == 0) { // Receive from your left, then pass it to your right.
  if (rank > 0) { // Not the leftmost processor; so receive from its left first.
    MPI_Recv(buffer, NUM_BYTES, MPI_CHAR, rank - 1, 0, MPI_COMM_WORLD, &status);
  }
  if (rank < num_procs - 1) { // Not the rightmost processor; so send to its right.
    MPI_Send(buffer, NUM_BYTES, MPI_CHAR, rank + 1, 0, MPI_COMM_WORLD);
  }
} else if (strcmp(bcast_implementation_name, "pipelined_ring_bcast") == 0) { 
  // Pipeline it into num_full_chunks + 1 chunks (last chunks may be smaller than the rest).
  int chunk;
  for (chunk = 0; chunk < num_full_chunks; chunk++) {
    if (rank > 0) { // Not the leftmost processor; so receive from its left first.
      MPI_Recv(buffer + chunk * chunk_size, chunk_size, MPI_CHAR, rank - 1, 0, MPI_COMM_WORLD, &status);
    }
    if (rank < num_procs - 1) { // Not the rightmost processor; so send to its right.
      MPI_Send(buffer + chunk * chunk_size, chunk_size, MPI_CHAR, rank + 1, 0, MPI_COMM_WORLD);
    }
  }
  // Communicate for the last "leftover" chunk which is not a full chunk (which could be empty as well).
  if (rank > 0) { // Not the leftmost processor; so receive from its left first
    MPI_Recv(buffer + num_full_chunks * chunk_size, NUM_BYTES % chunk_size, MPI_CHAR, rank - 1, 0, MPI_COMM_WORLD, &status);
  }
  if (rank < num_procs - 1) { // Not the rightmost processor; so send to its right
    MPI_Send(buffer + num_full_chunks * chunk_size, NUM_BYTES % chunk_size, MPI_CHAR, rank + 1, 0, MPI_COMM_WORLD);
  }
} else if (strcmp(bcast_implementation_name, "asynchronous_pipelined_ring_bcast") == 0) {
  // Pipeline it into num_full_chunks + 1 chunks (last chunks may be smaller than the rest).
  int chunk;
  for (chunk = 0; chunk < num_full_chunks; chunk++) {
    if (rank > 0) { // Not the leftmost processor; so receive from its left first.
      MPI_Recv(buffer + chunk * chunk_size, chunk_size, MPI_CHAR, rank - 1, 0, MPI_COMM_WORLD, &status);
    }
    if (rank < num_procs - 1) { // Not the rightmost processor; so send to its right.
      if (chunk > 0) { MPI_Wait(&request, &status); } // There was at least one MPI_Isend call before; so wait.
      MPI_Isend(buffer + chunk * chunk_size, chunk_size, MPI_CHAR, rank + 1, 0, MPI_COMM_WORLD, &request);
    }
  }
  // Communicate for the last "leftover" chunk which is not a full chunk (which could be empty as well).
  if (rank > 0) { // Not the leftmost processor; so receive from its left first.
    MPI_Recv(buffer + num_full_chunks * chunk_size, NUM_BYTES % chunk_size, MPI_CHAR, rank - 1, 0, MPI_COMM_WORLD, &status);
  }
  if (rank < num_procs - 1) { // Not the rightmost processor; so send to its right.
    if (num_full_chunks > 0) { MPI_Wait(&request, &status); } // There was at least one MPI_Isend call before; so wait.
    MPI_Send(buffer + num_full_chunks * chunk_size, NUM_BYTES % chunk_size, MPI_CHAR, rank + 1, 0, MPI_COMM_WORLD);
  }
} else if (strcmp(bcast_implementation_name, "asynchronous_pipelined_bintree_bcast") == 0) {
  int pair_offset;
  if (num_procs > 1) { pair_offset = 1 << (int)log2(num_procs - 1); }
  else { pair_offset = 0; }
  while (pair_offset) { // Pair offset has the offset bit set for the current communication round.
    int mask = pair_offset - 1;
    if (!(rank & mask)) { // If all bits of my rank on the right hand side of the current pair offset bit are zero,
                          // then I participate in this round of communication.
      int pair_rank = rank ^ pair_offset; // Rank of my pair in this round
      int num_full_chunks = NUM_BYTES / chunk_size; // Number of chunks for pipelining
      int chunk;
      for (chunk = 0; chunk < num_full_chunks; chunk++) {
        if (pair_rank < rank) { // I am the receiver.
          MPI_Recv(buffer + chunk * chunk_size, chunk_size, MPI_CHAR, pair_rank, 0, MPI_COMM_WORLD, &status);
        } else if (pair_rank < num_procs) { // I am the sender
          if (chunk > 0) { MPI_Wait(&request, &status); } // There was at least one MPI_Isend before; so wait.
          MPI_Isend(buffer + chunk * chunk_size, chunk_size, MPI_CHAR, pair_rank, 0, MPI_COMM_WORLD, &request);
        }
      }
      // Communicate for the last "leftover" chunk which is not a full chunk (which could be empty as well).
      if (pair_rank < rank) { // I am the receiver.
        MPI_Recv(buffer + num_full_chunks * chunk_size, NUM_BYTES % chunk_size, MPI_CHAR, pair_rank, 0, MPI_COMM_WORLD,
            &status);
      } else if (pair_rank < num_procs) { // I am the sender.
        if (num_full_chunks > 0) { MPI_Wait(&request, &status); } // There was at least one MPI_Isend before; so wait.
        MPI_Send(buffer + num_full_chunks * chunk_size, NUM_BYTES % chunk_size, MPI_CHAR, pair_rank, 0, MPI_COMM_WORLD);
      }
    }
    pair_offset = pair_offset >> 1;
  }
}
