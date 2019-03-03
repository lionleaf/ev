Requirements
========

You need cmake 3.13+
Maybe some opengl requirements (worked out of the box for me)

Mac
-------
`brew install cmake`

Build
========

With `make`
---------

```
mkdir build
cd build
cmake ..
make
```

On subsequent builds you only need to run `make` from the `build` folder. No need to rerun cmake. 

Visual Studio 2017
-------------

```
mkdir build-vs
cd build-vs
cmake -G "Visual Studio 15" ..
```

Note that if you run from windows it might be enough to just do `cmake ..`

Open the generated visual studio project file  :)

Xcode
----------
```
mkdir build-xcode
cd build-xcode
cmake -G Xcode ..
```

And you have an xcode project you can open :)


