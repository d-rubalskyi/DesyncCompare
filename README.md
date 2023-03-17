# DisplayClusterLogCompare

Log compare utility that parses UnrealEngine logs.

Useful to:
- Compare log entries across Cluster Nodes
- Highlight out of sync log entries
- Filter log entries by Actor name

Log entry must have following format

```
(EntryMark) Actor: ActorName, Info: EntryInfo
```

Currently EntryMark = "(DataIntegrityTest)"
