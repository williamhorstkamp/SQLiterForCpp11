# SQLiterForCpp11
SQLiter For C++11 is an SQLite3 wrapper with C++11 features.  
  
It was created to bridge the significant gap between the SQLite3 C interface and
the featureset available in the newest versions of C++. Containing just a small handful
of classes, only two of whose interface (SQLite and Statement wrappers) must be learned
to successfully use the wrapper.  
In addition to this, SQLiter For C++11 replaces the need to learn the numerous SQLite3
constructor and destructor functions with a predictable object oriented approach.

##SQLite?
SQLite3 is a simple and widely deployed relational database engine that is suitable for a wide
range of projects. While SQLite is a powerful platform with a complete C/C++ interface, the interface
doesn't utilize newer C++ functions, a problem which SQLiter seeks to remedy.  
An SQLite3 installation is required for SQLiter to function.  
SQLite can be found at http://www.sqlite.org/  


##Installation
An installation requires at least STL and SQLite3.  
SQLite3 can be included into a project in a number of ways, including by adding its source to the project or using
it as a library, and the choice is up to the user and is currently beyond the scope of this installation guide.  
All projects will take all header and source files to work properly.  
The easiest installation scenario involves placing the header and source files into
the directory you are already using in your project and simply using the namespace SQLiter
while compiling and linking against the standard SQLite3 library.  

##Example
The example can be compiled using any number of public C++11 toolkits. (GNU, Visual Studio, LLVM)  
Solutions and makefiles TBA

##Copyright Stuff
Copyright (c) 2015 William Horstkamp  
william.horstkamp@gmail.com  
MIT LICENSE  
License info can be found at license.txt  

The latest version of this code can always be found at  
https://github.com/williamhorstkamp/SQLiterForCpp11

This code is dependant on the SQLite3 interface for C, which can be found at  
https://www.sqlite.org/download.html