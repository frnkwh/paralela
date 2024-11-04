import subprocess
import csv
import time
from datetime import datetime

# Parameters
array_sizes = [1_000_000, 2_000_000, 4_000_000, 8_000_000, 16_000_000]
thread_counts = range(1, 9)  # From 1 to 8
iterations = 10

# Results list
results = []

# Benchmarking
for size in array_sizes:
    for threads in thread_counts:
        
        for iteration in range(1, iterations + 1):
            
            # Run the external program (replace 'your_program' with the actual command)
            result = subprocess.run(
                ['./bsearch_single', str(size), str(threads)],
                capture_output=True,
                text=True
            )
            
            # Assuming the output contains time and operations per second
            output_lines = result.stdout.strip().split('\n')
            print(f"iteration = {iteration}, size = {size}:{output_lines}")
            execution_time = float(output_lines[0])  # Assuming first line has time
            operations_per_second = float(output_lines[1])  # Assuming second line has operations
            
            # Save each iteration result
            results.append({
                'Array Size': size,
                'Threads': threads,
                'Iteration': iteration,
                'Time (s)': f"{execution_time:.5f}",
                'Ops/sec': f"{operations_per_second:.2f}"
            })

# Write results to CSV
formatted_time = datetime.now().strftime("%Y-%m-%d_%H%M%S")
filename = f'benchmark_results_B_{formatted_time}.csv'
with open(filename, 'w', newline='') as csvfile:
    fieldnames = ['Array Size', 'Threads', 'Iteration', 'Time (s)', 'Ops/sec']
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    
    writer.writeheader()
    for result in results:
        writer.writerow(result)

print(f"Benchmarking complete. Results saved to '{filename}'.")

