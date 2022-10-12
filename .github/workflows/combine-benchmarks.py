from pathlib import Path

import pyperf

for root in ("head", "base"):
    files = list(Path().glob(f"**/{root}-*.json"))
    suite = pyperf.BenchmarkSuite.load(str(files[0]))
    for file in files[1:]:
        subsuite = pyperf.BenchmarkSuite.load(str(file))
        for bench in subsuite.get_benchmarks():
            suite._add_benchmark_runs(bench)
    suite.dump(f"{root}-combined.json")
