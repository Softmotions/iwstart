# iwstart

 Boilerplate initial project generator for C projects based on iowow, iwnet, ejdb2 libraries.
## Usage 

```sh
iwstart -h

	IOWOW/IWNET/EJDB2 Project boilerplate generator

Usage iwstart [options] <project directory>

	Note: Options marked as * are required.

	* -a, --artifact=<>	Project main artifact name (required).
	* -n, --name=<>		Short project name.
	  -b, --base=<>		Project base lib. Either of: iowow,iwnet,ejdb2. Default: iwnet
	  -d, --description=<>	Project description text.
	  -l, --license=<>	Project license name.
	  -u, --author=<>	Project author.
	  -w, --website=<>	Project website.
	      --no-uncrustify	Disable uncrustify code form atter config
	      --no-lvimrc	Disable .lvimrc vim file generation
	  -c, --conf=<>		.ini configuration file.
	  -V, --verbose		Print verbose output.
	  -v, --version		Show program version.
	  -h, --help		Print usage help.
````

## Installation

```sh
git clone https://github.com/Softmotions/iwstart

mkdir ./iwstart/build && cd ./iwstart/build

cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/.local

make install
```

## Example

```sh
iwstart -a myproj --name="My project" -b iowow ./myproj

cd ./myproj/build

cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

make

ln -s ./build/compile_commands.json ../compile_commands.json 

```
