## Chitralok : A Jpeg Viewer

### Introduction
This repository aims to provide a simple jpeg decoder (and viewer).
It is intended to serve as a reference for any programmers writing their own jpeg decoder. How will this repo help?

* This Readme doc  shall provide a fairly top-level explanation, with links to more detailed explanations as needed.
* The code itself logs/stores the output of the various stages of the decoding process, so that the reader's own implementation may be compared against it.

**I do not comment on the encoding process here**, for it is has already been elsewhere explained  by people much smarter than me.

The idea is that the reader will study the encoding process, learn to parse the file, and only then will he look to this guide, as a kind of quick guide on constructing a decoder. 
___
### Section 0: Using the Code as a Reference
First, I  will mark down the aforementioned steps during which this program shall populate the log file. Important steps are emphasized in boldface; the others may be skipped.

* Step 1:
* Step 2:

___

### Section 1: JPEG

#### 1.1: The (Abridged) Anatomy of a JPEG File
We'll focus on JPEG/JFIF. This part does not need much explanation, as it is simple to understand. One can find a handy reference very easily. As of now, it suffices to say that the beginning of the file
must contain the byte sequence `FF D8`.

#### 1.2: The Decoding Process

Although I promised above not to focus on the encoding process, it is instructive to consider its steps in order.

1. Block preparation
2. Application of Discrete Cosine Transformation to each block
3. Quantization
4. Differential reduction
5. Linearization and run-length encoding

Therefore, to decode a JPEG image into a series of
raw pixels, we must follow these teps:

1. 