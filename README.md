# Parallel Chess Engine: Sequential vs. Parallel Search with OpenMP

---

## Streamlit Dashboard: Visual Highlights

Experience the interactive analysis and visualization features of this project through the following Streamlit UI screenshots:

---

### Engine Control & Summary

![Engine Run and Summary](images/Screenshot%202025-07-16%20223147.png)
*Run the chess engine, select search depth, and view a summary of performance metrics directly from the dashboard.*

---

### Best Move Sequence & Chessboard Visualization

![Best Move Sequence and Board](images/Screenshot%202025-07-16%20223245.png)
*See the principal variation (best move sequence) and step through it on a real chessboard, move by move.*

---

###  Move List & Animation Controls

![Move List and Animation](images/Screenshot%202025-07-16%20223343.png)
*Review the move list table, animate the best line, and visualize each position interactively.*

---

### Performance Analysis

![Performance Plots](images/Screenshot%202025-07-16%20223355.png)
*Compare sequential and parallel search with bar charts for execution time and nodes visited.*

---

### Error Handling & Insights

![Error Handling and Insights](images/Screenshot%202025-07-16%20223412.png)
*Robust error handling and clear interpretation of results, ensuring a smooth user experience.*

---

## Summary

This project demonstrates the impact of parallel programming on a chess engine's search algorithm using OpenMP. It compares the performance of a sequential minimax/alpha-beta search with a parallelized version (root splitting) in C. The focus is on execution time and search efficiency, not on chess strength.

## Background & Methodology

### Chess Search Algorithms

Chess engines use the minimax algorithm with alpha-beta pruning to efficiently search the game tree. Minimax recursively explores possible moves, evaluating each position to select the best outcome. Alpha-beta pruning eliminates branches that cannot affect the final decision, greatly reducing the number of nodes visited.

### The Challenge of Parallelization

Alpha-beta pruning is inherently sequential, making parallelization non-trivial. Several techniques exist:
- **Younger Brothers Wait**: Establishes bounds by searching the leftmost branch, then parallelizes the rest.
- **Lazy SMP**: Used in top engines like Stockfish, threads search independently with minimal communication, sharing a hash table.
- **Root Splitting**: The root's children are split among threads, each evaluating a subtree in parallel. Synchronization is only needed when updating the best move at the root.

### Why Root Splitting with OpenMP?

Root splitting is simple, effective, and easy to implement with OpenMP. Each thread evaluates a top-level move, and results are combined in a critical section to ensure correctness. This approach provides significant speedup, especially for positions with many legal moves.

![Root Splitting Diagram](images/Root_splitting.png)
*Figure: Root splitting in parallel game tree search*

## Project Structure

- `src/seq_vs_paral.c`: Main C source file implementing both sequential and parallel (OpenMP) chess search.
- `images/Root_splitting.png`: Illustration of the root splitting parallelization technique.
- `plot_results.py`: Python script to visualize results.
- `ui.py`: Streamlit dashboard for interactive analysis.
- `Makefile`: For building the project on Linux.

## Usage

1. Build: `make`
2. Run: `./chess 5`
3. Visualize: `python3 plot_results.py` or `streamlit run ui.py`

## Interpretation

The results.txt file contains node counts and timing for both sequential and parallel search. Visualization scripts show bar charts and speedup, making it easy to interpret the benefits of parallelization.
