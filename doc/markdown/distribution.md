# Distribution and release workflow (LaserBot)

Use this checklist whenever publishing a new firmware version.

## 1) Arduino Library metadata/version

1. Update `/grbl/library.properties`:
   - `version` must be SemVer (for example `1.1.1`).
   - Keep `name`, `url`, and `architectures` accurate.
2. Ensure example sketch exists at:
   - `/grbl/examples/grblUpload/grblUpload.ino`

## 2) Submit to Arduino Library Manager (one-time, then updates are automatic from tags)

1. Open: https://github.com/arduino/library-registry
2. Create a PR editing `repositories.txt` and add:
   - `https://github.com/porrey/grbl-MegaPi.git`
3. Wait for index validation and merge.

Notes:
- Library Manager indexes tagged releases.
- Keep tags stable and SemVer-compatible.

## 3) Create stable GitHub release per firmware update

1. Create a tag, e.g. `v1.1.1`.
2. Publish a GitHub release for that tag.
3. In release notes, include:
   - LaserBot/MegaPi compatibility statement
   - Upgrade steps (Arduino IDE + `grblUpload`)
   - Any breaking/default-setting changes
   - LightBurn profile compatibility notes

## 4) Keep latest stable pointer current

`README.md` points to:
- `https://github.com/porrey/grbl-MegaPi/releases/latest`

No README link changes are needed for each release; only publish a new stable release.
