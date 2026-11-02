# TextMetrics 0.1.0

Explicit font requests, observable native base-face resolution, and font metrics
for Buoy 0.72.0. Import `textmetrics.TextMetrics` (or `textmetrics` for the core API).
No renderer, face enumeration, font installation, file registration or glyph
coverage API is included. FontKit owns discovery and registration; ShapingKit
owns Linux shaped text drawing and measurement.

## Capability

| Platform | Explicit family | System / small system | Backend default | Runtime scope |
| --- | --- | --- | --- | --- |
| Linux, both architectures | PangoCairo base font, Pango >= 1.44 | Explicitly unsupported | Pango context's family at caller's explicit point size | x86_64 Linux native tests and direct Pango reference comparison |
| macOS, both architectures | Core Text matching | `CTFontCreateUIFontForLanguage`: system / smallSystem; zero size requests native default | Unsupported | ARM64 native suite and independent Swift/CoreText reference with stock linker resources |
| Windows, both architectures | GDI selected base font | System uses native message font; SmallSystem explicitly unsupported | GDI stock DEFAULT_GUI_FONT at caller's explicit size | x86_64 Windows native tests and independent C/GDI reference comparison |

`NativeMetricsSupported()` checks Linux library availability and Pango version;
it reports true on macOS and checks GDI/User32 availability on Windows. It does not guarantee a specific
font exists or that native resource allocation will succeed.
`NativeRoleSupported(role)` reports the implemented policy above. Unsupported
requests throw `UnsupportedOperationException`; they never silently pick an
invented family or size. Optional Linux FFI keeps a consumer launchable without
Pango installed. Missing native symbols may throw `LibraryUnavailableException`. The Windows System role requires Windows 10 version 1607+ (`SystemParametersInfoForDpi`).
Linux and Windows use Buoy source with system libraries. macOS uses a small
SDK-built `buoy_textmetrics` bridge so consumers do not require the missing
CoreText stub in Buoy 0.72.0. The bridge returns copied scalar/name snapshots,
retains no native fonts, and preserves the public API.

## Opt-in GTK 4 system-font adapter

Import `textmetrics.GtkFontRole As GTKFonts` and call
`GTKFonts.ResolveGTKSystemFont(size=0, dpi=96)` on the GUI thread after the
application has initialized GTK 4 with a default display. The helper never
initializes GTK or opens a display. `GTKSystemFontSupported()` reports that
precondition; `GTKSystemFontDescription()` snapshots the actual default-display
`gtk-font-name` string. Other platforms and uninitialized GTK fail explicitly.
The core Linux `NativeRoleSupported(System)` remains false: a headless Pango
backend default cannot stand in for the desktop's toolkit setting.

The adapter parses the setting with Pango, uses its family and size as GTK
specifies, converts absolute `px` sizes to points using caller DPI, and lets a
positive size override the setting. GTK uses family and size from this setting;
style words do not turn the request bold/italic. Per-widget CSS, font features,
variable axes, accessibility scaling outside the setting, nondefault displays,
and themes with widget-specific font overrides are outside this snapshot. Comma-separated family lists are explicitly unsupported by this single-family
resolver. The result retains an explicit family request; it does not pretend to be a universal
Linux System role. No small-system size is guessed. Serialized font registration
and resolution rules still apply.

## Explicit GTK point conversion for Buoy 0.72.0

Native inspection on this Linux x86-64 build shows `TextControl.FontSize = 24`
produces a 24-pixel run at a 96-DPI Pango context, although the control reference
calls the property a point size. `GTKFontDPI(control)` reads the actual widget
context/map resolution. Opt-in `ApplyGTKPointFont0720(control, family, pointSize)`
resolves an explicit point request, preserves the control's bold/italic requests,
and assigns the converted pixel size to this runtime's property. It returns the
base-face resolution snapshot and never changes global GTK settings.

The helper is explicitly for the observed 0.72.0 behavior. Do not use it after
upstream changes that property to typographic points; it is not automatically
selected or fingerprint-guarded. Reapply after relevant widget DPI changes.
Tests inspect the actual GtkLabel display run and a GtkEntry-created context
layout: an 18-point helper request at 96 DPI matches the 24-pixel fixture run.
The TextField's private display layout and other platforms' control units are
not inferred from that narrower entry-context check.

GTK point conversion operates in logical Pango/Cairo units. It does not establish physical display sizing or infer another control backend's text units.

## Requests and results

Construct `FontRequest(familyName, size)` and optionally set its public properties
before passing it to `ResolveFont(request)`. Requests are revalidated on every
call, including after mutation. Returned results are detached snapshots, retain
no native font objects, and do not change when the original request is mutated.
Serialize application font registration and resolution with font-dependent work;
this package does not coordinate concurrent changes to the native font registry.

| Request property | Meaning |
| --- | --- |
| `Role` | `FontRole.Family` by default; also `System`, `SmallSystem`, `BackendDefault` |
| `Family` | One native family name; must be nonblank for Family and empty for every other role. Comma-separated fallback lists are rejected. |
| `PointSize` | Logical points. Positive explicit size required for Family/BackendDefault; zero only means native size for System/SmallSystem. |
| `Bold`, `Italic` | Normal/bold and upright/italic style requests; both default false. Underline is a decoration and is outside font metrics. |
| `DPI` | Finite 1..9600, default 96; Linux context conversion scale; Windows rounds to an integer DPI for GDI pixel conversion. Point size itself is never multiplied. |
| `Language` | Native language hint; empty uses ambient Pango language or native Core Text UI default. Pango uses it for font loading and metrics. Core Text uses it for UI font creation; its explicit-name creation and scalar metrics have no language argument. Windows GDI ignores the language hint; it is preserved in the result. It is not a validated BCP47 parser. |

Sizes must be finite, either zero under the role rules or in 1/64..16384 points,
and at most 65536 device units at the requested DPI. These conservative limits
keep Pango's integer size/metric arithmetic bounded. ASCII control characters
and DEL in family/language are rejected. The current Buoy string runtime cannot
represent embedded NUL faithfully: on this host `String.Chr(0)` has zero length
and concatenating it removes the character. No package-level String validator
can detect information already lost before the call. No NUL-validation guarantee
is claimed; a length-bearing upstream String/FFI change is needed for that.

`FontResolution` preserves `RequestedFamily`, `RequestedRole`,
`RequestedPointSize`, `RequestedBold`, `RequestedItalic`, `DPI` and `Language`.
It reports `Backend`, `ResolvedFamily`, `ResolvedFace`, `ResolvedPointSize`,
`ResolvedBold`, `ResolvedItalic`, `StyleMatches`, and `BaseMatch` separately.
Linux `ResolvedFace` is the native Pango font description, including size/style;
it is not a unique installed-face identifier. macOS reports the native full name
and `ResolvedPostScriptName`; Linux leaves PostScript name empty rather than
inventing it. Windows reports `GetTextFaceW` in both family and face, with no PostScript name or full physical-face name guarantee. GDI family requests exceeding 31 UTF-16 units are rejected rather than truncated. Native names can be localized and can change with OS/font versions.

`BaseMatch` is deliberately conservative:

- `NotCompared`: a role/default request has no explicit family to compare.
- `FamilyNameMatches`: trimmed requested and resolved family names match after
  simple Unicode lowercasing; this is not a font-file availability assertion.
- `DifferentNameOrAlias`: names differ. This includes missing-family substitution,
  generic aliases such as `sans`, PostScript/full-name aliases, and localized
  naming differences. It does not prove which case occurred. Use FontKit's face
  discovery for independent exact installed-family checks.

`StyleMatches` compares the native logical face's reported bold/italic attributes
with the request. Pango oblique also satisfies italic. Windows reports actual selected TEXTMETRIC weight/italic attributes, including native synthesis. A requested style may be
synthesized by the native engine; neither this flag nor matching names prove a
separately installed physical face. If Core Text cannot create the requested
trait variant, the original face is retained and its actual traits are reported.
On Linux, Pango's loaded-font description is reported even when matching uses
substitution or synthesis. Variable axes, weight ranges and synthesis provenance
are not exposed.

`GlyphFallbackKnown` is always false. This is a single base font resolution,
without text or shaping. Even `FamilyNameMatches` plus `StyleMatches` says nothing
about missing glyphs, per-character font runs, emoji, ligatures or script support.
No boolean named “font available” or “glyph supported” is fabricated.

## Metric units and backend consistency

`Ascent`, `Descent`, `Leading`, `LineSpacing` are logical points, with positive
ascent/descent magnitudes. `LineSpacing` is a baseline-to-baseline recommendation,
not a glyph ink box or a string's actual rendered height. Linux converts native
Pango units by `72 / DPI / 1024`; point sizes are quantized to Pango's 1/1024-point
units. Pango supplies native line height when available. Otherwise
`NativeLineSpacingAvailable` is false, spacing is ascent + descent and leading
is zero. Where native height is supplied, leading is height - ascent - descent;
negative values are preserved. Core Text supplies ascent/descent/leading directly
in points, and spacing is their sum. DPI is retained but never rescales Core Text
point metrics. Windows converts selected GDI pixel metrics by `72 / roundedDPI`; ascent includes internal leading, Leading is external leading, and LineSpacing is text height plus external leading. ResolvedPointSize is the selected em height (text height minus internal leading) converted back to points; raster quantization can differ from the request. Hinting can make Linux logical metrics differ slightly with DPI.
Convert to device units only at the drawing boundary: points * DPI / 72.

The backend must match the renderer for layout to agree:

- Built-in `Graphics` documents Quartz/Core Text on macOS, GDI+ text on Windows, and Cairo on Linux.
  It exposes string width/height and style flags but no resolved base-face or
  glyph-fallback identity. These Pango base-font metrics do not replace its
  `MeasureString*` values or prove its draw/measure paths select the same face.
- Native controls obtain fonts and line layout through their toolkit/theme.
  PangoCairo's backend default is **not** a claim about a GTK/Qt desktop's UI font.
  GTK has a toolkit-specific `gtk-font-name` setting; a caller can supply its
  actual family/size explicitly. There is no universal Linux small-system-font
  role here, and no arbitrary point-size subtraction is used. GTK initialization,
  display selection and per-widget CSS stay with the application/toolkit.
- Core Text UI roles request actual system/small-system fonts and native sizes,
  without hardcoding Helvetica, San Francisco or point sizes. The returned font
  is a Core Text match, not proof that a particular AppKit control uses it after
  theme, accessibility or per-control changes.
- ShapingKit's Linux Pango/Cairo path is the appropriate companion for shaped
  text. Match family, point size, styles, DPI and language, along with renderer
  font options. TextMetrics does not mutate that renderer or guarantee equivalent
  string extents: fallback runs can increase a shaped line's height.
- Windows resolves a GDI HFONT selected into an MM_TEXT memory DC. System means the native DPI-specific `lfMessageFont`, not menu/caption fonts. BackendDefault means the legacy GDI stock GUI font, not the current themed system UI font. SmallSystem remains unsupported because Windows has no portable equivalent; the small-caption role is not substituted. Metrics match this GDI selection path, not Buoy Canvas's GDI+ or DirectWrite. No existing control is modified.
- PDF uses its own font resolver: document-loaded fonts, libharu Base14 fonts and
  aliases, then Helvetica fallback. TextMetrics/Pango/Core Text names are not PDF
  registration keys. Measure using the PDF backend for PDF output; this package
  does not embed fonts or guarantee PDF metrics match screen rendering.

## Build

See the [TextMetrics source guide](https://github.com/BuoyBay/textmetrics/blob/main/README.md) for package installation and native library prerequisites. Build a target-specific native bridge before sealing when adding a runtime slice.

## Remaining implementation work and upstream patch requirements

This checkout packages Buoy and libraries; upstream built-in compiler/runtime
implementations were not found among its sources. Library work cannot change
built-in `Graphics`, native control defaults or PDF's resolver. To make built-in
semantics consistent, upstream changes must:

1. Add a shared explicit font-request/result ABI with role, point size, DPI,
   styles, native base identity and capability/fallback status; expose scalar
   ascent/descent/line spacing independently of string ink bounds.
2. Route Linux built-in draw and string measure through the same Pango layout
   configuration rather than selecting one font through Cairo's toy text path;
   preserve baseline/coordinate semantics and expose fallback-run identities
   only after native shaping has actually produced them.
3. Use native Core Text/AppKit UI roles on macOS; query each Windows UI role and
   its DPI-specific size. Distinguish the toolkit's default font from backend
   aliases. Propagate font/style changes through native controls and invalidate
   dependent measurement caches.
4. Keep the PDF resolver/document embedding explicit; report aliases/Base14
   substitution rather than pretending a screen face name guarantees embedding.
5. Preserve string byte length across String construction and FFI validation so
   embedded NUL can be rejected before C-string conversion truncates it.

## Primary references

- [Pango font descriptions and loading](https://docs.gtk.org/Pango/pango_fonts.html)
- [Pango font metrics and language](https://docs.gtk.org/Pango/method.Font.get_metrics.html)
- [Pango line height, including unavailable height](https://docs.gtk.org/Pango/method.FontMetrics.get_height.html)
- [PangoCairo point/device-unit DPI conversion](https://docs.gtk.org/PangoCairo/func.context_set_resolution.html)
- [GTK 4 toolkit default font setting](https://docs.gtk.org/gtk4/property.Settings.gtk-font-name.html)
- [GTK initialized state](https://docs.gtk.org/gtk4/func.is_initialized.html)
- [Pango description parsing](https://docs.gtk.org/Pango/type_func.FontDescription.from_string.html)
- [Apple Core Text UI font roles](https://developer.apple.com/documentation/coretext/ctfontuifonttype)
- [Apple native UI font creation](https://developer.apple.com/documentation/coretext/ctfontcreateuifontforlanguage(_:_:_:))
- [Windows native UI font roles](https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-nonclientmetricsw)
- [Windows selected text metrics](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-gettextmetricsw)
- [Windows selected typeface identity](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-gettextfacew)
- [Windows DPI-specific native settings](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-systemparametersinfofordpi)

Buoy grammar/FFI and built-in backend contracts were read from installed
`/usr/share/buoy/docs/documentation/{ffi-declare,declare-selector,cstring,optional-declare,graphics,pdf,strings,standard-library}.md`.

## macOS integration

The macOS implementation uses an SDK-built Core Text bridge and returns copied font identity and metric snapshots. Intel macOS native execution remains unverified.
