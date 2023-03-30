assignment 1
---
- top-down: when calculating width, notice you should get a positive value of `Size * Aspect`

- use radias rather than degree

assignment2
---
- use bounding box to decrease the amount of pixels need drawing

- Black border around the triangle 

  The color at the border should be the blend of two triangles rather than the blend with black background

  So you can't shader a pixel by `color * count / 4`, you need to record the color in a buffer then average them

assignment3
---
- half vector is the normalization of the `l` plus the `v` which is `(v + l).nromalized()` not `(v + l) / 2`