# Tests

## How to run tests

From the repo root:

```bash
cmake -S Core/Test -B build-tests
cmake --build build-tests --config Debug
ctest --test-dir build-tests -C Debug --output-on-failure
```
