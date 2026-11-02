# Font metrics in points

`textmetrics` implements Explicit font requests, native base-face resolution and logical-point metrics.

## Install

From a Buoy consumer project with BPM installed:

```sh
bpm add textmetrics --git https://github.com/BuoyBay/textmetrics.git --rev refs/heads/main
```

BPM records the resolved commit in its lockfile. Keep that lockfile with the
consumer project for reproducible installs.

## API entrypoints

- `CTFontCreateUIFontForLanguage`
- `NativeMetricsSupported()`
- `NativeRoleSupported(role)`
- `UnsupportedOperationException`
- `LibraryUnavailableException`

## Build and API reference

Source is in `modules/`, with native/runtime inputs where required. Use the
[package guide](docs/API.md) for backend-specific build prerequisites, ownership
rules and supported targets. Rebuilding a native backend requires that target's
compiler and SDK; sealing an archive alone does not rebuild the backend.

## Contributing

[Contributing](CONTRIBUTING.md) · [License](LICENSE)
