import streamlit as st
import subprocess
import os
import numpy as np
import matplotlib.pyplot as plt
import chess
import chess.svg
import cairosvg
from PIL import Image
import io
import time

st.set_page_config(page_title="Parallel Chess Engine Dashboard", layout="centered")

st.title("Optimizing Chess Engine Search: Sequential vs. Parallel Alpha-Beta with OpenMP")
st.markdown("""
This dashboard demonstrates and compares the performance of sequential and parallel (OpenMP) chess engine search algorithms. 
You can run the engine, view the results, and see clear visualizations and interpretations.
""")

st.header("1. Run the Chess Engine")
engine_present = os.path.exists("chess")

if engine_present:
    depth = st.slider("Select search depth", min_value=2, max_value=7, value=5)
    if st.button("Run Engine"):
        with st.spinner("Running chess engine..."):
            try:
                subprocess.run(["./chess", str(depth)], check=True)
                st.success(f"Engine run complete at depth {depth}. Results saved to results.txt.")
            except Exception as e:
                st.error(f"Error running engine: {e}")
else:
    st.warning("chess executable not found. Please compile the engine first using 'gcc -fopenmp src/seq_vs_paral.c -o chess'.")

st.header("2. Analyze Results")

uploaded_file = st.file_uploader("Upload a results.txt file (optional)", type=["txt"])
results_path = "results.txt"

if uploaded_file is not None:
    results = uploaded_file.read().decode("utf-8").splitlines()
else:
    if os.path.exists(results_path):
        with open(results_path, "r") as f:
            results = f.read().splitlines()
    else:
        results = None

if results and len(results) >= 4:
    try:
        nodes_seq = int(results[0])
        nodes_par = int(results[1])
        times_seq = list(map(float, results[2].split()))
        times_par = list(map(float, results[3].split()))
        moves = list(range(1, max(len(times_seq), len(times_par)) + 1))

        st.subheader("Summary Table")
        st.table({
            "Mode": ["Sequential", "Parallel"],
            "Total Nodes Visited": [nodes_seq, nodes_par],
            "Avg Time per Move (s)": [np.mean(times_seq), np.mean(times_par)]
        })

        st.subheader("Execution Time per Move")
        fig1, ax1 = plt.subplots()
        ax1.bar(moves, times_seq, width=0.4, label="Sequential", color="lime", align="center")
        ax1.bar([m+0.4 for m in moves], times_par, width=0.4, label="Parallel", color="cyan", align="center")
        ax1.set_xlabel("Move Number")
        ax1.set_ylabel("Time (s)")
        ax1.legend()
        st.pyplot(fig1)

        st.subheader("Total Nodes Visited")
        fig2, ax2 = plt.subplots()
        ax2.bar(["Sequential", "Parallel"], [nodes_seq, nodes_par], color=["lime", "cyan"])
        ax2.set_ylabel("Nodes Visited")
        st.pyplot(fig2)

        st.header("3. Interpretation & Insights")
        if len(results) > 4 and results[-1].strip():
            pv_line = results[-1].strip()
            st.subheader("Best Move Sequence (Principal Variation)")
            pv_moves = pv_line.split()
            st.info(f"{' → '.join(pv_moves)}")

            st.subheader("Visualize Best Move Sequence on Chessboard")
            board = chess.Board()
            move_uci_list = pv_moves
            max_step = len(move_uci_list)

            # Move list table
            move_table = []
            for idx, move in enumerate(move_uci_list):
                move_num = (idx // 2) + 1
                side = "White" if idx % 2 == 0 else "Black"
                move_table.append({"Move #": move_num, "Side": side, "Move": move})
            st.table(move_table)

            # Download PGN button
            pgn = chess.pgn.Game()
            node = pgn
            for move in move_uci_list:
                node = node.add_variation(chess.Move.from_uci(move))
            import io as _io
            pgn_str = str(pgn)
            st.download_button("Download PGN", data=pgn_str, file_name="best_line.pgn", mime="text/plain")

            # Board animation controls
            play = st.checkbox("Play Animation", value=False)
            anim_speed = st.slider("Animation speed (sec per move)", 0.2, 2.0, 1.0, 0.1)
            if play:
                for step in range(max_step + 1):
                    board = chess.Board()
                    for i in range(step):
                        try:
                            board.push_uci(move_uci_list[i])
                        except Exception:
                            break
                    svg_board = chess.svg.board(board=board, lastmove=chess.Move.from_uci(move_uci_list[step-1]) if step > 0 else None)
                    png_bytes = cairosvg.svg2png(bytestring=svg_board.encode('utf-8'))
                    st.image(Image.open(io.BytesIO(png_bytes)), caption=f"Board after {step} move(s)")
                    time.sleep(anim_speed)
                st.stop()
            else:
                step = st.slider("Step through moves", 0, max_step, 0)
                board = chess.Board()
                for i in range(step):
                    try:
                        board.push_uci(move_uci_list[i])
                    except Exception:
                        break
                svg_board = chess.svg.board(board=board, lastmove=chess.Move.from_uci(move_uci_list[step-1]) if step > 0 else None)
                png_bytes = cairosvg.svg2png(bytestring=svg_board.encode('utf-8'))
                st.image(Image.open(io.BytesIO(png_bytes)), caption=f"Board after {step} move(s)")
        if np.mean(times_par) < np.mean(times_seq):
            st.success(f"Parallel search is faster than sequential search by {np.mean(times_seq) - np.mean(times_par):.3f} seconds per move on average.")
        else:
            st.info("Parallel search did not outperform sequential search in this run. This can happen for shallow depths or positions with few legal moves.")
        st.markdown(f"""
        - **Sequential search** uses classic minimax/alpha-beta, single-threaded.
        - **Parallel search** uses OpenMP to evaluate top-level moves in parallel (root splitting).
        - Both approaches visit a similar number of nodes, but parallel search can be much faster, especially for complex positions.
        """)
    except Exception as e:
        st.error(f"Error parsing results: {e}")
else:
    st.info("No results found. Please run the engine or upload a results.txt file.")

st.markdown("---")
st.caption("Developed for educational demonstration of parallel programming in AI.") 