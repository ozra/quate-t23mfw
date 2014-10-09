    /**
    * Created:  2014-08-16
    * Author:   Oscar Campbell
    * Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
    **/

Build Dir Structure
===================

Generally
---------
- "name things for what they are"  
- "put things where they belong"  
- "keep similar things close to each other", and finally  

Our outline
-----------------
- **_src_** - source files, a folder for each namespace/package to easily retrieve files (even header files for C/C\++)  
- **_inc_** - for C/C++ "interface" header files  
- **_ext_** - for externals/third-party libraries. Inside, a folder for each library (binaries, include files and source). _"lib"_ has gotten to many ad-hoc de facto associations, so we avoid that in the source-tree.  
- **_bin_** - for built binaries, could be quickly exported for release  
- **_obj_** - for all temporarily generated files (.o, .class, .obj etc...)  
- **_doc_** - for any documentation, usually generated with Doxygen  
- **_res_** - by placing resources here, it's possible to separate text source files and binary resources used by the program. No specific hierarchy inside.  

C++ learning
============

BOOST
-----

Boost has an unimaginable number of libraries. 

#### Easy ones to get started on are:

noncopyable  
array  
circular_buffer  
foreach  
operators (one of my personal favorites)  
smart_ptr  
date_time  

#### More advanced ones include:

lambda  
bind  
iostreams  
serialization  
threads  

More BOOST
----------

**boost::any** and **boost::variant** are good of you need a variant data type, with two different approaches.  
**boost::regex** if you need some text parsing.  
**boost::thread** and **boost::filesystem**  




SQLite wrapper header only
==========================

https://github.com/burner/sweet.hpp/blob/master/sweetql.hpp



C++ - Web-UI
===========

https://github.com/sihorton/appjs-deskshell
https://github.com/rogerwang/node-webkit
http://cppcms.com/wikipp/en/page/main
http://www.awesomium.com/
http://librocket.com/


HTML5 Canvas Charts jQuery based - pluginable
=============================================

http://www.flotcharts.org/flot/examples/


TradingView API - for ideas / reference
=======================================

https://docs.google.com/document/d/1rAigRhQUSLgLCzUAiVBJGAB7uchb-PzFVe0Bl8WTtF0/edit?pli=1#

