---
title: "textmetrics typed API"
summary: "Source-derived public signatures, properties, parameters, defaults, and return values."
category: "Reference"
order: 21
types: ["BaseFaceMatch", "FontRequest", "FontResolution", "FontRole"]
dependencies: []
symbols: ["BaseFaceMatch", "FontRequest", "FontResolution", "FontRole"]
related: ["api.md", "getting-started.md", "index.md"]
---

# textmetrics typed API

Generated from the package source declarations. Platform guards are shown where present; inherited members are documented on the base type. See the [API contracts](api.md) for behavior and limitations.

## GtkFontRole functions

### GTKSystemFontSupported

`Public Function GTKSystemFontSupported() As Boolean`

Requires an already initialized GTK 4 default display; never initializes GTK.

No parameters.

#### Returns

| Type |
| --- |
| `Boolean` |

### GTKFontDPI

`Public Function GTKFontDPI(control As TextControl) As Double`

Read the actual widget Pango context/map resolution, without guessing 96 DPI.

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `control` | `TextControl` | `ByVal` | Required |

#### Returns

| Type |
| --- |
| `Double` |

### ApplyGTKPointFont0720

`Public Function ApplyGTKPointFont0720(control As TextControl, family As String, pointSize As Double) As FontResolution`

Opt-in adaptation for the tested Buoy 0.72.0 GTK FontSize pixel behavior. Do not use after upstream changes FontSize to true typographic point units.

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `control` | `TextControl` | `ByVal` | Required |
| `family` | `String` | `ByVal` | Required |
| `pointSize` | `Double` | `ByVal` | Required |

#### Returns

| Type |
| --- |
| `FontResolution` |

### GTKSystemFontDescription

`Public Function GTKSystemFontDescription() As String`

Snapshot the current default-display gtk-font-name. Does not inspect widget CSS.

No parameters.

#### Returns

| Type |
| --- |
| `String` |

### ResolveGTKSystemFont

`Public Function ResolveGTKSystemFont(size As Double = 0.0, dpi As Double = 96.0) As FontResolution`

GTK uses family and size from gtk-font-name; native Pango resolves the base face. Zero size follows the setting, otherwise explicit point size overrides it.

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `size` | `Double` | `ByVal` | `0.0` |
| `dpi` | `Double` | `ByVal` | `96.0` |

#### Returns

| Type |
| --- |
| `FontResolution` |

## FontRole

`Public Enum FontRole As Integer`

### Values

| Name | Type | Value / Default | Description |
| --- | --- | --- | --- |
| `Family` | `FontRole` | `0` | — |
| `System` | `FontRole` | `1` | — |
| `SmallSystem` | `FontRole` | `2` | — |
| `BackendDefault` | `FontRole` | `3` | — |

## BaseFaceMatch

`Public Enum BaseFaceMatch As Integer`

### Values

| Name | Type | Value / Default | Description |
| --- | --- | --- | --- |
| `NotCompared` | `BaseFaceMatch` | `0` | — |
| `FamilyNameMatches` | `BaseFaceMatch` | `1` | — |
| `DifferentNameOrAlias` | `BaseFaceMatch` | `2` | — |

## FontRequest

`Public Class FontRequest`

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `Role` | `FontRole` | No | No | `FontRole.Family` | — |
| `Family` | `String` | No | No | `""` | — |
| `PointSize` | `Double` | No | No | `12.0` | — |
| `Bold` | `Boolean` | No | No | `False` | — |
| `Italic` | `Boolean` | No | No | `False` | — |
| `DPI` | `Double` | No | No | `96.0` | — |
| `Language` | `String` | No | No | `""` | — |

### Constructor

`Public Sub Constructor(familyName As String, size As Double)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `familyName` | `String` | `ByVal` | Required |
| `size` | `Double` | `ByVal` | Required |

Returns: no value.

## FontResolution

`Public Class FontResolution`

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `RequestedFamily` | `String` | No | No | — | — |
| `RequestedRole` | `FontRole` | No | No | — | — |
| `RequestedPointSize` | `Double` | No | No | — | — |
| `RequestedBold` | `Boolean` | No | No | — | — |
| `RequestedItalic` | `Boolean` | No | No | — | — |
| `DPI` | `Double` | No | No | — | — |
| `Language` | `String` | No | No | — | — |
| `Backend` | `String` | No | No | — | — |
| `ResolvedFamily` | `String` | No | No | — | — |
| `ResolvedFace` | `String` | No | No | — | — |
| `ResolvedPostScriptName` | `String` | No | No | — | — |
| `ResolvedPointSize` | `Double` | No | No | — | — |
| `ResolvedBold` | `Boolean` | No | No | — | — |
| `ResolvedItalic` | `Boolean` | No | No | — | — |
| `StyleMatches` | `Boolean` | No | No | — | — |
| `BaseMatch` | `BaseFaceMatch` | No | No | `BaseFaceMatch.NotCompared` | — |
| `Ascent` | `Double` | No | No | — | — |
| `Descent` | `Double` | No | No | — | — |
| `Leading` | `Double` | No | No | — | — |
| `LineSpacing` | `Double` | No | No | — | — |
| `NativeLineSpacingAvailable` | `Boolean` | No | No | `False` | — |
| `GlyphFallbackKnown` | `Boolean` | No | No | `False` | — |

## TextMetrics functions

### NativeMetricsSupported

`Public Function NativeMetricsSupported() As Boolean`

No parameters.

#### Returns

| Type |
| --- |
| `Boolean` |

### NativeRoleSupported

`Public Function NativeRoleSupported(role As FontRole) As Boolean`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `role` | `FontRole` | `ByVal` | Required |

#### Returns

| Type |
| --- |
| `Boolean` |

### ResolveFont

`Public Function ResolveFont(request As FontRequest) As FontResolution`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `request` | `FontRequest` | `ByVal` | Required |

#### Returns

| Type |
| --- |
| `FontResolution` |
