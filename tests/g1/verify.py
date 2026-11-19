"""Run focused G1 fixtures against a compiled list_insertion executable."""
import random
import subprocess
import sys
from pathlib import Path

def verify(data, output):
    numbers = iter(map(int, data.split()))
    n, p = next(numbers), next(numbers)
    costs = [[next(numbers) for _ in range(p)] for _ in range(n)]
    edges = [[next(numbers) for _ in range(n)] for _ in range(n)]
    order, header, *lines = output.splitlines()
    sequence = [int(i)-1 for i in order.split()]
    assert sorted(sequence) == list(range(n))
    placements = {}
    for line in lines:
        t, start, end, cpu = map(int, line.split())
        t -= 1
        cpu -= 1
        assert 0 <= t < n and t not in placements and 0 <= cpu < p
        assert start >= 0 and end-start == costs[t][cpu]
        placements[t] = (start, end, cpu)
    assert len(placements) == n
    for i, (s, f, k) in placements.items():
        for j, (a, b, q) in placements.items():
            if edges[i][j] >= 0:
                assert sequence.index(i) < sequence.index(j)
                assert a >= f + (edges[i][j] if q != k else 0)
            if i != j and q == k:
                assert min(b, f) <= max(a, s)
    return placements

def encode(costs, edges):
    n = len(costs)
    matrix = [[-1]*n for _ in costs]
    for i,j,c in edges:
        matrix[i][j] = c
    return '\n'.join(' '.join(map(str,row)) for row in [[n,len(costs[0])],*costs,*matrix])+'\n'

def run(data, method):
    result = subprocess.run([sys.argv[1],method],input=data,text=True,capture_output=True,check=True)
    return verify(data,result.stdout)

classic = Path(__file__).with_name('classic.d3').read_text()
for method, expected in [('heft',80), ('peft',85), ('lookahead',76)]:
    rows = run(classic,method)
    assert max(f for s,f,k in rows.values()) == expected

# Independent hand trace. A runs on CPU 1 until 5. B needs its
# output on CPU 2 at 15. C fits before B despite being selected later.
gap = encode([[5,100],[100,5],[100,3]],[(0,1,10)])
assert run(gap,'heft') == {0:(0,5,0),1:(15,20,1),2:(0,3,1)}
# On CPU 1 the parent finishes at 1 but its child finishes at 101.
# On CPU 2 the parent finishes at 3 and its child finishes at 4.
probe = encode([[1,3],[100,1]],[(0,1,100)])
assert run(probe,'lookahead') == {0:(0,3,1),1:(3,4,1)}
assert run(probe,'heft')[0] == (0,1,0)
rng = random.Random(411)
for _ in range(100):
    n,p = rng.randint(1,9),rng.randint(1,4)
    labels = rng.sample(range(n),n)
    costs = [[rng.randrange(8) for _ in range(p)] for _ in range(n)]
    edges = [(labels[i],labels[j],rng.randrange(15)) for i in range(n)
             for j in range(i+1,n) if rng.random()<0.35]
    for method in ('heft','peft','lookahead'):
        run(encode(costs,edges),method)
print("Passed classic, gap, lookahead and 300 seeded schedule-legality checks")

# Arabnejad and Barbosa, Figure 1 and Table 3. Validate the placement
# decisions in addition to the independently checked schedule legality.
example = Path(__file__).with_name('peft_fig1.d3').read_text()
result = subprocess.run([sys.argv[1], 'peft'], input=example, text=True,
                        capture_output=True, check=True)
assert list(map(int, result.stdout.splitlines()[0].split())) == [1,4,6,2,3,5,8,7,9,10]
rows = verify(example, result.stdout)
assert [(rows[t][1], rows[t][2]+1) for t in range(10)] == [
    (22,1),(51,1),(83,1),(29,1),(70,3),(46,2),(97,1),(77,2),(89,3),(122,2)]
for method, expected in [('heft',133), ('lookahead',127)]:
    assert max(f for _,f,_ in run(example,method).values()) == expected
