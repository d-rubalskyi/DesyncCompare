# DesyncCompare

Standalone utility to compare UnrealEngine log files.

Useful for:
- Comparing log entries across Cluster Nodes
- Highlighting out of sync log entries
- Filtering log entries by Actor name

Log entry must have the following format:

```
(EntryMark) EntryName: Name, EntryType: Type, EntryInfo
```

Currently EntryMark = "(DataIntegrityTest)"
