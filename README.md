### Low-Latency Stock Trading & Order Matching Engine

A high-performance, decoupled financial infrastructure simulator combining an optimized C++ algorithmic execution core with a modern Python desktop dashboard interface wrapper.

## 🛠️ System Architecture & Tech Stack
- **Algorithmic Engine Core**: Native C++ utilizing Standard Template Library (STL) containers (`std::vector`), custom price-time priority sorting comparators (`std::sort`), and memory optimization through pass-by-reference pointers.
- **Graphical Dashboard UI**: Python 3 built via `customtkinter` framework, using multi-threaded asynchronous subprocess pipelines to maintain persistent communication with the background C++ execution binary.
- **Algorithmic Complexity**: Optimizes standard matching logic down to an O(1) matching evaluation from the head of pre-sorted data buffers.

## 🚀 System Features
- **Price-Time Priority Queues**: Automatically positions the highest BUY limit bids and lowest SELL ask limits at index 0 for immediate clearing execution.
- **Stateful Memory Maintenance**: Keeps vectors persistently logged across consecutive independent interface order injections.
- **Asynchronous Execution Log**: Live trade confirmations rendering instantly onto terminal displays with detailed execution clearing data.
