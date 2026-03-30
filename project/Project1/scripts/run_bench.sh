#!/bin/bash
cd "$(dirname "$0")/.." || exit 1
echo "==============================================="
echo "  Parallel Matrix Extractor Benchmark Script  "
echo "==============================================="

# 1. 自動重新編譯
echo "Recompiling..."
make clean > /dev/null 2>&1
make > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "Compile Error！plz check code。"
    exit 1
fi
OUT_DIR="results"
mkdir -p "$OUT_DIR"
# Make sure the plots directory exists too
mkdir -p "$OUT_DIR/plots"

CSV_FILE="$OUT_DIR/benchmark_results.csv"
PNG_FILE="$OUT_DIR/plots/speedup_curve.png"
# 定義要測試的執行緒數與採樣次數
THREADS=(1 2 4 8 16)
RUNS=1
CSV_FILE="$OUT_DIR/benchmark_results.csv"
PNG_FILE="$OUT_DIR/plots/speedup_curve.png"
# 初始化 CSV 檔案並寫入標題
echo "Threads,AverageTime" > "$CSV_FILE"

echo "Compile Success！Start Benchmarking(Each number of threads will run $RUNS times)..."
echo "------------------------------------------"

# 2. 雙重迴圈：測試不同執行緒數，每次跑 5 遍
for t in "${THREADS[@]}"
do
    echo -n "$t threads... "
    TOTAL_TIME=0

    for ((i=1; i<=RUNS; i++))
    do
        # 執行程式並將結果存入變數 OUTPUT
        OUTPUT=$(./extractor $t)
        
        # 使用 grep 找出含有「耗時」的那一行，並用正則表達式提取出浮點數數字
        TIME=$(echo "$OUTPUT" | grep "核心計算耗時" | grep -oE '[0-9]+\.[0-9]+')
        
        # 萬一抓不到數字的防呆機制
        if [ -z "$TIME" ]; then
            echo "Error：Fetching time error, plz check main.c format"
            exit 1
        fi

        # 使用 awk 進行浮點數累加
        TOTAL_TIME=$(awk "BEGIN {print $TOTAL_TIME + $TIME}")
    done

    # 計算 5 次的平均值
    AVG_TIME=$(awk "BEGIN {print $TOTAL_TIME / $RUNS}")
    echo "Avg Time: $AVG_TIME 秒"

    # 將結果寫入 CSV 檔案
    echo "$t,$AVG_TIME" >> $CSV_FILE
done

echo "------------------------------------------"
echo "Data collected！Save in $CSV_FILE"
echo "Ready to plot (Speedup Curve)..."

# 3. 自動生成 Python 繪圖腳本
cat << 'EOF' > plot_speedup.py
import csv
import sys
import matplotlib.pyplot as plt
input_csv = sys.argv[1]
output_png = sys.argv[2]
try:
    threads = []
    times = []
    # 讀取剛剛 Bash 產生的 CSV
    with open(input_csv, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            threads.append(int(row['Threads']))
            times.append(float(row['AverageTime']))

    # 計算加速比 Speedup = T1 / Tn
    t1_time = times[0]
    speedup = [t1_time / t for t in times]

    # 設定圖表樣式
    plt.figure(figsize=(8, 5))
    
    # 畫出實際加速比 (藍線)
    plt.plot(threads, speedup, marker='o', linestyle='-', color='b', label='Actual Speedup')
    # 畫出理想加速比 (紅虛線：如果 2 個 Thread 快 2 倍，4 個快 4 倍的理論值)
    plt.plot(threads, threads, linestyle='--', color='r', alpha=0.6, label='Ideal Speedup')

    plt.title('Parallel Matrix Extractor - Speedup Curve')
    plt.xlabel('Number of Threads')
    plt.ylabel('Speedup (T1 / Tn)')
    plt.xticks(threads)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()

    # 儲存圖片
    plt.savefig(output_png, dpi=300)
    print(f"Success！Save as {output_png}")

except ImportError:
    print("Error, lack of python module: matplotlib。")
    print("Solution：Checked if'pip install matplotlib' ,execute 'python3 plot_speedup.py'。")
except Exception as e:
    print(f"plot error: {e}")
EOF

# 4. 呼叫 Python 執行繪圖
python3 plot_speedup.py "$CSV_FILE" "$PNG_FILE"

rm plot_speedup.py