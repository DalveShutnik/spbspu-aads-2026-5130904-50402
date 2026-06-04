# F0 — Cross-references. Text indexing

A text is loaded and immediately turned into an inverted index (word to
positions); the original text is dropped. Every other command works on the
index: it is restored to a grid of lines and words, transformed, and indexed
again. Cross-references are answered straight from the index.

## Data structures

- `List` (S1) — singly linked list, stored in `common`.
- `Stack`, `Queue` (S2) — built on top of `List`.
- `CuckooTable` (S3) — hash table using cuckoo hashing with two hash
  functions; it stores the inverted index, the document collection and the
  command table.

## Commands

Indices that name a line or a word are 1-based.

```
load <file> <id>            build the index for a file
save <id> <file>            restore the text and write it
list                        list document ids
show <id>                   print the restored text
drop <id>                   remove a document
help                        list commands
exit                        stop

replace <id> <line> <word> <new>
swap <id> <line1> <word1> <line2> <word2>
insert-line <id> <pos> <words...>
remove-line <id> <pos>

concat <id1> <id2> <new_id>
concat-lines <id1> <id2> <new_id>
repeat-vertical <id> <k> <new_id>
repeat-horizontal <id> <k> <new_id>
interleave-lines <id1> <id2> <new_id>
interleave-words <id1> <id2> <new_id>
reverse-lines <id> <new_id>
reverse-words <id> <new_id>
transpose <id> <new_id>

find <id> <word> [--limit=k] [--from=start|end] [--edge=left|right] [--context=k]
dump-index <id>
```

Commands are read from standard input; an invalid command prints
`<INVALID COMMAND>`.
