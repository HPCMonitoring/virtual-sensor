# Virtual Sensor

An agent run on each node in HPC system, which is responsible for monitoring & health checking node, push gathered informations to specified `Kafka` topic via configurations.

## Some commands

Build to binary:

```bash
make compile
```

Test memory leak:

```bash
make check_memory_leak
```

## Coding rules & conventions

Rules:

- No unused variables
- Prefer use const for not modified variables and parameters
- Unused method must throw "Unimplemented !" exception
- Use header guard when declare a header file and its implementation file
- NO hard codes
- Use RAII, not naked pointers

Conventions:

- Private method should have name with prefix `_`
- Class name must be in camel case with first letter capitilized `CamelCase`
- Method name must be in camel case with first letter in lowercase `camelCase`
- Header guard name must be look like this `__<header name>_H__`

Remarks:

- Consider use inline function when task has a seperate logic from other bodies of code
- Minimalize dependencies as much as possible
