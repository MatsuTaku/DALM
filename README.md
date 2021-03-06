# DALM
<http://www.jnory.com/research/dalm>

This is an implementation of the double-array language model (DALM).
This method is described on:
> Makoto Yasuhara, Toru Tanaka, Jun-ya Norimatsu, Mikio Yamamoto. 2013. An Efficient Language Model Using Double-Array Structures. In Proceedings of the 2013 Conference on Empirical Methods in Natural Language Processing.

* Paper : <http://www.aclweb.org/anthology/D13-1023>
* Slide : <http://www.slideshare.net/jnory/dalm-emnlp-27373037>

## Build Instruction
To build DALM, use waf build system.
Run the command:
> ./waf configure build install --prefix=[install dir]


### (Experimental) CMake build
To build DALM by CMake:
  mkdir build
  cd build
  cmake .. -DCMAKE_INSTALL_PREFIX=[install dir]
  make
  make install

## Usage
### Building a DALM model
To build a DALM model, run:
> [install dir]/scripts/build_dalm.sh [ARPA File] [Division Number] [Output Directory]

If you need to build a large language model (for example, over 30GB in ARPA Format),
we recommend to set the division number "8" or "16" (or larger!).

You may need Ruby 2.0 to run the script.
This script generates following files into [Output Directory]:

* dalm.ini : information of the model.
* dalm.bin : binary dumps of a double-array trie.
* words.bin : binary dumps of vocabulary data.
* words.txt : a set of words of the model.

### Building a DALM model in parallel
By default, build_dalm.sh uses all cores in your CPU.
If you want to reserve some computing resources, you may create /tmp/cpu_reserved file and write the number of cores you want to reserve.
If you have 4 cores and you want to use only 3, you may write only '1' in /tmp/cpu_resered file.

### Using DALM on your system
We include a sample program to use DALM on your system.
Please see sample/query_sample.cpp.
On the build time, you may link the libraries which are stored in the [install dir]/lib directory.

## External Libraries
We use following libraries:

* Darts-clone 0.32g : <https://code.google.com/p/darts-clone/>

To build the source code, we use waf build system version 1.7.13.
We include them into our code.
Here is an original download site of waf.

* <http://code.google.com/p/waf/>

## License
The source code is provided under LGPL v3.
