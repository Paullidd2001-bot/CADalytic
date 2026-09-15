# Dependency Rules

Allowed direction:

```text
UI -> commands -> document/transactions -> rebuild -> application geometry -> OCCT adapter
```

Core, document, dependency, rebuild, serialisation, and solver model code must not include Qt Widgets. They must not expose or directly include OCCT headers. Geometry adapter implementation may include OCCT. UI and presentation integration may include Qt and OCCT behind a documented boundary.

Features consume application geometry capabilities, not kernel classes. Tests may link any production target under test; production targets must not link tests. Plugins and future Python APIs use documented command and transaction interfaces and cannot mutate internal containers or retain kernel pointers.

Any exception requires an architecture decision record, a named owner, a migration condition, and a test proving the boundary is still controlled.
