# Worked examples from the baseline papers (fidelity checks)

Format d3: `n p`, the n x p computation matrix, the n x n communication matrix (-1 = no edge).

| File | Source | Paper's result | Reproduced |
|:--|:--|:--|:--|
| `lookahead_heft_fig1.d3` | Bittencourt, Sakellariou, Madeira, PDP 2010, Fig. 1 (doi:10.1109/PDP.2010.56); edge weights are the data sizes on equal-capacity links | HEFT 260, Look-ahead HEFT 184 | `la_heft` 184 (insertion and `append`), `heft_ins` 260, repository `heft` 260; every task's placement matches the figure |
| `aeft_fig1.d3` | Wang et al., FGCS 164 (2025) 107576, Fig. 1 / Table 3 (doi:10.1016/j.future.2024.107576) | AEFT 76; PEFT 85; HEFT 80; IPPTS 94 | `aeft_ins` and repository `aeft` 76 with every step of Table 3; `peft` 85; `ippts` 96 (95 with insertion) |

Other algorithms on `lookahead_heft_fig1.d3`: CGS 184, PEFT-ins 184, IPPTS-ins 184, BPPTS-ins 201, SCCT-ins 201, AEFT-ins 227.
