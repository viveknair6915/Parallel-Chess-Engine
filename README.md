# Parallel Chess Engine: Parallel Search, Visualization, and Analysis

---

## Project Overview

This project demonstrates the power and challenges of parallel programming in chess engine search algorithms. It features:
- A C engine implementing both sequential and parallel (OpenMP root splitting) alpha-beta search.
- A modern Streamlit UI for interactive analysis, visualization, and educational exploration.
- Robust support for custom FEN input, move legality, and detailed search statistics.
- Python scripts and a Jupyter notebook for further experimentation and visualization.

---

## Informative Details & Concepts

### Sequential vs. Parallel Search
- **Sequential Search:** The engine explores the game tree one branch at a time using the minimax algorithm with alpha-beta pruning. This is simple and reliable but only uses one CPU core, limiting speed for deep searches.
- **Parallel Search:** The engine uses OpenMP to split the evaluation of root moves among multiple threads, each working independently. This allows the engine to utilize all available CPU cores, significantly speeding up analysis for complex positions.

### Key Algorithms and Concepts
- **Minimax Algorithm:** Recursively evaluates chess positions, assuming both players play optimally.
- **Alpha-Beta Pruning:** Optimizes minimax by skipping branches that cannot affect the final decision, making search much faster.
- **OpenMP:** A parallel programming library for C/C++ that enables multi-threaded execution with simple compiler directives.
- **Root Splitting:** A parallelization technique where each legal move at the root is assigned to a separate thread for evaluation, then results are combined to select the best move.

### Educational and Robust Design
- The project is designed to be both a high-performance chess engine and a learning tool:
  - **Rich Evaluation Function:** Considers material, mobility, king safety, pawn structure, threats, and more.
  - **Interactive Visualization:** The Streamlit UI shows the search tree, principal variation, move annotations, and performance metrics.
  - **Error Handling:** Defensive programming and bounds checks prevent crashes and make the engine robust to bad input.
  - **Experimentation:** Python scripts and a Jupyter notebook allow for further analysis and educational experiments.

### Where to Find Key Features in the Code
- **Engine Logic:** `src/seq_vs_paral.c` (move generation, evaluation, search, parallelization)
- **UI and Visualization:** `ui.py` (Streamlit app for analysis and exploration)
- **Plotting and Statistics:** `plot_results.py` (visualizes engine performance)
- **Educational Experiments:** `parallel_chess_engine.ipynb` (notebook for step-by-step walkthroughs)
- **Images and Diagrams:** `images/` (visual aids for documentation and UI)

---

## Key Features

- **C Engine:** Implements both sequential and parallel alpha-beta search using OpenMP. Includes a rich, educational evaluation function with classic chess heuristics.
- **Streamlit UI:** Visualizes the search tree, principal variation, move annotations, and performance metrics. Allows custom FEN input and interactive exploration.
- **Robustness:** Handles illegal moves, edge cases, and provides clear error messages in both terminal and UI.
- **Visualization:** Includes a plotting script and notebook for performance analysis and experimentation.

---

## Directory Structure

- `src/seq_vs_paral.c` — Main C engine (sequential & parallel search)
- `ui.py` — Streamlit dashboard for interactive analysis
- `plot_results.py` — Python script for plotting engine results
- `parallel_chess_engine.ipynb` — Jupyter notebook for educational experiments
- `images/` — Diagrams and UI screenshots
- `Makefile` — Build instructions
- `.gitignore` — Excludes build artifacts, results, and temp files
- `README.md` — This guide

---

## How It Works: Engine & Parallelization

### Sequential Search
- Uses minimax with alpha-beta pruning to search the game tree.
- Evaluates each position using a rich evaluation function (material, mobility, king safety, pawn structure, etc.).

### Parallel Search (Root Splitting)
- At the root, each legal move is assigned to a separate thread using OpenMP.
- Each thread searches its subtree independently.
- Results are combined in a critical section to select the best move.
- This approach is simple, effective, and demonstrates real-world speedup.

![Root Splitting Diagram](images/Root_splitting.png)

---

## Streamlit UI: Features & Flow

- **Engine Control:** Run the C engine from the UI, select search depth, and upload previous results.
- **Custom FEN Input:** Enter any legal FEN to analyze arbitrary positions. Robust validation and feedback.
- **Results Summary:** View node counts, timing, and speedup for sequential vs. parallel search.
- **Chessboard Visualization:** Step through root moves and see the board update, with move highlighting.
- **Best Continuation Tree:** Interactive tree diagram of the best line, with per-node stats and click-to-explore.
- **Move List & Annotations:** See the principal variation with evaluation scores and chess motifs (check, capture, fork, etc.).
- **PGN Export:** Download the best line as a PGN for use in chess GUIs.
- **Error Handling:** Clear messages for illegal FENs, no legal moves, or engine errors.

---

## Step-by-Step Workflow

### 1. Build the Engine
```bash
make
```

### 2. (Optional) Set a Custom FEN
- Edit `fen.txt` or use the Streamlit sidebar to input a FEN.

### 3. Run the Engine
```bash
./chess 3
```
- Replace `3` with your desired search depth.
- Output is printed to the terminal and saved to `results.txt`.

### 4. Launch the Streamlit UI
```bash
streamlit run ui.py
```
- Open your browser to `http://localhost:8501`.
- Use the UI to analyze, visualize, and export results.

### 5. (Optional) Plot Results in Python
```bash
python3 plot_results.py
```
- Shows bar charts for execution time and nodes visited.

### 6. (Optional) Explore the Notebook
- Open `parallel_chess_engine.ipynb` for a step-by-step educational walkthrough and experiments.

---

## Output & Interpretation

- **results.txt:** Contains node counts, timing, principal variation, and legality checks.
- **UI Visuals:** See best move sequence, search tree, move annotations, and performance metrics.
- **Plotting:** Bar charts for time and nodes, speedup calculation.

---

## Troubleshooting & FAQ

- **Engine crashes or segfaults?**
  - All move generation now has bounds checks. If you still see issues, check your FEN or report the bug.
- **No legal moves found?**
  - The FEN may be illegal or a checkmate/stalemate position.
- **UI shows errors?**
  - Check `results.txt` for error messages. The UI will guide you to fix FEN or rerun the engine.
- **How do I add new evaluation features?**
  - Edit `estim` in `src/seq_vs_paral.c` and recompile.
- **How do I export a line to PGN?**
  - Use the "Export Principal Variation as PGN" button in the UI.

---

