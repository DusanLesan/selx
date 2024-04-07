# selx

`selx` is a simple X11 rectangle selection tool, meant to be a smaller
alternative to [`slop`][slop].

Some key features:

* Select a rectangle interactively.
* Select a window by right-click.
* Re-adjust the selection area by scroll-wheel up/down.
* Select a window by window ID.
* Adjustable selection line color and border width.

View the manpage for more details.

[slop]: https://github.com/naelstrof/slop

## Preview

![preview](https://images2.imgbox.com/0d/a4/kPvhiJFL_o.gif)

*(Note: the preview image is using the old project name `sx4` before it was
  renamed to `selx`)*

## Example Usage

* Take a screenshot of the selected area with [`sxot`][sxot]:

```console
$ sxot -g "$(selx)" > out.ppm
```

* Recording the selected area with [`ffmpeg`][ffmpeg]:

```sh
#!/bin/sh

IFS=, read x y w h << EOF
$(selx)
EOF
[ -z "$h" ] && exit 1

ffmpeg -f x11grab -s "${w}x${h}" -i ":0.0+$x,$y" -c:v libx264 -an \
	-framerate 30 -r 30 -threads "$(nproc)" -preset slow -tune zerolatency \
	-vf "format=yuv422p" -crf 18 \
	"$HOME/videos/$(date '+%F_%T').mp4"
```

[sxot]: https://codeberg.org/NRK/sxot
[ffmpeg]: https://ffmpeg.org

## Dependencies

- Build Dependencies:
  * C99 compiler
  * Necessary library headers (on some distros you need to install `*-dev`
    packages to get header files)

- Runtime Dependencies:
  * X11 server
  * Xlib
  * Xext
  * libXrandr

## Building

* Simple optimized build:

```console
$ cc -o selx selx.c -O3 -s -l X11 -l Xext -l Xrandr
```

The above command should also work with `c99`, `gcc`, `clang` or any other C
compiler that has a POSIX compatible cli interface.

* Debug build with `gcc` (also works with `clang`):

```console
$ gcc -o selx selx.c -std=c99 -Wall -Wextra -Wpedantic -Wshadow \
    -g3 -D DEBUG -O0 -fsanitize=address,undefined -l X11 -l Xext -l Xrandr
```

* If you're editing the code, you may optionally run some static analysis:

```console
$ make -f etc/analyze.mk
```

## Installing

Just copy the executable and the man-page to the appropriate location:

```console
# cp selx /usr/local/bin
# cp selx.1 /usr/local/share/man/man1
```

Or using the `install` utility:

```console
# install -Dm755 selx /usr/local/bin/selx
# install -Dm644 selx.1 /usr/local/share/man/man1/selx.1
```

## Limitation

May not work properly when a compositor is running.
