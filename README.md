HandyLILO
=========

*Jump to U-Boot directly from Windows CE running on a Handy Terminal PM251.(similar to BrainLILO)*


Build on x86_64 Linux
---------------------

1. Get [cegcc](https://github.com/brain-hackers/cegcc-build/releases) and unzip it
2. Copy the directory `cegcc` into `/opt`
3. Run `make` with the `PREFIX` specified

Equivalent shell snippet:

```shell
wget -O cegcc.zip https://github.com/brain-hackers/cegcc-build/releases/download/2022-04-11-133546/cegcc-2022-04-11-133546.zip
unzip -q cegcc.zip
mkdir -p /opt  # it may need sudo
cp -r cegcc /opt/  # it may need sudo
make PREFIX=/opt/cegcc
```

Install
---------------------------

1. Create a directory named `BrainLILO`
2. Copy dlls and the exe into it
3. Create `LOADER` directory in the SD card's root directory and copy `u-boot.bin` into it

Equivalent shell snippet:

```shell
mkdir BrainLILO
cp *.dll BrainLILO/
cp BrainLILO.exe BrainLILO/
```
