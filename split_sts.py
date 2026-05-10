import re
from collections import defaultdict
import os

input_file = r'STS-Rating\epd\STS1-STS15_LAN_v6.epd'
output_dir = r'STS-Rating\epd\categorized_v6'

if not os.path.exists(output_dir):
    os.makedirs(output_dir)

categories = defaultdict(list)

print(f"Reading {input_file}...")
with open(input_file, 'r') as f:
    for line in f:
        # Match pattern like: id "STS(v1.0) Undermine.001" or "STS(v2.2) Open Files and Diagonals.002"
        match = re.search(r'id "STS\(v.*?\) (.*?)\.', line)
        if match:
            # Replace spaces and slashes with underscores to create valid filenames
            cat_name = match.group(1).replace(' ', '_').replace('/', '_')
            categories[cat_name].append(line)
        else:
            categories['Unknown'].append(line)

for cat, lines in categories.items():
    output_path = os.path.join(output_dir, f'{cat}.epd')
    with open(output_path, 'w') as f:
        f.writelines(lines)
    print(f'Wrote {len(lines)} positions to {output_path}')
