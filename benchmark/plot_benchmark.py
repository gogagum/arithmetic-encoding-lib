import argparse
import json
import pandas as pd
import matplotlib.pyplot as plt
from math import log2

def entropy(p, maxOrd, m) -> float:
    mu = maxOrd - m
    q = 1.0 - p
    if q == 0.0:
        return log2(mu)
    if p == 0.0:
        return log2(m)
    return -p * log2(p/m) - q * log2(q/mu)

def minEntropy(maxOrd, m) -> float:
    return entropy(0, maxOrd, m) if (2 * m > maxOrd) else entropy(1, maxOrd, m)

def maxEntropy(maxOrd, m) -> float:
    return entropy(m / maxOrd, maxOrd, m)

class BenchmarksPlot:

    def __init__(self, inFilename: str, outFilename: str, coderType: str,
                 maxOrd: int = 256, m=64) -> None:
        self.inFilename = inFilename
        self.outFilename = outFilename
        self.maxOrd = maxOrd
        self.coderType = coderType
        self.m = m

    def plot(self) -> None:
        with open(self.inFilename) as benchmarkResultsFile:
            data = json.load(benchmarkResultsFile)
            benchmarks = pd.DataFrame(data['benchmarks'])
            self._plot(benchmarks)

    def entropy_(minEntropy, maxEntropy, quarter):
        return minEntropy + (maxEntropy - minEntropy) / 4 * quarter

    def _prepareData(self, data: pd.DataFrame) -> pd.DataFrame:
        data[["coder", "input_size", "h_quarter"]]\
                = data["name"].str.split('/', expand=True)
        data = data.astype({"input_size" : "int64"})
        return data

    def _plot(self, inData: pd.DataFrame):
        inData = self._prepareData(inData)
        benchmarkName = f"BM_benchmark_{self.coderType}_coder"

        def getBenchmark(h_quarter: int):
            return inData.where(inData.coder == benchmarkName)\
                         .where(inData.h_quarter == str(h_quarter))\
                         .dropna()

        data = [getBenchmark(i) for i in range(5)]

        input_size = [data[i]['input_size'] for i in range(5)]
        real_time = [data[i]['real_time'] for i in range(5)]

        minH = minEntropy(self.maxOrd, self.m)
        maxH = maxEntropy(self.maxOrd, self.m)

        fig, ax = plt.subplots(1, 1, figsize=(9, 6))

        for i in range(5):
            entropy = BenchmarksPlot.entropy_(minH, maxH, i)
            plt.plot(input_size[i], real_time[i], label=f"H={entropy:.2f}")

        ax.set_xlabel("Input size")
        ax.set_ylabel("Work time, ms")

        ax.legend()
        ax.set_title(self.coderType)

        fig.savefig(self.outFilename, bbox_inches='tight')

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("in_filename",
                        help="Input json file with benchmark.",
                        type=str)
    parser.add_argument("out_filename",
                        help="Output filename for a plot pdf.",
                        type=str)
    parser.add_argument("coder_type",
                        help="Coder type to plot benchmarks.",
                        type=str)
    args = parser.parse_args()
    plotter = BenchmarksPlot(args.in_filename,
                             args.out_filename,
                             args.coder_type)
    plotter.plot()

