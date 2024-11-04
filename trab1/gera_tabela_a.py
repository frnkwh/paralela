import subprocess
import csv
import time
from datetime import datetime

# Parameters
array_sizes = [1_000_000, 2_000_000, 4_000_000, 8_000_000, 16_000_000]
thread_counts = range(1, 9)  # From 1 to 8
iterations = 1

# Results list
results = []

# Benchmarking
for size in array_sizes:
    for threads in thread_counts:
        total_time = 0
        total_operations = 0
        
        for _ in range(iterations):
            
            # Run the external program (replace 'your_program' with the actual command)
            # Capture the output
            result = subprocess.run(
                ['./bsearch_single', str(size), str(threads)],
                capture_output=True,
                text=True
            )
            
            # Assuming the output contains time and operations per second
            output_lines = result.stdout.strip().split('\n')
            print(output_lines)
            # execution_time = float(output_lines[0].split()[0])  # Assuming first line has time
            execution_time = float(output_lines[0])  # Assuming first line has time
            # operations_per_second = float(output_lines[1].split()[0])  # Assuming second line has operations
            operations_per_second = float(output_lines[1])  # Assuming second line has operations
            
            # Accumulate results
            total_time += execution_time
            total_operations += operations_per_second
        
        # Average results
        avg_time = f"{total_time / iterations:.5f}"
        avg_operations = f"{total_operations / iterations:.2f}"
        
        # Save to results
        results.append({
            'Array Size': size,
            'Threads': threads,
            'Avg Time (s)': avg_time,
            'Avg Ops/sec': avg_operations
        })

# Write results to CSV
formatted_time = datetime.now().strftime("%Y-%m-%d_%H%M%S")
with open(f'benchmark_results_A_{formatted_time}.csv', 'w', newline='') as csvfile:
    fieldnames = ['Array Size', 'Threads', 'Avg Time (s)', 'Avg Ops/sec']
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    
    writer.writeheader()
    for result in results:
        writer.writerow(result)

print("Benchmarking complete. Results saved to 'benchmark_results.csv'.")

