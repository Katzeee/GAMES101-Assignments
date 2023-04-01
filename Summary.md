assignment 1

---
- top-down: when calculating width, notice you should get a positive value of `Size * Aspect`

- use radians rather than degree

assignment2
---
- use bounding box to decrease the amount of pixels need drawing

- Black border around the triangle 

  The color at the border should be the blend of two triangles rather than the blend with black background

  So you can't shader a pixel by `color * count / 4`, you need to record the color in a buffer then average them

assignment3
---
- half vector is the normalization of the `l` plus the `v` which is `(v + l).nromalized()` not `(v + l) / 2`

- mind the `u` `v` may out of bound [0, 1]

  weird bug on Windows: `u` and `v` can't equal to 1, must smaller than 1

- really take care of float and int, there is a big difference between `1 / x` and `1.0 / x`

- compute the TBN matrix with normalized normal vector

assignment5
---
- The light direction is gotten via translate the `raster coordinate` -> `NDC coordinate` -> `camera` -> `world`

  Mention that when converting to camera's, you should consider FOV and aspect ratio.