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
- Added enumeration for state for deciding if char is in single quote or double quote or plain text (rather than using int states, so its more readable)
- Made run\_builtin(), run\_external(), tokenizer() functions static so they have local scope in file.
- Added return code for run\_external() function and others too for later adding exit code checking through "$?"

## Known limitations
- Lines over 1023 characters are split into separate commands.
- `strtok` doesn't handle quotes (fixed in the next milestone).

## Tokenizer
- Custom Tokenizer uses inplace editing with r/w index design and then the input is tokenized
