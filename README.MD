gfxi
====
A PC-98 graphics library.
-------------------------

Building
--------
This library currently supports two toolchains: Borland Turbo-C++ (tested on 4.0J) and OpenWatcom (tested on 1.9).

To build on Turbo-C++:

`$ make /f MAKEFILE.TURBOC all`

To build on OpenWatcom, get a `make` implementation like [GNU Make](http://www.gnu.org/software/make/) or [NMake](http://msdn.microsoft.com/en-us/library/dd9y37ha.aspx). `wmake` will *not* suffice.

`$ make -f MAKEFILE.WATCOM` (GNU Make)

`$ make /f MAKEFILE.WATCOM all` (NMake)

These commands will generate `gfx.lib`, which you can statically link with your application using `tlink` (Turbo-C++) or `wlink` (OpenWatcom). Refer to the respective toolchains' manuals for more information.

License
-------
gfxi is licensed under the [WTFPL](http://wtfpl.net): see `LICENSE` for details.
