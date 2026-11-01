/* Core Text snapshot bridge. No native font escapes a resolve call. */
#include <CoreText/CoreText.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define API __attribute__((visibility("default")))
typedef struct {
    char *family, *face, *postscript;
    double size, ascent, descent, leading, bold, italic;
    const char *error;
} BTMResult;

static char *utf8(CFStringRef text) {
    if (!text) return NULL;
    CFIndex capacity = CFStringGetMaximumSizeForEncoding(CFStringGetLength(text), kCFStringEncodingUTF8) + 1;
    char *copy = malloc((size_t)capacity);
    if (copy && !CFStringGetCString(text, copy, capacity, kCFStringEncodingUTF8)) {
        free(copy);
        return NULL;
    }
    return copy;
}

API void *btm_resolve(const char *family, int64_t role, double size,
                       int64_t bold, int64_t italic, const char *language) {
    BTMResult *result = calloc(1, sizeof(*result));
    if (!result) return NULL;
    CFStringRef name = NULL, lang = NULL;
    CTFontRef font = NULL;
    if (language && *language) {
        lang = CFStringCreateWithCString(NULL, language, kCFStringEncodingUTF8);
        if (!lang) { result->error = "Core Foundation language string unavailable"; goto done; }
    }
    if (role == 0) {
        name = CFStringCreateWithCString(NULL, family, kCFStringEncodingUTF8);
        if (!name) { result->error = "Core Foundation family string unavailable"; goto done; }
        font = CTFontCreateWithName(name, size, NULL);
    } else if (role == 1 || role == 2) {
        font = CTFontCreateUIFontForLanguage(role == 1 ? kCTFontUIFontSystem : kCTFontUIFontSmallSystem, size, lang);
    }
    if (!font) { result->error = "Core Text font unavailable"; goto done; }
    CTFontSymbolicTraits traits = (bold ? kCTFontBoldTrait : 0) | (italic ? kCTFontItalicTrait : 0);
    CTFontRef styled = CTFontCreateCopyWithSymbolicTraits(font, 0, NULL, traits, kCTFontBoldTrait | kCTFontItalicTrait);
    if (styled) { CFRelease(font); font = styled; }
    CFStringRef names[] = { CTFontCopyFamilyName(font), CTFontCopyFullName(font), CTFontCopyPostScriptName(font) };
    char **destinations[] = { &result->family, &result->face, &result->postscript };
    for (unsigned i = 0; i < 3; i++) {
        *destinations[i] = utf8(names[i]);
        if (names[i]) CFRelease(names[i]);
        if (!*destinations[i]) result->error = "Core Text name conversion/allocation failed";
    }
    traits = CTFontGetSymbolicTraits(font);
    result->size = CTFontGetSize(font);
    result->ascent = CTFontGetAscent(font);
    result->descent = CTFontGetDescent(font);
    result->leading = CTFontGetLeading(font);
    result->bold = (traits & kCTFontBoldTrait) != 0;
    result->italic = (traits & kCTFontItalicTrait) != 0;
done:
    if (font) CFRelease(font);
    if (name) CFRelease(name);
    if (lang) CFRelease(lang);
    return result;
}

API const char *btm_error(BTMResult *result) {
    return !result ? "Core Text snapshot allocation failed" : result->error ? result->error : "";
}
API const char *btm_name(BTMResult *result, int64_t field) {
    if (!result) return "";
    const char *value = field == 0 ? result->family : field == 1 ? result->face : result->postscript;
    return value ? value : "";
}
API double btm_value(BTMResult *result, int64_t field) {
    if (!result) return 0;
    switch (field) {
        case 0: return result->size;
        case 1: return result->ascent;
        case 2: return result->descent;
        case 3: return result->leading;
        case 4: return result->bold;
        case 5: return result->italic;
        default: return 0;
    }
}
API void btm_free(BTMResult *result) {
    if (!result) return;
    free(result->family); free(result->face); free(result->postscript); free(result);
}
