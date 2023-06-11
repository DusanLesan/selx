# sx4

`sx4` is a simple X11 rectangle selection tool, meant to be a smaller
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

## Example Usage

* Take a screenshot of the selected area with [`sxot`][sxot]:

```console
$ sxot -g "$(sx4)" > out.ppm
```

* Recording the selected area with [`ffmpeg`][ffmpeg]:

```sh
#!/bin/sh

IFS=, read x y w h << EOF
$(sx4)
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
  * Necessary library headers

- Runtime Dependencies:
  * X11 server
  * Xlib
  * Xext

## Building

* Simple build:

```console
$ cc -o sx4 sx4.c -s -l X11 -l Xext
```

* Recommended optimized build:

```console
$ gcc -o sx4 sx4.c -Ofast -march=native -fwhole-program -fno-plt \
    -fno-semantic-interposition -fgraphite-identity -floop-nest-optimize \
    -fipa-pta -fno-asynchronous-unwind-tables -fno-ident -fno-pie \
    -s -no-pie -l X11 -l Xext
```

* Recommended debug build:

```console
$ gcc -o sx4 sx4.c -std=c99 -Wall -Wextra -Wpedantic -Wshadow \
    -g3 -D DEBUG -O0 -fsanitize=address,undefined -l X11 -l Xext
```

* Optionally run some static analysis:

```console
$ make -f etc/analyze.mk
```

## Installing

Just copy the executable and the man-page to the appropriate location:

```console
$ sudo cp sx4 /usr/local/bin
$ sudo cp sx4.1 /usr/local/share/man/man1
```

Or using the `install` utility:

```console
$ sudo install -Dm755 sx4 /usr/local/bin/sx4
$ sudo install -Dm644 sx4.1 /usr/local/share/man/man1/sx4.1
```

## Limitation

May not work properly when a compositor is running.
