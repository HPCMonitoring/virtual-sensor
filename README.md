# Virtual Sensor

An agent run on each node in HPC system, which is responsible for monitoring & health checking node, push gathered informations to specified `Kafka` topic via configurations.

## Prerequisite

Install [`librdkafka`](https://github.com/confluentinc/librdkafka):

```bash
sudo apt install librdkafka-dev
```

## Some commands

Build to binary:

```bash
make compile
```

Test memory leak:

```bash
make test_memory_leak
```

## Project structure

```cpp
📦include                           // All header files go here
 ┣ 📜common.h                               // Common typedef, reusable functions and classes
 ┣ 📜exceptions.h                           // Contain all exception types
 ┣ 📜kafka_producer.h                       // Kafka producer
 ┣ 📜main.h                                 // All libaries (standard & 3rd-party) was included here
 ┣ 📜process.h                              // Process info
 ┗ 📜sensor.h                               // Sensor
📦src                               // Implementaion files
 ┣ 📜main.cpp                               // Program entry
 ┗ 📜process.cpp                            
📦.github
 ┗ 📂workflows                      // CI/CD scripts
📦test                              // Tests
📜.gitignore                        // Git ignore file
📜CMakeLists.txt                    // Project build configurations
📜Makefile                          // Contains some helpful scripts
📜README.md
```

## Coding rules, conventions and considerations

**Rules**:

- **NO** unused variables
- **NO** hard codes
- Use header guard when declare a header file and its implementation file
- Follow [`RAII`](https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization) rules
- Follow [`Rule of three`](https://en.wikipedia.org/wiki/Rule_of_three_%28C++_programming%29)
- Unused method must throw `Unimplemented` exception

**Conventions**:

- Private method should have name with prefix `_`
- Class name must be in `CamelCase` with first letter capitalized
- Method name must be in `camelCase` with first letter in lowercase
- Header guard name must be `__CAPITALIZED_SNAKE_CASE__` with prefix & postfix are `__`. Example: `__COMMON_H__`
- Filename must be in snake case `snake_case`

**Consider to**:

- Use inline function when task has a seperate logic from other bodies of code
- Use `const` for not modified variables and parameters
- Use `typedef` to set variable's type a meaningful name
- Pass by reference for object values
- Minimalize dependencies as much as possible

## Testing tools

To host a kafka server in local (default port 9092), run:

```bash
docker-compose up -d
```

Create a topic named `purchases`:

```bash
docker-compose exec broker \
  kafka-topics --create \
    --topic purchases \
    --bootstrap-server localhost:9092 \
    --replication-factor 1 \
    --partitions 1
```

To start a kafka consumer (subscribe topic `purchases`) for testing, run:

```bash
cd checktools && ./consumer config.ini
```
