% ROOT Version 6.06 Release Notes
% 2015-11-27
<a name="TopOfPage"></a>

## Introduction

ROOT version 6.06/00 is scheduled for release in November, 2015.

For more information, see:

[http://root.cern.ch](http://root.cern.ch)

The following people have contributed to this new version:

 David Abdurachmanov, CERN, CMS,\
 Bertrand Bellenot, CERN/SFT,\
 Rene Brun, CERN/SFT,\
 Philippe Canal, FNAL,\
 Cristina Cristescu, CERN/SFT,\
 Olivier Couet, CERN/SFT,\
 Kyle Cranmer, NYU, RooStats,\
 Gerri Ganis, CERN/SFT,\
 Andrei Gheata, CERN/SFT,\
 Enrico Guiraud, CERN/SFT, \
 Lukasz Janyst, CERN/IT,\
 Christopher Jones, Fermilab, CMS,\
 Wim Lavrijsen, LBNL, PyRoot,\
 Sergey Linev, GSI, http, JSROOT, \
 Pere Mato, CERN/SFT,\
 Lorenzo Moneta, CERN/SFT,\
 Axel Naumann, CERN/SFT,\
 Danilo Piparo, CERN/SFT,\
 Timur Pocheptsov, CERN/SFT,\
 Fons Rademakers, CERN/IT/Openlab,\
 Enric Tejedor Saavedra, CERN/SFT,\
 Liza Sakellari, CERN/SFT,\
 Manuel Tobias Schiller,\
 David Smith, CERN/IT,\
 Matevz Tadel, UCSD/CMS, Eve, \
 Vassil Vassilev, CERN/SFT \
 Wouter Verkerke, NIKHEF/Atlas, RooFit, \
 Maciej Zimnoch

## ROOT reference manual

The ROOT reference manual has been moved into Doxygen. Still some work and
polish has to be done but the reference guide in this new format is now online
and can be seen from the [ROOT home page](https://root.cern.ch/doc/master/index.html).

## Core Libraries

### Dictionary generation

Fixed the dictionary generation in the case of class inside a namespace
marked inlined.

Added mechanisms to stop the dictionary generation while parsing the XML and while selecting in presence of duplicates.

Fix ROOT-7760: fully allow the usage of the dylib extension on OSx.

### Thread safety and thread awareness

We added the function `TMethodCall::GetCallFunc` to allow direct access to the function wrapper.

We reduced thread serialization in `TClass::GetCheckSum`, `TClass::GetBaseClassOffset` and `TClass::Property`

`TObjArray::Delete` was updated to allow its caller to explicitly avoid costly checks (extra RecursiveRemove and lock)

We removed the need to create a TThread object per thread in a multi-threaded application. Now ROOT can be used with any threading model (e.g. OpenMP, STL threads, TBB) transparently.  All the internal synchronisation mechanisms of ROOT are activated by a single call: `ROOT::EnableThreadSafety()` which is the successor of the existing `TThread::Initialize`.  This call must take place if ROOT needs to be used in a thread safe manner.

### TDirectory::TContext

We added a default constructor to `TDirectory::TContext` which record the current directory
and will restore it at destruction time and does not change the current directory.

The constructor for `TDirectory::TContext` that takes a single TDirectory pointer as
an argument was changed to set `gDirectory` to zero when being passed a null pointer;
previously it was interpreting a null pointer as a request to *not* change the current
directory - this behavior is now implement by the default constructor.

### Cleanups.

Several definition where moved from the global or ROOT namespace to the ROOT::Internal namespace as they are not intended to be used outside of ROOT, including: `gROOTLocal` and related functions, `TSchemaHelper`, `TSchemaMatch`, `TSchemaType`, `RStl`, `ROOT::TROOTAllocator`, `TSchemaRuleProcessor`, `TStdBitsetHelper`, `TInitBehavior`, `TDefaultInitBehavior`, `DefineBehavior`, `THnBaseBrowsable`, `THnBaseBinIter`, `GenericShowMembers`, `TOperatorNewHelper` and `BranchProxy` implementations classes.

Several definition where moved from the global or ROOT namespace to the ROOT::Details namespace as they are intended to be used in 'expert' level code and have a lower level of backward compatibility requirement.  This includes `TCollectionProxyInfo`, `TSchemaRuleSet`.

## I/O Libraries

### hadd

We extended the `hadd` options to allow more control on the compression settings use for the
output file.  In particular the new option -fk allows for a copy of the input
files with no decompressions/recompression of the TTree baskets even if they
do not match the requested compression setting.

New options:

- `-ff` allows to force the compression setting to match the one from the first input
- `-fk[0-209]` allows to keep all the basket compressed as is and to compress the meta data with the given compression setting or the compression setting of the first input file.
- `-a` option append to existing file
- The verbosity level is now optional after -v

### Command line utilities

We added command line utilities to streamline very common operations performed on root files, like listing their content or creating directories.
The command line utilities are:
- `rootbrowse`: to open the file in a TBrowser
- `rootcp`: to copy content from one file to another
- `rooteventselector`: to select a subset of the events in a tree contained in a file
- `rootls`: to list the content of a rootfile
- `rootmkdir`: to create a directory in a rootfile
- `rootmv`: to move content across files
- `rootprint`: to plot content (histograms, graphs) of files
- `rootrm`: to remove content from files
These utilities took inspiration from the well known *nix commands and all offer the `-h` switch which provides documentation for all options available and example invocation lines.


### I/O New functionalities

### I/O Behavior change.



## TTree Libraries

### Improvement of handling of default number of entries

A new const expression value: `TTree::kMaxEntries` has been introduced to
express the largest possible entry number in a `TTree`.  This is used in
two main cases:

- as the default value for the requested number of entries a routine should be
applied to; for example this is used for `TTree::Draw` and `TTree::Process`.
Previously the default was only 1 billions entries, causing those routines to
end early in case of very large trees.

- as the default value for the number of entries returned by TChain::GetEntriesFast.
The previous value was kBigNumber (set to 1234567890) and internally (but somewhat
inconsistently, see [ROOT-6885]) a larger value was used (named theBigNumber).  Now
`TTree::kMaxEntries` is used throughout TChain.

`TChain::kBigNumber` is deprecated and its value has been changed to be equal
to `TTree::kMaxEntries`.



## Histogram Libraries

### Change `TGraph::ComputeRange`: in case of log scale the minimum along X and
Y axis are now set to the lowest positive values of the graph. Previously a % of the
maximum was used which may hide some points like in the following example
``` {.cpp}
{
   TGraph * gr = new TGraph(10);
   for (int i = 0;i<10;i++) gr->SetPoint(i,i,TMath::Exp(-10.0*i));
   for (int i = 5;i<10;i++) gr->SetPoint(i,i,0.);
   gr->Draw("apl");
   gr->SetMarkerStyle(20);
   gPad->SetLogy(true);
}
```
The problem was reported [here](https://root.cern.ch/phpBB3/viewtopic.php?f=3&t=20484).

### TGraph

`TGraph::GetHistogram()` was resetting the TimeDisplay attribute of axis.
The problem was reported [here](https://sft.its.cern.ch/jira/browse/ROOT-7766).

### Fitting

* Improve thread safety of TH1::Fit by making static member of TVirtualFitter and TMinuitMinimize thread local.  This fixes [ROOT-7791].

## Math Libraries


## RooFit Libraries


## 2D Graphics Libraries

### THistPainter

Improve the algorithm to compute the lower limit of an axis in log scale when its
real lower limit is 0. The problem was reported in ROOT-7414.

Using the `COL` option with histograms having some negative bins; the empty bins
(containing 0) are drawn. In some cases one wants to not draw empty bins
(containing 0) of histograms having a negative minimum. The option `1`, used with
the option `COL`, allows to do that.

Implement the Log option for `CANDLE` plots as requested
[here](https://root.cern.ch/phpBB3/viewtopic.php?f=3&t=20225&p=87006#p87006).

### TTeXDump

From Dmitry Kalinkin (via github): Fix file corruption in `TTeXDump::DrawPolyMarker`
The current implementation of `TTeXDump` uses `TVirtualPS::PrintFast` based methods
to output TeX markup with automatic line-wraps. Yet these methods are optimized for
PostScript format where there are a lot of space characters that are used for newline
placement. Current `TTeXDump::DrawPolyMarker` would often produce a long contiguous lines
that trigger a forceful linewrap that can happen in the middle of real number constant
(ignored by latex) or even in the middle of latex command (producing incorrect file).
One solution would be to rewrite TTeXDump using only `PrintRaw` (that you can't mix
with `PrintStr/PrintFast/WriteReal`). The other would be to fix `PrintFast` to not
introduce forced newline. The third option is less intrusive and just adds additional
spaces to provide clues for the proper line wrapping (this is the one implemented in
this change).

### TLatex

Make sure the line width used to draw `#sqrt` is always >= 1.

When a global text alignment was set the `TLatex`characters `#minus`, `#plus`,
`#mp`, `#hbar`, and `#backslash` were mis-aligned. The following macro demonstrate
the problem:

``` {.cpp}
{
   gStyle->SetTextAlign(22);
   TLatex t(.5,.5,"#minus100 #mp100 #plus100 #hbar #backslash");
   t.Draw();
}
```

The angle of a `TLatex` object was set to 0 if the `GetYsize` method was called.

### TColor

New palette `kViridis`. It was presented at SciPy2015 by Stéfan van der Walt and
Nathaniel Smith. It is now matplotlib's current default color map.

![Viridis](palette_112.png)


### TMultiGraph

Ignore empty graphs when computing the multi-graph range at painting time.

### TASImage

A left click on a image produced a one pixel zoom.

### TCreatePrimitives

The ending of a polyline creation is based on the closeness of the two last
entered points. The previous algorithm was based on user coordinates. It is now
based on pixel to avoid the problem reported
[here](https://root.cern.ch/phpBB3/viewtopic.php?f=3&t=20343).

### TCanvas

When the first canvas created by ROOT was in batch mode, it was note possible to
comme back in interactive mode for the next canvases. this problem was reported
[here](https://root.cern.ch/phpBB3/viewtopic.php?f=3&t=20354).

### Cocoa Backend

Sometimes the mouse cursor did not change back to the window manager arrow when
exiting a `TCanvas`.

### `freetype` library

Updates `builtin_freetype` to 2.6.1 (current upstream version), which can detect
`PPC64LE` machine. This was compiled and tested on `SLC6 + ICC + x86_64`,
`F21 + GCC + ppc64le`, `MacOSX 10.11.1 + Xcode 7.1` and `Windows (ROOT 5.34)`.
`$ROOTSYS/graf2d/freetype/src/README` was removed, because no issues were noticed
with `ICC` compiler and `-Wall -pedantic -ansi` flags.
Additionally `--with-png=no --with-bzip2=no` flags are passed to freetype
configuration script. Default values for these options are auto.
`freetype` finds `libpng` and `libbzip2` on the system and builds extra
modules. Then attempting to link against `freetype` one would need to link
`-lpng -lbzip2` explicitly otherwise linking will returns in undefined
references. Otherwise we would need to check for `libpng` and `libbzip2` on the system
and adjust `FREETYPE_LIBRARIES` to include `-lpng` and `-lbzip2`.
The current solution goes for the minimal configuration. The original request for
this update was posted [here](https://sft.its.cern.ch/jira/browse/ROOT-7631).

## 3D Graphics Libraries


## Geometry Libraries


## Database Libraries


## Networking Libraries

### THttpServer

Support of POST HTTP requests. For example, ROOT objects can be send with POST request and used as arguments of
objects method execution in exe.bin and exe.json requests. Request and response HTTP headers are now directly accessible in THttpCallArg class

When command is registered with THttpServer::RegisterCommand() method,
one could configure additional arguments which should be submitted when
command is executed with cmd.json requests

Introduce restriction rules for objects access with THttpServer::Restrict() method.
Up to now general read-only flag was applied - either
everything read-only or everything is fully accessible.
Now one could restrict access to different parts of
objects hierarchy or even fully 'hide' them from the client.
Restriction based on user account name, which is applied
when htdigest authentication is configured.
One also able to allow execution of selected methods.

Implement multi.bin and multi.json requests.
One could request many items with single HTTP request.
Let optimize communication between server and client.

With *SNIFF* tag in ClassDef() comments one could expose different properties,
which than exposed by the TRootSniffer to the client with h.json requests.
Such possibility ease implementation of client-side code for custom classes.

Allow to bind http port with loopback address.
This restrict access to http server only from localhost.
One could either specify 'loopback' option in constructor:
    new THttpServer("http:8080?loopback")
or in clear text specify IP address to which http socket should be bind:
    new THttpServer("http:127.0.0.1:8080")
If host has several network interfaces, one could select one for binding:
    new THttpServer("http:192.168.1.17:8080")

### TNetXNGFileStager
Fixed ROOT-7703. This restores the behavior of Locate() to that found with
TXNetFileStager: Rather than return only the xrootd server's reply, the endpoint
hostname is looked up and Locate() returns the full url, including the path.


## GUI Libraries


## Montecarlo Libraries


## Multi-processing

With this version we introduce a new module, core/multiproc, for multi-processing on multi-core machines. This module is based on fork technology and offers an interface inspired from Python multiprocessor module. The new interface, implemented in the class TProcPool, provides the possibility to perform in parallel a very generic set of tasks, described by macros, functions or lambdas.
Tutorials illustrating the usage of the new class TProcPool are available under tutorials/multicore.


## Language Bindings

### Notebooks
We provided integration of ROOT with the Jupyter technology, integrating ROOT with Python Notebooks and providing a ROOT Kernel like functionality - de facto an enhanced C++ web based shell. Tab completion, output and graphics inlining have been added. These functionalities are automatically available upon import of the ROOT module in a Notebook or at startup of a ROOT prompt kernel.
We made it easier to use ROOT notebooks locally, by providing a 'root --notebook' command option to start a local notebook server customised with all the ROOT features.

New tutorials and code examples have been provided here: https://root.cern.ch/code-examples#notebooks

Support for capturing large outputs (stderr/stdout) coming from C++ libraries has been added.

## JavaScript ROOT

- support registered in THttpServer commands with arguments.
- provide workaround for websites using require.js and older jquery-ui
- support custom requests to remote objects, demonstrated in httptextlog.C tutorial
- rewrite draw.htm (page for individual object drawing) to support all custom features as main gui does

## Interpreter

ROOT can now dump the contect of STL collections, for instance `map<string,int>`. A few ROOT types print their content, too.

This release contains everal bug fixes and improvements, notably in unloading and performance.

> NOTE: The GCC 5 ABI is *not* supported yet, due to a lack of support in clang.


## Tutorials


## Class Reference Guide

## Build, Configuration and Testing Infrastructure

ROOT uses the CMake cross-platform build-generator tool as a primary build system. CMake does not build the project, it generates the files needed by your build tool (GNU make, Ninja, Visual Studio, etc) for building ROOT. The classic build with configure/make is is still available but it will not be evolving with the new features of ROOT.

Minor chages in the build system:
- Renamed CMake option POSTGRESQL_LIBRARIES to POSTGRESQL_LIBRARY
