#!/bin/sh

export skip_resolve=$1
export CFLAGS="$CFLAGS -DMACOSX -mmacosx-version-min=10.5"
export CXXFLAGS="$CXXFLAGS -DMACOSX -mmacosx-version-min=10.5"
set -e
cmake .
make

sudo -E sh << "EOF"
rm -rf /Applications/HeeksCAD.app
mkdir -p /Applications/HeeksCAD.app/Contents
mkdir -p /Applications/HeeksCAD.app/Contents/MacOS
cp -R bin/heekscad* /Applications/HeeksCAD.app/Contents/MacOS
mkdir -p /Applications/HeeksCAD.app/Contents/MacOS/wx
mkdir -p /Applications/HeeksCAD.app/Contents/MacOS/oce
mkdir -p /Applications/HeeksCAD.app/Contents/share/heekscad
cp -R ./bitmaps /Applications/HeeksCAD.app/Contents/share/heekscad
cp -R ./fonts /Applications/HeeksCAD.app/Contents/share/heekscad
cp -R ./icons /Applications/HeeksCAD.app/Contents/share/heekscad
EOF

if [ -d heekscnc ]; then
	cd heekscnc
	cmake .
	make
	cd ..
fi

sudo -E sh << "EOF"
if [ -f heekscnc/bin/libheekscnc.dylib ]; then
	mkdir -p /Applications/HeeksCAD.app/Contents/share/plugins/heekscnc
	cp -R heekscnc/bin/* /Applications/HeeksCAD.app/Contents/share/plugins/heekscnc
	cp heekscnc/*.py /Applications/HeeksCAD.app/Contents/share/plugins/heekscnc
fi

if [ -d heekscnc/bitmaps ]; then
	cp -R heekscnc/bitmaps /Applications/HeeksCAD.app/Contents/share/plugins/heekscnc
fi

if [ -d heekscnc/icons ]; then
	cp -R heekscnc/icons /Applications/HeeksCAD.app/Contents/share/plugins/heekscnc
fi

if [ -d heekscnc/nc ]; then
	cp -R heekscnc/nc /Applications/HeeksCAD.app/Contents/share/plugins/heekscnc
fi

if [ -d heekscnc/postprocessor ]; then
	cp -R heekscnc/postprocessor /Applications/HeeksCAD.app/Contents/share/plugins/heekscnc
fi

if [ -f heekscnc/script_ops.xml ]; then
	cp heekscnc/script_ops.xml /Applications/HeeksCAD.app/Contents/share/plugins/heekscnc
fi

cd /Applications/HeeksCAD.app/Contents/MacOS

resolve_dylibs()
{
	binary=$1
	echo "Resolving dependencies for $binary" 1>&2
	sudo install_name_tool -id "$binary" "$binary"
	otool -L $binary | tail +2 | awk '{ print $1 }' | while read dylib; do
		destdir=""
		if [ "`echo $dylib | grep libheeksintf`" -o \
		     "`echo $dylib | grep libtinyxml`" -o \
		     "`echo $dylib | grep libarea`" -o \
		     "`echo $dylib | grep libjpeg`" -o \
		     "`echo $dylib | grep libfreetype`" ]; then
			destdir="."
		fi
		if [ "`echo $dylib | grep libwx`" ]; then
			destdir="wx"
		fi
		if [ "`echo $dylib | grep libTK`" -o \
		     "`echo $dylib | grep libFWOSPlugin`" -o \
		     "`echo $dylib | grep libPTKernel`" ]; then
			destdir="oce"
		fi

		if [ ! -z "$destdir" ]; then
			srcdir=`dirname "$dylib"`
			base=`basename "$dylib"`
			if [ $srcdir == '.' ]; then
				srcdir="/usr/local/lib"
			fi

			src_dylib="${srcdir}/${base}"
			dest_dylib="${destdir}/${base}"

			if [ ! -f "$dest_dylib" ]; then
				while [ -L "$src_dylib" ]; do
					cp -R "$src_dylib" "$destdir"
					link=`readlink "$src_dylib"`
					linkdir=`dirname "$link"`
					if [ $linkdir == '.' ]; then
						linkdir="$srcdir"
					fi
					src_dylib="${linkdir}/${link}"
				done
				cp -R "$src_dylib" "$destdir"
				echo $dest_dylib
			fi

			install_name_tool -change "$dylib" "@executable_path/${dest_dylib}" "$binary"
		fi
	done;
}

if [ ! "$skip_resolve" ]; then
	foundlibs=`resolve_dylibs heekscad`
	while [ ! -z "$foundlibs" ]; do
		foundlibs=`echo "$foundlibs" | while read dylib; do resolve_dylibs "$dylib"; done`
	done

	if [ -f ../share/plugins/heekscnc/libheekscnc.dylib ]; then
		foundlibs=`resolve_dylibs ../share/plugins/heekscnc/libheekscnc.dylib`
		while [ ! -z "$foundlibs" ]; do
			foundlibs=`echo "$foundlibs" | while read dylib; do resolve_dylibs "$dylib"; done`
		done
	fi
fi

chmod -R 555 /Applications/HeeksCAD.app
find /Applications/HeeksCAD.app/Contents/share/heekscad/bitmaps -type f -exec chmod 444 {} \;
find /Applications/HeeksCAD.app/Contents/share/heekscad/icons -type f -exec chmod 444 {} \;

if [ -d /Applications/HeeksCAD.app/Contents/share/plugins/heekscnc/bitmaps ]; then
	find /Applications/HeeksCAD.app/Contents/share/plugins/heekscnc/bitmaps -type f -exec chmod 444 {} \;
fi

if [ -d /Applications/HeeksCAD.app/Contents/share/plugins/heekscnc/icons ]; then
	find /Applications/HeeksCAD.app/Contents/share/plugins/heekscnc/icons -type f -exec chmod 444 {} \;
fi

EOF
