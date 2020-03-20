# C++ Parser for IMDB list files
This parser will convert IMDB list files to CSV format.
CMake will automatically download compressed versions of the archives to the `./data/compressed/` folder.
To add a new archive, provide a file name and a download URL in designated section of the root `CMakeLists.txt` file.
It is possible to manually add files to the `./data/compressed/` and `./data/decompressed/` folders as well.

Additionally, to actually parse the data, a parsing method must be set in `ParseController.hpp`.


### Requirements
The C++ parser requires C++20 to compile. CMake is required to build the project.
Additionally, GZip is required for automatic extraction of the source archives. ([Windows](http://gnuwin32.sourceforge.net/packages/gzip.htm), [Linux](https://ftp.gnu.org/gnu/gzip/))


### Command Line Arguments
`--pause-except`:   Pause the parser if an exception occurs.  
`--pause-warn`:     Pause the parser if a warning is emitted.  
`--silence-all`:    Don't print warnings and errors to the console, but print a status message every 100K rows.
