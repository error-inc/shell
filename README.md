# shell
Minimalist Shell in C

# mysh

A small Unix shell written in C, built from scratch to learn
process control, pipes, and signals.

## Build and run
    make
    ./shell

## Features
- Run external commands (fork / execvp / waitpid)
- Builtins: `cd`, `exit`

## Roadmap
- [ ] Quote-aware tokenizer
- [ ] Redirection (`>`, `>>`, `<`)
- [ ] Pipes
- [ ] Environment variables and `$?`
- [ ] Signal handling
- [ ] Background jobs and job control

## Notes
See DESIGN.md for design decisions and bugs found along the way.
