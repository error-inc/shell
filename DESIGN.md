# Design notes

## Architecture
Read line -> tokenize -> check builtins -> fork/exec -> wait.
`argv` is the interface between the parser and the executor.

## Decisions
- `cd` and `exit` are builtins because they must affect the shell
  process itself; a child process can't change the parent's directory.
- `_exit(1)` after a failed `execvp`, so the child doesn't keep running
  as a second shell.

## Bugs and lessons
- Lines over the buffer size were split and run as multiple commands.
  Mitigated by a 1024-byte buffer; proper detection is still to do.
- `argv` wasn't always NULL-terminated for very long lines. Fixed by
  setting the last slot to NULL up front.

## Known limitations
- Lines over 1023 characters are split into separate commands.
- `strtok` doesn't handle quotes (fixed in the next milestone).
