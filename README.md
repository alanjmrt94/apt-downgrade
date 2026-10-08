# apt-downgrade

[![CI](https://github.com/alanjmrt94/apt-downgrade/actions/workflows/ci.yml/badge.svg)](https://github.com/alanjmrt94/apt-downgrade/actions/workflows/ci.yml)

[Español](README-es.md)

Command-line tool to downgrade packages on Ubuntu/Debian.

It finds installed packages matching an exact version (`--current`) and builds an
`apt install package=version` command for the target version (`--downgrade`),
with interactive confirmation.

**Version:** 1.2.0 — multi-language UI, `--json` plan output, localized man pages.

## Usage

```bash
# Help
apt-downgrade --help

# Simulate (no sudo required)
apt-downgrade --current <current_version> --downgrade <target_version> --dry-run

# Machine-readable plan (implies dry-run)
apt-downgrade --current <current_version> --downgrade <target_version> --json

# Apply (sudo only needed to install)
sudo apt-downgrade --current <current_version> --downgrade <target_version>

# Auto-confirm
sudo apt-downgrade --current <current_version> --downgrade <target_version> --yes
```

### Flow

1. Lists packages with `apt list --installed` (no root required).
2. Filters packages that match `--current` exactly.
3. Shows a summary (name, distribution) and the `apt install ...` command.
4. With `--dry-run`, exits without changes.
5. Otherwise asks for confirmation (`y/N`) unless `--yes` is used.
6. Installing requires root; if privileges are missing, it fails with a clear message.

If there are no matches, it exits without changes.

## Local development

### Requirements

```bash
sudo apt update
sudo apt install build-essential debhelper
# To sign and upload to the PPA:
sudo apt install devscripts dput
```

### Build, test, install

```bash
make build          # binary in build/apt-downgrade
make test           # fixture tests (does not modify the system)
./scripts/ci-check-metadata.sh
make install        # install to /usr/bin (use DESTDIR=... for staging)
make uninstall
make package        # creates dist/apt-downgrade_1.2.0-1_amd64.deb
make deb            # debhelper binary package → dist/
make deb-src        # source package → dist/
make sign           # sign the .changes in dist/

```

Artifacts (`.deb`, `.changes`, `.buildinfo`, `.dsc`, …) go under `dist/` (gitignored).

Local-only helpers (gitignored; not pushed): `scripts/build-deb.sh`,
`tests/test-language.sh`, `tests/run-phasing-test.sh`. Keep them on your machine
for manual checks; CI uses `make test` / `scripts/ci-check-metadata.sh` only.

### CI (GitHub Actions)

On every push/PR to `main`:

1. **Lint & metadata** — `shellcheck`, `bash -n`, VERSION/changelog/LICENSE consistency  
2. **Build/test** — `gcc`/`clang` × Ubuntu 22.04/24.04 with `-Werror`, tests and real dry-run  
3. **Package & lintian** — `make package`, `debhelper` build, source package, artifacts  
4. **Distro matrix** — containers `focal`…`resolute`: build, test, `.deb`, and `~series1` source  

It does **not** upload to the PPA (use `make ppa-series-upload` locally with your GPG key).

Install the local `.deb` (only if you want):

```bash
sudo dpkg -i dist/apt-downgrade_1.2.0-1_amd64.deb
```

### Language

Follows the system locale (`LANGUAGE` → `LC_ALL` → `LC_MESSAGES` → `LANG`).
Unsupported locales fall back to **English**.

Force a language:

```bash
APT_DOWNGRADE_LANG=es apt-downgrade --help
APT_DOWNGRADE_LANG=pt_BR apt-downgrade --help
APT_DOWNGRADE_LANG=zh_TW apt-downgrade --help
```

Supported: `en` (default), `es`, `pt`, `pt_BR`, `fr`, `de`, `it`, `ru`,
`zh_CN`, `zh_TW`, `ja`, `ko`, `ar`, `hi`, `nl`, `pl`, `tr`, `gl`, `ca`, `eu`.

### Test environment variable

`APT_DOWNGRADE_LIST_CMD` overrides the installed-package listing command (default
`apt list --installed 2>/dev/null`). Tests use `cat` on a fixture.

## Layout

```
apt-downgrade/
├── src/
│   ├── main.c
│   ├── i18n.c
│   └── i18n.h
├── tests/
│   ├── fixtures/apt-list-sample.txt   # CI
│   └── run_tests.sh                   # CI
├── debian/
│   ├── control
│   ├── rules
│   ├── changelog
│   ├── compat          # 12 (Ubuntu 20.04+)
│   ├── copyright
│   └── source/…        # format + options (PPA tar excludes)
├── man/                # en + localized man1 pages (es, pt, ja, …)
├── scripts/
│   ├── ci-check-metadata.sh           # CI
│   └── ppa-release-series.sh          # maintainer PPA upload
├── .github/workflows/ci.yml           # GitHub Actions (not in PPA tarball)
├── Makefile
├── LICENSE
├── README.md           # English
└── README-es.md        # Español
```

PPA source tarball excludes `.github/`, `.cursor/`, `dist/`, `build/`, `scripts/`,
and `tests/` (see `debian/source/options`).

## Ubuntu 20.04–26.x compatibility

Packaging targets **Ubuntu 20.04 (focal) through 26.x (resolute)**:

| Ubuntu | Series |
|--------|--------|
| 20.04 LTS | `focal` |
| 22.04 LTS | `jammy` |
| 24.04 LTS | `noble` |
| 25.04 | `plucky` |
| 25.10 | `questing` |
| 26.04 LTS | `resolute` |

- `Build-Depends: debhelper (>= 12)` (available since focal).
- On the PPA, Launchpad **builds once per series** (each release uses its own libc).
- Each series is published as `1.2.0-1~SERIES1` (e.g. `1.2.0-1~jammy1`).

## PPA: Ubuntu Tools

PPA: **[ppa:alanjmrt94/ubuntu-tools](https://launchpad.net/~alanjmrt94/+archive/ubuntu/ubuntu-tools)**

- Maintainer: `alanjmrt94 <alanjmartinez94@gmail.com>`
- GPG signing key: `5077A813F9AE818752168EA173140C59FF3EBE5D`  
  (`_shared-keys/launchpad/ubuntu-tools.env`)

### Publish to all series (recommended)

```bash
sudo apt install devscripts dput

# Build + sign for focal jammy noble plucky questing resolute
make ppa-series

# Sign and upload everything to the PPA
make ppa-series-upload
```

Subset:

```bash
SERIES="jammy noble resolute" make ppa-series-upload
```

### Publish only the changelog series (e.g. resolute)

```bash
make deb-src
make sign
make upload
# or: make ppa-release
```

Track builds at:  
https://launchpad.net/~alanjmrt94/+archive/ubuntu/ubuntu-tools

Install:

```bash
sudo add-apt-repository ppa:alanjmrt94/ubuntu-tools
sudo apt update
sudo apt install apt-downgrade
```

### Update

Bump `VERSION` / `DEB_VERSION` in the `Makefile` (and align `debian/changelog`),
then `make ppa-series-upload`.

## Maintainer vs Debian Developer

You are the package maintainer via the `Maintainer:` field in `debian/control`.
That is enough for the PPA. Being an official Debian Developer is a separate
process and is **not** required: https://www.debian.org/devel/join/newmaint

## Notes

- Automated tests **never** perform a real downgrade.
- Do not run `make upload` / `dput` until the `.changes` file is signed.
