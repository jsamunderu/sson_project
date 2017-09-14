
The Stupid-Simple Object Notation
===

Overview
---

This is a stupid simple object notation. It helps us format objects
for serialization to files, over the wire, or basically anywhere 
objects need to be stored. It's also human readable, so it allows for
a human to be able to parse and understand what the object looks like,
just by reading the output.

It's pretty simple, an object consists of key-value pairs or properties
and can consist of one or more children objects. An objects properties
are bound within '{' and '}' characters. A property is denoted as a key
value pair separated by ':' characeter. A property is ended with a ';'
character.

To keep things simple, all property values are treated as character strings.
This shifts the work of conversion and storage onto the client of my
library. Because I didn't want to have to deal with different types and
managing how they're interpreted. 

I'm pretty sure this is a decent way to store objects into files and
transfer them over the internet. I've seen other implementations,
they're more complicated and for what I'm doing it's not worth my
time or effort to use a large parser library.

I've been hoping to use this library in other projects across other
platforms and other compilers, such as clang and g++. However, 
I'm a Microsoft lacky so I use Visual Studio, since it's the best
IDE on the planet. But since I want to keep it cross platform, 
sticking to the C++ standards is key. No OS dependent calls in 
my libraries.

I'm also very keen on using unit tests to prove that my code is doing
what I think it's doing. I've really seen the benefit of creating
unit tests alongside my code, because it enables me to see how my 
interface is used, it helps other users to see how the interface is
intended to be used, it ensures that I prove the interface does what
it should, and it stops me from adding pieces of code that I don't
really need as I would need to test it.

Unfortunately, the unit testing that I'm using is built into Visual
Studio. So unless I would want to bootstrap some other kind of unit
test framework and port my tests, I'm stuck using VS2015... 

Good news everyone!

The community version is free though and I got it from 
[here](https://www.visualstudio.com/en-us/downloads/download-visual-studio-vs.aspx).

Usage
---

TODO - probably should explain how to use this thing... or something.


ToDo
---

Below is a list of things still left to do:

	i) Complete the output serializer - in order for me to actually use
	this thing, I'll need to complete the output serializer to help 
	output my objects to files.

	ii) Output serializer tests - I don't even have ONE test for the 
	output serializer... Probably doesn't even work.

	iii) More testing on the input serializer - I have a few tests 
	for my input serializer, definitely not enough.

	iv) Make the interface easier for accessing values - right now
	the client has to scan all the pairs to find their value. There
	has to be a better way to do this...

	v) Make it easier for clients to access different types - The
	more I think about it, storing all the values as strings might
	be too much work for the client to convert. Maybe I could give
	them a better way to do this.	

	vi) Come up with somewhere to use this thing - I'm convinced that
	SSON is the next big thing, I just don't know where I'd use it
	yet. I should probably come up with some kind of project that
	showcases it.

ChangeLog
---

	03.22.2016 Peter Lorimer Peter.Lorimer@SUBNET.com
	
		- initial project commit

	03.23.2016 Peter Lorimer Peter.Lorimer@SUBNET.com

		- completed input serialization
		- wrote some input tests