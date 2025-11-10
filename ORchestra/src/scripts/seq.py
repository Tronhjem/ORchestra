from itertools import cycle
from math import gcd
from functools import reduce

def lcm(a, b): return abs(a * b) // gcd(a, b)
def lcm_all(numbers): return reduce(lcm, numbers)

def find_output_pattern(control_seqs, payload_seq):
    lengths = [len(seq) for seq in control_seqs + [payload_seq]]
    max_cycle = lcm_all(lengths)

    # Set up cyclic iterators
    control_iters = [cycle(seq) for seq in control_seqs]
    payload_iter = cycle(payload_seq)
    payload = list(payload_seq)

    output = []
    payload_len = len(payload_seq)

    for i in range(max_cycle):
        control_values = [next(it) for it in control_iters]
        should_send = all(control_values)
        payload_val = payload[i % payload_len]  # advances every step
        if should_send:
            output.append(payload_val)
        else:
            output.append(0)

    return output

def find_shortest_pattern(output):
    # Build the prefix table (KMP "failure function")
    n = len(output)
    if n == 0:
        return []

    # Build the prefix table (KMP "failure function")
    pi = [0] * n
    for i in range(1, n):
        j = pi[i - 1]
        while j > 0 and output[i] != output[j]:
            j = pi[j - 1]
        if output[i] == output[j]:
            j += 1
        pi[i] = j

    # Compute the smallest period
    period = n - pi[-1]
    if n % period == 0:
        return output[:period]
    else:
        return output  # No shorter repeating pattern


s1 = [1, 1, 0, 0, 0, 1]
s2 = [1, 0, 0, 1]
seqs = [s1, s2]
payload = [64,65,66,67, 68,69,70,71,72,73]
res = find_output_pattern(seqs, payload)
finalRes = find_shortest_pattern(res)
print(res)
print(finalRes)
print("LCM of triggers " + str(lcm(len(s1), len(s2))))
print("length of shortest " + str(len(finalRes)))

