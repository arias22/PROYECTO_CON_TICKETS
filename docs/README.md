# PROYECTO_CON_TICKETS



# PROYECTO_CON_TICKETS

## Project Overview
This project demonstrates the implementation of the Agrawala algorithm for distributed mutual exclusion in a real distributed system. The system is written in C and uses shared memory, semaphores, and message queues to coordinate access to critical sections among multiple nodes. It simulates different process types (payments, reservations, administration, queries) and analyzes their behavior under various concurrency and priority conditions.

## What is the Agrawala Algorithm?
The Agrawala algorithm is a distributed mutual exclusion protocol that allows multiple processes (nodes) to safely access a shared resource (critical section) without conflicts, and without relying on a central coordinator. Each process requests access by sending its logical timestamp to all other processes. Access is granted when a process receives permission (ACKs) from all other nodes, ensuring requests are served in timestamp order. This algorithm is efficient and scalable for distributed systems.

## What Does Our Project Achieve?
- We adapted and implemented the Agrawala algorithm in a real distributed environment using C, POSIX shared memory, semaphores, and message queues.
- The system supports multiple types of operations (payments, reservations, administration, queries), each with its own priority and behavior.
- We provide scripts for automated testing, data collection, and performance analysis.
- The project demonstrates how distributed mutual exclusion can be achieved without a central server, and how priorities and concurrency affect system performance.

## Structure
- `src/`: C source code (`main_v3.c`, `receptor_v3.c`, `borrar_buzones.c`) and Makefile.
- `scripts/`: Shell scripts for automated testing and experiments.
- `data/`: Example input files and results from tests.
- `docs/`: Diagrams and documentation.

## How to Use
1. **Compile** (from `src/`):
  ```sh
  make
  ```
2. **Run the receiver** (for each node):
  ```sh
  ./receptor_v3 <position> <N>
  ```
3. **Run the main program**:
  ```sh
  ./main_v3 <position> <N> <priority> <process_id>
  ```
4. **Automated tests**:
  Use scripts in `scripts/` to run batch experiments.

**Note:** Run as root for proper message queue management. Close receiver terminals with right-click to ensure data is dumped to `mis_archivos`.

## Cleaning Up
Use `borrar_buzones.c` to remove message queues if needed.

## Results & Analysis
Test results are stored in `data/`. Diagrams in `docs/` illustrate system design and flow.

