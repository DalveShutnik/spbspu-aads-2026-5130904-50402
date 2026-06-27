# F0 — Перекрёстные ссылки. Индексация текста

Текст загружается и сразу превращается в инвертированный индекс (слово —
список позиций), оригинал отбрасывается. Все остальные команды работают с
индексом: он восстанавливается в сетку строк и слов, преобразуется и
индексируется заново. Перекрёстные ссылки выдаются прямо из индекса.

## Структуры данных

- `List` (S1) — односвязный список, лежит в `common`.
- `Stack`, `Queue` (S2) — построены поверх `List`.
- `CuckooTable` (S3) — хэш-таблица на кукушкином хэшировании с двумя хэш-
  функциями; хранит инвертированный индекс, коллекцию документов и таблицу
  команд.

## Команды

Номера строк и слов начинаются с единицы.

```
load <file> <id>            построить индекс по файлу
save <id> <file>            восстановить текст и сохранить
list                        вывести идентификаторы документов
show <id>                   вывести восстановленный текст
drop <id>                   удалить документ
help                        вывести список команд
exit                        завершить работу

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

Команды читаются из стандартного ввода; на некорректную команду выводится
`<INVALID COMMAND>`.
