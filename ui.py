import streamlit as st
import subprocess
import os
import chess.pgn
import numpy as np
import matplotlib.pyplot as plt
import chess
import chess.svg
import cairosvg
from PIL import Image
import io
import time
import re
# --- New imports for enhancements ---
from streamlit_agraph import agraph, Node, Edge, Config as AGraphConfig
import base64

# --- Redesigned Streamlit UI for Chess Engine Visualization ---
st.set_page_config(page_title="Parallel Chess Engine Dashboard", layout="wide")

st.title("Parallel Chess Engine: Sequential vs. Parallel Search Visualization")

st.markdown("""
This dashboard demonstrates the impact of parallel programming on a chess engine's search algorithm. Compare sequential and parallel alpha-beta search, visualize the best move sequence, and step through the game on a real chessboard.
""")

# --- Engine Control Section ---
st.header("1. Engine Control")
engine_present = os.path.exists("chess")
col1, col2 = st.columns([2, 1])
with col1:
    st.markdown("**Run the C engine to generate results.**")
    depth = st.slider("Select search depth", min_value=2, max_value=7, value=3, help="Higher depth = stronger but slower search.")
    if st.button("Run Engine"):
        with st.spinner("Running chess engine..."):
            try:
                subprocess.run(["./chess", str(depth)], check=True)
                st.success(f"Engine run complete at depth {depth}. Results saved to results.txt.")
            except Exception as e:
                st.error(f"Error running engine: {e}")
with col2:
    st.info("Upload a results.txt file to analyze a previous run.")
    uploaded_file = st.file_uploader("Upload results.txt", type=["txt"])

# --- Custom FEN input ---
st.sidebar.header("Custom Position (FEN)")
def_fen = chess.STARTING_FEN
fen_input = st.sidebar.text_input("Enter FEN for analysis", value=def_fen, help="Paste any legal FEN string.")
if st.sidebar.button("Set Position"):
    try:
        board = chess.Board(fen_input)
        # If no exception, FEN is valid
        with open("fen.txt", "w") as f:
            f.write(fen_input)
        st.success("Custom FEN set. Please rerun the engine to analyze this position.")
    except Exception as e:
        st.error(f"Invalid FEN: {e}")

# --- Parse TREE section for best continuation tree with stats ---
def parse_tree_section(results):
    tree_lines = []
    in_tree = False
    for line in results:
        if line.strip() == "TREE":
            in_tree = True
            continue
        if in_tree:
            if line.strip() == "":
                break
            tree_lines.append(line.rstrip("\n"))
    # Parse indented tree into nested dict, extracting stats
    def parse_lines(lines, depth=0):
        tree = []
        i = 0
        while i < len(lines):
            line = lines[i]
            indent = len(line) - len(line.lstrip())
            if indent // 2 == depth:
                # Extract move, score, nodes
                m = re.match(r"(\S+)(?: \[score=([-\d]+), nodes=(\d+)\])?", line.strip())
                move = m.group(1) if m else line.strip()
                score = int(m.group(2)) if m and m.group(2) else None
                nodes = int(m.group(3)) if m and m.group(3) else None
                # Find children
                children_lines = []
                j = i + 1
                while j < len(lines):
                    next_indent = len(lines[j]) - len(lines[j].lstrip())
                    if next_indent // 2 <= depth:
                        break
                    children_lines.append(lines[j])
                    j += 1
                children = parse_lines(children_lines, depth+1) if children_lines else []
                tree.append({"move": move, "score": score, "nodes": nodes, "children": children})
                i = j
            else:
                i += 1
        return tree
    return parse_lines(tree_lines)

# --- Parse root moves from TREE section for visualization ---
def parse_root_moves(results):
    tree_start = None
    for idx, line in enumerate(results):
        if line.strip() == "TREE":
            tree_start = idx + 1
            break
    if tree_start is None:
        return []
    root_moves = []
    for line in results[tree_start:]:
        if line.startswith("  ") or not line.strip():
            break
        m = re.match(r"(\S+) \[score=([-\d]+), nodes=(\d+)\]", line.strip())
        if m:
            move = m.group(1)
            score = int(m.group(2))
            nodes = int(m.group(3))
            root_moves.append({"move": move, "score": score, "nodes": nodes})
    return root_moves

# --- Helper: Convert path to PGN ---
def path_to_pgn(path, fen=chess.STARTING_FEN):
    board = chess.Board(fen)
    game = chess.pgn.Game()
    node = game
    for move in path:
        m = chess.Move.from_uci(move)
        if m in board.legal_moves:
            board.push(m)
            node = node.add_variation(m)
        else:
            break
    return str(game)

# --- Helper: Download PGN as file ---
def download_pgn(pgn_str):
    b64 = base64.b64encode(pgn_str.encode()).decode()
    href = f'<a href="data:text/plain;base64,{b64}" download="analysis.pgn">Download PGN</a>'
    st.markdown(href, unsafe_allow_html=True)

# --- Helper: Motif highlighting ---
def get_move_motifs(board, move):
    motifs = []
    # Check, double check, discovered check
    board_copy = board.copy()
    board_copy.push(move)
    if board_copy.is_check():
        attackers = list(board_copy.attackers(not board.turn, board_copy.king(board.turn)))
        if len(attackers) > 1:
            motifs.append("Double Check")
        else:
            # Try to detect discovered check
            if not board.is_check() and board_copy.is_check():
                motifs.append("Discovered Check")
            else:
                motifs.append("Check")
    # Capture
    if board.is_capture(move):
        motifs.append("Capture")
    # Promotion
    if move.promotion:
        motifs.append("Promotion")
    # En passant
    if board.is_en_passant(move):
        motifs.append("En Passant")
    # Castling
    if board.is_castling(move):
        motifs.append("Castling")
    # Fork (simple: move attacks 2+ major/minor pieces)
    board_copy = board.copy()
    board_copy.push(move)
    piece = board_copy.piece_at(move.to_square)
    if piece and piece.piece_type in [chess.KNIGHT, chess.BISHOP, chess.ROOK, chess.QUEEN]:
        attacked = [sq for sq in board_copy.attacks(move.to_square) if board_copy.piece_at(sq) and board_copy.piece_at(sq).color != piece.color and board_copy.piece_at(sq).piece_type in [chess.QUEEN, chess.ROOK, chess.BISHOP, chess.KNIGHT]]
        if len(attacked) >= 2:
            motifs.append("Fork")
    # Pin (simple: move attacks a piece that is pinned to king)
    # This is a basic check, not full pin logic
    for sq in chess.SQUARES:
        if board.piece_at(sq) and board.piece_at(sq).color != board.turn:
            if board.is_pinned(board.turn, sq):
                motifs.append("Pin")
                break
    return motifs

# --- Results Loading ---
results_path = "results.txt"
if uploaded_file is not None:
    results = uploaded_file.read().decode("utf-8").splitlines()
elif os.path.exists(results_path):
    with open(results_path, "r") as f:
        results = f.read().splitlines()
else:
    results = None

# --- Robust Results Summary Section ---
if results:
    st.header("2. Results Summary")
    # Node counts
    try:
        nodes_seq = int(results[0]) if len(results) > 0 and results[0].strip() else None
        nodes_par = int(results[1]) if len(results) > 1 and results[1].strip() else None
    except Exception:
        nodes_seq = nodes_par = None
    # Times
    try:
        times_seq = list(map(float, results[2].split())) if len(results) > 2 and results[2].strip() else []
    except Exception:
        times_seq = []
    try:
        times_par = list(map(float, results[3].split())) if len(results) > 3 and results[3].strip() else []
    except Exception:
        times_par = []
    error_lines = [line for line in results if line.startswith("# ERROR") or "Illegal move" in line]
    if error_lines:
        st.error("Engine output contains errors. Only summary and performance charts are shown. Please check your FEN or engine configuration.")
    # Show node counts
    if nodes_seq is not None and nodes_par is not None:
        st.markdown("**Performance Comparison**")
        st.table({
            "Mode": ["Sequential", "Parallel"],
            "Total Nodes Visited": [nodes_seq, nodes_par],
            "Avg Time per Move (s)": [np.mean(times_seq) if times_seq else '-', np.mean(times_par) if times_par else '-']
        })
        # --- Speedup Metric ---
        total_seq = sum(times_seq) if times_seq else None
        total_par = sum(times_par) if times_par else None
        if total_seq and total_par and total_par > 0:
            speedup = total_seq / total_par
            st.success(f"Speedup (Sequential / Parallel): {speedup:.2f}x")
        # --- Cumulative Time Chart ---
        if times_seq and times_par:
            st.markdown("**Cumulative Time per Move**")
            cum_seq = np.cumsum(times_seq)
            cum_par = np.cumsum(times_par)
            fig3, ax3 = plt.subplots()
            ax3.plot(np.arange(1, len(cum_seq)+1), cum_seq, label="Sequential", color="lime")
            ax3.plot(np.arange(1, len(cum_par)+1), cum_par, label="Parallel", color="cyan")
            ax3.set_xlabel("Move Number")
            ax3.set_ylabel("Cumulative Time (s)")
            ax3.legend()
            st.pyplot(fig3)
        # --- Node Count per Move Table (if possible) ---
        # If you have per-move node counts, you can add them here. For now, just show total.
        st.markdown("**Node Count Table**")
        st.table({
            "Mode": ["Sequential", "Parallel"],
            "Total Nodes": [nodes_seq, nodes_par]
        })
    else:
        st.info("Node counts not found in results.txt.")
    # Show times per move
    if times_seq or times_par:
        st.markdown("**Execution Time per Move**")
        fig1, ax1 = plt.subplots()
        if times_seq:
            ax1.bar(np.arange(1, len(times_seq)+1), times_seq, width=0.4, label="Sequential", color="lime", align="center")
        if times_par:
            ax1.bar(np.arange(1, len(times_par)+1)+0.4, times_par, width=0.4, label="Parallel", color="cyan", align="center")
        ax1.set_xlabel("Move Number")
        ax1.set_ylabel("Time (s)")
        ax1.legend()
        st.pyplot(fig1)
    else:
        st.info("No timing data found in results.txt.")
    # Show nodes bar chart
    if nodes_seq is not None and nodes_par is not None:
        st.markdown("**Total Nodes Visited**")
        fig2, ax2 = plt.subplots()
        ax2.bar(["Sequential", "Parallel"], [nodes_seq, nodes_par], color=["lime", "cyan"])
        ax2.set_ylabel("Nodes Visited")
        st.pyplot(fig2)
    # --- Root Move Visualization ---
    root_moves = parse_root_moves(results)
    if root_moves:
        st.header("3. Root Move Comparison (Best Steps Visualization)")
        st.markdown("This chart shows all root moves considered by the engine, their evaluation, and the search effort (nodes) for each. The best move is highlighted.")
        moves = [rm["move"] for rm in root_moves]
        scores = [rm["score"] for rm in root_moves]
        nodes = [rm["nodes"] for rm in root_moves]
        best_score = max(scores)
        best_indices = [i for i, s in enumerate(scores) if s == best_score]
        # Bar chart for scores
        fig, ax = plt.subplots()
        colors = ["orange" if i in best_indices else "skyblue" for i in range(len(moves))]
        ax.bar(moves, scores, color=colors)
        for i in best_indices:
            ax.text(i, scores[i], "Best", ha="center", va="bottom", color="red", fontweight="bold")
        ax.set_xlabel("Root Move")
        ax.set_ylabel("Evaluation Score")
        ax.set_title("Root Move Evaluations (Best Highlighted)")
        plt.setp(ax.get_xticklabels(), rotation=45, ha="right")
        fig.tight_layout()
        st.pyplot(fig)
        # Bar chart for node counts
        fig2, ax2 = plt.subplots()
        ax2.bar(moves, nodes, color=colors)
        ax2.set_xlabel("Root Move")
        ax2.set_ylabel("Nodes Searched")
        ax2.set_title("Root Move Node Counts")
        plt.setp(ax2.get_xticklabels(), rotation=45, ha="right")
        fig2.tight_layout()
        st.pyplot(fig2)
        # Table
        st.markdown("**Root Move Table**")
        table_data = []
        for i, rm in enumerate(root_moves):
            table_data.append({"Move": rm["move"], "Score": rm["score"], "Nodes": rm["nodes"], "Best": "Yes" if i in best_indices else ""})
        st.table(table_data)
        # --- Chessboard Visualization for Root Moves ---
        st.header("4. Chessboard Visualization: Explore Root Moves")
        st.markdown("Select a root move to see the resulting position, evaluation, and node count. This demonstrates the engine's parallel root splitting.")
        fen = fen_input if fen_input else chess.STARTING_FEN
        board = chess.Board(fen)
        legal_root_moves = []
        for rm in root_moves:
            try:
                move_obj = chess.Move.from_uci(rm['move'])
                if move_obj in board.legal_moves:
                    legal_root_moves.append(rm)
            except Exception:
                continue
        # Always show the board, even if no legal root moves
        st.markdown("**Initial Position:**")
        svg_start = chess.svg.board(board=board)
        png_start = cairosvg.svg2png(bytestring=svg_start.encode('utf-8')) if svg_start else None
        if png_start is not None:
            st.image(Image.open(io.BytesIO(png_start)), caption="Initial Position")
        if not legal_root_moves:
            st.warning("No legal root moves found for the current FEN. Please check your FEN or engine output.")
        else:
            root_move_options = [f"{rm['move']} (Score: {rm['score']}, Nodes: {rm['nodes']})" for rm in legal_root_moves]
            selected = st.selectbox("Select a root move to visualize:", root_move_options)
            selected_idx = root_move_options.index(selected)
            selected_move = legal_root_moves[selected_idx]["move"]
            # Show board after selected root move
            st.markdown(f"**After move {selected_move}:**")
            try:
                move_obj = chess.Move.from_uci(selected_move)
                board2 = chess.Board(fen)  # Use a fresh board for the move
                if move_obj in board2.legal_moves:
                    board2.push(move_obj)
                    svg_after = chess.svg.board(board=board2, lastmove=move_obj)
                    png_after = cairosvg.svg2png(bytestring=svg_after.encode('utf-8')) if svg_after else None
                    if png_after is not None:
                        st.image(Image.open(io.BytesIO(png_after)), caption=f"After {selected_move}")
                    st.success(f"Evaluation: {legal_root_moves[selected_idx]['score']}, Nodes: {legal_root_moves[selected_idx]['nodes']}")
                else:
                    st.warning(f"Move {selected_move} is not legal from the initial position.")
            except Exception as e:
                st.warning(f"Error visualizing move {selected_move}: {e}")
    # --- Tree Diagram Visualization Section ---
    st.header("3. Best Continuation Tree Visualization")
    tree = parse_tree_section(results)
    if tree:
        # Helper to recursively build nodes and edges for agraph
        def build_agraph(tree, parent_id=None, nodes=None, edges=None, depth=0):
            if nodes is None:
                nodes = []
            if edges is None:
                edges = []
            for idx, node in enumerate(tree):
                node_id = f"{parent_id or 'root'}_{idx}"
                label = f"{node['move']}\nScore: {node['score']}\nNodes: {node['nodes']}"
                nodes.append(Node(id=node_id, label=label, size=25, shape='circle'))
                if parent_id is not None:
                    edges.append(Edge(source=parent_id, target=node_id))
                if node['children']:
                    build_agraph(node['children'], node_id, nodes, edges, depth+1)
            return nodes, edges
        nodes, edges = build_agraph(tree)
        config = AGraphConfig(
            directed=True,
            nodeHighlightBehavior=True,
            highlightColor="#F7A7A6",
            collapsible=True,
            width=900,
            height=500,
            node={'labelProperty': 'label'},
            link={'labelProperty': 'label'},
            staticGraph=False,
        )
        st.markdown("**Click nodes to explore branches.**")
        agraph(nodes=nodes, edges=edges, config=config)
    else:
        st.info("No best continuation tree found in results.")
    # --- PGN Export Section ---
    # Try to extract the principal variation (PV) from results
    pv = []
    in_pv = False
    for line in results:
        if line.strip().startswith("PV:"):
            in_pv = True
            pv = line.strip().replace("PV:", "").strip().split()
            break
    if pv:
        st.header("4. Export Principal Variation as PGN")
        fen_for_pgn = fen_input if fen_input else chess.STARTING_FEN
        pgn_str = path_to_pgn(pv, fen=fen_for_pgn)
        st.code(pgn_str, language="pgn")
        if st.button("Download PV as PGN"):
            download_pgn(pgn_str)
    else:
        st.info("No principal variation (PV) found in results to export as PGN.")
    # --- Move Annotations and Evaluation Scores Section ---
    if pv:
        st.header("5. Move Annotations and Evaluation Scores")
        board = chess.Board(fen_for_pgn)
        move_data = []
        for idx, move_uci in enumerate(pv):
            move = chess.Move.from_uci(move_uci)
            san = board.san(move) if move in board.legal_moves else move_uci
            motifs = get_move_motifs(board, move)
            annotation = []
            if idx == 0:
                annotation.append("Best move")
            if "Check" in motifs:
                annotation.append("Check")
            if "Capture" in motifs:
                annotation.append("Capture")
            # Find score for this move from tree (if available)
            score = None
            def find_score(tree, path, depth=0):
                if not tree or depth >= len(path):
                    return None
                for node in tree:
                    if node['move'] == path[depth]:
                        if depth == len(path)-1:
                            return node['score']
                        return find_score(node['children'], path, depth+1)
                return None
            score = find_score(tree, pv[:idx+1]) if tree else None
            move_data.append({
                "Move #": idx+1,
                "Move": san,
                "Score": score,
                "Annotations": ", ".join(annotation) if annotation else "-"
            })
            board.push(move)
        st.table(move_data)
    st.markdown("---")
    st.caption("Developed for educational demonstration of parallel programming in AI. Use the controls above to run the engine, analyze results, and visualize the search process.")
else:
    st.info("No results found. Please run the engine or upload a results.txt file.")

# --- Educational Explanations ---
with st.expander("What is Alpha-Beta Search?"):
    st.markdown("""
    Alpha-beta search is an optimization of the minimax algorithm for two-player games like chess. It prunes branches that cannot affect the final decision, making the search much faster without changing the result.
    """)
with st.expander("What is Parallelization (Root Splitting)?"):
    st.markdown("""
    Parallelization in chess engines means splitting the search work among multiple CPU cores. Root splitting is a simple method: each possible move from the root is searched in parallel, and the best result is chosen. This can provide significant speedup, especially in positions with many legal moves.
    """)
with st.expander("What do 'Nodes' and 'Speedup' Mean?"):
    st.markdown("""
    - **Nodes**: Each position the engine examines is a 'node'. More nodes = more search effort.
    - **Speedup**: How much faster the parallel search is compared to sequential. Speedup = (sequential time) / (parallel time).
    """) 