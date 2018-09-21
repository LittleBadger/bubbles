# bubbles

See [here](https://littlebadger.github.io/bblwebgl.html) for the webgl demo.

Two dimensional bubbles satisfy a 2D version of Plateau's laws:
 * Every bubble wall is an arc of a circle with (signed) radius inversely proportional to the difference of pressures on either side. 
 * Bubble walls meet at junctions of three, with a 120 degree angle between each pair of walls.

For example, a solitary bubble's radius is inversely proportional to its pressure. Moreover, two bubbles of radii r1 and r2 that are in contact must be distance (from center to center) \sqrt{ r1^2 + r2^2 - r1 r2 } apart, and must be seperated by an arc of radius r1 r2 / | r1 - r2 | that curves into the bigger bubble. For three (or more) bubbles, it turns out that if every pair i,j of bubbles are distance \sqrt{ri^2 + rj^2 - ri rj} apart, then Plateau's laws are satisfied; in particular, the three walls seperating pairs of bubbles intersect at one point, with angle 120 degrees. 

A bubble cluster satisfying these laws can be drawn as a Voronoi/power diagram, with the function f_i(p) = 1/ri d(p,i)^2 - ri, where p is a point in R^2 and d(p,i) is the distance from p to the center of bubble i. In other words, a point p is inside bubble i if f_i(p) < 0 and f_i(p) < f_j(p) for all other bubbles j; if f_i(p) = f_j(p) then p is on the wall ij between bubble i and j.

For the animation, use a Lennard-Jones type force between every pair of bubbles in contact to force bubbles to have the correct seperation distance. When bubbles collide increase the radius a little bit to approximately conserve volume of bubbles (exact conservation looked unrealistic). Then draw the Voronoi diagram per pixel with the fragment shader, and simultaneously compute the adjacency matrix of bubbles in contact: if a pixel is on the bubble wall ij, then store that in the (i,j) pixel of a texture using the image load/store. Some day, I will add the ability for bubbles to pop or coalesce. 

Thanks to my friends at TU, especially Niklas, Albert, and Jan, for enthusiastic discussions.

