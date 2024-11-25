import pandas as pd
import argparse
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(description="Process a CSV file with benchmark results.")
parser.add_argument("file_path", type=str, help="Path to the CSV file to be processed")

args = parser.parse_args()

df = pd.read_csv(args.file_path)

grouped_df = df.groupby('Threads', as_index=False).agg({
    'Avg Time (s)': 'mean',
    'Avg Ops/sec': 'mean'
})

print(grouped_df)

plt.figure(figsize=(10, 5))
plt.plot(grouped_df['Threads'], grouped_df['Avg Time (s)'], marker='o', color='b')
plt.title("Average Time vs. Threads")
plt.xlabel("Threads")
plt.ylabel("Avg Time (s)")
plt.grid()
plt.savefig("avg_time_vs_threads.png")
plt.close()

plt.figure(figsize=(10, 5))
plt.plot(grouped_df['Threads'], grouped_df['Avg Ops/sec'], marker='o', color='g')
plt.title("Average Operations per Second vs. Threads")
plt.xlabel("Threads")
plt.ylabel("Avg Ops/sec")
plt.grid()
plt.savefig("avg_ops_vs_threads.png")
plt.close()

print("Graphs saved as 'avg_time_vs_threads.png' and 'avg_ops_vs_threads.png'.")

