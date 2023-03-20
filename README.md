# DisplayClusterLogCompare

Standalone utility to compare UnrealEngine log files.

Useful for:
- Comparing log entries across Cluster Nodes
- Highlighting out of sync log entries
- Filtering log entries by Actor name

Log entry must have the following format:

```
(EntryMark) Actor: ActorName, Type: CategoryType, EntryInfo
```

Currently EntryMark = "(DataIntegrityTest)"
