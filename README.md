What have I got in my pocket?
A collection of utilities of varying utility.
Intended for use with Cinder 0.8.5.
Written against clang/C++11.

These bits of software are written as a library; each piece should stand
on its own as something generally useful.

My current plan is to make the elements in Pockets more functional.
That is, they will do one thing and one thing only, with no side effects.
Further, I am starting to remove the interdependencies of things in Pockets.
I am concurrently developing a component system framework that will take
the place of utilities like sprite sheets and renderers which really want to
be part of a bigger picture.

To Keep:
Image Packing
Message Passing Library
Line Utilities for curve through a line (add adaptive resampling)
Lerp functions

Current Contents:
Sprites
- image packing for making sprite sheets
- sprite rendering from sheets
- sprite animation
Basic Renderers
- SimpleRenderer naively calls render() on all elements
- Renderer2d supports batch renderers
	- Renderer2dStrip
	- Renderer2dStripVbo
Message Passing / Event Passing Library
- Messenger/Receiver pattern allows for decoupled information passing
- Messenger<T> passes arbitrary message types with compile-time type safety
- Receiver<T> allows you to listen for different events with different handlers
Line Utilities
- curve through a collection of points
- path expansion to geometry on CPU
Color Palette
- color selection from an image
Animation Utilities
- lerp function for quaternions, colors, wrapped numbers
File Utilities
- Get next sequential filename

Todo:
CSV parsing a la d3/R

Conventions:

For the creation of smart pointers, we use the following typedefs:  
    std::shared_ptr<class Type>	TypeRef;
    std::unique_ptr<class Type> TypeUniqueRef;
Factory ::create() methods return TypeUniqueRef
You can easily get a TypeRef at creation if that's what you want
TypeRef t = Type::create(); // compiler will make a shared_ptr w/o explicit call to move()
auto t = Type::create(); // yields a unique_ptr
