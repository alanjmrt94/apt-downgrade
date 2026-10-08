# apt-downgrade

[English](README.md)

Herramienta de línea de comandos para hacer downgrade de paquetes en Ubuntu/Debian.

Busca paquetes instalados con una versión exacta (`--current`) y arma un
`apt install paquete=versión` hacia la versión objetivo (`--downgrade`), con
confirmación interactiva.

**Versión:** 1.1.0 — interfaz multilenguaje (locale del sistema, fallback inglés).

## Uso

```bash
# Ayuda
apt-downgrade --help

# Simular (no requiere sudo)
apt-downgrade --current <version_actual> --downgrade <version_objetivo> --dry-run

# Aplicar (requiere sudo solo para instalar)
sudo apt-downgrade --current <version_actual> --downgrade <version_objetivo>

# Confirmar automáticamente
sudo apt-downgrade --current <version_actual> --downgrade <version_objetivo> --yes
```

### Flujo

1. Lista paquetes con `apt list --installed` (no requiere root).
2. Filtra los que coinciden exactamente con `--current`.
3. Muestra resumen (nombre, distribución) y el comando `apt install ...`.
4. Con `--dry-run` termina sin cambios.
5. Sin dry-run, pide confirmación (`s/N` o `y/N` según idioma) salvo que uses `--yes`.
6. La instalación exige root; si no hay privilegios, falla con un mensaje claro.

Si no hay coincidencias, finaliza sin cambios.

## Desarrollo local

### Requisitos

```bash
sudo apt update
sudo apt install build-essential debhelper
# Para firmar y subir al PPA:
sudo apt install devscripts dput
```

### Compilar, testear e instalar

```bash
make build          # binario en build/apt-downgrade
make test           # tests con fixtures (sin tocar el sistema)
./scripts/ci-check-metadata.sh
make install        # instala en /usr/bin (usar DESTDIR=... para staging)
make uninstall
make package        # genera dist/apt-downgrade_1.1.0-1_amd64.deb
make deb            # paquete binario con debhelper → dist/
make deb-src        # paquete fuente → dist/
make sign           # firma el .changes en dist/

```

Los artefactos (`.deb`, `.changes`, `.buildinfo`, `.dsc`, …) quedan en `dist/` (gitignore).

Helpers solo locales (gitignore; no se pushean): `scripts/build-deb.sh`,
`tests/test-language.sh`, `tests/run-phasing-test.sh`. El CI usa `make test` /
`scripts/ci-check-metadata.sh`.

### CI (GitHub Actions)

En cada push/PR a `main` corre:

1. **Lint & metadata** — `shellcheck`, `bash -n`, consistencia VERSION/changelog/LICENSE  
2. **Build/test** — matriz `gcc`/`clang` × Ubuntu 22.04/24.04 con `-Werror`, tests y dry-run real  
3. **Package & lintian** — `make package`, build con `debhelper`, paquete fuente, artefactos  
4. **Distro matrix** — contenedores `focal`…`resolute`: build, test, `.deb` y source `~serie1`  

No sube al PPA (eso sigue siendo `make ppa-series-upload` en local con tu GPG).

Instalar el `.deb` local (solo si lo querés):

```bash
sudo dpkg -i dist/apt-downgrade_1.1.0-1_amd64.deb
```

### Idioma

Sigue el locale del sistema (`LANGUAGE` → `LC_ALL` → `LC_MESSAGES` → `LANG`).
Si el idioma no está soportado, usa **inglés**.

Forzar idioma:

```bash
APT_DOWNGRADE_LANG=es apt-downgrade --help
APT_DOWNGRADE_LANG=pt_BR apt-downgrade --help
APT_DOWNGRADE_LANG=zh_TW apt-downgrade --help
```

Idiomas soportados: `en` (default), `es`, `pt`, `pt_BR`, `fr`, `de`, `it`, `ru`,
`zh_CN`, `zh_TW`, `ja`, `ko`, `ar`, `hi`, `nl`, `pl`, `tr`, `gl`, `ca`, `eu`.

### Variable de entorno (tests)

`APT_DOWNGRADE_LIST_CMD` permite reemplazar el comando de listado (por defecto
`apt list --installed 2>/dev/null`). Los tests usan `cat` sobre un fixture.

## Estructura

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
│   └── source/…        # format + options (exclusiones del tarball PPA)
├── man/apt-downgrade.1
├── scripts/
│   ├── ci-check-metadata.sh           # CI
│   └── ppa-release-series.sh          # subida PPA (maintainer)
├── .github/workflows/ci.yml           # GitHub Actions (no va en el tarball PPA)
├── Makefile
├── LICENSE
├── README.md           # English
└── README-es.md        # Español
```

El tarball fuente del PPA excluye `.github/`, `.cursor/`, `dist/`, `build/`,
`scripts/` y `tests/` (ver `debian/source/options`).

## Compatibilidad Ubuntu 20.04–26.x

El código y el empaquetado apuntan a **Ubuntu 20.04 (focal) hasta 26.x (resolute)**:

| Ubuntu | Serie |
|--------|--------|
| 20.04 LTS | `focal` |
| 22.04 LTS | `jammy` |
| 24.04 LTS | `noble` |
| 25.04 | `plucky` |
| 25.10 | `questing` |
| 26.04 LTS | `resolute` |

- `Build-Depends: debhelper (>= 12)` (disponible desde focal).
- En el PPA, Launchpad **compila una vez por serie** (el binario de cada release usa su propia libc).
- Cada serie se publica con versión `1.1.0-1~SERIE1` (ej. `1.1.0-1~jammy1`).

## PPA: Ubuntu Tools

PPA: **[ppa:alanjmrt94/ubuntu-tools](https://launchpad.net/~alanjmrt94/+archive/ubuntu/ubuntu-tools)**

- Maintainer: `alanjmrt94 <alanjmartinez94@gmail.com>`
- Firma GPG: `5077A813F9AE818752168EA173140C59FF3EBE5D`  
  (`_shared-keys/launchpad/ubuntu-tools.env`)

### Publicar en todas las series (recomendado)

```bash
sudo apt install devscripts dput

# Generar + firmar para focal jammy noble plucky questing resolute
make ppa-series

# Firmar y subir todo al PPA
make ppa-series-upload
```

Subconjunto:

```bash
SERIES="jammy noble resolute" make ppa-series-upload
```

### Publicar solo la serie del changelog (ej. resolute)

```bash
make deb-src
make sign
make upload
# o: make ppa-release
```

Seguí los builds en:  
https://launchpad.net/~alanjmrt94/+archive/ubuntu/ubuntu-tools

Instalar:

```bash
sudo add-apt-repository ppa:alanjmrt94/ubuntu-tools
sudo apt update
sudo apt install apt-downgrade
```

### Actualizar

Bump `VERSION` / `DEB_VERSION` en el `Makefile` (y alineá `debian/changelog`),
luego `make ppa-series-upload`.

## Maintainer vs Debian Developer

Sos maintainer del paquete con el campo `Maintainer:` en `debian/control`.
Eso alcanza para el PPA. Ser Debian Developer oficial es otro proceso y **no**
es necesario: https://www.debian.org/devel/join/newmaint

## Notas

- Los tests automáticos **nunca** ejecutan un downgrade real.
- No ejecutes `make upload` / `dput` hasta haber firmado el `.changes`.
