import argparse
import subprocess

coders = {"ppma", "ppmd", "esc_ppma", "esc_ppmd"}

class Benchmarker:

    def __init__(self, coder: str, outFilename: str) -> None:
        self.coder = coder
        self.outFilename = outFilename

    def runBenchmark(self) -> None:
        subprocess.run(f"../build/benchmark/benchmark_adaptive_coders"
                       " --benchmark_out={self.outFilename}"
                       " --benchmark_filter=BM_benchmark_{self.coder}"
                       " --benchmark_out_format=json")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("coder", "Which coder to test.")
    parser.add_argument("benchmark_filename", "Filename.")
    args = parser.parse_args()
    
    