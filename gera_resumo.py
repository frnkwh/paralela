import pandas as pd
import argparse
import matplotlib.pyplot as plt

# Set up argument parsing
parser = argparse.ArgumentParser(description="Process a CSV file with benchmark results.")
parser.add_argument("file_path", type=str, help="Path to the CSV file to be processed")

# Parse the arguments
args = parser.parse_args()

# Load the CSV file into a DataFrame
df = pd.read_csv(args.file_path)

# Group by 'Threads' and calculate the mean of 'Avg Time (s)' and 'Avg Ops/sec'
grouped_df = df.groupby('Threads', as_index=False).agg({
    'Avg Time (s)': 'mean',
    'Avg Ops/sec': 'mean'
})

# Plot and save Avg Time (s)
plt.figure(figsize=(10, 5))
plt.plot(grouped_df['Threads'], grouped_df['Avg Time (s)'], marker='o', color='b')
plt.title("Average Time vs. Threads")
plt.xlabel("Threads")
plt.ylabel("Avg Time (s)")
plt.grid()
plt.savefig("avg_time_vs_threads.png")
plt.close()

# Plot and save Avg Ops/sec
plt.figure(figsize=(10, 5))
plt.plot(grouped_df['Threads'], grouped_df['Avg Ops/sec'], marker='o', color='g')
plt.title("Average Operations per Second vs. Threads")
plt.xlabel("Threads")
plt.ylabel("Avg Ops/sec")
plt.grid()
plt.savefig("avg_ops_vs_threads.png")
plt.close()

print("Graphs saved as 'avg_time_vs_threads.png' and 'avg_ops_vs_threads.png'.")

