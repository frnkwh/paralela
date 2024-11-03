import pandas as pd
import argparse
import matplotlib.pyplot as plt
from odf.opendocument import OpenDocumentSpreadsheet
from odf.table import Table, TableRow, TableCell
from odf.text import P
from odf.draw import Frame, Image

# Set up argument parsing
parser = argparse.ArgumentParser(description="Process a CSV file with benchmark results.")
parser.add_argument("file_path", type=str, help="Path to the CSV file to be processed")
args = parser.parse_args()

# Load the CSV file into a DataFrame
df = pd.read_csv(args.file_path)

# Group by 'Threads' and calculate the mean of 'Avg Time (s)' and 'Avg Ops/sec'
grouped_df = df.groupby('Threads', as_index=False).agg({
    'Avg Time (s)': 'mean',
    'Avg Ops/sec': 'mean'
})

# Plot and save Avg Time (s)
time_plot_path = "avg_time_vs_threads.png"
plt.figure(figsize=(10, 5))
plt.plot(grouped_df['Threads'], grouped_df['Avg Time (s)'], marker='o', color='b')
plt.title("Average Time vs. Threads")
plt.xlabel("Threads")
plt.ylabel("Avg Time (s)")
plt.grid()
plt.savefig(time_plot_path)
plt.close()

# Plot and save Avg Ops/sec
ops_plot_path = "avg_ops_vs_threads.png"
plt.figure(figsize=(10, 5))
plt.plot(grouped_df['Threads'], grouped_df['Avg Ops/sec'], marker='o', color='g')
plt.title("Average Operations per Second vs. Threads")
plt.xlabel("Threads")
plt.ylabel("Avg Ops/sec")
plt.grid()
plt.savefig(ops_plot_path)
plt.close()

# Create an ODS document
doc = OpenDocumentSpreadsheet()

# Add a table for grouped_df results
table = Table(name="Benchmark Results")
doc.spreadsheet.addElement(table)

# Add headers for the results table
headers = ["Threads", "Avg Time (s)", "Avg Ops/sec"]
header_row = TableRow()
for header in headers:
    cell = TableCell()
    cell.addElement(P(text=header))
    header_row.addElement(cell)
table.addElement(header_row)

# Add data rows to the results table
for _, row in grouped_df.iterrows():
    data_row = TableRow()
    for value in row:
        cell = TableCell()
        cell.addElement(P(text=str(value)))
        data_row.addElement(cell)
    table.addElement(data_row)

# Create a separate table for images
image_table = Table(name="Graphs")
doc.spreadsheet.addElement(image_table)

# Add a row with "Avg Time vs Threads" image
image_row_time = TableRow()
cell_time = TableCell()
frame_time = Frame(name="TimeGraph", width="15cm", height="7cm", anchortype="as-char")
image_time = Image(href=time_plot_path, type="simple")
frame_time.addElement(image_time)
cell_time.addElement(frame_time)
image_row_time.addElement(cell_time)
image_table.addElement(image_row_time)

# Add a row with "Avg Ops/sec vs Threads" image
image_row_ops = TableRow()
cell_ops = TableCell()
frame_ops = Frame(name="OpsGraph", width="15cm", height="7cm", anchortype="as-char")
image_ops = Image(href=ops_plot_path, type="simple")
frame_ops.addElement(image_ops)
cell_ops.addElement(frame_ops)
image_row_ops.addElement(cell_ops)
image_table.addElement(image_row_ops)

# Save the document
output_path = "benchmark_results_with_graphs.ods"
doc.save(output_path)

print(f"ODS file saved as '{output_path}' with table and embedded images.")

