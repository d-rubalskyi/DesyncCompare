# DisplayClusterLogCompare

Utility to compare UnrealEngine log files.

Useful to:
- Compare log entries across Cluster Nodes
- Highlight out of sync log entries
- Filter log entries by Actor name

Log entry must have following format

```
(EntryMark) Actor: ActorName, EntryInfo
```

Currently EntryMark = "(DataIntegrityTest)"
