# FarmWhisper Assets

This directory contains graphical assets compiled directly into the firmware.

These files are intentionally stored as source code (`.h`) rather than image files so they can be placed in program flash (`PROGMEM`) and displayed immediately during boot without requiring a filesystem, SD card, or other external storage.

## Philosophy

FarmWhisper is designed to be:

* Simple
* Local-first
* Repairable
* Fully reproducible from source

Keeping assets under version control as generated headers ensures that every firmware build contains the exact artwork it was developed and tested with.

## Current Assets

### RoosterLogo.h

Monochrome FarmWhisper rooster logo.

Current uses include:

* Boot splash screen
* Future About screen
* Branding throughout the firmware

## Replacing an Asset

When updating artwork:

1. Create or edit the original image.
2. Convert it to a 1-bit monochrome bitmap compatible with the target display.
3. Generate the corresponding C/C++ header.
4. Replace the existing asset while preserving its filename whenever practical.
5. Test the image on real hardware before committing.

If changing image dimensions, verify that the display code is updated accordingly.

## Source Artwork

Whenever possible, retain the original artwork (SVG, PNG, XCF, etc.) in the project or a companion graphics repository so future contributors can regenerate the bitmap instead of reverse-engineering it from generated data.

Generated headers should be considered build artifacts derived from the original artwork.

## Design Guidelines

FarmWhisper graphics should favor:

* Clean, high-contrast artwork
* Readability on small monochrome OLED displays
* Minimal visual clutter
* Consistent branding across devices

The goal is functional graphics that remain recognizable even on low-resolution displays.

---

*"Every generated asset should have an identifiable source, and every source should be reproducible."*
