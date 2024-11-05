import subprocess
import csv
import time
from datetime import datetime

array_sizes = [1_000_000, 2_000_000, 4_000_000, 8_000_000, 16_000_000]
thread_counts = range(1, 9)
iterations = 10

results = []

for size in array_sizes:
    for threads in thread_counts:
        
        for iteration in range(1, iterations + 1):
            
            result = subprocess.run(
                ['./bsearch_multiple', str(size), str(threads)],
                capture_output=True,
                text=True
            )
            
            output_lines = result.stdout.strip().split('\n')
            print(f"iteration = {iteration}, size = {size}:{output_lines}")
            execution_time = float(output_lines[0])
            operations_per_second = float(output_lines[1])
            
            results.append({
                'Array Size': size,
                'Threads': threads,
                'Iteration': iteration,
                'Time (s)': f"{execution_time:.5f}",
                'Ops/sec': f"{operations_per_second:.2f}"
            })

formatted_time = datetime.now().strftime("%Y-%m-%d_%H%M%S")
filename = f'benchmark_results_B_{formatted_time}.csv'
with open(filename, 'w', newline='') as csvfile:
    fieldnames = ['Array Size', 'Threads', 'Iteration', 'Time (s)', 'Ops/sec']
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    
    writer.writeheader()
    for result in results:
        writer.writerow(result)

print(f"Benchmarking complete. Results saved to '{filename}'.")

