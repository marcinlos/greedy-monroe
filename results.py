#!/usr/bin/env python

candidates = []
with open('candidates', 'r') as candidates_file:
    for line in candidates_file:
        x, y = line.split()
        candidates.append((x, y))


with open('result', 'r') as results_file:
    with open('committee', 'w') as committee_file:
        for line in results_file:
            idx = int(line)
            x, y = candidates[idx]
            committee_file.write('{} {}\n'.format(x, y))
