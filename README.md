# Virtual Sensor

An agent run on each node in HPC system, which is responsible for monitoring & health checking node, push gathered informations to specified `Kafka` topic via configurations.

## How to build

```bash
mkdir -p build && cd build && cmake .. && cmake --build .
```
