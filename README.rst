Orange'S
--------

The current version of Orange'S.

If you are reading my books, click `here`_.

.. _`here`: https://github.com/yyu/osfs00

Develop Orange'S in Mac OS X
````````````````````````````

Install the Mac version "gcc":

1. install Xcode
2. in Xcode: Preferences > Downloads > Components > Command Line Tools > install

Build cross-compilers::

    $ tar vxjf binutils-2.22.tar.bz2
    $ cd binutils-2.22
    $ ./configure --target=i686-elf --disable-nls
    $ make all
    $ make install

    $ tar vxjf gcc-4.6.3.tar.bz2
    $ cd gcc-4.6.3
    $ ./configure --target=i686-elf --disable-nls --enable-languages=c,c++ --without-headers
    $ make all-gcc
    $ make install-gcc

Tip: how to know what have been installed::

    $ make DESTDIR=/tmp/binutils install
    $ make DESTDIR=/tmp/gcc install-gcc

Change Makefile::

    -DASM           = objdump
    -CC             = gcc
    -LD             = ld
    +DASM           = i686-elf-objdump
    +CC             = i686-elf-gcc
    +LD             = i686-elf-ld
    +AR             = i686-elf-ar

    -LDFLAGS                = -Ttext $(ENTRYPOINT) -Map krnl.map
    +LDFLAGS                = -Ttext $(ENTRYPOINT) -Map krnl.map -m elf_i386

    -       sudo mount -o loop a.img /mnt/floppy/
    +       hdiutil mount -mountpoint /mnt/floppy a.img

Install Bochs::

    $ tar vxzf bochs-2.5.1.tar.gz
    $ cd bochs-2.5.1
    $ ./configure --with-x11
    $ make

we don't have to really "make install" it.
creating a symbol link will suffice::

    $ cd ..
    $ mv bochs-2.5.1 /opt/
    $ ln -s /opt/bochs-2.5.1/bochs /opt/bin/

Change bochsrc::

    -romimage: file=/usr/share/bochs/BIOS-bochs-latest
    -vgaromimage: file=/usr/share/vgabios/vgabios.bin
    +romimage: file=/opt/bochs-2.5.1/bios/BIOS-bochs-latest
    +vgaromimage: file=/opt/bochs-2.5.1/bios/VGABIOS-elpin-2.40
     
    -keyboard_mapping: enabled=1, map=/usr/share/bochs/keymaps/x11-pc-us.map
    +keyboard_mapping: enabled=1, map=/opt/bochs-2.5.1/gui/keymaps/x11-pc-us.map

Make sure /opt/bin is in `$PATH`. If not, append a line in ~/.bashrc::

    export PATH=/opt/bin:$PATH

Run Orange'S::

    $ cd /path/to/Orange'S/
    $ make image
    $ bochs

