# What is this

This is an attempt to convert openni2 -> openni1 (and in theory it should be relatively easy to do it the other way)

# The status of this

1. Reading a openni2 .oni: I ran into a bug in openni2: When replaying a recorded .oni file that has image data, when you call readFrame on the image data, it will never return, making it impossible. Pulling the latest git/development branch didn't fix this.
2. Passing-through openni2 to openni1, and recording it with openni1: This seems to cause a segfault internal to the two libraries, probably due to the fact they share the same namespace/etc. (I did not investigate this too much)
3. Recording openni2 as a folder of images (using openCV), and using them to play-back the images: It works(?), however NiViewer seems to crash looking for an irStream (which I never implemented), and using some other utilities I have, I found them to segfault, again I didn't look into this too much at this point.

# What I did:

At this point I had folders which had all the colour/depth frames I wanted, in sequential order, which is all I needed out of this. So I simply load-in the images with OpenCV and re-wrote the OpenNI input's to my programs.

This is provided as a reference, and in hopes that it may be Useful to someone somewhere for something. If there are any questions, feel free to email me (ttoocs@gmail.com), or leave something in the issues.

# License:

Apache License, Version 2.0
-As my code originated from a OpenNI2 sample, which still has some unchanged areas in it.

